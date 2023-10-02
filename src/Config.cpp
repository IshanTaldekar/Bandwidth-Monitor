#include "include/Config.h"

std::mutex packet_queue_access_mutex;
std::mutex analyzer_mutex;

std::condition_variable producer_condition_variable;
std::condition_variable consumer_condition_variable;
std::condition_variable analyzer_condition_variable;
std::condition_variable reporter_condition_variable;
