#pragma once

#include <bits/stdc++.h>
#include "../utils-functions/insertions_deletions.h"

using namespace std;
using ll = long long;
using cll = const long long;
using List = vector<ll>;

struct hashFunction {
   size_t operator()(const pair<int ,int> &x) const{return x.first ^ x.second;}
};

class Bf_Graph_Utils {  

  public:         
  	Bf_Graph_Utils() {
	}
	
    List get_bridge_map() const {
        return bridge_tree_map;
    }

	void clear_map() {
		bridge_tree_map.clear();
	}
	
	void algorithm(cll total_vertices, vector<List> graph_repr) {
        unordered_set<pair<ll, ll>, hashFunction> bridges_set;
		find_bridges_simple(total_vertices, graph_repr, bridges_set);
        find_bridge_tree_simple(total_vertices, graph_repr, bridge_tree_map, bridges_set);
		bridges_set.clear();
	}

	private:
		List bridge_tree_map; 

		void dfs_find_bridges_simple(cll v, List& pre, List& low, vector<bool>& visited, 
			unordered_set<pair<ll, ll>, hashFunction>& bridge_edges, List& parent, ll& time, const vector<List>& graph_repr) {
			visited[v] = true;
			time++;
			low[v] = time;
			pre[v] = time;
			for (auto u : graph_repr[v])
				if (!visited[u]) {
				parent[u] = v;
				dfs_find_bridges_simple(u, pre, low, visited, bridge_edges, parent, time, graph_repr); 
				low[v]= min(low[v], low[u]);
				if (low[u] > pre[v]) {
						bridge_edges.insert(make_pair(v, u));
						parent[u] = -1;
				}
				} else if (u != parent[v]) {
					low[v] = min(low[v], pre[u]);
				}	
		}

		void find_bridges_simple(cll total_vertices, const vector<List>& graph_repr, unordered_set<pair<ll, ll>, hashFunction>& bridge_edges) {
			List pre(total_vertices, int(0));
			List low(total_vertices, int(0));
			List parent(total_vertices, int(-1));
			vector<bool> visited(total_vertices, false); 
			ll component_number = 0, time = 0;
			for (int i = 0; i < total_vertices; i++)
			if (!visited[i])
				dfs_find_bridges_simple(i, pre, low, visited, bridge_edges, parent, time, graph_repr);
		}

		void dfs_bridge_tree_simple(cll v,  vector<bool>& visited, const vector<List>& graph_repr, List& components_map, cll component_number) {
			visited[v] = true;
			components_map[v] = component_number;
			for (const auto& u : graph_repr[v])
			if (!visited[u])
				dfs_bridge_tree_simple(u, visited, graph_repr, components_map, component_number);
		}

		void find_bridge_tree_simple(cll total_vertices, vector<List>& graph_repr, List& components_map, 
					const unordered_set<pair<ll, ll>, hashFunction>& bridge_edges) {
			for (auto u : bridge_edges)
				remove_edge(u.first, u.second, graph_repr);
			ll component_number= 0;
			vector <bool> visited(total_vertices, false);
			components_map = List(total_vertices, 0);
			for (int i= 0; i < total_vertices; i++)   
				if (!visited[i]) {
					dfs_bridge_tree_simple(i, visited, graph_repr, components_map, component_number);	 
					component_number++;
				}
		}
};