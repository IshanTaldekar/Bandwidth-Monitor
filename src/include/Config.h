#ifndef BANDWIDTH_MONITOR_CONFIG_H
#define BANDWIDTH_MONITOR_CONFIG_H

#include <mutex>
#include <condition_variable>

extern std::mutex packet_queue_access_mutex;
extern std::condition_variable producer_condition_variable;
extern std::condition_variable consumer_condition_variable;
extern std::condition_variable analytics_condition_variable;

#endif //BANDWIDTH_MONITOR_CONFIG_H
