#include "../net_service.h"

class net_service::implementation
{
public:
    implementation(const std::string& type);
    ~implementation();

    void add_txt_record(
        const std::string& k,
        const std::string& v);

    bool publish(
        const std::string& name,
        const uint16_t port);

    void suppress();

private:
};
