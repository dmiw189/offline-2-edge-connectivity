#pragma once

extern AugmentedEventsList eventsList;

class Logs_and_Printers {
    public: 
        static void test_print_eventsList(cll start, cll end) {
            cout << "---------- Printing Eventslist --------------" << endl;
            for(ll i = start; i <= end; i++) {
                const auto& [type, u, v, w] = eventsList[i];
                cout << "(" << type << ", " << u << ", " << v << ", " << w << ")" << endl;
            }
            cout << "---------- End of printing Eventslist --------------" << endl;
        }

        static void log_memory_usage(const std::string& name, size_t bytes) {
            cout << "[Memory] " << name << ": " << bytes << " bytes (" 
                << bytes/1024 << " KB, " << bytes/(1024*1024) << " MB)\n";
        }
        
        static void print_duration(chrono::nanoseconds ns, const string& label) {
            if (ns.count() == 0) {
                cout << "No timing data available for " << label << ".\n";
                return;
            }
            cout << "Time at " << label << ": " << fixed << setprecision(2) << ns.count() / 1e9 << " s\n";
        }
};
