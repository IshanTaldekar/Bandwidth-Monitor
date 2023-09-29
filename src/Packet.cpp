#include "include/Packet.h"

Packet::Packet(timeval timestamp, unsigned int bytes) {
    this->bytes = bytes;
    this->timestamp = timestamp;
}

unsigned int Packet::getBytes() const {
    return this->bytes;
}

int Packet::getWeekNumber() {
    if (this->week_number == -1) {
        this->parseTimestamp();
    }

    return this->week_number;
}

std::string Packet::getDateTime() {
    if (this->formatted_date_time.empty()) {
        this->parseTimestamp();
    }

    return this->formatted_date_time;
}

void Packet::parseTimestamp() {
    auto epoch_time = static_cast<time_t>(this->timestamp.tv_sec);

    struct tm time_info {};
    localtime_r(&epoch_time, &time_info);

    this->week_number = time_info.tm_yday / DAYS_IN_WEEK + 1;

    char buffer[80];
    strftime(buffer, sizeof(buffer), DATE_TIME_FORMAT.c_str(), &time_info);

    this->formatted_date_time = std::string(buffer);
}
