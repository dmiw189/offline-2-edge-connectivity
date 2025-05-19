#pragma once
#include "insertions_deletions.h"

#include <bits/stdc++.h>
using ll = long long;
using cll = const long long;
using cb = const bool;
using List = vector<ll>;
using cList = const vector<ll>;
using AugmentedEventsList = vector<tuple<char, ll, ll, ll>>;

void bridge_tree_dfs_simple(cll v, cll total_vertices, vector<bool> &visited, const vector<List> &total_neighbours, 
		List &component, cll &component_number) {
 	visited[v]= true;
	component[v]= component_number;
	const auto adj = total_neighbours[v];
	for (const auto& u : adj)
	   if (!visited[u])
		bridge_tree_dfs_simple(u, total_vertices, visited, total_neighbours, component, component_number);
}

void find_bridge_tree_simple(cll total_vertices, vector<List> &total_neighbours, List &bridge_tree_map, 
			const unordered_set<pair<ll, ll>, hashFunction>& total_bridges) {
	for (auto u : total_bridges)
		remove_edge(u.first, u.second, total_neighbours);

	ll k= 0;
	vector <bool> visited(total_vertices, false);
	bridge_tree_map = List(total_vertices, 0);
	for (int i= 0; i< total_vertices; i++)   
		if (!visited[i]) {
			bridge_tree_dfs_simple(i, total_vertices, visited, total_neighbours, bridge_tree_map, k);	 
			k++;
		}
}

void enhanced_bridge_tree_dfs(cll v, cll total_vertices, vector<bool> &visited,  
			vector<vector<tuple<ll, ll, bool>>> &enhanced_total_neighbours, List &component, cll &component_number){
 	visited[v]= true;
	component[v]= component_number;
	for (auto tuple : enhanced_total_neighbours[v]){
		//ignore the bridges
		if (get<2>(tuple)) continue;

		auto u = get<0>(tuple);
	   	if (!visited[u]){
			enhanced_bridge_tree_dfs(u, total_vertices, visited, enhanced_total_neighbours, component, component_number);
	   	}
	}
}

void find_bridge_tree(cll total_vertices, vector<vector<tuple<ll, ll, bool>>> &enhanced_total_neighbours, List &bridge_tree_map){
	ll k= 0;
	vector <bool> visited(total_vertices, false);
	bridge_tree_map = List(total_vertices, 0);
	for (int i= 0; i< total_vertices; i++) {  
		if (!visited[i]) {
			enhanced_bridge_tree_dfs(i, total_vertices, visited, enhanced_total_neighbours, bridge_tree_map, k);	 
			k++;
		}
	}
}


void dfs_find_bridges(cll v, List &pre, List &low, vector<bool> &visited, vector<pair<ll, ll>> &edges, 
		List &parent, ll total_vertices, ll &time, vector<vector<tuple<ll, ll, bool>>> &en_adj){
	visited[v]= true;
	time++;
	low[v]= time;
	pre[v]= time;
	for (auto &tuple : en_adj[v]){
		auto u = get<0>(tuple);
		if (!visited[u]){
		  parent[u]= v;
		  dfs_find_bridges(u, pre, low, visited, edges, parent, total_vertices, time, en_adj); 

		  // Update low-link value
		  low[v]= min(low[v], low[u]);

		  // Check for bridge
		  if (low[u] > pre[v]){
				get<2>(tuple) = true;
				get<2>(en_adj[u][get<1>(tuple)]) = true;
			  	edges.emplace_back(v, u);
			  	parent[u]= -1;
		  }
		} else if (u != parent[v]){ 
			// Update low-link value	
		  	low[v]= min(low[v], pre[u]);	
		}
	}
}

void find_bridges(const ll total_vertices, vector<vector<tuple<ll, ll, bool>>> &enhanced_total_neighbours, vector<pair<ll, ll>> &edges){
	List pre(total_vertices, int(0));
	List low(total_vertices, int(0));
	ll component_number= 0;
	List parent(total_vertices, int(-1));
	vector<bool> visited(total_vertices, false); 
	ll time= 0;
	
	for (int i= 0; i< total_vertices; i++){
	  if (!visited[i]){ 
		dfs_find_bridges(i, pre, low, visited, edges, parent, total_vertices, time, enhanced_total_neighbours);
	  }
	}
}

void dfs_find_bridges_simple(const ll v, List &pre, List &low, vector<bool> &visited, unordered_set<pair<ll, ll>, hashFunction> &total_bridges, 
		List &parent, ll total_vertices, ll &time, vector<List> &total_neighbours){
	visited[v]= true;
	time++;
	low[v]= time;
	pre[v]= time;
	for (auto u : total_neighbours[v]){
		if (!visited[u]){
		  parent[u]= v;
		  dfs_find_bridges_simple(u, pre, low, visited, total_bridges, parent, total_vertices, time, total_neighbours); 

		  // Update low-link value
		  low[v]= min(low[v], low[u]);

		  // Check for bridge
		  if (low[u] > pre[v]){
			  	total_bridges.insert(make_pair(v, u));
			  	parent[u]= -1;
		  }
		} else if (u != parent[v]){ 
			// Update low-link value	
		  	low[v]= min(low[v], pre[u]);	
		}
	}
}

void find_bridges_simple(const ll total_vertices, vector<List> &total_neighbours, unordered_set<pair<ll, ll>, hashFunction> &total_bridges){
	List pre(total_vertices, int(0));
	List low(total_vertices, int(0));
	ll component_number= 0;
	List parent(total_vertices, int(-1));
	vector<bool> visited(total_vertices, false); 
	ll time= 0;
	
	for (int i= 0; i< total_vertices; i++){
	  if (!visited[i]){ 
		dfs_find_bridges_simple(i, pre, low, visited, total_bridges, parent, total_vertices, time, total_neighbours);
	  }
	}
}
