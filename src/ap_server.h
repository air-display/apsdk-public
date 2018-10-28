#pragma once
#include <memory>

namespace aps {
    class ap_server
    {
    public:
        ap_server();
        ~ap_server();

        bool initialize();
        bool start();
        void stop();
        void uninitialize();

    private:
        class detail;
        std::unique_ptr<detail> impl_;
    };
}
