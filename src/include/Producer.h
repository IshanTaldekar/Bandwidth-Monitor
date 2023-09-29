#ifndef BANDWIDTH_MONITOR_PRODUCER_H
#define BANDWIDTH_MONITOR_PRODUCER_H

#include <pcap.h>
#include <string>
#include <cstdio>
#include <iostream>

#include "PacketQueue.h"
#include "Packet.h"
#include "Config.h"

class Producer {
    std::string device;
    int buffer_size;
    static PacketQueue<Packet> *packet_queue;

public:
    Producer();
    Producer(const std::string &device, int buffer_size);

    void run();

private:
    void capture();
    static void enqueuePacket(u_char *args, const pcap_pkthdr *header, const u_char *packet);
};

#endif //BANDWIDTH_MONITOR_PRODUCER_H
