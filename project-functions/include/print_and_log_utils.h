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


        static void print_components(c_str label, const unordered_map<ll, ll>& components_map) {
            cout << "-----------------Start of " << label << "-------------------------\n";
            unordered_map<ll, vector<ll>> grouped;

            // Group vertices by component ID
            for (const auto& [vertex, component_id] : components_map) {
                grouped[component_id].push_back(vertex);
            }

            // Print each component group
            for (const auto& [comp_id, vertices] : grouped) {
                cout << "[" << comp_id << ": ";
                for (const auto& v : vertices)
                    cout << v << ", ";
                cout << "]" << endl;
            }
            cout << "---------------End of " << label << "---------------------\n";
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
