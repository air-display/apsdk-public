#include <memory>
#include "service/ap_airplay_service.h"
#include "ap_config.h"
#include "ap_server.h"

using namespace aps::service;
using namespace aps::network;

namespace aps {
    class ap_server::detail
    {
    public:
        detail() {}
        ~detail() {}

        bool initialize()
        {
            ap_config::get().init();

            airplay_service_ = std::make_shared<ap_airplay_service>(8888);

            return true;
        }

        bool start()
        {
            if (airplay_service_ && airplay_service_->start())
                return true;

            return false;
        }

        void stop()
        {
            if (airplay_service_)
                airplay_service_->stop();
        }

        void uninitialize()
        {
            airplay_service_.reset();
        }

    private:
        tcp_service_ptr airplay_service_;
    };

    ap_server::ap_server()
    {
        impl_ = std::unique_ptr<detail>(new detail());
    }

    ap_server::~ap_server()
    {
        impl_.reset();
    }

    bool ap_server::initialize()
    {
        return impl_->initialize();
    }

    bool aps::ap_server::start()
    {
        return impl_->start();
    }

    void ap_server::stop()
    {
        impl_->stop();
    }

    void ap_server::uninitialize()
    {
        impl_->uninitialize();
    }
}
