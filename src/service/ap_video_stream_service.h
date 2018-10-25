#pragma once
#include <vector>
#include <asio.hpp>
#include "../utils/packing.h"
#include "../network/tcp_service.h"
#include "../ap_crypto.h"
#include "ap_video_stream_service_details.h"

using namespace aps;
using namespace aps::network;

namespace aps { namespace service { 
    class ap_video_stream_session
        : public aps::network::tcp_session_base
        , public std::enable_shared_from_this<ap_video_stream_session>
    {
    public:
        ap_video_stream_session(asio::io_context& io_ctx, ap_crypto& crypto);

        ~ap_video_stream_session();

        virtual void start() override;

    protected:

        void post_receive_packet_header();

        void on_packet_header_received(const asio::error_code& e, std::size_t bytes_transferred);

        void post_receive_packet_payload();

        void on_packet_payload_received(const asio::error_code& e, std::size_t bytes_transferred);

        void process_packet();

        void error_handler(const asio::error_code& e);

    private:
        details::stream_packet packet_;

        ap_crypto& crypto_;
    };

    class ap_video_stream_service
        : public tcp_service_base
    {
    public:
        explicit ap_video_stream_service(ap_crypto& crypto, uint16_t port= 0);
        ~ap_video_stream_service();

    protected:
        virtual tcp_session_ptr prepare_new_session() override;

    private:
        ap_crypto& crypto_;
    };
} }