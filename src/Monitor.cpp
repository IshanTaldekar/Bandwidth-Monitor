#include <thread>
#include <unistd.h>

#include "include/Producer.h"
#include "include/Consumer.h"
#include "include/Analyzer.h"
#include "include/Reporter.h"

int main() {
    Producer producer;
    std::thread producer_thread (&Producer::run, &producer);

    Consumer consumer;
    std::thread consumer_thread (&Consumer::run, &consumer);

    Analyzer *analyzer = Analyzer::init();
    std::thread analyzer_thread (&Analyzer::run, analyzer);

    Reporter reporter;
    std::thread reporter_thread (&Reporter::run, &reporter);

    producer_thread.join();
    consumer_thread.join();
    analyzer_thread.join();
    reporter_thread.join();
}