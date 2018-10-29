#ifndef AP_SERVER_H
#define AP_SERVER_H
#pragma once

#include <memory>
#include <ap_config.h>

namespace aps {
    class ap_server
    {
    public:
        ap_server(const aps::ap_config& config);
        ~ap_server();

        bool start();

        void stop();

    private:
        class implementation;
        std::unique_ptr<implementation> impl_;
    };
}
#endif // AP_SERVER_H
