#include "include/Analyzer.h"

Analyzer* Analyzer::current_instance = nullptr;

Analyzer::Analyzer() {
    this->analysis_week = 0;
    this->analysis_ready = false;
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
        analyzer_condition_variable.wait(analyzer_lock, [this] { return this->processing_queue.size() == 2; });

        this->analysis_week = this->processing_queue.front();
        this->processing_queue.pop();

        this->processLogs();
        this->generateDailyMetrics();
        this->generateWeeklyMetrics();

        this->analysis_ready = true;

        reporter_condition_variable.notify_one();
    }
}

void Analyzer::setCurrentWeek(int current_week) {
    this->processing_queue.push(current_week);
}

void Analyzer::processLogs() {
    this->processed_hourly_bandwidth = std::vector<std::vector<Usage>>(DAYS_IN_WEEK, std::vector<Usage>(HOURS_IN_DAY, Usage()));
    int current_day_in_week = -1;

    std::ifstream weekly_packet_log_file ("../logs/" + std::to_string(this->analysis_week) + ".log", std::ifstream::in);
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

Usage Analyzer::getDayMaxHourlyUsage(int day) {
    if (day >= this->processed_daily_max_usage.size() || day < 0) {
        return Usage {};
    }

    return this->processed_hourly_bandwidth.at(day).at(this->processed_daily_max_usage.at(day));
}

int Analyzer::getDayMaxHourlyUsageHour(int day) {
    if (day >= this->processed_daily_max_usage.size() || day < 0) {
        return -1;
    }

    return this->processed_daily_max_usage.at(day);
}

Usage Analyzer::getWeekUsage() {
    return this->processed_weekly_bandwidth;
}

int Analyzer::getCurrentWeek() {
    return this->analysis_week;
}

bool Analyzer::isAnalysisReady() {
    return this->analysis_ready;
}

void Analyzer::analysisProcessed() {
    this->weekdates.clear();
    this->weekdays.clear();
    this->processed_hourly_bandwidth.clear();
    this->processed_daily_bandwidth.clear();
    this->processed_daily_max_usage.clear();
    this->processed_weekly_bandwidth.reset();
    this->analysis_ready = false;
}

std::string Analyzer::getWeekStartDate() {
    return this->weekdates.front();
}

std::string Analyzer::getWeekEndDate() {
    return this->weekdates.at(this->weekdates.size() - 1);
}