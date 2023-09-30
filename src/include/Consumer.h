#ifndef BANDWIDTH_MONITOR_CONSUMER_H
#define BANDWIDTH_MONITOR_CONSUMER_H

#include <iostream>
#include <string>
#include <fstream>

#include "PacketQueue.h"
#include "Packet.h"
#include "Config.h"
#include "Analyzer.h"

class Consumer {
    static PacketQueue<Packet> *packet_queue;
    static Analyzer *analyzer;

public:
    Consumer() = default;
    [[noreturn]] void run();

private:
    static void processAllPackets();
};

#endif //BANDWIDTH_MONITOR_CONSUMER_H
