#include "include/Config.h"

std::mutex packet_queue_access_mutex;
std::condition_variable producer_condition_variable;
std::condition_variable consumer_condition_variable;
std::condition_variable analytics_condition_variable;
