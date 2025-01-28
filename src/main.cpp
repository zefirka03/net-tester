#include <stdio.h>
#include <vector>
#include <thread>
#include <numeric>

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include "utils.h"

using namespace boost;

struct Config {
    const char* ip;
    int port;
    uint32_t packetSize;
    uint64_t dataSize;
};

class Server {
public:
    Server(Config const& cfg){
        m_config = cfg;
    }
    ~Server() = default;

    Server(Server const& other) = delete;
    Server& operator=(Server const& other) = delete;

    void run(){
        asio::ip::udp::socket socket(
            m_context,
            asio::ip::udp::endpoint(
                asio::ip::make_address(m_config.ip), 
                m_config.port
            )
        );

        asio::ip::udp::endpoint sender_ep;
        std::vector<char> buffer(m_config.packetSize);

        LOG("Listening...\n");
        while(true){
            size_t bytes_recvd = socket.receive_from(asio::buffer(buffer.data(), buffer.size()), sender_ep);
            
            if(bytes_recvd >= 3 && std::string(buffer.data(), 3) == "END"){
                LOGF("Recieved from %s\n", sender_ep.address().to_string().c_str());
            }

            //socket.send_to(asio::buffer("OK", 2), sender_ep);
        }
    }

private:
    asio::io_context m_context;
    Config m_config;
};

class Client {
public:
    Client(Config const& cfg) : m_socket(m_context){
        m_config = cfg;
        m_socket.open(asio::ip::udp::v4());
    }
    ~Client() = default;

    void sendData(const char* data, size_t size, asio::ip::udp::endpoint const& server_ep){
        size_t total_packets = (size + m_config.packetSize - 1) / m_config.packetSize;

        for(size_t i = 0; i < total_packets; ++i){
            size_t offset = i * m_config.packetSize;
            size_t bytesToSend = std::min(size_t(m_config.packetSize), size - offset);

            m_socket.send_to(asio::buffer(data + offset, bytesToSend), server_ep);            
        }
    }

    void run(uint64_t dataSize){
        asio::ip::udp::endpoint server_ep(
            asio::ip::address::from_string(m_config.ip), 
            m_config.port
        );

        std::vector<char> data(dataSize, 'x');
        std::vector<double> latencies;
        size_t total_bytes_sent = 0;

        LOGF("Sending on %s\n", m_config.ip);

        Timer timer;
        for(int i=0; i<10; ++i){
            timer.restart();
            sendData(data.data(), data.size(), server_ep);
            sendData("END", 3, server_ep);
            latencies.emplace_back(timer.ms());
        }
        
        LOGF("Size: %ld, AVG Latency: %f ms\n", dataSize, std::reduce(latencies.begin(), latencies.end()) / latencies.size());
    }

private:
    asio::io_context m_context;
    asio::ip::udp::socket m_socket;
    Config m_config;
};

int main(int argc, char* argv[]){
    Config cfg = {
        .ip = "127.0.0.1",
        .port = 8080,
        .packetSize = 1024 << 5,
        .dataSize = 1ull << 32
    };

    std::string m(argv[1]);

    if(m == "s"){
        LOG("Run server\n");
        Server server(cfg);
        server.run();
    } else if(m == "c"){
        LOG("Run client\n");
        Client client(cfg);
        for(int i = 4; i <= 30; ++i){
            client.run(1 << i);
        }
    }

    return 0;
}