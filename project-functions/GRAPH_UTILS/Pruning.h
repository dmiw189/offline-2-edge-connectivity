#include "Event_reader.h"

using ll = long long;

void print_removed_map(const vector<ll> removed_map) {
    //cout << "Size of removed_map: " << removed_map.size() << endl;

    //cout << "Contents of removed_map: ";
    for (const auto& value : removed_map) {
        //cout << value << " ";
    }
    //cout << endl;
}

void print_temp_component(const vector<ll>& temp_component) {
    //cout << "Size of temp_component: " << temp_component.size() << endl;

    //cout << "Contents of temp_component: ";
    for (const auto& value : temp_component) {
        //cout << value << " ";
    }
    //cout << endl;
}

void print_for_error(ll node, ll component_node, vector<ll> bridge_tree_map, vector<ll> pruned_map) {
		ll value = bridge_tree_map[component_node];
		ll value_value = pruned_map[value]; 
		//cout << "orig node=" << node << ", ";
		//cout << "orig comp= " << component_node << ", ";
		//cout << "b_tree comp= " << value << ", ";
		//cout << "pruned comp= " << value_value << endl << "---------------------";
}


void prune_inactive_bridge_nodes_1(BridgeGraph &b_object){
  	queue<ll> temp_leaves;
  	ll parent;

  	b_object.removed.resize(b_object.total_bridge_vertices, false);
  
  	while (true){
		while (!b_object.leaves.empty()) {
			auto leaf = b_object.leaves.front();
			b_object.leaves.pop();
			if (!b_object.bridge_active_nodes[leaf]){
				b_object.removed[leaf]= true;
				b_object.total_bridge_degree[leaf] -= 1;

				ll parent = -1;
				for (const auto u : b_object.total_bridge_neighbours[leaf]) {
					if (!b_object.removed[u]) {
						parent = u;
						break;
					}
				}
				b_object.total_bridge_neighbours[leaf].clear();

				if (parent != -1) {
					b_object.total_bridge_degree[parent] -= 1;
					if (b_object.total_bridge_degree[parent] == 1 && !b_object.bridge_active_nodes[parent]){
						temp_leaves.push(parent);
					}
				}	
			}	  
		}
		if (temp_leaves.empty()) {
			break;
		} 
		swap(b_object.leaves, temp_leaves);
	}
}

void prune_inactive_bridge_nodes_2(BridgeGraph &b_object){
  ll n1, n2;  
  for (auto v= 0; v< b_object.total_bridge_vertices; v++){
	 if (!b_object.bridge_active_nodes[v] && b_object.total_bridge_degree[v] == 2){
		
		for (auto u : b_object.total_bridge_neighbours[v]) {
			if (!b_object.removed[u]) {
				n1 = u;
			}
		}
		for (auto &u : b_object.total_bridge_neighbours[v]) {
			if (!b_object.removed[u]) {
				if (u == n1) continue;
				n2 = u;
			}
		}
		
		b_object.removed[v]= true;
		b_object.total_bridge_neighbours[v].clear();
		b_object.total_bridge_degree[v] == -1;

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
		b_object.total_bridge_degree[v] == -1;
	 }
  }
}

void extract_edges(const vector<vector<ll>> &adj, vector<pair<ll, ll>> &edges) {
    for (auto u = 0; u < adj.size(); u++)  
        for (const auto v : adj[u])       
            if (u < v) edges.emplace_back(u, v);
}

void update_graph(BridgeGraph &b_object, vector<ll> &removed_map){
	ll counter_non_removed_nodes= 0, u_map, v_map;
	  
	for (ll v = 0; v < b_object.total_bridge_vertices; v++) {
		removed_map[v] = !b_object.removed[v] ? counter_non_removed_nodes++ : -1;
	}

	vector<vector<ll>> temp_bridge_pruned_neighbours(counter_non_removed_nodes); 
	for (ll v = 0; v < b_object.total_bridge_vertices; v++) {
		if (b_object.removed[v]) continue;

		v_map = removed_map[v];
        for (auto u : b_object.total_bridge_neighbours[v]) {
			if (removed_map[u] == -1) continue;
			u_map = removed_map[u];
            if (u_map != -1) {
                temp_bridge_pruned_neighbours[v_map].push_back(u_map);
            }
        }
	}

	b_object.total_vertices= counter_non_removed_nodes;
	vector<pair<ll, ll>> edges;
	extract_edges(temp_bridge_pruned_neighbours, edges);
	b_object.enhanced_total_neighbours = vector<vector<tuple<ll, ll, bool>>>(counter_non_removed_nodes);
	add_edges_to_graph(edges, b_object.enhanced_total_neighbours);
}

ll original_res_mapping(const ll component_node, const vector<ll> &bridge_tree_map, const vector<ll> &pruned_map) {  
	return pruned_map[bridge_tree_map[component_node]]; 
}

void update_components(const vector<ll> &bridge_tree_map, unordered_map<ll, ll> &umap, vector<ll> &pruned_map, const vector<ll> &original_active_nodes_list) {
    unordered_map<ll, ll> temp_umap;
	for (const auto u : original_active_nodes_list) {
        temp_umap[u] = original_res_mapping(umap[u], bridge_tree_map, pruned_map);
    }
	umap = move(temp_umap);
}

void final_pruning(BridgeGraph &b_object, const vector<ll> &original_active_nodes_list){
	prune_inactive_bridge_nodes_1(b_object);
	prune_inactive_bridge_nodes_2(b_object);
	prune_inactive_bridge_nodes_0(b_object);

	vector<ll> pruned_map(b_object.total_bridge_vertices);

	update_graph(b_object, pruned_map);	
	update_components(b_object.bridge_tree_map, b_object.get_umap(), pruned_map, original_active_nodes_list);

	b_object.test_print_components("after component update");
	b_object.print_enhanced_graph("New neighbours(= pruned)");

}
