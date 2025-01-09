#include <list>
#include <bits/stdc++.h>
#include <chrono>
#include "GRAPH_UTILS\\Event_reader.h"
#include "GRAPH_UTILS\\insertions_deletions.h"

using namespace std;
using ll = long long;

void compute_connectivity(string z, ll total_vertices, vector<vector<ll>> adj, vector<bool> &results) {
	ifstream infile("..\\events\\events_"+z+".txt");
	int T;
	ll v1, v2, total_lines;
	char query;

	if(infile.is_open()){
		infile >> T; 		
		infile >> total_lines;
		while ((infile >> query >> v1 >> v2)){
		  switch (query){
			case 'D':
				// cout << "Removed " << v1 << " " << v2 << endl;
				remove_edge(v1, v2, adj);
				break;
			
			case 'I':
				// cout << "Added " << v1 << " " << v2 << endl;
				insert_edge(v1, v2, adj);
				break;
			
			case 'Q':
				// cout << "Check Biconnectivity " << v1 << " " << v2 << endl;
				BridgeGraph b(total_vertices, adj);
				find_bridges_simple(total_vertices, b.total_neighbours, b.total_bridges);	  
				find_bridge_tree_simple(total_vertices, b.total_neighbours, b.bridge_tree_map, b.total_bridges);
				results.push_back(b.bridge_tree_map[v1] == b.bridge_tree_map[v2]);
				break;
		  }  
		}
		infile.close();
	}
}
int main(int argc, char *argv[]){

	// cout.setstate(ios_base::failbit);

	
	cout << "Give the input id" << endl;
	
	string z;
	cin >> z;
   	Event ev(z);

	vector<bool> results;
	ll total_vertices= read_total_vertices(z);
	vector<vector<ll>> adj(total_vertices);
	read_graph(z, adj);
	
	compute_connectivity(z, total_vertices, adj, results);

	// cout.clear();
	
	// Create the output directory if it doesn't exist
    const string output_dir = "output_files";
    filesystem::create_directory(output_dir);

    // Generate the output file path
    string output_file = output_dir + "/output_" + z + ".txt";

    // Write results to the file
    ofstream file(output_file);
    if (!file) {
        cerr << "Error: Could not create output file: " << output_file << endl;
        return 1;
    }

    for (bool result : results) {
        file << result << "\n";
    }
    cout << "Results written to: " << output_file << endl;

 	return 0;
}