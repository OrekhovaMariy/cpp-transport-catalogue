#include <cstdint>
#include <fstream>
#include <sstream>
#include <utility>
#include <variant>
#include <algorithm>
#include <map>

#include "serialization.h"
#include "transport_catalogue.pb.h"
#include "map_renderer.pb.h"
#include "geo.h"

namespace serialize {

    Serialization::Serialization(transport_db::TransportCatalogue& transport_catalogue)
        : transport_catalogue_(transport_catalogue) {}

    void Serialization::SetSetting(const std::filesystem::path& path_to_base) {
        path_ = path_to_base;
    }

    void Serialization::Serialize(transport_db::TransportCatalogue& cat)
    {
        std::ofstream out_file(path_, std::ios::binary);
        OutputStops();
        OutputDistanceFromTo();
        OutputBuses();
        base_.SerializeToOstream(&out_file);
    }

    void Serialization::DeserializeCatalogue(transport_db::TransportCatalogue& cat) {
        std::ifstream in_file(path_, std::ios::binary);
        base_.ParseFromIstream(&in_file);
        InputStops();
        InputDistanceFromTo();
        InputBuses();
    }

    proto_catalogue::Stop Serialization::SaveStop(const domain::Stop& stop) const {
        proto_catalogue::Stop tmp;
        tmp.set_id(stop.edge_id);
        tmp.set_stop_name(stop.name);
        tmp.mutable_coordinates()->set_lat(stop.coords.lat);
        tmp.mutable_coordinates()->set_lon(stop.coords.lng);
        return tmp;
    }
    
    proto_catalogue::DistanceFromTo Serialization::SaveDistanceFromTo(domain::Stop* from, domain::Stop* to, size_t distance) const {
        proto_catalogue::DistanceFromTo tmp;
        tmp.set_from(from->name);
        tmp.set_to(to->name);
        tmp.set_distance(distance);
        return tmp;
    }

    proto_catalogue::Bus Serialization::SaveBus(const domain::Bus& bus) const {
        proto_catalogue::Bus tmp;
        tmp.set_is_roundtrip(bus.is_roundtrip);
        tmp.set_bus_name(bus.bus_number);
        for (const auto& stop : bus.stops) {
            tmp.add_names_of_stops(stop->name);
        }
        return tmp;
    }

    void Serialization::OutputStops() {
        for (const auto& stop : transport_catalogue_.GetAllStops()) {
            *base_.add_stops() = std::move(SaveStop(stop));
        }
    }

    void Serialization::OutputDistanceFromTo() {
        for (const auto& [from_to, distance] : transport_catalogue_.GetStopsFromTo()) {
            *base_.add_distance_from_to() = std::move(SaveDistanceFromTo(from_to.first, from_to.second, distance));
        }
    }

    void Serialization::OutputBuses() {
        for (const auto& bus : transport_catalogue_.GetAllBuses()) {
            *base_.add_buses() = std::move(SaveBus(bus));
        }
    }

    void Serialization::OutputRouterSetVelosity(std::map <std::string, double> router_settings_velosity)
    {
        base_.mutable_router_set()->set_bus_velocity(router_settings_velosity.at("bus_velocity"));
    }

    void Serialization::OutputRouterSetTime(std::map <std::string, int> router_settings_time)
    {
        base_.mutable_router_set()->set_bus_wait_time(router_settings_time.at("bus_wait_time"));
    }

    void Serialization::SetMapRender(std::string map) {
        base_.mutable_map_ren()->set_str_of_result_map_render(map);
    }

    void Serialization::LoadStop(const proto_catalogue::Stop& stop) {
        geo::Coordinates coordinates;
        coordinates.lat = stop.coordinates().lat();
        coordinates.lng = stop.coordinates().lon();
        transport_catalogue_.AddStop(coordinates, stop.stop_name());
    }

    void Serialization::LoadDistanceFromTo(const proto_catalogue::DistanceFromTo& distance_from_to) {
        const auto& from = transport_catalogue_.GetStopByName(distance_from_to.from());
        const auto& to = transport_catalogue_.GetStopByName(distance_from_to.to());
        transport_catalogue_.SetDistance(from, to, distance_from_to.distance());
    }

    void Serialization::LoadBus(const proto_catalogue::Bus& bus) {
        domain::Bus tmp;
        tmp.is_roundtrip = bus.is_roundtrip();
        tmp.bus_number = bus.bus_name();
        for (int i = 0; i < bus.names_of_stops_size(); ++i) {
            tmp.stops.push_back(transport_catalogue_.GetStopByName(bus.names_of_stops(i)));
        }
        transport_catalogue_.AddRoute(tmp);
    }

    void Serialization::InputStops() {
        for (int i = 0; i < base_.stops_size(); ++i) {
            LoadStop(base_.stops(i));
        }
    }

    void Serialization::InputDistanceFromTo() {
        for (int i = 0; i < base_.distance_from_to_size(); ++i) {
            LoadDistanceFromTo(base_.distance_from_to(i));
        }
    }

    void Serialization::InputBuses() {
        for (int i = 0; i < base_.buses_size(); ++i) {
            LoadBus(base_.buses(i));
        }
    }

    std::string Serialization::InputMapRenderer() {
        return base_.map_ren().str_of_result_map_render();
    }

    double Serialization::InputRouterSetVelosity() {
        return base_.router_set().bus_velocity();
    }

   int Serialization::InputRouterSetTime() {
       return base_.router_set().bus_wait_time();
    }
}
