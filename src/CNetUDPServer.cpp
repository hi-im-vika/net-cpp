/**
 * CNetUDPServer.cpp - low latency UDP server
 * 2024-03-03
 * vika <https://github.com/hi-im-vika>
 */

#include "../include/CNetUDPServer.h"

CNetUDPServer::CNetUDPServer() = default;

CNetUDPServer::~CNetUDPServer() {
    setdn();
}

bool CNetUDPServer::init_net() {

    spdlog::info("Beginning socket init.");
    if (!_port) {
        spdlog::error("No port specified.");
        return false;
    }

    // create new socket, exit on failure
    if ((_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        spdlog::error("Error opening socket");
        return false;
    }

    // bind program to address and port
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(_port);
    _server_addr.sin_addr.s_addr = INADDR_ANY;

    if ((bind(_socket_fd, (struct sockaddr *) &_server_addr, sizeof(_server_addr))) < 0) {
        spdlog::error("Error opening socket");
        close(_socket_fd);
        return false;
    }

    spdlog::info("Listening on udp://0.0.0.0:" + std::to_string(_port));
    spdlog::info("Socket init complete.");
    return true;
}

bool CNetUDPServer::do_listen() {

    _recv_buffer.reserve(65535);

    // listen for client
    _client_addr_len = sizeof(_client_addr);
    _read_code = recvfrom(_socket_fd, _recv_buffer.data(), _recv_buffer.capacity(), 0, (struct sockaddr *) &_client_addr, &_client_addr_len);
    if (_read_code < 0) {
        spdlog::error("Error reading data.");
        close(_socket_fd);
        return false;
    }

    spdlog::info("Received: " + std::string(_recv_buffer.begin(), _recv_buffer.end()) + " from " + std::string(inet_ntoa(_client_addr.sin_addr)));
    _msg = "Hello!";

    // respond to client
    if (sendto(_socket_fd, _msg.data(), _msg.length(), 0, (struct sockaddr *) &_client_addr, _client_addr_len) <
        0) {
        spdlog::error("Error sending data.");
        close(_socket_fd);
        return false;
    }
    return true;
}

void CNetUDPServer::setup(const std::string &port) {
    spdlog::info("Beginning UDP server setup.");
    _port = std::stoi(port);
    if (!init_net()) {
        spdlog::error("Error during UDP server setup. Shutting down!");
        exit(1);
    }
}

void CNetUDPServer::setdn() const {
    close(_socket_fd);
}
