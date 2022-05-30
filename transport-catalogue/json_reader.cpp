#include "json_reader.h"
#include "domain.h"
#include "map_renderer.h"

#include <sstream>
#include <optional>
#include <cstdint>
#include <utility>
#include <vector>
#include <string>

namespace json_pro
{
    void LoadJSON(transport_db::TransportCatalogue& t_c, std::istream& input) {
        json::Document doc = json::Load(input);
        FillCatalogBusAndStop(t_c, doc);
        auto& js = doc.GetRoot().AsMap().at("render_settings").AsMap();
        renderer::MapRenderer ren(js, t_c);
        auto str = ren.DocumentMapToPrint();
        PrintAnswer(t_c, doc, str);
    }

    void FillCatalogBusAndStop(transport_db::TransportCatalogue& t_c, json::Document& doc) {
        if (doc.GetRoot().AsMap().count("base_requests")) {
            for (const auto& node_map : doc.GetRoot().AsMap().at("base_requests").AsArray()) {
                if (node_map.AsMap().at("type").AsString() == "Stop") {
                    domain::Stop st;
                    st.name = node_map.AsMap().at("name").AsString();
                    st.coords = { node_map.AsMap().at("latitude").AsDouble(),  node_map.AsMap().at("longitude").AsDouble() };
                    t_c.AddStop(st);
                }
                else if (node_map.AsMap().at("type").AsString() == "Bus") {
                    domain::Bus bs;
                    bs.bus_number = node_map.AsMap().at("name").AsString();
                    bs.is_roundtrip = node_map.AsMap().at("is_roundtrip").AsBool();
                    t_c.AddRoute(bs);
                }
            }
        }
        FillCatalogBusInfoandStopInfo(t_c, doc);
    }

    void FillCatalogBusInfoandStopInfo(transport_db::TransportCatalogue& t_c, json::Document& doc) {
            for (const auto& node_map : doc.GetRoot().AsMap().at("base_requests").AsArray()) {
                if (node_map.AsMap().at("type").AsString() == "Stop") {
                    for (const auto& [key, val] : node_map.AsMap().at("road_distances").AsMap()) {
                        t_c.SetDistance(t_c.GetStopByName(node_map.AsMap().at("name").AsString()), t_c.GetStopByName(key), val.AsInt());
                    }
                }
                else if (node_map.AsMap().at("type").AsString() == "Bus") {
                    auto b = node_map.AsMap().at("name").AsString();
                    for (size_t i = 0; i < node_map.AsMap().at("stops").AsArray().size(); ++i) {
                        t_c.GetRouteByName(b)->stops.push_back(t_c.GetStopByName(node_map.AsMap().at("stops").AsArray()[i].AsString()));
                    }
                    t_c.AddStopToBusMap(b);
                    int j = node_map.AsMap().at("stops").AsArray().size();
                        if (node_map.AsMap().at("is_roundtrip").AsBool() == false && j>=2) {
                            for (j = j-2 ; j >= 0; ) {
                                t_c.GetRouteByName(b)->stops.push_back(t_c.GetStopByName(node_map.AsMap().at("stops").AsArray()[j].AsString()));
                                --j;
                            }
                        }
                    }
                }
        }

    void PrintAnswer(transport_db::TransportCatalogue& t_c, json::Document& doc, std::string result_map_render) {
        using namespace std::literals;
        json::Array arr;
        if (doc.GetRoot().AsMap().count("stat_requests")) {
            for (const auto& node_map : doc.GetRoot().AsMap().at("stat_requests").AsArray()) {
                int id_q = node_map.AsMap().at("id").AsInt();
                if (node_map.AsMap().at("type").AsString()[0] == 'B') {
                    arr.emplace_back(PrintBus(t_c, node_map, id_q));
                }
                if (node_map.AsMap().at("type").AsString()[0] == 'S') {
                    arr.emplace_back(PrintStop(t_c, node_map, id_q));

                }
                if (node_map.AsMap().at("type").AsString()[0] == 'M') {
                    arr.emplace_back(PrintVisual(result_map_render, id_q));
                }
                
            }
        }
        json::Print(json::Document(arr), std::cout);
    }

    json::Dict PrintVisual(std::string result_map_render, int id) {
        return json::Dict{
                            {"map", json::Node(result_map_render)},
                            {"request_id", id},
            };
    }

    json::Dict PrintBus(transport_db::TransportCatalogue& t_c, const json::Node& node_map, int id) {
        using namespace std::literals;
                    std::string tmp = node_map.AsMap().at("name").AsString();
                    if (t_c.GetRouteByName(tmp) != nullptr) {
                        return json::Dict{
                            {"curvature", json::Node(t_c.GetBusInfo(tmp).curvature_)},
                            {"request_id", id},
                            {"route_length", json::Node(t_c.GetBusInfo(tmp).meters_route_length_)},
                            {"stop_count", json::Node(t_c.GetBusInfo(tmp).stops_count_)},
                            {"unique_stop_count", json::Node(t_c.GetBusInfo(tmp).unique_stops_)},
                            };
                    }
                    else {
                        return json::Dict{
                            {"request_id", id},
                            {"error_message", "not found"s}, };
                    }
                }

    json::Dict PrintStop(transport_db::TransportCatalogue& t_c, const json::Node& node_map, int id) {
        using namespace std::literals;
                    std::string tmp = node_map.AsMap().at("name").AsString();
                    if (t_c.GetStopByName(tmp) != nullptr) {
                        json::Array arr_bus;
                        for (const auto& elem : t_c.GetStopInfo(tmp).bus_number_) {
                            arr_bus.emplace_back(elem);
                        }
                        return json::Dict{
                            {"buses", arr_bus},
                            {"request_id", id},
                            };
                    }
                    else {
                        return json::Dict{
                            {"request_id", id},
                            {"error_message", "not found"s}, };

                    }
                }

            }

