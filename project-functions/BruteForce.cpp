#include <bits/stdc++.h>
#include "brute-force-utils/brute-force-event-utils.h"
#include "brute-force-utils/brute-force-graph-utils.h"
#include "utils-functions/insertions_deletions.h"

using namespace std;
using ll = long long;
using cll = const long long;
using List = vector<ll>;
using c_i = const int;

ofstream create_out_time_file(c_i graph_id, const string& file_prefix) {
    string output_file_path = "results/time_files/" + file_prefix + "_res_" 
        + to_string(graph_id) + ".txt";
    ofstream output_file(output_file_path, ios::trunc);
    if (!output_file) {
        cerr << "Error: Could not create output file: " << output_file_path << endl;
        exit(1);
    }
    return output_file;
}

ofstream create_output_file(c_i graph_id, c_i event_id, const string& algorithm_name) {
    string output_file_path = "results/output_files/" + algorithm_name + "_" 
        + to_string(graph_id) + "." + to_string(event_id) + ".txt";
    ofstream output_file(output_file_path, ios::trunc);
    if (!output_file) {
        cerr << "Error: Could not create output file: " << output_file_path << endl;
        exit(1);
    }
    return output_file;
}

vector<bool> compute_brute_force_connectivity(const Event& ev) {
    vector<bool> results;
    cll total_vertices = ev.get_total_vertices();
    vector<List> graph_repr(total_vertices); 
    Bf_Graph_Utils gd;

    const vector<tuple<char, ll, ll>> eventsList = ev.get_eventsList();

    for (const auto& [query_type, v1, v2] : eventsList) {
        switch (query_type) {
            case 'D':
                remove_edge(v1, v2, graph_repr);
                break;
            case 'I':
                insert_edge(v1, v2, graph_repr);
                break;
            case 'Q': {
                gd.algorithm(total_vertices, graph_repr);
                bool are_connected = gd.get_bridge_map()[v1] == gd.get_bridge_map()[v2];
                results.push_back(are_connected);
                gd.clear_map();
                break;
            }
            default:
                cerr << "Unknown query type: " << query_type << endl;
                break;
        };
    }
    return results;
}

void process_brute_force(c_i graph_id, c_i event_id, ofstream& time_file) {
    Event ev(graph_id, event_id);
    auto start_time = chrono::high_resolution_clock::now();
    vector<bool> bf_results = compute_brute_force_connectivity(ev);
    auto end_time = chrono::high_resolution_clock::now();
    auto time_spent = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    cout << "Time spent = " << time_spent << " ms (or " << (double)time_spent / 1000.0 << " s)\n";
    time_file << to_string(ev.get_total_events()) + ": " << (double)time_spent / 1000.0 << endl;
    ofstream output_file = create_output_file(graph_id, event_id, "output_file");
	for (const auto& res : bf_results)
    	output_file << res << '\n';
    
    output_file.close();
}

void process_graphs(c_i graphFrom, c_i graphTo, c_i eventFrom, c_i eventTo) {
    for (auto graph_id = graphFrom; graph_id <= graphTo; graph_id++) {
        cout << "Processing graph id = " << graph_id << endl;
        ofstream time_file = create_out_time_file(graph_id, "bf");
        for (auto event_id = eventFrom; event_id <= eventTo; event_id++) {
            cout << "Processing event_" << graph_id << "." << event_id << endl;
            process_brute_force(graph_id, event_id, time_file);
			cout << "Results written to: results/output_files/outfile_" 
                << graph_id << "." << event_id << ".txt" << endl;
        }
        time_file.close();
        cout << "Results written to: results/time_files/bf_res_" << graph_id  << ".txt" << endl;
    }
}

void prompt_user_input(int& graph_from, int& graph_to, int& event_from, int& event_to) {
    cout << "Enter graph id from [0-8], to [from-8] and event id from [0-4], to [from - 4]: ";
    while (!(cin >> graph_from >> graph_to >> event_from >> event_to) 
            || graph_from < 0 || graph_from > 8 || graph_to < graph_from || graph_to > 8 
            || event_from < 0 || event_from > 4 || event_to < event_from || event_to > 5) {
        cout << "Invalid input. raph id from [0-8], to [from-8]. Enter event id from [0-5], to [from - 5: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int main() {
    ios::sync_with_stdio(false);
    srand(time(nullptr));
    filesystem::create_directories("results");
	filesystem::create_directories("results/time_files");
    filesystem::create_directories("results/output_files");
    int graphFrom, graphTo, eventFrom, eventTo;
    prompt_user_input(graphFrom, graphTo, eventFrom, eventTo);
    process_graphs(graphFrom, graphTo, eventFrom, eventTo);
    process_graphs(256, 257, eventFrom, eventTo);
    return 0;
}