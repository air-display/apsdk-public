#include <memory>
#include "service/ap_airplay_service.h"
#include "ap_config.h"
#include "ap_server.h"
#include "mdns/net_service.h"

using namespace aps::service;
using namespace aps::network;

namespace aps {
    class ap_server::implementation
    {
    public:
        implementation(const ap_config& config)
            : config_(config)
            , airplay_net_service_("_airplay._tcp")
            , raop_net_service_("_raop._tcp")
            , airplay_tcp_service_(0)
        {
            airplay_net_service_.add_txt_record(
                "deviceId", config_.deviceID());
            airplay_net_service_.add_txt_record(
                "features", config_.features_hex_string());
            airplay_net_service_.add_txt_record(
                "srcvers", config_.sourceVersion());
            airplay_net_service_.add_txt_record(
                "model", config_.model());
        }

        ~implementation() {}

        bool start()
        {
            if (airplay_tcp_service_)
                return true;

            airplay_tcp_service_ = std::make_shared<ap_airplay_service>(config_);
            if (!airplay_tcp_service_)
                return false;

            if (!airplay_tcp_service_->start())
            {
                airplay_tcp_service_.reset();
                return false;
            }

            if (!airplay_net_service_.publish(
                config_.name(), 
                airplay_tcp_service_->port()))
            {
                airplay_tcp_service_->stop();
                airplay_tcp_service_.reset();
                return false;
            }

            return true;
        }

        void stop()
        {
            airplay_net_service_.suppress();
            
            if (airplay_tcp_service_)
            {
                airplay_tcp_service_->stop();
                airplay_tcp_service_.reset();
            }
        }

    private:
        ap_config config_;

        net_service airplay_net_service_;

        net_service raop_net_service_;
      
        tcp_service_ptr airplay_tcp_service_;
    };

    ap_server::ap_server(const aps::ap_config& config)
        : impl_(new implementation(config))
    {
    }

    ap_server::~ap_server()
    {
        impl_.reset();
    }

    bool aps::ap_server::start()
    {
        return impl_->start();
    }

    void ap_server::stop()
    {
        impl_->stop();
    }
}
