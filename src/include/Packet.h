#ifndef BANDWIDTH_MONITOR_PACKET_H
#define BANDWIDTH_MONITOR_PACKET_H

#include <sys/time.h>
#include <string>
#include <ctime>

class Packet {
    const int DAYS_IN_WEEK = 7;
    const std::string DATE_TIME_FORMAT = "%Y-%m-%d@%H:%M:%S";

    timeval timestamp {};
    unsigned int bytes;

    int week_number = -1;
    std::string formatted_date_time;

public:
    Packet(timeval timestamp, unsigned int bytes);

    unsigned int getBytes() const;
    int getWeekNumber();
    std::string getDateTime();

private:
    void parseTimestamp();
};

#endif //BANDWIDTH_MONITOR_PACKET_H
