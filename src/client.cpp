#include "net.h"

Client::Client(Config const& cfg) : m_socket(m_context){
    m_config = cfg;
    m_socket.open(asio::ip::udp::v4());
}

void Client::sendData(
    const char* data, 
    size_t size, 
    size_t packet_size, 
    asio::ip::udp::endpoint const& server_ep
){
    size_t total_packets = (size + packet_size - 1) / packet_size;
    for(size_t i = 0; i < total_packets; ++i){
        size_t offset = i * packet_size;
        size_t bytes_to_send = std::min(packet_size, size - offset);
        m_socket.send_to(asio::buffer(data + offset, bytes_to_send), server_ep);            
    }
}

BenchmarkData Client::bench(uint64_t data_size, uint64_t pakcet_size){
    asio::ip::udp::endpoint server_ep(
        asio::ip::address::from_string(m_config.ip), 
        m_config.port
    );
    std::vector<char> data(data_size, 'x');
    size_t total_bytes_sent = 0;
    LOGF("Sending on %s\n", m_config.ip);

    Timer timer;
    sendData(data.data(), data.size(), pakcet_size, server_ep);
    sendData("END", 3, pakcet_size, server_ep);
    double latency = timer.ms();

    total_bytes_sent = data.size() + 3;
        
    double total_time_seconds = latency / 1000.0;
    double bandwidth_bps = (total_bytes_sent * 8) / total_time_seconds;
    
    LOGF("Total bytes sent: %lu, Latency: %f ms\n, Bandwidth: %f bps\n", total_bytes_sent, latency, bandwidth_bps);
    
    return BenchmarkData({
        .latency = latency,
        .bandwidth = bandwidth_bps
    });
}