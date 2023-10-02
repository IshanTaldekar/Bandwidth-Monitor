#include "include/Usage.h"

Usage::Usage() {
    this->bytes = 0;
    this->kilobytes = 0;
    this->megabytes = 0;
    this->gigabytes = 0;
}

void Usage::addBytes(int bytes_processed) {
    this->bytes += bytes_processed;

    if (this->bytes >= CONVERSION_FACTOR) {
        this->bytes %= CONVERSION_FACTOR;
        ++this->kilobytes;
    }

    if (this->kilobytes >= CONVERSION_FACTOR) {
        this->kilobytes %= CONVERSION_FACTOR;
        ++this->megabytes;
    }

    if (this->megabytes >= CONVERSION_FACTOR) {
        this->megabytes %= CONVERSION_FACTOR;
        ++this->gigabytes;
    }
}

void Usage::addUsage(const Usage &usage) {
    this->bytes += usage.getBytes();

    if (this->bytes >= CONVERSION_FACTOR) {
        this->kilobytes += (this->bytes / CONVERSION_FACTOR);
        this->bytes %= CONVERSION_FACTOR;
    }

    this->kilobytes += usage.getKilobytes();

    if (this->kilobytes >= CONVERSION_FACTOR) {
        this->megabytes += (this->kilobytes / CONVERSION_FACTOR);
        this->kilobytes %= CONVERSION_FACTOR;
    }

    this->megabytes += usage.getMegabytes();

    if (this->megabytes >= CONVERSION_FACTOR) {
        this->gigabytes += (this->megabytes / CONVERSION_FACTOR);
        this->megabytes %= CONVERSION_FACTOR;
    }

    this->gigabytes += usage.gigabytes;
}

int Usage::getBytes() const {
    return this->bytes;
}

int Usage::getMegabytes() const {
    return this->megabytes;
}

int Usage::getGigabytes() const {
    return this->gigabytes;
}

int Usage::getKilobytes() const {
    return this->kilobytes;
}

bool Usage::operator > (const Usage &other) const {
    if (this->getGigabytes() > other.getGigabytes()) {
        return true;
    } else if (this->getGigabytes() == other.getGigabytes() && this->getMegabytes() > other.getMegabytes()) {
        return true;
    } else if (this->getGigabytes() == other.getGigabytes() && this->getMegabytes() == other.getMegabytes() &&
               this->getBytes() > other.getBytes()) {
        return true;
    }

    return false;
}

void Usage::reset() {
    this->bytes = 0;
    this->megabytes = 0;
    this->gigabytes = 0;
}