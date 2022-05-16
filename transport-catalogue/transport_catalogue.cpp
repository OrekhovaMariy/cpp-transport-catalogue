#include "transport_catalogue.h"


namespace transport_catalogue
{
    TransportCatalogue::TransportCatalogue()
    {}

    TransportCatalogue::~TransportCatalogue()
    {}

    void TransportCatalogue::AddStop(const Stop& stop)
    {
        if (all_stops_map_.count(GetStopName(&stop)) == 0)
        {
            auto& ref = all_stops_.emplace_back(std::move(stop));
            all_stops_map_.insert({ std::string_view(ref.name), &ref });
        }
    }

    void TransportCatalogue::AddRoute(const Bus& route)
    {
        if (all_buses_map_.count(route.bus_number) == 0)
        {
            auto& ref = all_buses_.emplace_back(std::move(route));
            all_buses_map_.insert({ std::string_view(ref.bus_number), &ref });
            for (size_t i = 0; i < ref.stops.size(); ++i) {
                stop_to_bus_map_[ref.stops[i]].insert(&ref);
            }
        }
    }

    BusInfo TransportCatalogue::GetBusInfo(const std::string_view route)
    {
        if (all_buses_map_.count(route))
        {
            auto& Bus = all_buses_map_[route];
            std::vector<const Stop*> tmp = Bus->stops;
            size_t stops_num = static_cast<int>(Bus->stops.size());
            size_t uniq = 0U;
            std::sort(tmp.begin(), tmp.end());
            auto last = std::unique(tmp.begin(), tmp.end());
            uniq = (last != tmp.end() ? std::distance(tmp.begin(), last) : tmp.size());
            double geo_length = 0L;
            size_t meters_length = 0U;
            double curv = 0L;
            if (stops_num > 1)
            {
                for (int i = 0; i < stops_num - 1; ++i)
                {
                    geo_length += ComputeDistance(Bus->stops[i]->coords, Bus->stops[i + 1]->coords);
                    meters_length += GetDistance(Bus->stops[i], Bus->stops[i + 1]);
                }
                curv = meters_length / geo_length;
            }
            return { std::string_view(route), stops_num, uniq, geo_length, meters_length, curv };
        }
        else {
            return { std::string_view(route), 0, 0, 0.0, 0, 0.0 };
        }
    }

    void TransportCatalogue::SetDistance(const Stop* stop_from, const Stop* stop_to, size_t dist)
    {
        if (stop_from != nullptr && stop_to != nullptr)
        {
            distances_.insert({ { stop_from, stop_to }, dist });
        }
    }

    size_t TransportCatalogue::GetDistance(const Stop* stop_from, const Stop* stop_to)
    {
        size_t result = GetDistanceDirectly(stop_from, stop_to);
        return (result > 0 ? result : GetDistanceDirectly(stop_to, stop_from));
    }

    size_t TransportCatalogue::GetDistanceDirectly(const Stop* stop_from, const Stop* stop_to)
    {
        if (distances_.count({ stop_from, stop_to }) > 0)
        {
            return distances_.at({ stop_from, stop_to });
        }
        else
        {
            return 0U;
        }
    }

    std::string_view TransportCatalogue::GetStopName(const Stop* stop_ptr)
    {
        return std::string_view(stop_ptr->name);
    }

    std::string_view TransportCatalogue::GetStopName(const Stop stop)
    {
        return std::string_view(stop.name);
    }

    std::string_view TransportCatalogue::GetBusName(const Bus* route_ptr)
    {
        return std::string_view(route_ptr->bus_number);
    }

    std::string_view TransportCatalogue::GetBusName(const Bus route)
    {
        return std::string_view(route.bus_number);
    }

    const Stop* TransportCatalogue::GetStopByName(std::string_view stop_name)
    {
        if (all_stops_map_.count(stop_name) == 0)
        {
            return nullptr;
        }
        else
        {
            return all_stops_map_.at(stop_name);
        }
    }

    Bus* TransportCatalogue::GetRouteByName(std::string_view bus_name)
    {
        if (all_buses_map_.count(bus_name) == 0)
        {
            return nullptr;
        }
        else
        {
            return all_buses_map_.at(bus_name);
        }
    }

    StopInfo TransportCatalogue::GetStopInfo(std::string_view stop_name)
    {
        auto* result = GetStopByName(stop_name);
        StopInfo stop_info;
        stop_info.name_ = stop_name;
        if (result != nullptr)
        {
            if (stop_to_bus_map_.count(result)) {
                auto set_buses = stop_to_bus_map_.at(result);

                for (const auto& bus : set_buses)
                {
                    stop_info.name_ = stop_name;
                    stop_info.bus_number_.push_back(bus->bus_number);
                }
                std::sort(stop_info.bus_number_.begin(), stop_info.bus_number_.end());
                return stop_info;
            }
            return stop_info;
        }
        stop_info.absent_ = true;
        return stop_info;
    }
}
