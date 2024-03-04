#include "../include/CNetUDPServer.h"
#include "../include/CNetUDPClient.h"

/**
 * main.cpp - tests for net-cpp
 * 2024-03-03
 * vika <https://github.com/hi-im-vika>
 */

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: test [-s | -c] <host> <port>" << std::endl;
        return 1;
    }

    char opt;
    bool is_server = false;

    while ((opt = (char) getopt(argc, argv, "sc:")) != -1) {
        switch (opt) {
            case 's':
                spdlog::info("Starting as server.");
                is_server = true;
                break;
            case 'c':
                spdlog::info("Starting as client.");
                break;
            default:
                exit(1);
        }
    }

    if (is_server) {
        CNetUDPServer s;
        s.setup(argv[3]);
        while(s.do_listen());
    } else {
//        CNetUDPClient c;
    }
    return 0;
}