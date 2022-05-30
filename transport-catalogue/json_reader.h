#pragma once

#include "transport_catalogue.h"
#include "json.h"

#include <iostream>

namespace json_pro {
	void LoadJSON(transport_db::TransportCatalogue& transport_catalogue, std::istream& input);

	void FillCatalogBusAndStop(transport_db::TransportCatalogue& transport_catalogue, json::Document& doc);
	void FillCatalogBusInfoandStopInfo(transport_db::TransportCatalogue& transport_catalogue, json::Document& doc);

	void PrintAnswer(transport_db::TransportCatalogue& transport_catalogue, json::Document& doc, std::string result_map_render);
	json::Dict PrintStop(transport_db::TransportCatalogue& transport_catalogue, const json::Node& node_map, int id);
	json::Dict PrintBus(transport_db::TransportCatalogue& transport_catalogue, const json::Node& node_map, int id);
	json::Dict PrintVisual(std::string result_map_render, int id);
}
