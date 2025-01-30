#include <stdio.h>
#include <vector>
#include <thread>
#include <fstream>
#include <string>
#include <numeric>

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include "utils.h"
#include "net.h"

using namespace boost;

#define ITERATIONS 10000
constexpr size_t DATA_SIZE = 1024;

int main(int argc, char* argv[]){
    Config cfg = {
        .ip = "127.0.0.1",
        .port = 8080
    };

    std::string m(argv[1]);

    if(m == "s"){
        LOG("Run server\n");
        Server server(cfg);
        server.run();
    } else if(m == "c"){
        LOG("Run client\n");
        Client client(cfg);

        std::ofstream avg_bench_file;
        avg_bench_file.open("bench_avg.csv");
        avg_bench_file << "packet_size" << ',' << "avg_latency" << ',' << "avg_bandwidth" << '\n';
        int packetSizes[] = {8, 16, 64, 1024, 1024 << 2, 1024 << 5};
        for (const auto& packetSize : packetSizes) {
            BenchmarkData acc_bench = {};
            std::ofstream bench_file;
            bench_file.open("bench_" + std::to_string(packetSize) + ".csv");
            bench_file << "packet size" << ',' << "latency" << ',' << "bandwidth" << '\n';

            for (uint64_t i = 0; i < ITERATIONS; i += 1) {
                auto bm = client.bench(DATA_SIZE, packetSize);
                acc_bench.latency += bm.latency;
                acc_bench.bandwidth += bm.bandwidth;

                bench_file << (1 << i) << ',' << bm.latency << ',' << bm.bandwidth << '\n';
            }
            bench_file.close();

            double avg_latency = acc_bench.latency / (double)ITERATIONS;
            double avg_bandwidth = acc_bench.bandwidth / (double)ITERATIONS;
            avg_bench_file << packetSize << ',' << avg_latency << ',' << avg_bandwidth << '\n';
        }

        avg_bench_file.close();
    }

    return 0;
}