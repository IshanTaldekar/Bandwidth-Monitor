//
// Created by wasp on 9/30/23.
//

#ifndef BANDWIDTH_MONITOR_USAGE_H
#define BANDWIDTH_MONITOR_USAGE_H

class Usage {
    int bytes;
    int megabytes;
    int gigabytes;

    const int BYTES_IN_MEGABYTE = 1048576;
    const int MEGABYTES_IN_GIGABYTE = 1024;

public:
    Usage();

    void addBytes(int bytes_processed);
    void addUsage(const Usage &usage);

    [[nodiscard]] int getBytes() const;
    [[nodiscard]] int getMegabytes() const;
    [[nodiscard]] int getGigabytes() const;

    void reset();

    bool operator > (const Usage &other) const;
};

#endif //BANDWIDTH_MONITOR_USAGE_H
