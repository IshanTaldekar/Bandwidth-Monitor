#ifndef BANDWIDTH_MONITOR_PACKETQUEUE_H
#define BANDWIDTH_MONITOR_PACKETQUEUE_H

#include <queue>

#include "Packet.h"

template <typename T>
class PacketQueue: public std::queue<T> {
    int buffer_size = 1024;

public:
    static PacketQueue<T> *current_instance;
    static PacketQueue<T> *init();

    T get();
    bool full();
};

#endif //BANDWIDTH_MONITOR_PACKETQUEUE_H
