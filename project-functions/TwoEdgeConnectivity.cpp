#include "GRAPH_UTILS\\Pruning.h"
#include <chrono>

using ll = long long;

vector<ll> time_active, time_static, time_bridges, time_pruning;


void test_print(BridgeGraph &bridge_graph, Event &ev, ll start_index, ll end_index, bool left) {
    const string side = left ? "left" : "right";
    cout << "-------Total " << side << " events [" << start_index << ", " << end_index << "]-------" << endl;
    for (int i = start_index; i <= end_index; ++i) {
        auto &event = ev.total_augmented_events[i];
        cout << get<0>(event) << " " << get<1>(event) << " " << get<2>(event) << " " << get<3>(event) << endl;
    }
    cout << "-----------------\n\n";

    cout << "-------" << side << " active nodes [" << start_index << ", " << end_index << "]-------" << endl;
    auto &active_nodes = left ? ev.left_active_nodes : ev.right_active_nodes;
    for (int i = 0; i < bridge_graph.total_vertices; ++i) {
        cout << i << ": " << (active_nodes[i] ? "Active" : "Inactive") << endl;
    }
    cout << "--------------------\n";
}

void signalHandler(int signal) {
    cerr << "Error: Signal " << signal << " received." << endl;
    exit(signal);
}

void print_time(vector<ll> &time, string text) {
    if (time.empty()) {
        cout << "No timing data available.\n";
        return;
    }

    ll total_time = accumulate(time.begin(), time.end(), 0LL);
    ll avg_time = total_time / time.size();
    auto [min_time, max_time] = minmax_element(time.begin(), time.end());

	cout << "Time spent at " + text + ":\n"; 
    cout << "Total Time: " << total_time << " ms\n";
    cout << "Average Time: " << avg_time << " ms\n";
    cout << "Minimum Time: " << *min_time << " ms\n";
    cout << "Maximum Time: " << *max_time << " ms\n";
	time.clear();
}


void print_time_now(ll time) {
    cout << "Total Time: " << time << " ms\n";
}

vector<bool> read_from_file(string file_name) {
	ifstream file(file_name); 
    if (!file) {
        cerr << "Error: Could not open output file: " << file_name << endl;
        exit;
    }
    vector<bool> file_results;
    bool value;
    while (file >> value) {
        file_results.push_back(value);
    }
    file.close();
	return file_results;
}

void process_single_event(const Event &ev, ll index, vector<bool> &results, const unordered_map<ll, ll> &umap) {
    auto [type, x, y, z] = ev.total_augmented_events[index];
	if (type == 'Q') {
		results.push_back(umap.at(x) == umap.at(y));
	}
	cout << " Single Process = [" << umap.at(x) << ", " << umap.at(y) << "]" << endl;
}

void compute_static_component_edges(const vector<pair<ll, ll>> &static_edges,  vector<pair<ll, ll>> &static_component_edges, 
        const bool isleft, const unordered_map<ll, ll> &umap) {
    
	ll comp_1, comp_2; 

    cout << "---------Static Component Edges from the " << (isleft ? "left" : "right") 
        << " side for the " << (!isleft ? "left" : "right") << "--------------" << endl;

    for (const auto [u, v] : static_edges) {
        try {
            // Try to access the components
            comp_1 = umap.at(u);
            comp_2 = umap.at(v);
        } catch (const out_of_range &e) {
            // Handle missing keys in the map
            cerr << "Error: Node not found in map. Node pair: (" << u << ", " << v << ")\n";
            cerr << "Exception: " << e.what() << endl;
            continue; // Skip to the next edge
        }

        cout << "[" << comp_1 << ", " << comp_2 << "]" << endl; 

        if (comp_1 != comp_2) {
            static_component_edges.emplace_back(comp_1, comp_2);
        }
    }
    cout << "----------------------\n";
}

void compute_active_component_nodes(const vector<ll> active_nodes_list, vector<bool> &active_component_nodes, const bool isleft, const unordered_map<ll, ll> &umap) {
	for (auto u : active_nodes_list) {
        active_component_nodes[umap.at(u)] = true;
    }
}

