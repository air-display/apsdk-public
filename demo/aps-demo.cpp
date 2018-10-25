// aps-demo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../src/ap_server.h"

int main()
{
    aps::ap_server server;
    
    server.initialize();
    server.start();

    getchar();

    server.stop();
    server.uninitialize();
}
