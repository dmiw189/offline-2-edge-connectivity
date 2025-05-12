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

	int file_number = 0;

	while(file_number < 5) {

		string z = to_string(file_number);
		cout << "input graph id="+z << endl;		

		int file_sub_number = 0;
		while(file_sub_number < 5) {

			Event ev(z);
			vector<bool> results;
			ll total_vertices= read_total_vertices(z);
			vector<vector<ll>> adj(total_vertices);
			read_graph(z, adj);

			string w = to_string(file_sub_number);

			cout << "input events id= "+z+"."+w << endl;
		
			auto start_time = chrono::high_resolution_clock::now();
			compute_connectivity(z + "." + w, total_vertices, adj, results);

			
			auto end_time = chrono::high_resolution_clock::now();
			auto time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
			cout << "Time spent=" << time << "ms (or " << (double) time /1000.0 << "s)\n";

			// cout.clear();
			
			// Create the output directory if it doesn't exist
			const string output_dir = "output_files";
			filesystem::create_directory(output_dir);

			string output_file = output_dir + "/output_" + z + "." + w + ".txt";
			ofstream file(output_file);
			if (!file) {
				cerr << "Error: Could not create output file: " << output_file << endl;
				return 1;
			}

			for (bool result : results) {
				file << result << "\n";
			}
			cout << "Results written to: " << output_file << endl;
			file_sub_number++;
		}
		file_number++;
	}
 	return 0;
}