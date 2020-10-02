#include <pcap.h>
#include <ctime>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
using std::system;
using std::vector;
using std::string;
using std::to_string;
using std::mutex;
using std::thread;
using std::ofstream;
using std::stringstream;
using std::filebuf;
using std::istream;
using std::atol;

struct Data {
/* container for bandwidth use information */

    long int bytes {};
    long int megabytes {};
    long int gigabytes {};  // max registrable value = 2147480 terabytes

    Data(): bytes(0), megabytes(0), gigabytes(0) {}
    Data(long int b, long int mb, long int gb): bytes(b), megabytes(mb), gigabytes(gb) {}

};

/* Globals */
static constexpr long int bytes_in_megabyte {1048576};
static constexpr long int megabytes_in_gigabyte {1024};
struct tm * timeinfo;
bool timer_ticking {false};
mutex write_lock;
ofstream output_file ("packet_monitor.log", ofstream::out | ofstream::trunc & ofstream::app);
Data data_piped;
char current_working_directory[1024];
char* status {getcwd(current_working_directory, 1024)};

/* Function declarations */
int monitor_usage();
void process_packet(u_char*, const struct pcap_pkthdr*, const u_char*);
void write_to_log(struct Data, struct tm*);
void next_write_timer(int);
void prepare_weekly_report();
void update_daily_max_utilization(vector<Data>&, vector<int>&, Data&, int);
bool greater_than(Data&, Data&);
Data compute_last_week_usage(Data[7]);
vector<int> identify_common_timeslots(const vector<vector<int>>&);
vector<int> vector_intersection(const vector<int>&, const vector<int>&);
vector<Data> compute_heavy_use_averages(const vector<vector<Data>>&, const vector<vector<int>>&, const vector<int>&);
string send_email_system_call();

int main() {
/* Start a daemon process that will run in the background to collect and log data on internet usage */

    pid_t pid = fork();

    if (pid < 0)
        return (EXIT_FAILURE);
    else
        if (pid > 0) exit(0);  // parent terminates, child is now a daemon.

    monitor_usage();

}

int monitor_usage() {
/* packet sniffer to capture packets sent and received by the system */

    char err_buf[PCAP_ERRBUF_SIZE];
    pcap_t* handle {};  // session handle

    handle = pcap_open_live(nullptr, BUFSIZ, 0, 0, err_buf);  // open a device for sniffing.

    if (!handle) {

        fprintf(stderr, "Device could not be opened: %s \n", err_buf);
        return (EXIT_FAILURE);

    }

    pcap_loop(handle, -1, process_packet, nullptr);

    pcap_close(handle);
    output_file.close();

    return (EXIT_SUCCESS);

}

void process_packet(u_char* args, const struct pcap_pkthdr* header, const u_char* packet) {

    struct tm* capture_time{localtime(&header->ts.tv_sec)};
    static int days_count {7};
    static int hours_since_decrement {0};
    static bool decrement_possible_flag {true};

    if (!timer_ticking) {

        if(decrement_possible_flag && capture_time->tm_hour == 0) --days_count;

        if(!decrement_possible_flag) {

            ++hours_since_decrement;

            if (hours_since_decrement == 2) {

                decrement_possible_flag = true;
                hours_since_decrement = 0;

            }

        }

        timeinfo = capture_time;
        timer_ticking = true;

        thread timer(next_write_timer, (60 * 60 * 60) - ((timeinfo->tm_min * 60) + timeinfo->tm_sec));
        timer.detach();

        if (days_count == 0) {

            thread report_writer(prepare_weekly_report);
            report_writer.detach();

            days_count = 7;

        }

    }

    write_lock.lock();

    data_piped.bytes += header->len;

    if (data_piped.bytes >= bytes_in_megabyte) {

        ++data_piped.megabytes;
        data_piped.bytes = data_piped.bytes % bytes_in_megabyte;

        if (data_piped.megabytes >= megabytes_in_gigabyte) {

            ++data_piped.gigabytes;
            data_piped.megabytes = data_piped.megabytes % megabytes_in_gigabyte;

        }

    }


    write_lock.unlock();

}

void write_to_log(struct Data pipe_size, struct tm* time) {

    if (output_file.fail()) output_file.clear();

    output_file << pipe_size.bytes << " " << pipe_size.megabytes << " " << pipe_size.gigabytes << " " << asctime(time);
    output_file.flush();

    pthread_exit(nullptr);

}

void next_write_timer(int seconds_counter) {

    sleep(seconds_counter);

    thread write_thread (write_to_log, data_piped, timeinfo);
    write_thread.detach();

    write_lock.lock();

    timer_ticking = false;
    data_piped.bytes = 0;
    data_piped.megabytes = 0;
    data_piped.gigabytes = 0;

    write_lock.unlock();

    pthread_exit(nullptr);

}

