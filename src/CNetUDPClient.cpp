/**
 * CNetUDPClient.cpp - new file
 * 2024-03-03
 * vika <https://github.com/hi-im-vika>
 */

#include "../include/CNetUDPClient.h"

CNetUDPClient::CNetUDPClient() = default;

CNetUDPClient::~CNetUDPClient() {
    setdn();
}

bool CNetUDPClient::init_net() {
    spdlog::info("Beginning socket init.");
    if (_host.empty() || !_port) {
        spdlog::error("No host/port specified.");
        return false;
    }

    // create new socket, exit on failure
    if ((_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        spdlog::error("Error opening socket");
        return false;
    }

    spdlog::info("Setting socket to nonblocking.");
    int flags = fcntl(_socket_fd, F_GETFL, 0);
    fcntl(_socket_fd, F_SETFL, flags | O_NONBLOCK);

    spdlog::info("Connecting to " + _host + ":" + std::to_string(_port));

    // set server details
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(_port);
    _server_addr.sin_addr.s_addr = inet_addr(_host.data());

    do {
        spdlog::info("Sending ENQ...");
        _socket_ok = ping();
        if (_socket_ok) {
            spdlog::info("ACK received");
        } else {
            spdlog::info("Timeout");
        }
    } while (!_socket_ok);

    spdlog::info("Yapping to udp://" + _host + ":" + std::to_string(_port));
    return true;
}

void CNetUDPClient::setup(const std::string &host, const std::string &port) {
    spdlog::info("Beginning UDP client setup.");
    _host = host;
    _port = std::stoi(port);
    if (!init_net()) {
        spdlog::error("Error during UDP client setup. Shutting down!");
        exit(1);
    }
}

void CNetUDPClient::setdn() const {
    close(_socket_fd);
}

bool CNetUDPClient::ping() {
    uint8_t buffer = '\5';

    // send ping
    _read_code = sendto(_socket_fd, &buffer, sizeof(buffer), 0,
                        (struct sockaddr *) &_server_addr, sizeof(_server_addr));
    if (_read_code < 0) {
        spdlog::error("Error sending data");
        return false;
    }

    _server_addr_len = sizeof(_server_addr);
    _read_code = 0;

    _perf_rtt_start = std::chrono::steady_clock::now();

    // spins until ping response or timeout
    while (_read_code <= 0) {
        _read_code = recvfrom(_socket_fd, &buffer, sizeof(buffer), 0,
                              (struct sockaddr *) &_server_addr, &_server_addr_len);

        if ((int) std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - _perf_rtt_start).count() > 1000) {
            spdlog::warn("No response to ping.");
            return false;
        }
    }
    return (buffer == '\6');
}

bool CNetUDPClient::do_yap(const std::vector<uint8_t> &yap_data, std::vector<uint8_t> &opposite_of_yap, int &rtt) {

    // check if socket is ok
    if (!_socket_ok) {
        spdlog::error("Socket not ok, cannot yap.");
        return false;
    }

    // preallocate receiving buffer
    opposite_of_yap.resize(65535);

    // send message to server
    _read_code = sendto(_socket_fd, yap_data.data(), yap_data.size(), 0,
                        (struct sockaddr *) &_server_addr, sizeof(_server_addr));

    // if problem with sending data, return false
    if (_read_code < 0) {
        spdlog::error("Error sending data");
        return false;
    }

    // get length of server address
    _server_addr_len = sizeof(_server_addr);

    // reset rx return code
    _read_code = 0;

    // begin measuring server response time
    _perf_rtt_start = std::chrono::steady_clock::now();

    // spins until complete response or timeout
    // TODO: make thread handle ping instead of using timeout here
    while (_read_code <= 0 && _socket_ok) {
        // send data
        _read_code = recvfrom(_socket_fd, opposite_of_yap.data(), opposite_of_yap.capacity(), 0,
                              (struct sockaddr *) &_server_addr, &_server_addr_len);

        // if timeout
        if ((int) std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - _perf_rtt_start).count() > 1000) {
            spdlog::warn("UDP recv time out. Are you still there?");
            _socket_ok = false;
            return false;
        }
    }

    // stop rtt timer
    _perf_rtt = (int) std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - _perf_rtt_start).count();

//    opposite_of_yap = _recv_buffer;
    rtt = _perf_rtt;
    return true;
}
