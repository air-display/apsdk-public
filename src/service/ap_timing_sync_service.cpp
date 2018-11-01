#include <utils/utils.h>
#include "ap_timing_sync_service.h"

using namespace std::chrono;
using namespace aps::network;

aps::service::ap_timing_sync_service::ap_timing_sync_service(
    const std::string& server_address, 
    const uint16_t server_port, 
    const uint16_t port /*= 0*/)
    : io_context_()
    , io_work_(io_context_)
    , udp_socket_(io_context_)
    , local_endpoint_(asio::ip::udp::v6(), port)
    , remote_endpoint_(asio::ip::address::from_string(server_address), server_port)
    , loop_thread_(0)
{
}

aps::service::ap_timing_sync_service::~ap_timing_sync_service()
{
}

const uint16_t aps::service::ap_timing_sync_service::port() const
{
    return local_endpoint_.port();
}

bool aps::service::ap_timing_sync_service::open()
{
    // Create the work thread
    loop_thread_ = std::make_shared<asio::thread>(
        std::bind<std::size_t(asio::io_service::*)()>(
            &asio::io_context::run, &io_context_));

    if (!loop_thread_)
        return false;

    // Create the acceptor
    udp_socket_.open(local_endpoint_.protocol());
    udp_socket_.set_option(asio::ip::v6_only(false));
    udp_socket_.bind(local_endpoint_);
    local_endpoint_ = udp_socket_.local_endpoint();

    return true;
}

void aps::service::ap_timing_sync_service::close()
{
    udp_socket_.close();

    io_context_.stop();

    if (loop_thread_)
    {
        loop_thread_->join();
        loop_thread_.reset();
    }
}

void aps::service::ap_timing_sync_service::post_send_query()
{
    query_packet_.header.marker = 1;
    query_packet_.header.payload_type = timing_query;
    query_packet_.original_timestamp = htonll(0);
    query_packet_.receive_timestamp = htonll(0);
    query_packet_.transmit_timestamp = htonll(get_ntp_timestamp());

    udp_socket_.async_send_to(
        asio::buffer(&query_packet_, sizeof(query_packet_)),
        remote_endpoint_,
        std::bind(
            &ap_timing_sync_service::on_query_send,
            this,
            std::placeholders::_1,
            std::placeholders::_2));

    post_recv_reply();
}

void aps::service::ap_timing_sync_service::on_query_send(const asio::error_code& e, std::size_t bytes_transferred)
{
    if (e)
        LOGE() << "Failed to send timing query: " << e.message();
    else
        LOGI() << "Timing query packet sent successfully";
}

void aps::service::ap_timing_sync_service::post_recv_reply()
{
    udp_socket_.async_receive_from(
        asio::buffer(&reply_packet_, sizeof(query_packet_)),
        remote_endpoint_,
        std::bind(
            &ap_timing_sync_service::on_reply_recv,
            this,
            std::placeholders::_1,
            std::placeholders::_2));
}

void aps::service::ap_timing_sync_service::on_reply_recv(const asio::error_code& e, std::size_t bytes_transferred)
{
    if (e)
        LOGE() << "Failed to send timing query: " << e.message();
    else
    {
        reply_packet_.header.sequence = ntohs(reply_packet_.header.sequence);
        reply_packet_.header.timestamp = ntohl(reply_packet_.header.timestamp);
        reply_packet_.original_timestamp = ntohll(reply_packet_.original_timestamp);
        reply_packet_.receive_timestamp = ntohll(reply_packet_.receive_timestamp);
        reply_packet_.transmit_timestamp = ntohll(reply_packet_.transmit_timestamp);
        LOGI() << "Timing reply packet received successfully";
    }
}
