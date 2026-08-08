#include "echoform/EchoState.h"

namespace echoform {

namespace {

constexpr uint32_t kErrorHoldFrames = 90;
constexpr uint32_t kSpeakSettleFrames = 18;

float clampf(float v, float lo, float hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}

float approach(float x, float target, float rate) {
  return x + (target - x) * rate;
}

// Static pose target per expression (state.rs pose_target).
ExpressionPose poseTarget(Expression e) {
  ExpressionPose p;  // NEUTRAL defaults
  switch (e) {
    case Expression::Neutral:
      break;
    case Expression::Listening:
      p.eyeOpen = 0.7f;
      p.browRaise = 0.2f;
      break;
    case Expression::Thinking:
      p.eyeOpen = 0.4f;
      p.browTilt = 0.2f;
      p.gazeX = 0.25f;
      break;
    case Expression::Curious:
      p.eyeOpen = 0.85f;
      p.browRaise = 0.4f;
      p.browTilt = 0.15f;
      break;
    case Expression::Speaking:
      p.eyeOpen = 0.6f;
      p.browRaise = 0.1f;
      break;
    case Expression::Amused:
      p.eyeOpen = 0.5f;
      p.mouthCurve = 0.4f;
      p.browRaise = 0.1f;
      break;
    case Expression::Concerned:
      p.browRaise = 0.3f;
      p.browTilt = -0.25f;
      p.mouthCurve = -0.2f;
      break;
    case Expression::Confused:
      p.browTilt = 0.4f;
      p.mouthCurve = -0.1f;
      p.gazeX = -0.2f;
      break;
    case Expression::Error:
      p.eyeOpen = 0.2f;
      p.browTilt = -0.4f;
      p.mouthCurve = -0.3f;
      break;
  }
  return p;
}

// Damped spring (state.rs): shifts expressed as divisions.
void spring(float &x, float &v, float target, float stiffDiv,
            float dampMul) {
  const float a = (target - x) / stiffDiv - v * dampMul;
  v += a;
  x += v;
}

}  // namespace

void EchoController::enter(AvatarState state, Expression expression) {
  state_ = state;
  expression_ = expression;
  hold_ = 0;
  activity_ = 0.0f;
  if (state == AvatarState::Processing) {
    oscStep_ = 300;
  }
}

void EchoController::pump(float amount) {
  activity_ = clampf(activity_ + amount, 0.0f, 0.25f);
}

float EchoController::breath() const {
  return fxSin(static_cast<uint16_t>(frame_ * 140)) * 0.03f;
}

void EchoController::handle(EchoEvent event) {
  using S = AvatarState;
  using E = EchoEvent;
  switch (event) {
    case E::PushToTalkPressed:
      if (state_ == S::Idle) enter(S::Listening, Expression::Listening);
      break;
    case E::PushToTalkReleased:
      if (state_ == S::Listening) enter(S::Processing, Expression::Thinking);
      break;
    case E::VoiceStarted:
      if (state_ == S::Listening) expression_ = Expression::Curious;
      break;
    case E::VoiceEnded:
      if (state_ == S::Listening) expression_ = Expression::Listening;
      break;
    case E::AudioBlockReady:
      if (state_ == S::Listening) pump(0.02f);
      break;
    case E::RequestStarted:
      if (state_ == S::Processing) pump(0.10f);
      break;
    case E::TranscriptPartial:
      if (state_ == S::Processing) pump(0.05f);
      break;
    case E::TranscriptFinal:
      if (state_ == S::Processing) {
        expression_ = Expression::Curious;
        pump(0.125f);
      }
      break;
    case E::ProviderConnected:
      if (state_ == S::Processing) pump(0.10f);
      break;
    case E::ProviderDelta:
      if (state_ == S::Processing) {
        oscStep_ = oscStep_ + 8 > 700 ? 700 : oscStep_ + 8;
        pump(0.04f);
      } else if (state_ == S::Speaking) {
        pump(0.025f);
      }
      break;
    case E::SpeechAudioStarted:
      if (state_ == S::Processing) enter(S::Speaking, Expression::Speaking);
      break;
    case E::SpeechAudioEnded:
    case E::RequestCompleted:
      if (state_ == S::Speaking) {
        hold_ = kSpeakSettleFrames;
      } else if (state_ == S::Processing &&
                 event == E::RequestCompleted) {
        enter(S::Idle, Expression::Neutral);
      }
      break;
    case E::Cancelled:
      if (state_ == S::Listening) {
        enter(S::Idle, Expression::Neutral);
        break;
      }
      // fallthrough to fault handling
    case E::Timeout:
    case E::NetworkError:
    case E::ProtocolError:
      if (state_ != S::Idle) {
        enter(S::Error, Expression::Error);
        instability_ = 1.0f;
        hold_ = kErrorHoldFrames;
      }
      break;
  }
}

void EchoController::step(float speechRms, float speechLow, float speechMid) {
  using S = AvatarState;
  ++frame_;

  // Coherence target per state.
  float target;
  switch (state_) {
    case S::Idle:
      target = 0.10f + breath();
      break;
    case S::Listening:
      target = 0.25f + activity_;
      break;
    case S::Processing:
      oscPhase_ = static_cast<uint16_t>(oscPhase_ + oscStep_);
      target = 0.45f + fxSin(oscPhase_) * 0.25f + activity_;
      break;
    case S::Speaking:
      target = 0.85f + speechRms * 0.15f;
      break;
    case S::Error:
    default:
      target = 0.10f;
      break;
  }
  const float rate = state_ == S::Error      ? 1.0f / 8.0f
                     : state_ == S::Speaking ? 1.0f / 12.0f
                                             : 1.0f / 20.0f;
  coherence_ = approach(coherence_, clampf(target, 0.0f, 1.0f), rate);
  activity_ *= 0.95f;

  if (state_ == S::Error) {
    instability_ *= 0.94f;
    const float jitter =
        fxSin(static_cast<uint16_t>(frame_ * 9000)) * instability_;
    coherence_ = clampf(coherence_ + jitter / 6.0f, 0.0f, 1.0f);
    if (hold_ > 0 && --hold_ == 0) {
      enter(S::Idle, Expression::Neutral);
    }
  }
  if (state_ == S::Speaking && hold_ > 0 && --hold_ == 0) {
    enter(S::Idle, Expression::Neutral);
  }

  // Pose easing toward the expression target, with speech-driven mouth.
  ExpressionPose targetPose = poseTarget(expression_);
  if (state_ == S::Speaking) {
    targetPose.mouthOpen = clampf(speechRms * 1.8f, 0.0f, 1.0f);
    targetPose.jawDrop = clampf(speechLow * 1.5f, 0.0f, 1.0f);
    targetPose.mouthWidth = clampf(0.5f + speechMid * 0.5f, 0.0f, 1.0f);
  }
  const float openRate =
      targetPose.mouthOpen > pose_.mouthOpen ? 0.5f : 1.0f / 6.0f;
  pose_.mouthOpen = approach(pose_.mouthOpen, targetPose.mouthOpen, openRate);
  pose_.jawDrop = approach(pose_.jawDrop, targetPose.jawDrop, 0.2f);
  constexpr float kEase = 1.0f / 12.0f;
  pose_.browRaise = approach(pose_.browRaise, targetPose.browRaise, kEase);
  pose_.browTilt = approach(pose_.browTilt, targetPose.browTilt, kEase);
  pose_.mouthWidth = approach(pose_.mouthWidth, targetPose.mouthWidth, kEase);
  pose_.mouthCurve = approach(pose_.mouthCurve, targetPose.mouthCurve, kEase);
  pose_.gazeX = approach(pose_.gazeX, targetPose.gazeX, kEase);
  pose_.gazeY = approach(pose_.gazeY, targetPose.gazeY, kEase);

  // Head pose targets: restrained, state-led, expression-tinted.
  float yawT = 0.0f, pitchT = 0.0f, rollT = 0.0f;
  switch (state_) {
    case S::Idle:
      break;
    case S::Listening:
      yawT = 0.12f;
      pitchT = 0.10f;
      break;
    case S::Processing:
      yawT = fxSin(static_cast<uint16_t>(oscPhase_ / 2 + 8000)) * 0.55f;
      pitchT = 0.18f;
      break;
    case S::Speaking:
      yawT = fxSin(static_cast<uint16_t>(frame_ * 70)) * 0.08f;
      pitchT = speechLow * 0.12f;
      break;
    case S::Error:
      yawT = fxSin(static_cast<uint16_t>(frame_ * 11000)) * instability_;
      rollT = instability_ * 0.30f;
      break;
  }
  if (expression_ == Expression::Curious) {
    yawT += 0.08f;
    rollT += 0.25f;
  } else if (expression_ == Expression::Concerned) {
    rollT -= 0.10f;
    pitchT += 0.20f;
  }
  // Layered value noise + breathing (asymmetric triangle, ~13/min).
  yawT += fxValueNoise(frame_, 293, 11) * 0.28f;
  const uint32_t bt = frame_ % 280;
  const float breathT = bt < 112
                            ? static_cast<float>(bt) / 112.0f
                            : 1.0f - static_cast<float>(bt - 112) / 168.0f;
  pitchT += breathT * 0.10f;

  // Damped springs (~2.4 Hz, zeta ~0.75): stiff /16, damping v*0.375.
  spring(head_.yaw, yawV_, clampf(yawT, -1.0f, 1.0f), 16.0f, 0.375f);
  spring(head_.pitch, pitchV_, clampf(pitchT, -1.0f, 1.0f), 16.0f, 0.375f);
  // Roll follows yaw velocity (follow-through) plus the expression tilt.
  const float rollTarget = clampf(rollT + yawV_ * 3.0f, -1.0f, 1.0f);
  spring(head_.roll, rollV_, rollTarget, 16.0f, 0.375f);

  // Lateral drift: two noise octaves through a heavier spring (/64, 0.1875).
  const float wander = fxValueNoise(frame_, 197, 7) * 4.0f +
                       fxValueNoise(frame_, 89, 13) * 2.0f;
  float shiftT;
  switch (state_) {
    case S::Idle: shiftT = wander * 0.20f; break;
    case S::Listening: shiftT = 3.0f + wander * 0.30f; break;
    case S::Processing: shiftT = wander; break;
    case S::Speaking: shiftT = wander * 0.45f; break;
    case S::Error:
    default:
      shiftT = fxSin(static_cast<uint16_t>(frame_ * 9500)) *
               instability_ * 4.0f;
      break;
  }
  spring(head_.shiftX, shiftV_, shiftT, 64.0f, 0.1875f);

  // Key-light orbit (~35 s) advances every frame.
  head_.lightPhase = static_cast<uint16_t>(head_.lightPhase + 31);

  // Blinks: scheduled by hash 2.5-6 s apart, 9-frame envelope.
  eyeBase_ = approach(eyeBase_, targetPose.eyeOpen, 1.0f / 12.0f);
  if (blinkPhase_ > 0) {
    --blinkPhase_;
  } else if (frame_ >= blinkAt_) {
    blinkPhase_ = 9;
    blinkAt_ = frame_ + 150 + fxWang(frame_) % 210;
  }
  float blinkF;
  switch (blinkPhase_) {
    case 9: blinkF = 2.0f / 3.0f; break;
    case 8: blinkF = 1.0f / 3.0f; break;
    case 7:
    case 6: blinkF = 0.0f; break;
    case 5: blinkF = 0.2f; break;
    case 4: blinkF = 0.4f; break;
    case 3: blinkF = 0.6f; break;
    case 2: blinkF = 0.8f; break;
    case 1: blinkF = 0.9f; break;
    default: blinkF = 1.0f; break;
  }
  pose_.eyeOpen = eyeBase_ * blinkF;
}

}  // namespace echoform
