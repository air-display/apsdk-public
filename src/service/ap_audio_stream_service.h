#pragma once
#include <memory>
#include <vector>
#include <asio.hpp>
#include <utils/packing.h>
#include <network/udp_service.h>
#include <crypto/ap_crypto.h>
#include "ap_audio_stream_service_details.h"

using namespace aps::service::audio::details;

namespace aps { namespace service { 
    class ap_audio_stream_service
    {
    public:
        explicit ap_audio_stream_service(
            aps::ap_crypto& crypto);

        ~ap_audio_stream_service();

        uint16_t data_port() const;

        uint16_t control_port() const;

        uint16_t timing_port() const;

        bool start();

        void stop();

    protected:
        void data_handler(
            aps::network::udp_endpoint_ptr remote_endpoint,
            const asio::error_code& e,
            std::size_t bytes_transferred);

        void control_handler(
            aps::network::udp_endpoint_ptr remote_endpoint,
            const asio::error_code& e,
            std::size_t bytes_transferred);

    private:
        aps::ap_crypto& crypto_;

        aps::network::udp_service_base data_service_;

        aps::network::udp_service_base control_service_;
    };

    typedef std::shared_ptr<ap_audio_stream_service> ap_audio_stream_service_ptr;
} }