void compute_2_edge_connectivity(BridgeGraph bridge_graph, Event ev, const ll start_index, const ll end_index, vector <bool> &results, 
		unordered_map<ll, ll> umap_left, unordered_map<ll, ll> umap_right, bool right= false){
	
	if (!ev.query_exists_in_events(start_index, end_index)) return;
	
	auto start = chrono::high_resolution_clock::now();
	cout.clear();
	cout << "Total events: [start= " << start_index << ", end= " << end_index << "] from " << (right ? "RIGHT" : "LEFT") << " side. " << endl;
	cout.setstate(std::ios_base::failbit);

	if (start_index == end_index) {
		process_single_event(ev, start_index, results, umap_left);
		return;
    }
	
	bridge_graph.print_enhanced_graph("At the beginning----");
	
	ev.clear_active_nodes();
	ev.clear_static_edges();

	BridgeGraph right_bridge_graph = bridge_graph;

	bridge_graph.set_umap(umap_left);
	right_bridge_graph.set_umap(umap_right);

	ll left_start= start_index;
	ll left_end = left_start + floor((end_index - start_index)/2);
	ll right_start= left_end + 1;
	ll right_end= end_index;
	vector<pair<ll, ll>> left_static_component_edges, right_static_component_edges;

	// if (ev.query_exists_in_events(left_start, left_end)) {
	// 	return;
	// }
	// if (ev.query_exists_in_events(right_start, right_end)) {
	// 	return;
	// }

	auto start_time = chrono::high_resolution_clock::now();

	ev.find_active_nodes(left_start, left_end, true);
	ev.find_active_nodes(right_start, right_end, false);

	vector<bool> left_active_component_nodes(bridge_graph.total_vertices, false);
	vector<bool> right_active_component_nodes(bridge_graph.total_vertices, false);

	compute_active_component_nodes(ev.left_active_nodes_list, left_active_component_nodes, true, umap_left);
	compute_active_component_nodes(ev.right_active_nodes_list, right_active_component_nodes, false, umap_right);
	
	auto end_time = chrono::high_resolution_clock::now();
	auto time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	time_active.push_back(time);

	
	start_time = chrono::high_resolution_clock::now();
	ev.find_static_edges(start_index, end_index); 
	// ev.print_static_edges();
	compute_static_component_edges(ev.static_left, left_static_component_edges, true, umap_left);
	add_edges_to_graph(left_static_component_edges, bridge_graph.enhanced_total_neighbours);
	compute_static_component_edges(ev.static_right, right_static_component_edges, false, umap_right);
	add_edges_to_graph(right_static_component_edges, right_bridge_graph.enhanced_total_neighbours);
	end_time = chrono::high_resolution_clock::now();
	time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	time_static.push_back(time);

	test_print(bridge_graph, ev, left_start, left_end, true);
	test_print(right_bridge_graph, ev, right_start, right_end, false);

	start_time = chrono::high_resolution_clock::now();
	cout << "-----Start Bridge Tree Stage-------" << endl;
	cout << "--------Left-------" << endl;
	bridge_graph.construct_bridges(left_active_component_nodes);
	cout << "--------Right-------" << endl;
	right_bridge_graph.construct_bridges(right_active_component_nodes);
	cout << "-----End Bridge Tree Stage-------" << endl;
	end_time = chrono::high_resolution_clock::now();
    time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	time_bridges.push_back(time);

	start_time = chrono::high_resolution_clock::now();

	cout << "-----Start Pruning Stage-------" << endl;
	cout << "--------Left-------" << endl;
	final_pruning(bridge_graph, ev.left_active_nodes_list);
	cout << "--------Right-------" << endl;
	final_pruning(right_bridge_graph, ev.right_active_nodes_list);
	cout << "-----End Pruning Stage-------" << endl;

	end_time = chrono::high_resolution_clock::now();
    time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	time_pruning.push_back(time);
	
	bridge_graph.clear();	
	right_bridge_graph.clear(); 

	// auto end = chrono::high_resolution_clock::now();
	// time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	// cout.clear();
	// cout << "Time Spent = " << time << "ms\n";
	// cout.setstate(std::ios_base::failbit);

	compute_2_edge_connectivity(bridge_graph, ev, left_start, left_end, results, umap_left, umap_left);
	compute_2_edge_connectivity(right_bridge_graph, ev, right_start, right_end, results, umap_right, umap_right, true);
}


int main(int argc, char *argv[]){

	signal(SIGSEGV, signalHandler); 
    signal(SIGABRT, signalHandler); 

	while(true) {
		string z;
		cout << "You need to give the id (as number) of the graph inside the graph folder" << endl;
		cin >> z;

		if (z == "exit") {
			break;
		}
		
		vector<bool> results;
		BridgeGraph bridge_graph(z);

		auto vertices = bridge_graph.total_vertices;	

		Event ev(z, vertices);
		ev.read_event_file();

		cout << "Start the Program with input: " << z << endl;
		cout.setstate(std::ios_base::failbit);

		unordered_map<ll, ll> umap;
		for (auto i = 0; i < vertices; i++) {
			umap[i] =  i;
		}

		auto start_index= 0;
		auto end_index= ev.total_augmented_events.size()-1; 

		compute_2_edge_connectivity(bridge_graph, ev, start_index, end_index, results, umap, umap);

		cout.clear();
		print_time(time_active, "Active Nodes");
		print_time(time_static, "Static Edges");
		print_time(time_bridges, "Bridges");
		print_time(time_pruning, "Pruning");
		
		string output_file = "output_files/output_" + z + ".txt"; 
		vector<bool> correct_results = read_from_file(output_file);

		cout << "----Results---\n"; 
		// for (bool res : results){
		// 	cout << res << endl;
		// }

		cout << (correct_results == results ? "CORRECT!!!" : "WRONG!!!") << endl;
	}
	return 0;
}