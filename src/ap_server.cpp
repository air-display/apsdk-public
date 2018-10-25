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

            rtsp_service_ = std::make_shared<ap_airplay_service>(8888);

            return true;
        }

        bool start()
        {
            if (rtsp_service_ && rtsp_service_->start())
                return true;

            return false;
        }

        void stop()
        {
            if (rtsp_service_)
                rtsp_service_->stop();
        }

        void uninitialize()
        {
            rtsp_service_.reset();
        }

    private:
        tcp_service_ptr rtsp_service_;
    };

    ap_server::ap_server()
    {
        impl_ = std::make_shared<detail>();
    }

    ap_server::~ap_server()
    {
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
        return impl_->stop();
    }

    void ap_server::uninitialize()
    {
        return impl_->uninitialize();
    }
}
