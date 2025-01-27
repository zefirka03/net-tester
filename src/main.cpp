#include <stdio.h>
#include <chrono>

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

using namespace boost;

#ifdef DEBUG
#define LOG(msg) printf(msg);
#define LOGF(msg, ...) printf(msg, __VA_ARGS__);
#else
#define LOG(msg)
#define LOGF(msg, ...)
#endif

struct Config {
    const char* ip;
    int port;
    uint32_t packetSize;
    uint32_t packetNum;
};

class Server {
public:
    Server(Config const& cfg){
        m_config = cfg;
    }

    ~Server(){

    }

    void run(){
        asio::ip::tcp::acceptor acceptor(
            m_context,
            asio::ip::tcp::endpoint(
                asio::ip::make_address(m_config.ip),
                m_config.port
            )
        );
        LOG("Acceptor created\n");

        asio::ip::tcp::socket socket(m_context);
        LOG("Waiting for connection\n");

        acceptor.accept(socket);
        LOG("Accepted\n");

        std::vector<char> buffer(m_config.packetSize);
        int receivedPackets = 0;

        auto startTime = std::chrono::steady_clock::now();
        system::error_code error;
        while(socket.read_some(asio::buffer(buffer), error) && !error){
            ++receivedPackets;
        }
        auto endTime = std::chrono::steady_clock::now();

        std::chrono::duration<double> elapsedSeconds = endTime - startTime;
        LOGF("Recieved: %d packets, time: %lf sec\n", receivedPackets, elapsedSeconds.count());
    }

    Server(Server const& other) = delete; 
    Server& operator=(Server const& other) = delete; 

private:
    asio::io_context m_context;
    Config m_config;
};

int main(){
    Config cfg = {
        .ip = "127.0.0.1",
        .port = 8080,
        .packetSize = 1024,
        .packetNum = 100
    };

    Server server(cfg);
    server.run();

    return 0;
}