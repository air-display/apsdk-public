#ifndef AP_HANDLER_H
#define AP_HANDLER_H
#pragma once

#include <memory>

namespace aps {
    class ap_handler
    {
    public:
        virtual ~ap_handler() {};

    };

    typedef std::shared_ptr<ap_handler> ap_handler_ptr;
}
#endif // AP_HANDLER_H