void prepare_weekly_report() {

    filebuf input_fb;
    istream input_stream {input_fb.open("packet_monitor.log", std::ios::in)};
    stringstream sstream;

    string tokens[8];  // array{[0]: bytes, [1]: megabytes, [2]: gigabytes, [3]: day, [4]: month, [5]: date, [6]: timestamp, [7]: year}
    string line {};
    string day {};
    string weekdays [6];
    Data daily_usage[7];
    vector<vector<Data>> daily_max_utilization_values (7, vector<Data> (10, Data()));
    vector<vector<int>> daily_max_utilization_hours (7, vector<int> (10, 0));

    int index {0};

    while (input_stream.good()) {

        getline(input_stream, line);
        sstream.str(line);
        if (!(sstream.str()).empty()) {

            for (int i = 0; i < 8; ++i) {

                getline(sstream, tokens[i], ' ');

                if (!sstream.good())
                    if (i < 7) sstream.clear();
            }

            sstream.clear();

        }

        if (day.empty()) day = tokens[3];


        if (day != tokens[3]) {

            weekdays[index] = day;
            day = tokens[3];

            if (index < 6) ++index;

        }

        Data stats_read(atol(tokens[0].c_str()), atol(tokens[1].c_str()), atol(tokens[2].c_str()));

        daily_usage[index].bytes += stats_read.bytes;

        if (daily_usage[index].bytes >= bytes_in_megabyte) {

            ++daily_usage[index].megabytes;
            daily_usage[index].bytes = daily_usage[index].bytes % bytes_in_megabyte;

        }

        daily_usage[index].megabytes += stats_read.megabytes;

        if (daily_usage[index].megabytes >= megabytes_in_gigabyte) {

            ++daily_usage[index].gigabytes;
            daily_usage[index].megabytes = daily_usage[index].megabytes % megabytes_in_gigabyte;

        }

        daily_usage[index].gigabytes += stats_read.gigabytes;

        if (!tokens[6].empty()) update_daily_max_utilization(daily_max_utilization_values.at(index), daily_max_utilization_hours.at(index), stats_read, stoi(tokens[6].substr(0, 2)));

    }

    Data weeks_usage {compute_last_week_usage(daily_usage)};
    vector<int> heavy_use_hours {identify_common_timeslots(daily_max_utilization_hours)};
    vector<Data> heavy_use_stats {compute_heavy_use_averages(daily_max_utilization_values, daily_max_utilization_hours, heavy_use_hours)};
    Data average_daily_usage;


    average_daily_usage.gigabytes = weeks_usage.gigabytes / 7;
    average_daily_usage.megabytes = (weeks_usage.gigabytes % 7) * megabytes_in_gigabyte;
    average_daily_usage.megabytes += weeks_usage.megabytes;
    average_daily_usage.bytes = (average_daily_usage.megabytes % 7) * bytes_in_megabyte;
    average_daily_usage.megabytes = average_daily_usage.megabytes / 7;
    average_daily_usage.bytes = (average_daily_usage.bytes + weeks_usage.bytes) / 7;

    ofstream report_file("report.txt", ofstream::out | ofstream::trunc & ofstream::app);

    string header {"BANDWIDTH MONITOR REPORT (" + tokens[4] + "/" + (to_string(stoi(tokens[5]) - 7)) + "/" + tokens[7]};
    header = header + " - " + tokens[4] + "/" + tokens[5] + "/" + tokens[7] + ")";

    report_file << header;
    report_file << "\n*****************************************************************\n";
    report_file << "\n1. Total Bandwidth used -> " << weeks_usage.gigabytes << " GB, " <<
                weeks_usage.megabytes << " MB, " << (weeks_usage.bytes / 1000) << " KB, " << (weeks_usage.bytes % 10) << " B.\n\n";
    report_file << "2. Daily Usage Statistics \n";

    for (int i = 0; i < 7; ++i) {

        if (i < 6) report_file << "   [+] " <<  tokens[4] + "/"  << (stoi(tokens[5]) - 7) <<
                               "/" + tokens[7] << " (" + weekdays[i] + ")" + " -> " << daily_usage[i].gigabytes << " GB, " << daily_usage[i].megabytes <<
                               " MB, " << (daily_usage[i].bytes / 1000) << " KB, " << (daily_usage[i].bytes % 10) << " B.\n";

        else report_file << "   [+] " <<  tokens[4] + "/" << (stoi(tokens[5]) - 7) << "/" + tokens[7] << " ("+ tokens[3] + ") -> " <<
                         daily_usage[i].gigabytes << " GB, " << daily_usage[i].megabytes << " MB, " << (daily_usage[i].bytes / 1000) <<
                         " KB, " << (daily_usage[i].bytes % 10) << " B.\n";

    }

    report_file << "\n3. Heavy Traffic Timeslots (Weekly averages)\n";

    for (int i = 0; i < heavy_use_hours.size(); ++i) {

        report_file << "   [+] " << heavy_use_hours.at(i) << " : " <<  tokens[6].substr(3, 2) << " : " << tokens[6].substr(6, 2) <<
                    " -> " << (heavy_use_stats.at(i)).gigabytes << " GB, " << (heavy_use_stats.at(i)).megabytes << " MB, " <<
                    ((heavy_use_stats.at(i)).bytes / 1000) << " KB, " << ((heavy_use_stats.at(i)).bytes % 10) << " B.\n";

    }

    report_file << "\n4. Daily Average Bandwidth Used -> " << average_daily_usage.gigabytes << " GB, " << average_daily_usage.megabytes << " MB, " <<
                (average_daily_usage.bytes / 1000) << " KB, " << (average_daily_usage.bytes % 10) << " B.";

    report_file.close();

    string command {send_email_system_call()};
    system(command.c_str());

}

