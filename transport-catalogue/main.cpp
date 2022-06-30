#include "transport_catalogue.h"
#include "json_reader.h"
#include <iostream>

int main() {
    transport_db::TransportCatalogue catalog;
    json_pro::LoadJSON(catalog, std::cin);
}

