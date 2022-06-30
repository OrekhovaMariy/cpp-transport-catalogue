#include "transport_router.h"

namespace transport_router {

    void TransportRouter::SetWaitTime(int wait_time) {
        wait_time_ = wait_time;
    }

    void TransportRouter::SetVelocity(double velocity) {
        velocity_ = velocity;
    }

    int TransportRouter::GetWaitTime() const {
        return wait_time_;
    }
}
