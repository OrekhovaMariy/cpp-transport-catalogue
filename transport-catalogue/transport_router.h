#pragma once

#include "router.h"


namespace transport_router {


    class TransportRouter {

    public:

        explicit TransportRouter() = default;

        explicit TransportRouter(int wait_time, double velocity)
            : wait_time_(wait_time)
            , velocity_(velocity) {
        }

        void SetWaitTime(int wait_time);

        void SetVelocity(double velocity);

        int GetWaitTime() const;

    private:
        int wait_time_ = 0;
        double velocity_ = 0.;
        graph::Router<double>* router_ = nullptr;
    };
}
