#include "include/Reporter.h"

Reporter::Reporter() {
    this->analyzer = Analyzer::init();
}

void Reporter::run() {
    while (true) {
        std::unique_lock<std::mutex> reporter_lock (analyzer_mutex);
        reporter_condition_variable.wait(reporter_lock, [this] { return this->analyzer->isAnalysisReady(); });

        this->writeReport();
        this->sendEmail();

        this->analyzer->analysisProcessed();
    }
}

void Reporter::writeReport() {
    std::ofstream report ("../reports/report-" + std::to_string(this->analyzer->getCurrentWeek()) + ".txt");

    std::string title = this->getTitle();

    report << std::endl;
    report << title << std::endl;
    report << std::string(title.length() + 10, '-') << std::endl;
    report << std::endl;
    report << this->getFormattedWeeksUsage() << std::endl;
    report << std::endl;
    report << this->getFormattedDailyUsage() << std::endl;
    report << std::endl;
    report << this->getFormattedDailyUsage();
    report << std::endl;
    report << this->getFormattedDailyHeavyUsage();
    report << std::endl;
    report << std::string(title.length() + 10, '*');

    report.close();
}

std::string Reporter::getTitle() {
    return "     BANDWIDTH MONITOR REPORT (" + this->analyzer->getWeekStartDate() + " to " +
            this->analyzer->getWeekEndDate() + ")";
}

std::string Reporter::getFormattedWeeksUsage() {
    Usage week_usage = this->analyzer->getWeekUsage();

    return "1. Total Bandwidth Consumed -> " +
            std::to_string(week_usage.getGigabytes()) + "GB, " +
            std::to_string(week_usage.getMegabytes()) + "MB, " +
            std::to_string(week_usage.getKilobytes()) + "KB, " +
            std::to_string(week_usage.getBytes()) + "B";
}

std::string Reporter::getFormattedDailyUsage() {
    std::stringstream buffer;
    buffer << "2. Daily Usage Statistics" << std::endl;

    for (int i = 0; i < DAYS_IN_WEEK; ++i) {
        std::string date = this->analyzer->getDate(i);

        if (date.empty()) {
            break;
        }

        Usage daily_usage = this->analyzer->getDayUsage(i);

        buffer << "[+] " + date << " (" << this->analyzer->getDayName(i) << ") -> ";
        buffer << daily_usage.getGigabytes() << "GB, ";
        buffer << daily_usage.getMegabytes() << "MB, ";
        buffer << daily_usage.getKilobytes() << "KB, ";
        buffer << daily_usage.getBytes() << "B" <<  std::endl;
    }

    return buffer.str();
}

std::string Reporter::getFormattedDailyHeavyUsage() {
    std::stringstream buffer;
    buffer << "3. Daily Heavy Usage Statistics" << std::endl;

    for (int i = 0; i < DAYS_IN_WEEK; ++i) {
        std::string date = this->analyzer->getDate(i);

        if (date.empty()) {
            break;
        }

        Usage daily_usage = this->analyzer->getDayMaxHourlyUsage(i);

        buffer << "[+] " + date << " (" << this->analyzer->getDayName(i) << ") @";
        buffer << this->getFormattedHour(i) << " -> ";
        buffer << daily_usage.getGigabytes() << "GB, ";
        buffer << daily_usage.getMegabytes() << "MB, ";
        buffer << daily_usage.getKilobytes() << "KB, ";
        buffer << daily_usage.getBytes() << "B" <<  std::endl;
    }

    return buffer.str();
}

std::string Reporter::getFormattedHour(int day) {
    int hour = this->analyzer->getDayMaxHourlyUsageHour(day);

    std::string formatted_hour = "";
    formatted_hour += hour < 10 ? "0" + std::to_string(hour) : std::to_string(hour);
    formatted_hour += ":00:00";

    return formatted_hour;
}

void Reporter::sendEmail() {
    std::string weekly_report_relative_path = "../reports/report-" + std::to_string(this->analyzer->getCurrentWeek()) + ".txt";
    std::string command = "cat " + weekly_report_relative_path + " | sudo ssmtp ishantaldekar1@gmail.com";
    system(command.c_str());
}