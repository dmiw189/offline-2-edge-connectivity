#pragma once
#include "insertions_deletions.h"

#include <bits/stdc++.h>
using ll = long long;
using cll = const long long;
using List = vector<ll>;

struct hashFunction {
   size_t operator()(const pair<int ,int> &x) const{return x.first ^ x.second;}
};

void dfs_bridge_tree(cll v, cll total_vertices, vector<bool>& visited,  
			vector<vector<tuple<ll, ll, bool>>>& en_graph_repr, List& components_map, cll component_number) {
 	visited[v] = true;
	components_map[v] = component_number;
	for (auto tuple : en_graph_repr[v]) {
		//ignore the bridges
		if (get<2>(tuple)) 
			continue;
		auto u = get<0>(tuple);
	   	if (!visited[u])
			dfs_bridge_tree(u, total_vertices, visited, en_graph_repr, components_map, component_number);
	}
}

void calculate_bridge_map(cll total_vertices, vector<vector<tuple<ll, ll, bool>>> &en_graph_repr, List& components_map) {
	ll component_number = 0;
	vector <bool> visited(total_vertices, false);
	components_map = List(total_vertices, 0);
	for (int i = 0; i < total_vertices; i++) 
		if (!visited[i]) {
			dfs_bridge_tree(i, total_vertices, visited, en_graph_repr, components_map, component_number);	 
			component_number++;
		}

}

void dfs_find_bridges(cll v, List &pre, List &low, vector<bool>& visited, vector<pair<ll, ll>>& bridge_edges, 
		List& parent, ll& time, vector<vector<tuple<ll, ll, bool>>>& en_graph_repr){
	visited[v] = true;
	time++;
	low[v] = time;
	pre[v] = time;
	for (auto& tuple : en_graph_repr[v]) {
		auto u = get<0>(tuple);
		if (!visited[u]) {
		  parent[u] = v;
		  dfs_find_bridges(u, pre, low, visited, bridge_edges, parent, time, en_graph_repr); 
		  low[v]= min(low[v], low[u]);
		  if (low[u] > pre[v]) {
				get<2>(tuple) = true;
				get<2>(en_graph_repr[u][get<1>(tuple)]) = true;
			  	bridge_edges.emplace_back(v, u);
			  	parent[u] = -1;
		  }
		} else if (u != parent[v])
		  	low[v] = min(low[v], pre[u]);	
	}
}

void mark_bridge_edges(const ll total_vertices, vector<vector<tuple<ll, ll, bool>>> &en_graph_repr, vector<pair<ll, ll>>& bridge_edges) {
	List pre(total_vertices, int(0));
	List low(total_vertices, int(0));
	List parent(total_vertices, int(-1));
	vector<bool> visited(total_vertices, false); 
	ll component_number = 0, time = 0;
	for (int i = 0; i < total_vertices; i++) 
	  if (!visited[i])
		dfs_find_bridges(i, pre, low, visited, bridge_edges, parent, time, en_graph_repr);
}