#include "net.h"

Server::Server(Config const& cfg){
    m_config = cfg;
}

void Server::run(){
    asio::ip::udp::socket socket(
        m_context,
        asio::ip::udp::endpoint(
            asio::ip::make_address(m_config.ip), 
            m_config.port
        )
    );
    asio::ip::udp::endpoint sender_ep;
    std::vector<char> buffer(BUFFER_SIZE);
    LOG("Listening...\n");
    while(true){
        size_t bytes_recvd = socket.receive_from(asio::buffer(buffer.data(), buffer.size()), sender_ep);
        
        if(bytes_recvd >= 3 && std::string(buffer.data(), 3) == "END"){
            LOGF("Recieved from %s\n", sender_ep.address().to_string().c_str());
        }
    }
}
