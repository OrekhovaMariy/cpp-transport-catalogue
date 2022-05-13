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
    std::ostream& operator<<(std::ostream&, const Stop*);
    std::ostream& operator<<(std::ostream& os, const Bus* route);

    std::ostream& ProcessRequests(std::ostream&, TransportCatalogue&, std::istream&);   
    std::ostream& ExecuteRequest(std::ostream&, TransportCatalogue&, RequestQuery&);    

}
