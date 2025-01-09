#include "GRAPH_UTILS\\Pruning.h"
#include <chrono>

using ll = long long;

vector<ll> time_active, time_static, time_bridges, time_pruning;


void test_print(BridgeGraph &bridge_graph, Event &ev, ll start_index, ll end_index, bool left) {
    const string side = left ? "left" : "right";
    //cout << "-------Total " << side << " events [" << start_index << ", " << end_index << "]-------" << endl;
    for (int i = start_index; i <= end_index; ++i) {
        auto &event = ev.total_augmented_events[i];
        //cout << get<0>(event) << " " << get<1>(event) << " " << get<2>(event) << " " << get<3>(event) << endl;
    }
    //cout << "-----------------\n\n";

    //cout << "-------" << side << " active nodes [" << start_index << ", " << end_index << "]-------" << endl;
    auto &active_nodes = left ? ev.left_active_nodes : ev.right_active_nodes;
    for (int i = 0; i < bridge_graph.total_vertices; ++i) {
        //cout << i << ": " << (active_nodes[i] ? "Active" : "Inactive") << endl;
    }
    //cout << "--------------------\n";
}

void print_time(vector<ll> &time, string text) {
    if (time.empty()) {
        cout << "No timing data available.\n";
        return;
    }

    auto total_time = accumulate(time.begin(), time.end(), 0LL);
    auto avg_time = total_time / time.size();
    auto [min_time, max_time] = minmax_element(time.begin(), time.end());

	cout << "Time spent at " + text + ":\n"; 
    cout << "Total Time: " << total_time << " ms\n";
    cout << "Average Time: " << avg_time << " ms\n";
    cout << "Minimum Time: " << *min_time << " ms\n";
    cout << "Maximum Time: " << *max_time << " ms\n";
	time.clear();
}


void print_time_now(ll time) {
    //cout << "Total Time: " << time << " ms\n";
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
	//cout << " Single Process = [" << umap.at(x) << ", " << umap.at(y) << "]" << endl;
}

void compute_static_component_edges(const vector<pair<ll, ll>> &static_edges,  vector<pair<ll, ll>> &static_component_edges, 
        const bool isleft, const unordered_map<ll, ll> &umap) {
    
	ll comp_1, comp_2; 

    //cout << "---------Static Component Edges from the " << (isleft ? "left" : "right") << " side for the " << (!isleft ? "left" : "right") << "--------------" << endl;

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

        //cout << "[" << comp_1 << ", " << comp_2 << "]" << endl; 

        if (comp_1 != comp_2) {
            static_component_edges.emplace_back(comp_1, comp_2);
        }
    }
    //cout << "----------------------\n";
}

void compute_active_component_nodes(const vector<ll> active_nodes_list, vector<bool> &active_component_nodes, const bool isleft, const unordered_map<ll, ll> &umap) {
	for (auto u : active_nodes_list) {
        active_component_nodes[umap.at(u)] = true;
    }
}

void compute_2_edge_connectivity(BridgeGraph bridge_graph, Event ev, const ll start_index, const ll end_index, vector<bool>& results, 
                                 unordered_map<ll, ll> umap_left, unordered_map<ll, ll> umap_right) {
    
	// Base check: if no events exist in the range, return
    if (!ev.query_exists_in_events(start_index, end_index)) return;

    // cout << "[" << start_index << ", " << end_index << "]" << endl;

    // Handle single-event case
    if (start_index == end_index) {
        process_single_event(ev, start_index, results, umap_left);
        return;
    }

    // Initial setup
    bridge_graph.print_enhanced_graph("At the beginning----");
    ev.clear_active_nodes();
    ev.clear_static_edges();

    BridgeGraph right_bridge_graph = bridge_graph;
    bridge_graph.set_umap(umap_left);
    right_bridge_graph.set_umap(umap_right);

    // Determine left and right ranges
    ll left_start = start_index;
    ll left_end = left_start + floor((end_index - start_index) / 2);
    ll right_start = left_end + 1;
    ll right_end = end_index;

    // Initialize static component edges for left and right
    vector<pair<ll, ll>> left_static_component_edges, right_static_component_edges;

    // Query event existence for left and right ranges
    bool leftExists = ev.query_exists_in_events(left_start, left_end);
    bool rightExists = ev.query_exists_in_events(right_start, right_end);

    // Lambda to handle processing for each side
    auto process_side = [&](bool exists, auto& ev_nodes_list, auto& static_edges, auto& static_component_edges,
                            auto& umap, BridgeGraph& graph, ll start, ll end, bool isLeft) {
        if (!exists) return;

        // Find active nodes and compute active component nodes
        ev.find_active_nodes(start, end, isLeft);
        vector<bool> active_component_nodes(graph.total_vertices, false);
        compute_active_component_nodes(ev_nodes_list, active_component_nodes, isLeft, umap);

        // Find and compute static component edges
        ev.find_static_edges(start_index, end_index, isLeft);
        compute_static_component_edges(static_edges, static_component_edges, isLeft, umap);

        // Add static component edges to graph
        add_edges_to_graph(static_component_edges, graph.enhanced_total_neighbours);

        // Construct bridges and prune
        graph.construct_bridges(active_component_nodes);
        final_pruning(graph, ev_nodes_list);

        // Clear the graph
        graph.clear();

        // Recursive call for further subdivisions
        compute_2_edge_connectivity(graph, ev, start, end, results, umap, umap);
    };

    // Process left and right sides
    process_side(leftExists, ev.left_active_nodes_list, ev.static_left, left_static_component_edges,
                 umap_left, bridge_graph, left_start, left_end, true);

    process_side(rightExists, ev.right_active_nodes_list, ev.static_right, right_static_component_edges,
                 umap_right, right_bridge_graph, right_start, right_end, false);
}

int main(int argc, char *argv[]){
	while(true) {
		string z;
		cout << "Give the input id" << endl;
		cin >> z;

		if (z == "exit") {
			break;
		}
		
		vector<bool> results;
		BridgeGraph bridge_graph(z);

		auto vertices = bridge_graph.total_vertices;	

		Event ev(z, vertices);
		ev.read_event_file();

		// cout << "Start Program: " << z << endl;
		//cout.setstate(std::ios_base::failbit);

		unordered_map<ll, ll> umap;
		for (auto i = 0; i < vertices; i++) {
			umap[i] =  i;
		}

		auto start_index= 0;
		auto end_index= ev.total_augmented_events.size()-1; 

		compute_2_edge_connectivity(bridge_graph, ev, start_index, end_index, results, umap, umap);

		//cout.clear();
		// print_time(time_active, "Active Nodes");
		// print_time(time_static, "Static Edges");
		// print_time(time_bridges, "Bridges");
		// print_time(time_pruning, "Pruning");
		
		string output_file = "output_files/output_" + z + ".txt"; 
		vector<bool> correct_results = read_from_file(output_file);

		//cout << "----Results---\n"; 
		// for (bool res : results){
		// 	//cout << res << endl;
		// }

		cout << (correct_results == results ? "CORRECT output!" : "WRONG output!") << endl;
	}
	return 0;
}