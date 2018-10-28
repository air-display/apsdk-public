#include "../net_service_impl.h"

class net_service::net_service_impl
    : public net_service::implementation
{
public:
    net_service_impl(const std::string& type)
    {

    }

    ~net_service_impl()
    {

    }

    virtual void add_txt_record(const std::string& k, const std::string& v) override
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual bool publish(const std::string& name, const uint16_t port) override
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    virtual void suppress() override
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

private:
};

net_service::implementation* net_service::implementation::get(const std::string& type)
{
    return new net_service_impl(type);
}
