#include "stat_reader.h"

namespace transport_catalogue::stat_reader
{
    std::string PrintBus(const transport_catalogue::BusInfo& bus)
    {
        std::stringstream stream;
        if (bus.stops_count_)
        {
            stream << "Bus " << bus.bus_number_ << ": " << bus.stops_count_ << " stops on route, "
                << bus.unique_stops_ << " unique stops, " << bus.meters_route_length_ << " route length"
                << ", " << std::setprecision(6) << bus.curvature_ << " curvature" << std::endl;
        }
        else
        {
            stream << "Bus " << bus.bus_number_ << ": "
                << "not found" << std::endl;
        }
        return stream.str();
    }

    std::string PrintStop(const transport_catalogue::StopInfo& stop)
    {
        std::stringstream os;
        if (!stop.absent_)
        {
            if (stop.bus_number_.size())
            {
                os << "Stop " << stop.name_ << ": "
                    << "buses ";

                for (auto& bus : stop.bus_number_)
                {
                    os << bus << " ";
                }
                os << std::endl;
            }
            else
            {
                os << "Stop " << stop.name_ << ": "
                    << "no buses" << std::endl;
            }
        }
        else
        {
            os << "Stop " << stop.name_ << ": "
                << "not found" << std::endl;
        }
        return os.str();
    }
}
