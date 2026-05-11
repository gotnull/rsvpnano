#pragma once

#include <cstddef>

#include "app/event/Events.h"

// Minimal allocation-free pub/sub for embedded use. Bounded to 16 slots —
// register handlers once at boot and the bus never touches the heap.
//
// Delivery is synchronous: handlers run inline on publish(), so they must
// be quick and must not take any of the same locks they may be invoked
// under. For long-running reactions, post to a background task from the
// handler and return immediately.
class EventBus {
 public:
  using Handler = void(*)(const Event&, void* userdata);

  // Returns false if the subscription table is full. Lifetime of the
  // handler / userdata is caller-managed (typically a static App method
  // with `this` as userdata).
  bool subscribe(EventType type, Handler h, void* userdata);

  // Fan-out synchronously to every subscriber matching ev.type.
  void publish(const Event& ev);

 private:
  struct Subscription {
    EventType type = EventType::None;
    Handler h = nullptr;
    void* userdata = nullptr;
  };
  static constexpr size_t kMaxSubs = 16;
  Subscription subs_[kMaxSubs];
  size_t subCount_ = 0;
};
