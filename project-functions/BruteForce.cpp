#include <list>
#include <bits/stdc++.h>
#include <chrono>
#include "GRAPH_UTILS\\Event_reader.h"
#include "GRAPH_UTILS\\insertions_deletions.h"

using namespace std;
using ll = long long;

// Function to compute connectivity after processing events
ll compute_connectivity(const string z, const ll total_vertices, vector<vector<ll>> adj, vector<bool> &results) {
    ifstream infile("../events/event_" + z + ".txt");
    if (!infile.is_open()) {
        cerr << "Failed to open file: ../events/event_" + z + ".txt" << endl;
        return 0;
    }

    int T;
    ll v1, v2, total_lines;
    char query;

    if (infile.is_open()) {
        // infile >> T;
        infile >> total_lines;
        while (infile >> query >> v1 >> v2) {
            switch (query) {
                case 'D':
                    remove_edge(v1, v2, adj);
                    break;
                case 'I':
                    insert_edge(v1, v2, adj);
                    break;
                case 'Q': {
                    BridgeGraph b(total_vertices, adj);
                    find_bridges_simple(total_vertices, b.total_neighbours, b.total_bridges);
                    find_bridge_tree_simple(total_vertices, b.total_neighbours, b.bridge_tree_map, b.total_bridges);
                    results.push_back(b.bridge_tree_map[v1] == b.bridge_tree_map[v2]);
                    break;
                }
                default:
                    cerr << "Unknown query type: " << query << endl;
                    break;
            }
        }
        infile.close();
    }
    return total_lines;
}

// Utility to read valid user input
void read_input_range(int &graphFrom, int &graphTo, int &numEventFiles) {
    cout << "Enter graph file from (0-18), graph file to (from-18), and number of event files (1-5): \n";
    while (!(cin >> graphFrom >> graphTo >> numEventFiles) || 
           graphFrom < 0 || graphFrom > 18 || 
           graphTo < graphFrom || graphTo > 18 || 
           numEventFiles < 1 || numEventFiles > 5) {
        cout << "Invalid input. Please enter from (0-18), to (from-18), and number of event files (1-5): ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Prepare the output file for results
ofstream prep_time_file(const string &graph_id) {
    string output_file_path = "time_files/bf_res_" + graph_id + ".txt";
    ofstream output_file(output_file_path, ios::trunc);
    if (!output_file) {
        cerr << "Error: Could not create output file: " << output_file_path << endl;
        exit(1);
    }
    return output_file;
}

ofstream prep_output_file(const string &graph_id, const string &event_id) {
    string output_file_path = "output_files/output_file_" + graph_id + "." + event_id + ".txt";
    ofstream output_file(output_file_path, ios::trunc);
    if (!output_file) {
        cerr << "Error: Could not create output file: " << output_file_path << endl;
        exit(1);
    }
    return output_file;
}

// Process a single event file for a graph
void process_event_file(const string graph_id, const string event_id, ofstream &time_file) {
    cout << "Processing event_" << graph_id << "." << event_id << endl;

    Event ev(graph_id);
    vector<bool> results;
    ll total_vertices = read_total_vertices(graph_id);
    vector<vector<ll>> adj(total_vertices);
    read_graph(graph_id, adj);

    auto start_time = chrono::high_resolution_clock::now();
    ll total_events = compute_connectivity(graph_id + "." + event_id, total_vertices, adj, results);
    auto end_time = chrono::high_resolution_clock::now();
    auto time_spent = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    cout << "Time spent = " << time_spent << " ms (or " << (double)time_spent / 1000.0 << " s)\n";

    time_file << to_string(total_events) + ": " << (double)time_spent / 1000.0 << endl;
	
	ofstream output_file = prep_output_file(graph_id, event_id);
	for (const auto res : results)
    	output_file << res << '\n';

}

// Process all graphs in the given range
void process_graphs(int graphFrom, int graphTo, int numEventFiles) {
    for (int graph_id = graphFrom; graph_id < graphTo; graph_id++) {
        string graph_id_str = to_string(graph_id);
        cout << "Processing graph id = " << graph_id_str << endl;

        ofstream output_file = prep_time_file(graph_id_str);
        for (int event_file_idx = 0; event_file_idx < numEventFiles; event_file_idx++) {
            process_event_file(graph_id_str, to_string(event_file_idx), output_file);
			cout << "Results written to: output_files/outfile_" << graph_id << "." << event_file_idx << ".txt" << endl;
        }

        cout << "Results written to: time_files/bf_res_" << graph_id  << ".txt" << endl;
        output_file.close();
    }
}

// Main function
int main(int argc, char *argv[]) {
    ios::sync_with_stdio(false);
    srand(time(nullptr));
	filesystem::create_directories("time_files");
    filesystem::create_directories("output_files");

    int graphFrom, graphTo, numEventFiles;
    read_input_range(graphFrom, graphTo, numEventFiles);
    process_graphs(graphFrom, graphTo, numEventFiles);
    return 0;
}