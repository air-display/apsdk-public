#ifndef MDNS_H
#define MDNS_H
#pragma once

#include <string>
#include <cstdint>
#include <map>

class net_service
{
    typedef std::map<std::string, std::string> txt_record;
public:
    net_service(const std::string& type);

    ~net_service();

    void add_txt_record(
        const std::string& k,
        const std::string& v);

    bool publish(
        const std::string& name,
        const uint16_t port);

    void suppress();

private:
    class implementation;
    std::unique_ptr<implementation> impl_;
};

#endif // MDNS_H