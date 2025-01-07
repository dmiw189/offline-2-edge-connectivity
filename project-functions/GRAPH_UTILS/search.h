#pragma once

#include <bits/stdc++.h>
#include "insertions_deletions.h"

using namespace std;
using ll = long long;


void bridge_tree_dfs(ll v, ll total_vertices, vector<bool> &visited, vector< vector <ll> > &total_neighbours, vector<ll> &component, ll &component_number)
{
 	visited[v]= true;
	component[v]= component_number;
	auto adj= total_neighbours[v];
	for (auto u = adj.begin(); u != adj.end(); u++){
	   if (!visited[*u]){
		bridge_tree_dfs(*u, total_vertices, visited, total_neighbours, component, component_number);
	   }
	}
}

void dfs_find_bridges(ll v, vector<ll> &pre, vector<ll> &low, vector<bool> &visited, unordered_set<pair<ll, ll>, hashFunction> &total_bridges, vector<ll> &parent, ll total_vertices, ll &time, vector < vector <ll> > &total_neighbours){
	visited[v]= true;
	time++;
	low[v]= time;
	pre[v]= time;
	for (auto u : total_neighbours[v]){
		if (!visited[u]){
		  parent[u]= v;
		  dfs_find_bridges(u, pre, low, visited, total_bridges, parent, total_vertices, time, total_neighbours); 
		  low[v]= min(low[v], low[u]);
		  if (low[u] > pre[v]){
			  total_bridges.insert(make_pair(v, u));
			  parent[u]= -1;
		  }
		} else if (u != parent[v]){ 	
		  	low[v]= min(low[v], pre[u]);	
		}
	}
}

void find_bridges(ll total_vertices, vector < vector <ll> > &total_neighbours, unordered_set<pair<ll, ll>, hashFunction> &total_bridges){
	vector<ll> pre(total_vertices, int(0));
	vector<ll> low(total_vertices, int(0));
	ll component_number= 0;
	vector<ll> parent(total_vertices, int(-1));
	vector<bool> visited(total_vertices, false); 
	ll time= 0;
	
	for (int i= 0; i< total_vertices; i++){
	  if (!visited[i]){ 
		dfs_find_bridges(i, pre, low, visited, total_bridges, parent, total_vertices, time, total_neighbours);
	  }
	}
}
