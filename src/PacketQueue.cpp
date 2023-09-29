#include "include/PacketQueue.h"

template class PacketQueue<Packet>;

template<typename T>
PacketQueue<T> *PacketQueue<T>::current_instance = nullptr;

template <typename T>
PacketQueue<T> *PacketQueue<T>::init() {
    if (!current_instance) {
        current_instance = new PacketQueue<T>();
    }

    return current_instance;
}

template <typename T>
T PacketQueue<T>::get() {
    T element = this->front();
    this->pop();
    return element;
}

template <typename T>
bool PacketQueue<T>::full() {
    return this->size() == buffer_size;
}