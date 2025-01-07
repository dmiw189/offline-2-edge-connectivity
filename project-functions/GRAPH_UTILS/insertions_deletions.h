#pragma once

#include <bits/stdc++.h>
using namespace std;
using ll = long long;

void add_edges_to_graph(const vector<pair<ll, ll>> edges, vector<vector<ll>> &adj) {
	for (const auto &e : edges) {
		adj[e.first].push_back(e.second);
		adj[e.second].push_back(e.first);
	}
}

void insert_edge(ll u, ll v, vector<vector<ll>> &total_neighbours){
  total_neighbours[u].push_back(v);
  total_neighbours[v].push_back(u);
}

void insert_edge_enhanced(const ll u, const ll v, vector<vector<tuple<ll, ll, bool>>> &adj) {
  const ll size_u = adj[u].size();
  const ll size_v = adj[v].size();

  adj[u].emplace_back(v, size_v, false);
  adj[v].emplace_back(u, size_u, false);
}

void remove_edge(const ll u, const ll v, vector<vector<ll>> &total_neighbours){
  auto it = find(total_neighbours[u].begin(), total_neighbours[u].end(), v);
  if (it != total_neighbours[u].end()){
    total_neighbours[u].erase(it);
  }
  it = find(total_neighbours[v].begin(), total_neighbours[v].end(), u);
  if (it != total_neighbours[v].end()){
   total_neighbours[v].erase(it);
  } 
}

void remove_edges_by_vertex(const ll total_vertices, unordered_set<pair<ll, ll>, hashFunction> &edges, ll v, vector< vector <ll> > &total_neighbours){
  for (int u = 0; u < total_vertices; u++) {
    if (edges.find(make_pair(v, u)) != edges.end()){
      edges.erase(make_pair(v, u));	
      auto it = find(total_neighbours[v].begin(), total_neighbours[v].end(), u);
      if (it != total_neighbours[v].end())   {
        total_neighbours[v].erase(it);
      }
    }
    
    if (edges.find(make_pair(u, v)) != edges.end()){
      edges.erase(make_pair(u, v));	
      auto it = find(total_neighbours[u].begin(), total_neighbours[u].end(), v);
      if (it != total_neighbours[u].end()){
      total_neighbours[u].erase(it);
      }
    }
  }
}
