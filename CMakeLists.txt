cmake_minimum_required(VERSION 3.16)
project(Bandwidth_Monitor)

add_subdirectory(lib)

add_executable(
        Bandwidth_Monitor
        src/Config.cpp
        src/PacketQueue.cpp
        src/Packet.cpp
        src/Usage.cpp
        src/Analyzer.cpp
        src/Reporter.cpp
        src/Consumer.cpp
        src/Producer.cpp
        src/Monitor.cpp
)

target_link_libraries(Bandwidth_Monitor PRIVATE pcap)