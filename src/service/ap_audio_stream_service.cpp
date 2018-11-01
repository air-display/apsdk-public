#include <functional>
#include <utils/logger.h>
#include <crypto/ap_crypto.h>
#include "ap_audio_stream_service.h"

namespace aps { namespace service { 
    ap_audio_stream_service::ap_audio_stream_service(aps::ap_crypto& crypto)
        : crypto_(crypto)
    {
        data_service_.bind_recv_handler(
            std::bind(
                &ap_audio_stream_service::data_handler, this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3));

        control_service_.bind_recv_handler(
            std::bind(
                &ap_audio_stream_service::control_handler, this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3));
    }

    ap_audio_stream_service::~ap_audio_stream_service()
    {

    }

    uint16_t ap_audio_stream_service::data_port() const
    {
        return data_service_.port();
    }

    uint16_t ap_audio_stream_service::control_port() const
    {
        return control_service_.port();
    }

    bool ap_audio_stream_service::start()
    {
        if (!data_service_.start())
            return false;

        if (!control_service_.start())
        {
            data_service_.stop();
            return false;
        }

        return true;
    }

    void ap_audio_stream_service::stop()
    {
        control_service_.stop();
        data_service_.stop();
    }

    void ap_audio_stream_service::data_handler(
        aps::network::udp_endpoint_ptr remote_endpoint, 
        const asio::error_code& e, 
        std::size_t bytes_transferred)
    {
        LOGI() << "ap_audio_stream_service::data_handler, " << bytes_transferred;

        data_service_.post_recv_from();
    }

    void ap_audio_stream_service::control_handler(
        aps::network::udp_endpoint_ptr remote_endpoint, 
        const asio::error_code& e, 
        std::size_t bytes_transferred)
    {
        LOGI() << "ap_audio_stream_service::control_handler, " << bytes_transferred;

        control_service_.post_recv_from();
    }
} }