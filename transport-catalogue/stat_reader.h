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

    template<typename T>
    void Read(T& is, TransportCatalogue& tc)
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
            RequestQuery query;
            query.requery_ = tmp.second;
            if (tmp.first == "Stop"sv)
            {
                query.outputtype_ = RequestQueryType::GetBusesForStop;
                tc.request_data_.push_back({ query.outputtype_, query.requery_ });
            }
            if (tmp.first == "Bus"sv) {
                query.outputtype_ = RequestQueryType::GetRouteByName;
                tc.request_data_.push_back({ query.outputtype_, query.requery_ });
            }
        }
    }

    template<typename T>
    void Write(T& os, TransportCatalogue& tc)
    {
        auto req = tc.request_data_;
        for (const auto& data : req)
        {
            const auto info = data;
            if (info.outputtype_ == RequestQueryType::GetRouteByName)
            {
                const auto bus_info = tc.GetBusInfo(info.requery_);
                os << PrintBus(bus_info);
            }
            if (info.outputtype_ == RequestQueryType::GetBusesForStop)
            {
                const auto stop_info = tc.GetStopInfo(info.requery_);
                os << PrintStop(stop_info);
            }
        }
        req.clear();
    }
}
