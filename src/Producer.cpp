#include "include/Producer.h"

PacketQueue<Packet> *Producer::packet_queue = PacketQueue<Packet>::init();

Producer::Producer() {
    this->device = "null";
    this->buffer_size = BUFSIZ;  // stdio default buffer size
}

Producer::Producer(const std::string &device, int buffer_size) {
    this->device = device;
    this->buffer_size = buffer_size;
}

void Producer::run() {
    this->capture();
}

void Producer::capture() {
    char error_buffer[PCAP_ERRBUF_SIZE];

    pcap_t *session_handle;
    session_handle = pcap_open_live(
            this->device == "null" ? nullptr : this->device.c_str(),
            this->buffer_size,
            0,
            0,
            error_buffer
    );

    if (!session_handle) {
        std::cout << "[Producer] Device could not be opened: " << error_buffer << std::endl;
        return;
    }

    if (pcap_loop(session_handle, -1, enqueuePacket, nullptr) == PCAP_ERROR) {
        pcap_close(session_handle);
        std::cout << "[Producer] Pcap loop error" << std::endl;
        return;
    }
}

void Producer::enqueuePacket(u_char *args, const pcap_pkthdr *header, const u_char *packet) {
    std::unique_lock<std::mutex> producer_lock (packet_queue_access_mutex);
    producer_condition_variable.wait(producer_lock, [] { return !packet_queue->full(); });

    packet_queue->push({header->ts, header->len});

    consumer_condition_variable.notify_one();
}
