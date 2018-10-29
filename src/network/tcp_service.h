#pragma once
#include <memory>
#include <asio.hpp>
#include <utils/logger.h>

namespace aps { namespace network {
    class tcp_session
    {
    public:
        virtual ~tcp_session() {};

        virtual void start() = 0;

        virtual void close() = 0;

        virtual asio::ip::tcp::socket& socket() = 0;
    };

    typedef std::shared_ptr<tcp_session> tcp_session_ptr;

    class tcp_service
    {
    public:
        virtual ~tcp_service() {};

        virtual const uint16_t port() const = 0;

        virtual bool start() = 0;;

        virtual void stop() = 0;

        virtual asio::io_context& io_context() = 0;

        virtual aps::network::tcp_session_ptr prepare_new_session() = 0;
    };

    typedef std::shared_ptr<tcp_service> tcp_service_ptr;

    class tcp_session_base : public tcp_session
    {
    public:
        explicit tcp_session_base(asio::io_context& io_ctx)
            : socket_(io_ctx)
            , strand_(io_ctx)
        {
        };

        virtual ~tcp_session_base()
        {
        }

        virtual void close()
        {
            socket_.shutdown(asio::ip::tcp::socket::shutdown_both);

            socket_.close();
        }

        asio::ip::tcp::socket& socket()
        { 
            return socket_;
        }

    protected:
        asio::ip::tcp::socket socket_;
        asio::io_context::strand strand_;
    };

    class tcp_service_base : public tcp_service
    {
    public:
        tcp_service_base(uint16_t port = 0, bool single_client = false)
            : port_(port)
            , single_client_(single_client)
            , io_context_()
            , io_work_(io_context_)
            , acceptor_(io_context_)
            , loop_thread_(0)
        {

        }

        ~tcp_service_base()
        {
            stop();
        }

        virtual const uint16_t port() const override
        {
            return port_;
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
            asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port_);
            acceptor_.open(endpoint.protocol());
            acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(endpoint);
            acceptor_.listen();
            port_ = acceptor_.local_endpoint().port();

            // Post the first accept operation 
            post_accept();

            return true;
        }

        virtual void stop() override
        {
            acceptor_.close();

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

    protected:
        void post_accept()
        {

            // Create a new client session for incoming connection
            new_session_ = prepare_new_session();

            // Perform accept operation on the new client session
            acceptor_.async_accept(new_session_->socket(),
                std::bind(&tcp_service_base::on_accept, this, std::placeholders::_1));
         
            LOGI() << "Session (" << std::hex << new_session_.get() << ") is waiting";
        }

        void on_accept(const asio::error_code& e)
        {
            if (!e)
            {
                // Start the new session 
                new_session_->start();

                LOGI() << "Session (" << std::hex << new_session_.get() << ") accepted and started";

                if (single_client_)
                    return;

                // Post a new accept operation 
                post_accept();
            }
            else
            {
                LOGE() << "Failed to accept the new session: " << e.message();
            }
        }

    private:
        uint16_t port_;
        bool single_client_;
        asio::io_context io_context_;
        asio::io_context::work io_work_;
        asio::ip::tcp::acceptor acceptor_;
        std::shared_ptr<asio::thread> loop_thread_;

        tcp_session_ptr new_session_;
    };
} }
