#include "include/Consumer.h"

PacketQueue<Packet> *Consumer::packet_queue = PacketQueue<Packet>::init();

void Consumer::run() {
    while (true) {
        processAllPackets();
    }
}

void Consumer::processAllPackets() {
    static int current_week = -1;

    std::unique_lock<std::mutex> consumer_lock (packet_queue_access_mutex);
    consumer_condition_variable.wait(consumer_lock, [] { return !packet_queue->empty(); });

    while (!packet_queue->empty()) {
        Packet current_packet = packet_queue->get();

        if (current_week != current_packet.getWeekNumber()) {
            current_week = current_packet.getWeekNumber();

        }

        std::ofstream weekly_packet_log_file ("../logs/" + std::to_string(current_packet.getWeekNumber()) + ".log", std::ofstream::out | std::ofstream::app);
        weekly_packet_log_file << current_packet.getDateTime() << " " << current_packet.getBytes() << std::endl;
        weekly_packet_log_file.close();
    }

    producer_condition_variable.notify_one();
}