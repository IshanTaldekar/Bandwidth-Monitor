#include "include/Producer.h"

Producer::Producer() {
    this->device = "null";
    this->buffer_size = BUFSIZ;  // stdio default buffer size
}

Producer::Producer(const std::string &device, long buffer_size) {
    this->device = device;
    this->buffer_size = buffer_size;
}

void Producer::capture() {
    char error_buffer[PCAP_ERRBUF_SIZE];
    pcap_t *session_handle;

    session_handle = pcap_open_live(
            this->device == "null" ? nullptr : this->device,
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
    }
}
