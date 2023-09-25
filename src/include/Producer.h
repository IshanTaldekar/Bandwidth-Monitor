#ifndef BANDWIDTH_MONITOR_PRODUCER_H
#define BANDWIDTH_MONITOR_PRODUCER_H

#include <pcap.h>
#include <string>
#include <stdio.h>
#include <iostream>

class Producer {
    std::string device;
    long buffer_size;

public:
    Producer();
    Producer(const std::string &device, long buffer_size);

    void capture();
};

#endif //BANDWIDTH_MONITOR_PRODUCER_H
