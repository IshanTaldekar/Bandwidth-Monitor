#ifndef BANDWIDTH_MONITOR_PACKET_H
#define BANDWIDTH_MONITOR_PACKET_H

#include <sys/time.h>

class Packet {
    timeval timestamp;
    int bytes;

public:
    Packet(timeval timestamp, int bytes) {
        this->bytes = bytes;
        this->timestamp = timestamp;
    }

    void setBytes(int bytes) {
        this->bytes = bytes;
    }

    void setTimestamp(timeval timestamp) {
        this->timestamp = timestamp;
    }

    int getBytes() {
        return this->bytes;
    }

    timeval getTimestamp() {
        return this->timestamp;
    }
};

#endif //BANDWIDTH_MONITOR_PACKET_H
