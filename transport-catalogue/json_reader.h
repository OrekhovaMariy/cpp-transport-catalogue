#pragma once

#include "transport_catalogue.h"
#include "json.h"

#include <iostream>

namespace json_pro {
	void LoadJSON(transport_db::TransportCatalogue& transport_catalogue, std::istream& input);
	void FillCatalog(transport_db::TransportCatalogue& transport_catalogue, json::Document& doc);
	void PrintAnswer(transport_db::TransportCatalogue& transport_catalogue, json::Document& doc, std::string result_map_render);
}
