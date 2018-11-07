#include <functional>
#include <utils/logger.h>
#include <crypto/ap_crypto.h>
#include "ap_audio_stream_service.h"

using namespace aps::network;

namespace aps { namespace service { 
    audio_udp_service::audio_udp_service(const std::string& name)
        : aps::network::udp_service_base(name)
        , recv_buf_(aps::network::RTP_PACKET_MAX_LEN, 0)
    {
    }

    audio_udp_service::~audio_udp_service()
    {
    }

    bool audio_udp_service::open()
    {
        if (aps::network::udp_service_base::open())
        {
            post_recv_from(recv_buf_, remote_endpoint_);
            return true;
        }
        return false;
    }

    void audio_udp_service::on_recv_from(
        asio::ip::udp::endpoint& remote_endpoint, 
        const asio::error_code& e, 
        std::size_t bytes_transferred)
    {
        if (recv_from_handler_)
            recv_from_handler_(recv_buf_.data(), e, bytes_transferred);

        if (e)
            LOGE() << "Failed to recv data: " << e.message();
        else
            post_recv_from(recv_buf_, remote_endpoint_);
    }

    ap_audio_stream_service::ap_audio_stream_service(aps::ap_crypto& crypto)
        : crypto_(crypto)
        , data_service_("audio_data_service")
        , control_service_("audio_control_service")
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
        if (!data_service_.open())
            return false;

        if (!control_service_.open())
        {
            data_service_.close();
            return false;
        }

        return true;
    }

    void ap_audio_stream_service::stop()
    {
        control_service_.close();
        data_service_.close();
    }

    void ap_audio_stream_service::data_handler(
        const uint8_t* buf,
        const asio::error_code& e,
        std::size_t bytes_transferred)
    {
        if (!e)
        {
            if (bytes_transferred < aps::network::RTP_PACKET_MIN_LEN)
            {
                LOGE() << "Packet too small: " << bytes_transferred;
                return;
            }

            rtp_packet_header_t* header = (rtp_packet_header_t*)buf;
            if (header->payload_type != audio_data)
            {
                LOGE() << "Invalid audio data packet: " << bytes_transferred;
                return;
            }

            audio_data_packet((rtp_audio_data_packet_t*)header, bytes_transferred);
        }
    }

    void ap_audio_stream_service::audio_data_packet(
        aps::network::rtp_audio_data_packet_t* packet,
        size_t length)
    {
        LOGD() << "audio DATA packet: " << length;


    }

    void ap_audio_stream_service::control_handler(
        const uint8_t* buf,
        const asio::error_code& e,
        std::size_t bytes_transferred)
    {
        if (!e)
        {
            if (bytes_transferred < aps::network::RTP_PACKET_MIN_LEN)
            {
                LOGE() << "Packet too small: " << bytes_transferred;
                return;
            }

            LOGV() << "ap_audio_stream_service::control_handler, " << bytes_transferred;

            rtp_packet_header_t* header = (rtp_packet_header_t*)buf;
            if (header->payload_type == ctrl_timing_sync
                && bytes_transferred == sizeof(rtp_control_sync_packet_t))
                control_sync_packet((rtp_control_sync_packet_t*)header);
            else if (header->payload_type == ctrl_retransmit_request
                && bytes_transferred == sizeof(rtp_control_retransmit_packet_t))
                control_retransmit_packet((rtp_control_retransmit_packet_t*)header);
            else
                LOGE() << "Unknown RTP control packet, type: " << header->payload_type
                << " size: " << bytes_transferred;
        }
    }

    void ap_audio_stream_service::control_sync_packet(
        aps::network::rtp_control_sync_packet_t* packet)
    {
        LOGD() << "audio CONTROL SYNC packet";


    }

    void ap_audio_stream_service::control_retransmit_packet(
        aps::network::rtp_control_retransmit_packet_t* packet)
    {
        LOGD() << "audio CONTROL RETRANSMIT packet";


    }

} }