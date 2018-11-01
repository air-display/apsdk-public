#pragma once
#include <memory>
#include <vector>
#include <asio.hpp>
#include <crypto/ap_crypto.h>
#include <network/rtp_types.h>
#include <utils/packing.h>

namespace aps { namespace service { 
    class ap_timing_sync_service
    {
    public:
        ap_timing_sync_service(const std::string& server_address, const uint16_t server_port, const uint16_t port = 0);
        
        ~ap_timing_sync_service();

        const uint16_t port() const;

        bool open();

        void close();

        void post_send_query();

    protected:

        void on_query_send(const asio::error_code& e, std::size_t bytes_transferred);

        void post_recv_reply();

        void on_reply_recv(const asio::error_code& e, std::size_t bytes_transferred);

    private:
        asio::io_context io_context_;
        asio::io_context::work io_work_;
        asio::ip::udp::socket udp_socket_;
        asio::ip::udp::endpoint local_endpoint_;
        asio::ip::udp::endpoint remote_endpoint_;
        std::shared_ptr<asio::thread> loop_thread_;

        aps::network::rtp_timming_sync_packet_t query_packet_;
        aps::network::rtp_timming_sync_packet_t reply_packet_;
    };

    typedef std::shared_ptr<ap_timing_sync_service> ap_timing_sync_service_ptr;
} }