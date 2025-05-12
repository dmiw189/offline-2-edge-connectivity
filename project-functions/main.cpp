#include <bits/stdc++.h>
#include "include/two_edge_connectivity.h"

using namespace std;
using ll = long long;

// ─────────────────────────────────────────────────────────────
// Input handling
// ─────────────────────────────────────────────────────────────
void prompt_user_input(int& from, int& to, int& numEventFiles) {
    cout << "Enter from (0-8), to (from-8), and number of event files (1-5): ";
    while (!(cin >> from >> to >> numEventFiles) ||
           from < 0 || from > 8 ||
           to < from || to > 8 ||
           numEventFiles < 1 || numEventFiles > 5) {
        cout << "Invalid input. Please enter from (0-8), to (from-8), and number of event files (1-5): ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ─────────────────────────────────────────────────────────────
// ID Mapping
// ─────────────────────────────────────────────────────────────
unordered_map<ll, ll> create_identity_map(ll total_vertices) {
    unordered_map<ll, ll> map;
    for (ll i = 0; i < total_vertices; ++i) map[i] = i;
    return map;
}

// ─────────────────────────────────────────────────────────────
// Output Validation
// ─────────────────────────────────────────────────────────────
bool validate_results(const string graph_id, const string event_id, const vector<bool>& results) {
    vector<bool> expected = read_from_file("output_files/output_file_" + graph_id + "." + event_id + ".txt");
    return results == expected;
}

// ─────────────────────────────────────────────────────────────
// Timing Measurement
// ─────────────────────────────────────────────────────────────
ll compute_and_time(BridgeGraph& bridge_graph, Event& event, vector<bool>& results, const unordered_map<ll, ll>& map) {
    auto start = chrono::high_resolution_clock::now();
    compute_2_edge_connectivity(bridge_graph,event, 0, event.total_augmented_events.size() - 1, results, map, map);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

// ─────────────────────────────────────────────────────────────
// Duration Printer
// ─────────────────────────────────────────────────────────────
void print_all_timers() {
    print_duration(time_query_exists, "Query Exists");
    print_duration(time_clear_events, "Clear Events");
    print_duration(time_set_up_graph, "Set Up Graph");
    print_duration(time_active_nodes, "Active Nodes");
    print_duration(time_active_component_nodes, "Active Component Nodes");
    print_duration(time_static_edges, "Static Edges");
    print_duration(time_static_component_edges, "Static Component Edges");
    print_duration(time_add_edges, "Add Edges");
    print_duration(time_bridges, "Bridges");
    print_duration(time_pruning, "Pruning");
    print_duration(time_clear_graph, "Clear Graph");
    clear_time_measurements();
}

// ─────────────────────────────────────────────────────────────
// Individual Test Case Execution
// ─────────────────────────────────────────────────────────────
void run_test(const string& graph_id, int event_index, ofstream& time_output) {
    cout << "Input events ID = " << graph_id << "." << event_index << endl;

    BridgeGraph graph(graph_id);
    Event ev(graph_id, graph.total_vertices);
    ev.read_event_file(to_string(event_index));

    vector<bool> results;
    auto map = create_identity_map(graph.total_vertices);

    ll elapsed = compute_and_time(graph, ev, results, map);
    double seconds = static_cast<double>(elapsed) / 1000.0;

    cout << "Total Time: " << elapsed << "ms\n";
    time_output << ev.total_events << ": " << seconds << "sec, ";

    bool correct = validate_results(graph_id, to_string(event_index), results);
    auto answer = (correct ? "Correct" : "Wrong") + string(" Output!");
    cout << answer << endl;
    time_output << answer << endl;

    print_all_timers();
}

// ─────────────────────────────────────────────────────────────
// Full Range Test Execution
// ─────────────────────────────────────────────────────────────
void run_all_tests(int from, int to, int num_files) {
    const string output_dir = "time_files";
    filesystem::create_directory(output_dir);

    for (int graph_index = from; graph_index < to; ++graph_index) {
        string id = to_string(graph_index);
        cout << "Input graph ID = " << id << endl;

        ofstream out(output_dir + "/my_res_" + id + ".txt");
        if (!out) {
            cerr << "Failed to open output file for graph " << id << ".\n";
            continue;
        }

        for (int i = 0; i < num_files; ++i)
            run_test(id, i, out);
    }
}

// ─────────────────────────────────────────────────────────────
// Entry Point
// ─────────────────────────────────────────────────────────────
int main() {
    ios::sync_with_stdio(false);
    srand(time(nullptr));

    int from, to, num_files;
    prompt_user_input(from, to, num_files);
    run_all_tests(from, to, num_files);

    return 0;
}
