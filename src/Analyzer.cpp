#include "include/Analyzer.h"

Analyzer* Analyzer::current_instance = nullptr;

Analyzer::Analyzer() {
    this->week_in_process = -1;
    this->week_to_analyze = -1;
}

Analyzer *Analyzer::init() {
    if (!current_instance) {
        current_instance = new Analyzer();
    }

    return current_instance;
}

void Analyzer::run() {
    while (true) {
        std::unique_lock<std::mutex> analyzer_lock (analyzer_mutex);
        analytics_condition_variable.wait(analyzer_lock, [this] { return this->week_in_process != this->week_to_analyze; });

        this->processLogs();
        this->generateDailyMetrics();
        this->generateWeeklyMetrics();
    }
}

void Analyzer::setCurrentWeek(int current_week) {
    if (this->week_to_analyze == -1) {
        this->week_to_analyze = current_week;
    }

    this->week_in_process = current_week;
}

void Analyzer::processLogs() {
    this->weekdates.clear();
    this->processed_hourly_bandwidth = std::vector<std::vector<Usage>>(DAYS_IN_WEEK, std::vector<Usage>(HOURS_IN_DAY, Usage()));
    int current_day_in_week = -1;

    std::ifstream weekly_packet_log_file ("../logs/" + std::to_string(week_to_analyze) + ".log", std::ifstream::in);
    std::string line;

    while (std::getline(weekly_packet_log_file, line, '\n')) {
        std::vector<std::string> tokens = tokenizeLine(line);

        if (this->weekdates.empty() || this->weekdates.back() != tokens.at(Token::DATE)) {
            this->weekdates.push_back(tokens.at(Token::DATE));
            this->weekdays.push_back(tokens.at(Token::WEEKDAY));
            ++current_day_in_week;
        }

        HourMinuteSecond packet_arrival_time = this->processTimestamp(tokens.at(Token::TIME));
        this->processed_hourly_bandwidth.at(current_day_in_week).at(packet_arrival_time.hour).addBytes(std::stoi(tokens.at(Token::BYTES)));
    }

    weekly_packet_log_file.close();
    this->week_to_analyze = this->week_in_process;
}

std::vector<std::string> Analyzer::tokenizeLine(const std::string &line) {
    std::stringstream line_stream (line);
    std::string token;

    std::vector<std::string> tokens (4);
    int index = 0;

    while (line_stream >> token) {
        tokens.at(index++) = token;
    }

    return tokens;
}

HourMinuteSecond Analyzer::processTimestamp(const std::string &timestamp) {
    std::stringstream timestamp_stream (timestamp);
    std::string token;
    int index = 0;
    HourMinuteSecond result {};

    while (std::getline(timestamp_stream, token, ':')) {
        switch (index++) {
            case 0:
                result.hour = std::stoi(token);
                break;

            case 1:
                result.minute = std::stoi(token);
                break;

            case 2:
                result.second = std::stoi(token);
                break;
        }
    }

    return result;
}

void Analyzer::generateDailyMetrics() {
    this->processed_daily_bandwidth = std::vector<Usage>(DAYS_IN_WEEK, Usage());
    this->processed_daily_max_usage = std::vector<int>{};

    for (int i = 0; i < DAYS_IN_WEEK; ++i) {
        int current_daily_max_index = 0;

        for (int j = 0; j < HOURS_IN_DAY; ++j) {
            this->processed_daily_bandwidth.at(i).addUsage(this->processed_hourly_bandwidth.at(i).at(j));

            if (this->processed_hourly_bandwidth.at(i).at(j) > this->processed_hourly_bandwidth.at(i).at(current_daily_max_index)) {
                current_daily_max_index = j;
            }
        }

        this->processed_daily_max_usage.push_back(current_daily_max_index);
    }
}

void Analyzer::generateWeeklyMetrics() {
    this->processed_weekly_bandwidth.reset();

    for (int i = 0; i < DAYS_IN_WEEK; ++i) {
        this->processed_weekly_bandwidth.addUsage(this->processed_daily_bandwidth.at(i));
    }
}

std::string Analyzer::getDate(int day) {
    if (day >= this->weekdates.size() || day < 0) {
        return "";
    }

    return this->weekdates.at(day);
}

std::string Analyzer::getDayName(int day) {
    if (day >= this->weekdays.size() || day < 0) {
        return "";
    }

    return this->weekdays.at(day);
}

Usage Analyzer::getDayUsage(int day) {
    if (day >= this->processed_daily_bandwidth.size() || day < 0) {
        return Usage {};
    }

    return this->processed_daily_bandwidth.at(day);
}

Usage Analyzer::getDayHourlyMax(int day) {
    if (day >= this->processed_daily_max_usage.size() || day < 0) {
        return Usage {};
    }

    return this->processed_hourly_bandwidth.at(day).at(this->processed_daily_max_usage.at(day));
}

Usage Analyzer::getWeekUsage() {
    return this->processed_weekly_bandwidth;
}