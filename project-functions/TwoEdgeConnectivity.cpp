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

void process_single_event(Event &ev, ll index, vector<bool> &results, const vector<ll> &component) {
    if (get<0>(ev.total_augmented_events[index]) == 'Q') {
        ll u = get<1>(ev.total_augmented_events[index]);
        ll v = get<2>(ev.total_augmented_events[index]);
        results.push_back(component[u] == component[v]);

        cout << "u and v: " << u << " " << v << endl;
    }
}

void compute_static_component_edges(BridgeGraph bridge_graph, Event ev, vector<pair<ll, ll>> &static_component_edges, bool isleft, vector<ll> components) {
    const auto &static_edges = isleft ? ev.static_left : ev.static_right;
	ll first_component, second_component; 

	// cout<< "---------Static Component Edges from the " << (isleft ? "left" : "right") << " side for the " << (!isleft ? "left" : "right")<< "--------------" << endl;
    for (const auto &edge : static_edges) {
        first_component = components[edge.first];
        second_component = components[edge.second];
		// cout<< "[" << first_component << ", " << second_component << "]" << endl; 
        if (first_component != second_component) {
            static_component_edges.emplace_back(first_component, second_component);
        }
    }
	// cout<< "----------------------\n";
}

void compute_active_component_nodes(BridgeGraph bridge_graph, Event ev, vector<bool> &active_component_nodes, bool isleft, const vector<ll>* components_ptr) {
	const auto &active_nodes_list = isleft ? ev.left_active_nodes_list : ev.right_active_nodes_list;
	for (auto u : active_nodes_list) {
		auto component = (*components_ptr)[u];
        active_component_nodes[component] = true;
    }
}

void compute_2_edge_connectivity(BridgeGraph bridge_graph, Event ev, ll start_index, ll end_index, vector <bool> &results, vector<ll> left_components, vector<ll> right_components, bool right= false){
	
	if (!ev.query_exists_in_events(start_index, end_index)) return;
	
	cout.clear();
	cout << "Total events: [start= " << start_index << ", end= " << end_index << "] from " << (right ? "RIGHT" : "LEFT") << " side" << endl;
	cout.setstate(ios_base::failbit);
	if (start_index == end_index) {
		process_single_event(ev, start_index, results, left_components);
		return;
    }
	
	bridge_graph.test_print_graph(bridge_graph.total_neighbours, bridge_graph.total_vertices, "At the beginning----");
	
	ev.clear_active_nodes();
	ev.clear_static_edges();

	
	BridgeGraph right_bridge_graph = bridge_graph;
	bridge_graph.set_component(left_components);
	right_bridge_graph.set_component(right_components);

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

	ev.find_active_nodes(left_start, left_end, true);
	ev.find_active_nodes(right_start, right_end, false);
	vector<bool> left_active_component_nodes(bridge_graph.total_vertices, false);
	vector<bool> right_active_component_nodes(bridge_graph.total_vertices, false);

	auto start_time = chrono::high_resolution_clock::now();
	compute_active_component_nodes(bridge_graph, ev, left_active_component_nodes, true, &left_components);
	compute_active_component_nodes(right_bridge_graph, ev, right_active_component_nodes, false, &right_components);
	auto end_time = chrono::high_resolution_clock::now();
	auto time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	time_active.push_back(time);
	cout.clear();
	cout << "Active component nodes: ";
	print_time_now(time);
	cout.setstate(ios_base::failbit);

	
	start_time = chrono::high_resolution_clock::now();
	ev.find_static_edges(start_index, end_index); // ev.print_static_edges();
	compute_static_component_edges(bridge_graph, ev, left_static_component_edges, true, left_components);
	add_edges_to_graph(left_static_component_edges, bridge_graph.total_neighbours);
	compute_static_component_edges(right_bridge_graph, ev, right_static_component_edges, false, right_components);
	add_edges_to_graph(right_static_component_edges, right_bridge_graph.total_neighbours);
	end_time = chrono::high_resolution_clock::now();
	// cout.clear();
	// cout << "Static Edges: ";
	// print_time_now(time_active);
	// cout.setstate(ios_base::failbit);
    
	time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	time_static.push_back(time);

	// test_print(bridge_graph, ev, left_start, left_end, true);
	// test_print(right_bridge_graph, ev, right_start, right_end, false);

	start_time = chrono::high_resolution_clock::now();
	bridge_graph.construct_bridges(left_active_component_nodes);
	right_bridge_graph.construct_bridges(right_active_component_nodes);
	end_time = chrono::high_resolution_clock::now();
    time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	time_bridges.push_back(time);

	start_time = chrono::high_resolution_clock::now();
	final_pruning(bridge_graph, ev.left_active_nodes_list);
	final_pruning(right_bridge_graph, ev.right_active_nodes_list);
	end_time = chrono::high_resolution_clock::now();
    time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	time_pruning.push_back(time);
	
	bridge_graph.clear();	
	right_bridge_graph.clear();

	compute_2_edge_connectivity(bridge_graph, ev, left_start, left_end, results, left_components, left_components);
	compute_2_edge_connectivity(right_bridge_graph, ev, right_start, right_end, results, right_components, right_components, true);
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

		Event ev(z, bridge_graph.total_vertices);
		ev.read_event_file();

		cout << "Start the Program with input: " << z << endl;
		cout.setstate(ios_base::failbit);

		vector<ll> component(bridge_graph.total_vertices);
		iota(component.begin(), component.end(), 0);

		ll start_index= 0;
		ll end_index= ev.total_augmented_events.size()-1; 

		compute_2_edge_connectivity(bridge_graph, ev, start_index, end_index, results, component, component);

		cout.clear();
		print_time(time_active, "Active Nodes");
		print_time(time_static, "Static Edges");
		print_time(time_bridges, "Bridges");
		print_time(time_pruning, "Pruning");
		
		string output_file = "output_files/output_" + z + ".txt"; 
		vector<bool> correct_results = read_from_file(output_file);

		cout << "----Results---\n"; 
		for (bool res : results){
			cout << res << endl;
		}

		cout << (correct_results == results ? "CORRECT!!!" : "WRONG!!!") << endl;
	}
	return 0;
}