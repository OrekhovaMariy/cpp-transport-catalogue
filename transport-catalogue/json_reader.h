#pragma once

#include "transport_catalogue.h"
#include "json_builder.h"
#include "router.h"
#include "transport_router.h"
#include "router.h"

#include <iostream>

namespace json_pro {
	void LoadJSON(transport_db::TransportCatalogue& transport_catalogue, std::istream& input);

	void FillCatalogBusAndStop(transport_db::TransportCatalogue& transport_catalogue, json::Document& doc);
	void FillCatalogBusInfoandStopInfo(transport_db::TransportCatalogue& transport_catalogue, json::Document& doc);
	void FillCatalogGrafOfRoute(const json::Dict& doc, transport_db::TransportCatalogue& transport_catalogue, graph::DirectedWeightedGraph<double>& graph, transport_router::TransportRouter& router);

	void PrintAnswer(transport_db::TransportCatalogue& transport_catalogue, json::Document& doc, std::string result_map_render, graph::Router<double> transport_router, transport_router::TransportRouter router);
	json::Dict PrintStop(transport_db::TransportCatalogue& transport_catalogue, const json::Node& node_map, int id);
	json::Dict PrintBus(transport_db::TransportCatalogue& transport_catalogue, const json::Node& node_map, int id);
	json::Dict PrintGraf(transport_db::TransportCatalogue& transport_catalogue, const json::Node& node_map, int id, graph::Router<double> transport_router, transport_router::TransportRouter router);
	json::Dict PrintVisual(std::string result_map_render, int id);
}
