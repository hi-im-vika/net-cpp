/**
 * CNetUDPClient.h - new file
 * 2024-03-03
 * vika <https://github.com/hi-im-vika>
 */

#pragma once

#include <thread>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <spdlog/spdlog.h>

class CNetUDPClient {
private:
    bool init_net();

    std::string _host;
    int _port = 0;
    int _socket_fd = 0;
    bool _socket_ok = false;
    ssize_t _read_code = 0;
    struct sockaddr_in _server_addr{};
    struct sockaddr_in _client_addr{};
    socklen_t _server_addr_len = 0;
    std::vector<uint8_t> _recv_buffer;

    std::chrono::steady_clock::time_point _perf_rtt_start;
    int _perf_rtt = 0;
public:
    CNetUDPClient();
    ~CNetUDPClient();
    void setup(const std::string& host, const std::string& port);
    void setdn() const;
    bool do_yap(const std::vector<uint8_t> &yap_data, std::vector<uint8_t> &opposite_of_yap, int &rtt);
    bool ping();
};
