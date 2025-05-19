#include <bits/stdc++.h>

#include "brute-force-utils/brute-force-utils.h"
#include "utils-functions/insertions_deletions.h"
// #include "utils-functions/search.h"
#include "main-functions/identical-graph.h"

using namespace std;
using ll = long long;
using cll = const long long;
using c_str = c_str&;

// Function to compute connectivity after processing events
vector<bool> compute_connectivity(const Event& ev) {
    cll total_vertices = ev.get_total_vertices();
    vector<bool> results;
    vector<vector<ll>> adj(total_vertices);
    // int T;
    ll v1, v2, total_lines;
    char query_type;
    const vector<tuple<char, ll, ll>> eventsList = ev.get_eventsList();

    for (const auto& [query_type, v1, v2] : eventsList) {
        switch (query_type) {
            case 'D':
                remove_edge(v1, v2, adj);
                break;
            case 'I':
                insert_edge(v1, v2, adj);
                break;
            case 'Q': {
                Graph_Details graph_details(total_vertices, adj);
                find_bridges_simple(total_vertices, graph_details.total_neighbours, graph_details.total_bridges);
                find_bridge_tree_simple(total_vertices, graph_details.total_neighbours, graph_details.bridge_tree_map, graph_details.total_bridges);
                results.push_back(graph_details.bridge_tree_map[v1] == graph_details.bridge_tree_map[v2]);
                break;
            }
            default:
                cerr << "Unknown query type: " << query_type << endl;
                break;
        };
    }
    return results;
}

// Utility to read valid user input
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
}

// Prepare the output file for results
ofstream prep_time_file(const int graph_id) {
    string output_file_path = "results/time_files/bf_res_" + to_string(graph_id) + ".txt";
    ofstream output_file(output_file_path, ios::trunc);
    if (!output_file) {
        cerr << "Error: Could not create output file: " << output_file_path << endl;
        exit(1);
    }
    return output_file;
}

ofstream prep_output_file(const int graph_id, const int event_id) {
    string output_file_path = "results/output_files/output_file_" + to_string(graph_id) + "." + to_string(event_id) + ".txt";
    ofstream output_file(output_file_path, ios::trunc);
    if (!output_file) {
        cerr << "Error: Could not create output file: " << output_file_path << endl;
        exit(1);
    }
    return output_file;
}

// Process a single event file for a graph
void process_event_file(const int graph_id, const int event_id, ofstream& time_file) {
    cout << "Processing event_" << graph_id << "." << event_id << endl;

    Event ev(graph_id, event_id);
    auto start_time = chrono::high_resolution_clock::now();
    vector<bool> results = compute_connectivity(ev);
    auto end_time = chrono::high_resolution_clock::now();
    auto time_spent = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    cout << "Time spent = " << time_spent << " ms (or " << (double)time_spent / 1000.0 << " s)\n";
    time_file << to_string(ev.get_total_events()) + ": " << (double)time_spent / 1000.0 << endl;
	ofstream output_file = prep_output_file(graph_id, event_id);
	for (const auto& res : results)
    	output_file << res << '\n';

}

// Process all graphs in the given range
void process_graphs(const int graphFrom, const int graphTo, const int eventFrom, int eventTo) {
    for (auto graph_id = graphFrom; graph_id <= graphTo; graph_id++) {
        cout << "Processing graph id = " << graph_id << endl;
        ofstream output_file = prep_time_file(graph_id);
        for (auto event_id = eventFrom; event_id <= eventTo; event_id++) {
            process_event_file(graph_id, event_id, output_file);
			cout << "Results written to: results/output_files/outfile_" << graph_id << "." << event_id << ".txt" << endl;
        }
        cout << "Results written to: results/time_files/bf_res_" << graph_id  << ".txt" << endl;
        output_file.close();
    }
}

// Main function
int main() {
    ios::sync_with_stdio(false);
    srand(time(nullptr));
    filesystem::create_directories("results");
	filesystem::create_directories("results/time_files");
    filesystem::create_directories("results/output_files");

    int graphFrom, graphTo, eventFrom, eventTo;
    prompt_user_input(graphFrom, graphTo, eventFrom, eventTo);
    process_graphs(graphFrom, graphTo, eventFrom, eventTo);
    return 0;
}