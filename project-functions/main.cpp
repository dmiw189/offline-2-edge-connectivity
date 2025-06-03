#include <bits/stdc++.h>

#include "main-functions/two_edge_connectivity.h"
#include "events-functions/events_reader.h"

using namespace std;
using ll = long long;
using cll = const long long;
using cb = const bool;
using c_str = const string&;
using List = vector<ll>;
using cList = const vector<ll>;
using AugmentedEventsList = vector<tuple<char, ll, ll, ll>>;

inline bool allMeasuremTime;
inline chrono::time_point<std::chrono::high_resolution_clock> rec_start_t = chrono::high_resolution_clock::now();
inline chrono::time_point<std::chrono::high_resolution_clock> rec_end_t = chrono::high_resolution_clock::now();

inline AugmentedEventsList eventsList;
inline EventsHandler ev_handler;

// ─────────────────────────────────────────────────────────────
// Input handling [Inclusive]
// ─────────────────────────────────────────────────────────────
void prompt_user_input(int& graph_from, int& graph_to, int& event_from, int& event_to) {
    cout << "Enter graph id from [0-8], to [from-8] and event id from [0-4], to [from - 4]: ";
    while (!(cin >> graph_from >> graph_to >> event_from >> event_to) ||
           graph_from < 0 || graph_from > 8 ||
           graph_to < graph_from || graph_to > 8 ||
           event_from < 0 || event_from > 4 || event_to < event_from || event_to > 5) {
        cout << "Invalid input. raph id from [0-8], to [from-8]. Enter event id from [0-5], to [from - 5: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    const unordered_set<string> yes_answers = {"y", "Y", "yes", "Yes"};
    string measure_tests;
    cout << "Do you want to measure the tests (y/n): ";
    cin >> measure_tests;
    allMeasuremTime = yes_answers.count(measure_tests);   
}

// ─────────────────────────────────────────────────────────────
// ID Mapping
// ─────────────────────────────────────────────────────────────
unordered_map<ll, ll> create_identity_map(const ll total_vertices) {
    unordered_map<ll, ll> map;
    for (ll i = 0; i < total_vertices; ++i) 
        map[i] = i;
    return map;
}

// ─────────────────────────────────────────────────────────────
// Output Validation
// ─────────────────────────────────────────────────────────────
bool validate_results(c_str graph_id, c_str event_id, const vector<bool>& results) {
    auto id = graph_id + "." + event_id;
    auto file_name = "results/output_files/output_file_" + id + ".txt";
    vector<bool> expected = read_results_from_file(file_name);
    auto correct = expected == results;
    
    ofstream out("results/output_files/my_output_" + id + ".txt");
    if (!out) {
        cerr << "Failed to open output file for graph " << id << ".\n";
        exit;
    }

    if (correct) out << "Correct Answers" <<  endl;
    else {
        out << "Wrong Answers" << endl << "Expected -> Result" << endl;
        for (auto i = 0; i < results.size(); i++) 
            out << expected[i] << "->" << results[i] << (results[i] != expected[i] ? "!!!" : "") << endl;
    }
    out << endl;
    out.close();
    return correct;
}

// ─────────────────────────────────────────────────────────────
// Timing Measurement
// ─────────────────────────────────────────────────────────────
ll compute_and_time(Graph& graph, vector<bool>& results, const unordered_map<ll, ll>& components_map) {
    cll ev_start = 0, ev_end = eventsList.size() - 1;
    
    auto start = chrono::high_resolution_clock::now();
    compute_2_edge_connectivity(ev_start, ev_end, graph, results, components_map);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

// ─────────────────────────────────────────────────────────────
// Save Answer
// ─────────────────────────────────────────────────────────────
void print_and_save(bool isCorrect, ofstream& time_output) {
    auto answer = (isCorrect ? "Correct" : "Wrong") + string(" Output!");
    cout << answer << endl;
    time_output << answer << endl;
}


// ─────────────────────────────────────────────────────────────
// Initialize the events list
// ─────────────────────────────────────────────────────────────
void initialize_events_list(c_str graph_id, c_str event_id) {
    eventsList.clear();
    EventsLoader events_loader;
    eventsList = events_loader.getTotalEvents(graph_id, event_id);
}

// ─────────────────────────────────────────────────────────────
// Individual Test Case Execution
// ─────────────────────────────────────────────────────────────
void run_test(c_str graph_id, c_str event_id, ofstream& time_output) {
    cout << "Input events ID = " + graph_id + "." + event_id << endl;
    
    Graph graph(graph_id);
    cll total_vertices = graph.get_total_vertices();
    const unordered_map<ll, ll> components_map = create_identity_map(total_vertices);
    // graph.set_components_map(components_map);

    initialize_events_list(graph_id, event_id);
    ev_handler = EventsHandler(total_vertices);

    vector<bool> results;

    ll elapsed_time = compute_and_time(graph, results, components_map);
    
    double elapsed_time_in_seconds = static_cast<double>(elapsed_time) / 1000.0;
    cout << "Total Time: " << elapsed_time_in_seconds << "s\n";
    time_output << eventsList.size() << ": " << elapsed_time_in_seconds << "s. ";

    bool correct = validate_results(graph_id, event_id, results);
    print_and_save(correct, time_output);
}

// Full Range Test Execution
// ─────────────────────────────────────────────────────────────
// ─────────────────────────────────────────────────────────────
void run_all_tests(const int graph_from, const int graph_to, const int event_from, const int event_to) {
    const string output_dir = "results/time_files";
    filesystem::create_directory(output_dir);

    for (int graph_index = graph_from; graph_index <= graph_to; graph_index++) {
        string graph_id = to_string(graph_index);
        cout << "Input graph ID = " << graph_id << endl;

        ofstream out(output_dir + "/my_res_" + graph_id + ".txt");
        if (!out) {
            cerr << "Failed to open output file for graph " << graph_id << ".\n";
            exit;
        }

        for (int event_id = event_from; event_id <= event_to; event_id++)
            run_test(graph_id, to_string(event_id), out);
    }
}

// ─────────────────────────────────────────────────────────────
// Entry Point
// ─────────────────────────────────────────────────────────────
int main() {
    ios::sync_with_stdio(false);
    srand(time(nullptr));
    ofstream null_stream;
    auto old_cout_buf = std::cout.rdbuf();
    

    string repeat;
    const unordered_set<string> yes_answers = {"y", "Y", "yes", "Yes"};


    int graph_from, graph_to, event_from, event_to;
    do {
        prompt_user_input(graph_from, graph_to, event_from, event_to);

        // cout.rdbuf(null_stream.rdbuf());
        run_all_tests(graph_from, graph_to, event_from, event_to);
        // cout.rdbuf(old_cout_buf);
        
        cout << "Do you want another one? (y/n): ";
        cin >> repeat;
        cout << endl;
    } while (yes_answers.count(repeat));

    run_all_tests(256, 257, 0, 4);
    
    return 0;
}
