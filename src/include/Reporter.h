#ifndef BANDWIDTH_MONITOR_REPORTER_H
#define BANDWIDTH_MONITOR_REPORTER_H

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <exception>
#include <iostream>

#include "Analyzer.h"

class Reporter {
    const int DAYS_IN_WEEK = 7;
    const int PATH_LENGTH_MAX = 250;

    Analyzer *analyzer;

public:
    Reporter();
    void run();

private:
    void writeReport();
    void sendEmail();

    std::string getTitle();
    std::string getFormattedWeeksUsage();
    std::string getFormattedDailyUsage();
    std::string getFormattedDailyHeavyUsage();
    std::string getFormattedHour(int day);
};

#endif //BANDWIDTH_MONITOR_REPORTER_H
