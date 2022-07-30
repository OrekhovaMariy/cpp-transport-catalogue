#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <optional>

#include "domain.h"
#include "transport_catalogue.h"

#include "transport_catalogue.pb.h"
#include "map_renderer.pb.h"

namespace serialize {
	class Serialization {
	public:
		Serialization(transport_db::TransportCatalogue& transport_catalogue);

		void SetSetting(const std::filesystem::path& path_to_base);
		void Serialize(transport_db::TransportCatalogue& cat);
		void DeserializeCatalogue(transport_db::TransportCatalogue& cat);
		std::string InputMapRenderer();
		void SetMapRender(std::string map);
		void OutputRouterSetVelosity(std::map <std::string, double> router_settings_velosity);
		void OutputRouterSetTime(std::map <std::string, int> router_settings_time);
		double InputRouterSetVelosity();
		int InputRouterSetTime();

	private:
        proto_catalogue::Stop SaveStop(const domain::Stop& stop) const;
		proto_catalogue::DistanceFromTo SaveDistanceFromTo(domain::Stop* from, domain::Stop* to, size_t dist) const;
		proto_catalogue::Bus SaveBus(const domain::Bus& bus) const;

        void OutputStops();

		void OutputDistanceFromTo();

        void OutputBuses();

        void LoadStop(const proto_catalogue::Stop& stop);
		void LoadDistanceFromTo(const proto_catalogue::DistanceFromTo& distance_from_to);
    void LoadBus(const proto_catalogue::Bus& bus);

    void InputStops();

	void InputDistanceFromTo();

	void InputBuses();

		std::filesystem::path path_;
		transport_db::TransportCatalogue& transport_catalogue_;
		mutable proto_catalogue::TransportCatalogue base_;
	};

}
