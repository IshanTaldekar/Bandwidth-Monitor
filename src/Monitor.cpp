#include <thread>

#include "include/Producer.h"
#include "include/Consumer.h"
#include "include/Analyzer.h"

int main() {
    Producer producer;
    std::thread producer_thread (&Producer::run, &producer);

    Consumer consumer;
    std::thread consumer_thread (&Consumer::run, &consumer);

    Analyzer *analyzer = Analyzer::init();
    std::thread analyzer_thread (&Analyzer::run, analyzer);
}