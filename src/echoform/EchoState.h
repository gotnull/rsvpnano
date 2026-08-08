#pragma once

#include <stdint.h>

#include "echoform/EchoFace.h"

// The avatar state controller - C++ port of rusty-nail's
// crates/fcecho/src/state.rs Controller: coherence per state (idle breath,
// listening, the thinking oscillator, speaking with speech-level lift,
// error collapse), expression pose easing with speech-driven mouth, the
// damped-spring head pose with layered value noise and breathing, and the
// blink scheduler. step() exactly once per rendered frame.
namespace echoform {

enum class AvatarState : uint8_t { Idle, Listening, Processing, Speaking, Error };

enum class EchoEvent : uint8_t {
  PushToTalkPressed,
  PushToTalkReleased,
  VoiceStarted,
  VoiceEnded,
  AudioBlockReady,
  RequestStarted,
  TranscriptPartial,
  TranscriptFinal,
  ProviderConnected,
  ProviderDelta,
  SpeechAudioStarted,
  SpeechAudioEnded,
  RequestCompleted,
  Cancelled,
  Timeout,
  NetworkError,
  ProtocolError,
};

enum class Expression : uint8_t {
  Neutral,
  Listening,
  Thinking,
  Curious,
  Speaking,
  Amused,
  Concerned,
  Confused,
  Error,
};

class EchoController {
 public:
  void handle(EchoEvent event);
  // speech* come from the features of the PCM actually queued for playback.
  void step(float speechRms, float speechLow, float speechMid);

  AvatarState state() const { return state_; }
  float coherence() const { return coherence_; }
  const ExpressionPose &pose() const { return pose_; }
  const HeadPose &headPose() const { return head_; }

 private:
  void enter(AvatarState state, Expression expression);
  void pump(float amount);
  float breath() const;

  AvatarState state_ = AvatarState::Idle;
  float coherence_ = 0.10f;
  ExpressionPose pose_;
  Expression expression_ = Expression::Neutral;
  uint16_t oscPhase_ = 0;
  uint16_t oscStep_ = 300;
  float activity_ = 0.0f;
  uint32_t hold_ = 0;
  float instability_ = 0.0f;
  HeadPose head_;
  float yawV_ = 0.0f, pitchV_ = 0.0f, rollV_ = 0.0f, shiftV_ = 0.0f;
  float eyeBase_ = 0.5f;
  uint32_t blinkPhase_ = 0;
  uint32_t blinkAt_ = 200;
  uint32_t frame_ = 0;
};

}  // namespace echoform
