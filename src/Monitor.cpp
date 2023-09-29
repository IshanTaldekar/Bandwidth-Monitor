#include <thread>

#include "include/Producer.h"
#include "include/Consumer.h"

int main() {
    Producer producer;
    Consumer consumer;

    std::thread producer_thread (&Producer::run, &producer);
    std::thread consumer_thread (&Consumer::run, &consumer);

    producer_thread.join();
    consumer_thread.join();
}