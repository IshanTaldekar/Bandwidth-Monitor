#ifndef BANDWIDTH_MONITOR_CONFIG_H
#define BANDWIDTH_MONITOR_CONFIG_H

#include <mutex>
#include <condition_variable>

extern std::mutex packet_queue_access_mutex;
extern std::mutex analyzer_mutex;

extern std::condition_variable producer_condition_variable;
extern std::condition_variable consumer_condition_variable;
extern std::condition_variable analyzer_condition_variable;
extern std::condition_variable reporter_condition_variable;

enum Token {
    DATE = 0,
    WEEKDAY,
    TIME,
    BYTES
};

struct HourMinuteSecond {
    int hour;
    int minute;
    int second;
};

#endif //BANDWIDTH_MONITOR_CONFIG_H
