#include <bits/stdc++.h>
#include "brute-force-utils/brute-force-event-utils.h"
#include "brute-force-utils/brute-force-graph-utils.h"
#include "utils-functions/insertions_deletions.h"

using namespace std;
using ll = long long;
using cll = const long long;
using List = vector<ll>;
using c_i = const int;

vector<bool> compute_other_brute_force_connectivity(const Event& ev) {
    vector<bool> results;
    cll total_vertices = ev.get_total_vertices();
    const vector<tuple<char, ll, ll>> eventsList = ev.get_eventsList();
    //init graph
    
    for (const auto& [query_type, v1, v2] : eventsList) {
        switch (query_type) {
            case 'D':
                //remove edge
                break;
            case 'I':
                //insert edge
                break;
            case 'Q': {
                //compute query and answer
                bool random_answer = true;
                results.push_back(random_answer); 
                break;
            }
            default:
                cerr << "Unknown query type: " << query_type << endl;
                break;
        };
    }
    return results;
}

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

ofstream prep_time_file(c_i graph_id, const string& file_prefix) {
    string output_file_path = "results/time_files/" + file_prefix + "_res_" + to_string(graph_id) + ".txt";
    ofstream output_file(output_file_path, ios::trunc);
    if (!output_file) {
        cerr << "Error: Could not create output file: " << output_file_path << endl;
        exit(1);
    }
    return output_file;
}

bool validate_results(c_str graph_id, c_str event_id, const vector<bool>& results) {
    string id = graph_id + "." + event_id;
    ofstream out("results/output_files/euler_output_" + id + ".txt");
    if (!out) {
        cerr << "Failed to open output file for graph " << id << ".\n";
        exit;
    }
    auto file_name = "results/output_files/output_file_" + id + ".txt";
    vector<bool> expected = read_results_from_file(file_name);
    
    bool correct = expected == results;
    if (correct) {
        out << "Correct Answers" <<  endl;
    } else {
        out << "Wrong Answers" << endl << "Expected -> Result" << endl;
        for (auto i = 0; i < results.size(); i++) 
            out << expected[i] << "->" << results[i] << (results[i] != expected[i] ? "!!!" : "") << endl;
    }

    out << endl;
    out.close();

    return correct;
}

ofstream create_output_file(c_i graph_id, c_i event_id, const string& algorithm_name) {
    string output_file_path = "results/output_files/" + algorithm_name + "_" + to_string(graph_id) + "." + to_string(event_id) + ".txt";
    ofstream output_file(output_file_path, ios::trunc);
    if (!output_file) {
        cerr << "Error: Could not create output file: " << output_file_path << endl;
        exit(1);
    }
    return output_file;
}

void process_other_brute_force(c_i graph_id, c_i event_id) {
    Event ev(graph_id, event_id);
    
    auto start_time = chrono::high_resolution_clock::now();

    vector<bool> euler_results = compute_other_brute_force_connectivity(ev);

    auto end_time = chrono::high_resolution_clock::now();
    auto time_spent = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    cout << "Time spent = " << time_spent << " ms (or " << (double)time_spent / 1000.0 << " s)\n";
    
    ofstream time_file = prep_time_file(graph_id, "euler");
    time_file << to_string(ev.get_total_events()) + ": " << (double)time_spent / 1000.0 << endl;
	
    validate_results(to_string(graph_id), to_string(event_id), euler_results);
    time_file.close();
}


void process_graphs(c_i graphFrom, c_i graphTo, c_i eventFrom, int eventTo) {
    for (auto graph_id = graphFrom; graph_id <= graphTo; graph_id++) {
        cout << "Processing graph id = " << graph_id << endl;
        for (auto event_id = eventFrom; event_id <= eventTo; event_id++) {
            cout << "Processing event_" << graph_id << "." << event_id << endl;

            process_other_brute_force(graph_id, event_id);

			cout << "Results written to: results/output_files/outfile_" 
                << graph_id << "." << event_id << ".txt" << endl;
        }
        cout << "Results written to: results/time_files/bf_res_" << graph_id  << ".txt" << endl;
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
    return 0;
}