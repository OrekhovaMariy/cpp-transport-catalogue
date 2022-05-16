#pragma once
#include "geo.h"           
#include <deque>
#include <string>
#include <string_view>
#include <ostream>         
#include <sstream>         
#include <iomanip>         
#include <unordered_set>
#include <unordered_map>
#include <algorithm>       
#include <utility>         
#include <cctype>          
#include <functional>      

namespace transport_catalogue
{
    enum class InputQueryType
    {
        AddStop,
        AddRoute,
        AddStopsDistance,
    };

    struct InputQuery
    {
        InputQueryType inputtype_{};
        std::string query_ {};
    };

    struct Stop
    {
    public:
        std::string name;
        Coordinates coords{ 0.0, 0.0 };
    };

    struct StopInfo
    {
        std::string_view name_{};
        std::vector<std::string> bus_number_{};
        bool absent_ = false;
    };

    struct Bus
    {
        std::string bus_number;
        std::vector<const Stop*> stops;
    };

    struct BusInfo
    {
        std::string_view bus_number_;
        size_t stops_count_ = 0;
        size_t unique_stops_ = 0;
        double geo_route_length_ = 0.0;
        size_t meters_route_length_ = 0;
        double curvature_ = 0.0;
    };

    class PointersHasher
    {
    public:
        std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_of_pointers) const noexcept
        {
            return hasher_(static_cast<const void*>(pair_of_pointers.first)) * 41 + hasher_(static_cast<const void*>(pair_of_pointers.second));
        }

        std::size_t operator()(const Stop* stop) const noexcept
        {
            return hasher_(static_cast<const void*>(stop)) * 41;
        }
    private:
        std::hash<const void*> hasher_;
    };

    class TransportCatalogue
    {
    public:
        TransportCatalogue();
        ~TransportCatalogue();

        void AddStop(const Stop& stop);
        void AddRoute(const Bus& route);

        BusInfo GetBusInfo(const std::string_view route);
        StopInfo GetStopInfo(std::string_view stop_name);

        void SetDistance(const Stop* stop_from, const Stop* stop_to, size_t dist);
        size_t GetDistance(const Stop* stop_from, const Stop* stop_to);
        size_t GetDistanceDirectly(const Stop* stop_from, const Stop* stop_to);

        const Stop* GetStopByName(std::string_view stop_name);
        Bus* GetRouteByName(std::string_view bus_name);
        
    private:
        std::deque<Stop> all_stops_;
        std::unordered_map<std::string_view, const Stop*> all_stops_map_;
        std::deque<Bus> all_buses_;
        std::unordered_map<std::string_view, Bus*> all_buses_map_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, size_t, PointersHasher> distances_;
        std::unordered_map<const Stop*, std::unordered_set<Bus*>, PointersHasher> stop_to_bus_map_;

        std::string_view GetStopName(const Stop* stop_ptr);
        std::string_view GetStopName(const Stop stop);

        std::string_view GetBusName(const Bus* route_ptr);
        std::string_view GetBusName(const Bus route);
    };
}
