#pragma once
#include "transport_catalogue.h"    
#include "input_reader.h"           
#include <utility>          
#include <string>
#include <string_view>      
#include <sstream>          
#include <iomanip>         
#include <istream>          
#include <iostream>         

namespace transport_catalogue::stat_reader
{
    std::string PrintBus(const transport_catalogue::BusInfo& bus);
    std::string PrintStop(const transport_catalogue::StopInfo& stop);

   
    template<typename T, typename U>
    void ProcessStat(T& is, U& os, TransportCatalogue& tc)
    {
        size_t request_num = 0;
        is >> request_num;
        is.ignore();

        for (size_t i = 0; i < request_num; ++i)
        {
            using namespace std::literals;
            std::string line;
            std::getline(is, line);
            auto tmp = detail::Split(line, ' ');
            tmp.first = detail::TrimString(tmp.first);
            tmp.second = detail::TrimString(tmp.second);
            if (tmp.first == "Stop"sv)
            {
                const auto stop_info = tc.GetStopInfo(tmp.second);
                os << PrintStop(stop_info);
            }
            if (tmp.first == "Bus"sv) {
                const auto bus_info = tc.GetBusInfo(tmp.second);
                os << PrintBus(bus_info);
            }
        }
    }
}
