#pragma once

namespace MemoryTracker {
    // Conversion helper
    inline double bytes_to_mb(size_t bytes) {
        return bytes / (1024.0 * 1024.0);
    }

    template<typename T>
    size_t estimate_memory(const T& obj) {
        return sizeof(obj);
    }

    template<typename T>
    size_t estimate_memory(const vector<T>& v) {
        return sizeof(v) + (v.capacity() * sizeof(T));
    }

    template<>
    size_t estimate_memory(const vector<bool>& v) {
        return sizeof(v) + (v.capacity() / 8);
    }

    template<typename K, typename V>
    size_t estimate_memory(const unordered_map<K,V>& m) {
        return sizeof(m) + (m.size() * (sizeof(K) + sizeof(V) + 32));
    }

    class ArgMemoryTracker {
        struct ArgStats {
            size_t total = 0;
            size_t max = 0;
            size_t count = 0;
            
            void update(size_t size) {
                total += size;
                max = std::max(max, size);
                count++;
            }
        };

        static inline unordered_map<string, unordered_map<string, ArgStats>> stats;
        
        public:
            template<typename... Args>
            static void log(const string& context, const Args&... args) {
                const vector<string> arg_labels = {"graph_details", "components_map"};
                size_t i = 0;
                ((stats[context][arg_labels[i++]].update(estimate_memory(args))), ...);
            }
            
            static void print_report() {
                cout << "-------------------------------------------------" << endl;
                cout << "-------------------------------------------------" << endl;
                cout << "=== Per-Argument Memory Analysis (All values in MB) ===\n";
                for (const auto& [ctx, arg_data] : stats) {
                    cout << "\nContext: " << ctx << " Memory Usage:\n";
                    cout << "---------------------------------------------\n";
                    cout << left << setw(20) << "Argument" 
                        << setw(20) << "Total" 
                        << setw(20) << "Avg" 
                        << setw(20) << "Max" << "\n";
                    
                    for (const auto& [arg_name, data] : arg_data) {
                        cout << setw(20) << arg_name 
                            << setw(20) << fixed << setprecision(3) << bytes_to_mb(data.total)
                            << setw(20) << bytes_to_mb(data.count ? data.total/data.count : 0)
                            << setw(20) << bytes_to_mb(data.max) << "\n";
                    }
                }
                cout << "---------------------------------------------\n";
                cout << "(1 MB = 1024 KB = 1048576 bytes)\n";
                cout << "-------------------------------------------------" << endl;
                cout << "-------------------------------------------------" << endl;
            }
            
            static void reset() {
                stats.clear();
            }
    };

    #define MEM_START() MemoryTracker::ArgMemoryTracker::reset()
    #define MEM_LOG(ctx, ...) MemoryTracker::ArgMemoryTracker::log(ctx, __VA_ARGS__)
    #define MEM_REPORT() MemoryTracker::ArgMemoryTracker::print_report()

}