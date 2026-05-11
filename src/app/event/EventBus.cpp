#include "app/event/EventBus.h"

bool EventBus::subscribe(EventType type, Handler h, void* userdata) {
  if (subCount_ >= kMaxSubs || h == nullptr) return false;
  Subscription &slot = subs_[subCount_++];
  slot.type = type;
  slot.h = h;
  slot.userdata = userdata;
  return true;
}

void EventBus::publish(const Event& ev) {
  for (size_t i = 0; i < subCount_; ++i) {
    if (subs_[i].type == ev.type && subs_[i].h) {
      subs_[i].h(ev, subs_[i].userdata);
    }
  }
}
