#ifndef BANDWIDTH_MONITOR_ANALYZER_H
#define BANDWIDTH_MONITOR_ANALYZER_H

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "Config.h"
#include "Usage.h"

class Analyzer {
    const int DAYS_IN_WEEK = 7;
    const int HOURS_IN_DAY = 24;

    int week_to_analyze;
    int week_in_process;

    std::vector<std::string> weekdates;
    std::vector<std::string> weekdays;

    std::vector<std::vector<Usage>> processed_hourly_bandwidth;
    std::vector<Usage> processed_daily_bandwidth;
    std::vector<int> processed_daily_max_usage;

    Usage processed_weekly_bandwidth;

public:
    static Analyzer *current_instance;

    Analyzer();
    static Analyzer *init();

    void run();
    void setCurrentWeek(int current_week);

    std::string getDate(int day);
    std::string getDayName(int day);

    Usage getDayUsage(int day);
    Usage getDayHourlyMax(int day);
    Usage getWeekUsage();

private:
    void processLogs();
    void generateDailyMetrics();
    void generateWeeklyMetrics();

    std::vector<std::string> tokenizeLine(const std::string &line);
    HourMinuteSecond processTimestamp(const std::string &timestamp);
};

#endif //BANDWIDTH_MONITOR_ANALYZER_H
