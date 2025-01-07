#include "Event_reader.h"

using ll = long long;

void prune_inactive_bridge_nodes_1(BridgeGraph &b_object){
  vector <ll> temp_leaves;
  ll parent;
  b_object.removed.resize(b_object.total_bridge_vertices, false);
  
  while (true){
	for (auto lid = b_object.leaves.begin(); lid != b_object.leaves.end(); lid++){
	  if (!b_object.bridge_active_nodes[*lid]){
		b_object.removed[*lid]= true;
		parent= b_object.total_bridge_neighbours[*lid][0]; 
		b_object.total_bridge_degree[*lid] -= 1;
		b_object.total_bridge_degree[parent] -= 1;
		b_object.total_bridge_neighbours[*lid].clear();
		
		//TODO the time complexity is WRONG (2)
		b_object.total_bridge_neighbours[parent].erase(
			find(b_object.total_bridge_neighbours[parent].begin(), b_object.total_bridge_neighbours[parent].end(), *lid)
		);
		if (b_object.total_bridge_degree[parent] == 1 && !b_object.bridge_active_nodes[parent]){
			temp_leaves.push_back(parent);
		}		 
	  }	  
	}
	if (temp_leaves.empty()) {
    	break;
	} 
	b_object.leaves = move(temp_leaves);
  }
}

void prune_inactive_bridge_nodes_2(BridgeGraph &b_object){
  ll n1, n2;  
  for (ll v= 0; v< b_object.total_bridge_vertices; v++){
	 if (!b_object.bridge_active_nodes[v] && b_object.total_bridge_degree[v] == 2){
		n1= b_object.total_bridge_neighbours[v][0]; 
		n2= b_object.total_bridge_neighbours[v][1];
		
		b_object.removed[v]= true;
		b_object.total_bridge_neighbours[v].clear();
		b_object.total_bridge_degree[v] == 0;

		//time complexity is WRONG (3)
		for (auto& n : {n1, n2}) {
			auto& neighbours = b_object.total_bridge_neighbours[n];
			auto it = find(neighbours.begin(), neighbours.end(), v);
			if (it != neighbours.end()) {
				neighbours.erase(it);
			}
		}

		b_object.total_bridge_neighbours[n1].push_back(n2);
		b_object.total_bridge_neighbours[n2].push_back(n1);
	 }
  }
  
}

void prune_inactive_bridge_nodes_0(BridgeGraph &b_object){
  for (ll v= 0; v< b_object.total_bridge_vertices; v++){
	 if (!b_object.bridge_active_nodes[v] && b_object.total_bridge_degree[v] == 0){
		b_object.removed[v]= true;
		b_object.total_bridge_neighbours[v].clear(); 
	 }
  }
}

void create_removed_bridged_map(BridgeGraph &b_object, vector<ll> &removed_map){
	ll counter_non_removed_nodes= 0;
	ll u_map, v_map;
	
	//Create a Temporary Map for Non-Removed Nodes
	vector<ll> temp_remove_bridge_tree_map(b_object.total_bridge_vertices);
  
    // Populate temp_remove_bridge_tree_map
	for (ll v = 0; v < b_object.total_bridge_vertices; v++) {
		temp_remove_bridge_tree_map[v] = !b_object.removed[v] ? counter_non_removed_nodes++ : -1;
	}

	// Copy temp_remove_bridge_tree_map to b_object.remove_bridge_tree_map
	removed_map.insert(removed_map.end(), temp_remove_bridge_tree_map.begin(), temp_remove_bridge_tree_map.end());

	vector<vector<ll>> temp_bridge_pruned_neighbours(counter_non_removed_nodes); 
	for (ll v = 0; v < b_object.total_bridge_vertices; v++) {
		if (b_object.removed[v]) continue;

		v_map = removed_map[v];
        for (auto u : b_object.total_bridge_neighbours[v]) {
        	u_map = removed_map[u];
            if (u_map != -1) {
                temp_bridge_pruned_neighbours[v_map].push_back(u_map);
            }
        }
	}

	b_object.total_vertices= counter_non_removed_nodes;
    b_object.total_neighbours = move(temp_bridge_pruned_neighbours);
}

void print_removed_map(const vector<ll> removed_map) {
    cout << "Size of removed_map: " << removed_map.size() << endl;

    cout << "Contents of removed_map: ";
    for (const auto& value : removed_map) {
        cout << value << " ";
    }
    cout << endl;
}

void print_temp_component(const vector<ll>& temp_component) {
    cout << "Size of temp_component: " << temp_component.size() << endl;

    cout << "Contents of temp_component: ";
    for (const auto& value : temp_component) {
        cout << value << " ";
    }
    cout << endl;
}

void print_for_error(ll node, ll component_node, vector<ll> bridge_tree_map, vector<ll> pruned_map) {
		ll value = bridge_tree_map[component_node];
		ll value_value = pruned_map[value]; 
		cout << "orig node=" << node << ", ";
		cout << "orig comp= " << component_node << ", ";
		cout << "b_tree comp= " << value << ", ";
		cout << "pruned comp= " << value_value << endl << "---------------------";
}

ll original_res_mapping(ll component_node, vector<ll> bridge_tree_map, vector<ll> pruned_map) {  
	return pruned_map[bridge_tree_map[component_node]]; 
}

void update_components(BridgeGraph &b_object, vector<ll> pruned_map, vector<ll> active_nodes_list) {
        vector<ll>& temp_component = b_object.get_component();

		// print_temp_component(temp_component);
		// print_removed_map(pruned_map);

        for (const auto &u : active_nodes_list) {
            try {
                temp_component[u] = original_res_mapping(temp_component[u], b_object.bridge_tree_map, pruned_map);
            } catch (const exception& e) {
                cerr << "Unexpected error updating component for node " << u << ": " << e.what() << endl;
				print_for_error(u, temp_component[u], b_object.bridge_tree_map, pruned_map);
                continue;
            }
        }
}


void final_pruning(BridgeGraph &b_object, vector <ll> active_nodes_list){
	prune_inactive_bridge_nodes_1(b_object);
	prune_inactive_bridge_nodes_2(b_object);
	prune_inactive_bridge_nodes_0(b_object);

	vector<ll> pruned_map;

	create_removed_bridged_map(b_object, pruned_map);	
	update_components(b_object, pruned_map, active_nodes_list);

	b_object.test_print_components("after component update");
	b_object.test_print_graph(b_object.total_neighbours, b_object.total_vertices, "New neighbours(= pruned)");

}
