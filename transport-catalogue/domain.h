#pragma once

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

#include <string_view>
#include <vector>
#include <string>
#include <set>
#include "geo.h"

namespace domain {
    struct Stop
    {
    public:
        std::string name;
        geo::Coordinates coords{ 0.0, 0.0 };
    };

    struct Bus
    {
        std::string bus_number;
        std::vector<const Stop*> stops = {};
        bool is_roundtrip = false;
    };
    
} // namespace domain
