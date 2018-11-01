#pragma once
#include <memory>
#include <asio.hpp>
#include <utils/logger.h>

namespace aps { namespace network {
    class udp_service
    {
    public:
        virtual ~udp_service() {};

        virtual const uint16_t port() const = 0;

        virtual bool start() = 0;;

        virtual void stop() = 0;

        virtual asio::io_context& io_context() = 0;
    };

    typedef std::shared_ptr<udp_service> udp_service_ptr;

    typedef std::shared_ptr<asio::ip::udp::endpoint> udp_endpoint_ptr;

    class udp_service_base : public udp_service
    {
    public:
        typedef std::function<void(udp_endpoint_ptr, const asio::error_code&, std::size_t)> recv_handler;

        udp_service_base(uint16_t port = 0)
            : io_context_()
            , io_work_(io_context_)
            , udp_socket_(io_context_)
            , local_endpoint_(asio::ip::udp::v6(), port)
            , loop_thread_(0)
            , recv_buffer_(2048, 0)
        {

        }

        ~udp_service_base()
        {
            stop();
        }

        virtual const uint16_t port() const override
        {
            return local_endpoint_.port();
        }

        void bind_recv_handler(recv_handler handler)
        {
            recv_handler_ = handler;
        }

        virtual bool start() override
        {
            // Create the work thread
            // For the RTSP service single worker thread is enough
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

            // Post the first recv from operation 
            post_recv_from();

            return true;
        }

        virtual void post_recv_from()
        {
            udp_endpoint_ptr remote_endpoint
                = std::make_shared<asio::ip::udp::endpoint>();
            udp_socket_.async_receive_from(
                asio::buffer(recv_buffer_.data(), recv_buffer_.size()), 
                *(remote_endpoint.get()),
                std::bind(
                    &udp_service_base::on_recv_from,
                    this,
                    remote_endpoint,
                    std::placeholders::_1,
                    std::placeholders::_2));
        }

        virtual void on_recv_from(
            udp_endpoint_ptr remote_endpoint,
            const asio::error_code& e, 
            std::size_t bytes_transferred)
        {
            if (recv_handler_)
                recv_handler_(remote_endpoint, e, bytes_transferred);
        }

        virtual void post_send_to(udp_endpoint_ptr remote_endpoint)
        {
            //udp_socket_.async_send_to(
            //    0,
            //    *(remote_endpoint.get()),
            //    std::bind(
            //        &udp_service_base::on_send_to,
            //        this,
            //        remote_endpoint,
            //        std::placeholders::_1,
            //        std::placeholders::_2));
        }

        virtual void on_send_to(
            udp_endpoint_ptr remote_endpoint,
            const asio::error_code& e,
            std::size_t bytes_transferred)
        {

        }

        virtual void stop() override
        {
            udp_socket_.close();

            io_context_.stop();

            if (loop_thread_) 
            {
                loop_thread_->join();
                loop_thread_.reset();
            }
        }

        virtual asio::io_context& io_context() override
        {
            return io_context_;
        }

    private:
        asio::io_context io_context_;
        asio::io_context::work io_work_;
        asio::ip::udp::socket udp_socket_;
        asio::ip::udp::endpoint local_endpoint_;
        std::shared_ptr<asio::thread> loop_thread_;
        std::vector<uint8_t> recv_buffer_;
        recv_handler recv_handler_;
    };
} }
