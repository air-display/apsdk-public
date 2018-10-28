// aps-demo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../src/ap_server.h"
#include "../src/ap_config.h"

int main()
{
    aps::ap_server server(aps::ap_config::default());
    
    server.start();

    getchar();

    server.stop();
}