Data compute_last_week_usage(Data daily_usage[7]) {

    Data total_usage;

    for (int i = 0; i < 7; ++i) {

        total_usage.bytes += daily_usage[i].bytes;

        if (total_usage.bytes >= bytes_in_megabyte) {

            ++total_usage.megabytes;
            total_usage.bytes = total_usage.bytes % bytes_in_megabyte;

        }

        total_usage.megabytes += daily_usage[i].megabytes;

        if (total_usage.megabytes >= megabytes_in_gigabyte) {

            ++total_usage.gigabytes;
            total_usage.megabytes = total_usage.megabytes % megabytes_in_gigabyte;

        }

        total_usage.gigabytes += daily_usage[i].gigabytes;

    }

    return total_usage;

}

vector<int> identify_common_timeslots(const vector<vector<int>>& daily_max_utilization_hours) {

    vector <int> time_slots { daily_max_utilization_hours.at(0) };
    for (int i = 1; i < 7; ++i) time_slots = vector_intersection(time_slots, daily_max_utilization_hours.at(i));

    return time_slots;

}

vector<int> vector_intersection(const vector<int>& list1, const vector<int>& list2) {

    vector<int> commons;

    for (int list1_hour: list1) {

        for (int list2_hour: list2) {

            if (list1_hour == list2_hour) {

                commons.push_back(list1_hour);
                break;

            }

        }

    }

    return commons;

}

vector<Data> compute_heavy_use_averages(const vector<vector<Data>>& daily_max_utilization_values, const vector<vector<int>>& daily_max_usage_times, const vector<int>& timeslots) {

    vector<Data> hourly_timeslot_average(timeslots.size(), (timeslots.size(), Data()));

    for (int index = 0; index < timeslots.size(); ++index) {

        for (int i = 0; i < daily_max_utilization_values.size(); ++i) {

            for (int j = 0; j < (daily_max_utilization_values.at(0).size()); ++j) {

                if (timeslots.at(index) == (daily_max_usage_times.at(i)).at(j)) {

                    (hourly_timeslot_average.at(index)).bytes += ((daily_max_utilization_values.at(i)).at(j)).bytes;

                    if ((hourly_timeslot_average.at(index)).bytes >= bytes_in_megabyte) {

                        ++(hourly_timeslot_average.at(index)).megabytes;
                        (hourly_timeslot_average.at(index)).bytes = (hourly_timeslot_average.at(index)).bytes % bytes_in_megabyte;

                    }

                    (hourly_timeslot_average.at(index)).megabytes += ((daily_max_utilization_values.at(i)).at(j)).megabytes;

                    if ((hourly_timeslot_average.at(index)).megabytes >= megabytes_in_gigabyte) {

                        ++(hourly_timeslot_average.at(index)).gigabytes;
                        (hourly_timeslot_average.at(index)).megabytes = (hourly_timeslot_average.at(index)).megabytes % megabytes_in_gigabyte;

                    }

                    (hourly_timeslot_average.at(index)).gigabytes += ((daily_max_utilization_values.at(i)).at(j)).gigabytes;

                    break;

                }

            }

        }

    }

    return hourly_timeslot_average;

}

void update_daily_max_utilization(vector<Data>& max_utilization_data, vector<int>& max_utilization_hours, Data& new_data, int hour) {

    if (greater_than(max_utilization_data.at(9), new_data)) {

        int i {9};

        for (; i > 0; --i) {

            if (i >= 1 && greater_than(max_utilization_data.at(i - 1), new_data)) {

                max_utilization_data.at(i) = max_utilization_data.at(i - 1);
                max_utilization_hours.at(i) = max_utilization_hours.at(i - 1);

            } else break;

        }

        max_utilization_data.at(i) = new_data;
        max_utilization_hours.at(i) = hour;

    }

}

bool greater_than(Data& lhs, Data& rhs) {

    if (rhs.gigabytes > lhs.gigabytes) return true;
    if (rhs.gigabytes == lhs.gigabytes && rhs.megabytes > lhs.megabytes) return true;
    if (rhs.megabytes == lhs.megabytes && rhs.megabytes > lhs.megabytes) return true;
    return false;

}

string send_email_system_call() {

    string command {"cat "};

    stringstream ss;
    ss.str(current_working_directory);

    for (char ch: ss.str()) {

        if (ch == '/') command += ch;
        command += ch;

    }

    command += "//report.txt | mail -s \"Bandwidth Usage Report\" ishantaldekar1@gmail.com";

    return command;

}