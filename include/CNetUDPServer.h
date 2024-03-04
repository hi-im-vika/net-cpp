/**
 * CNetUDPServer.h - low latency UDP server
 * 2024-03-03
 * vika <https://github.com/hi-im-vika>
 */

#pragma once

#include <thread>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <spdlog/spdlog.h>

class CNetUDPServer {
private:
    bool init_net();

    int _port = 0;
    int _socket_fd = 0;
    ssize_t _read_code = 0;
    struct sockaddr_in _server_addr{};
    struct sockaddr_in _client_addr{};
    socklen_t _client_addr_len = 0;
    std::vector<uint8_t> _recv_buffer;
    std::string _msg;
public:
    CNetUDPServer();
    ~CNetUDPServer();
    void setup(const std::string& port);
    void setdn() const;
    bool do_listen();
};
