#pragma once
#include <numeric>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include "utils.h"

using namespace boost;

constexpr size_t BUFFER_SIZE = 1024 << 6;

struct Config {
    const char* ip;
    int port;
};

struct BenchmarkData {
    double latency;
    double bandwidth;
};

class Server {
public:
    Server(Config const& cfg);
    ~Server() = default;

    Server(Server const& other) = delete;
    Server& operator=(Server const& other) = delete;

    void run();

private:
    asio::io_context m_context;
    Config m_config;
};

class Client {
public:
    Client(Config const& cfg);
    ~Client() = default;

    void sendData(
        const char* data, 
        size_t size, 
        size_t packet_size, 
        asio::ip::udp::endpoint const& server_ep
    );
    BenchmarkData bench(uint64_t data_size, uint64_t pakcet_size);
private:
    asio::io_context m_context;
    asio::ip::udp::socket m_socket;
    Config m_config;
};
