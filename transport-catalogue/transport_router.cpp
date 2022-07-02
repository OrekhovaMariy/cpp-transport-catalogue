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

    double TransportRouter::GetVelocity() const {
        return velocity_;
    }

    void TransportRouter::FillCatalogGraph(transport_db::TransportCatalogue& t_c, graph::DirectedWeightedGraph<double>& graph)
    {
        int bus_wait_time = this->GetWaitTime();
        double velocity = this->GetVelocity();
        for (const auto& bus : t_c.GetAllBuses()) {
            const auto& stops = bus.stops;
            size_t span = 0;
            double weight = bus_wait_time * 1.0;
            auto map = t_c.GetStopsFromTo();
            if (stops.size() > 1) {
                for (size_t i = 0; i < stops.size() - 1; ++i) {
                    span = 1;
                    weight = bus_wait_time * 1.0;
                    for (size_t j = i + 1; j < stops.size(); ++j) {
                        if (stops[i] != stops[j]) {
                            auto it = map.find({ stops[j - 1], stops[j] });
                            if (it == map.end()) {
                                it = map.find({ stops[j], stops[j - 1] });
                            }
                            auto k = it->second / velocity;
                            weight += k * (60.0 / 1000.0);
                            graph::Edge edge(stops[i]->edge_id, stops[j]->edge_id, span, bus.bus_number, weight);
                            graph.AddEdge(edge);
                            ++span;
                        }
                    }
                }
                if (!bus.is_roundtrip) {
                    for (size_t i = stops.size() - 1u; i > 0; --i) {
                        weight = bus_wait_time * 1.0;
                        span = 1;

                        for (size_t j = i; j > 0; --j) {
                            if (stops[i] != stops[j - 1]) {
                                auto it = map.find({ stops[j], stops[j - 1] });
                                if (it == map.end()) {
                                    it = map.find({ stops[j - 1], stops[j] });
                                }
                                auto k = it->second / velocity;
                                weight += k * (60.0 / 1000.0);
                                graph::Edge edge(stops[i]->edge_id, stops[j - 1]->edge_id, span, bus.bus_number, weight);
                                graph.AddEdge(edge);
                                ++span;
                            }
                        }
                    }
                }
            }
        }
    }
}
