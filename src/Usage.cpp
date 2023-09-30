#include "include/Usage.h"

Usage::Usage() {
    this->bytes = 0;
    this->megabytes = 0;
    this->gigabytes = 0;
}

void Usage::addBytes(int bytes_processed) {
    this->bytes += bytes_processed;

    if (this->bytes >= BYTES_IN_MEGABYTE) {
        this->bytes %= BYTES_IN_MEGABYTE;
        ++this->megabytes;
    }

    if (this->megabytes >= MEGABYTES_IN_GIGABYTE) {
        this->megabytes %= MEGABYTES_IN_GIGABYTE;
        ++this->gigabytes;
    }
}

void Usage::addUsage(const Usage &usage) {
    this->bytes += usage.getBytes();

    if (this->bytes >= BYTES_IN_MEGABYTE) {
        this->megabytes += (this->bytes / BYTES_IN_MEGABYTE);
        this->megabytes %= BYTES_IN_MEGABYTE;
    }

    this->megabytes += usage.getMegabytes();

    if (this->megabytes >= MEGABYTES_IN_GIGABYTE) {
        this->gigabytes += (this->megabytes / MEGABYTES_IN_GIGABYTE);
        this->megabytes %= MEGABYTES_IN_GIGABYTE;
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