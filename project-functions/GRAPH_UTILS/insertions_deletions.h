#pragma once

#include <bits/stdc++.h>
using namespace std;
using ll = long long;

void insert_edge(const ll u, const ll v, vector<vector<ll>> &total_neighbours){
  total_neighbours[u].push_back(v);
  total_neighbours[v].push_back(u);
}

void insert_edge_enhanced(const ll u, const ll v, vector<vector<tuple<ll, ll, bool>>> &adj) {
  const auto size_u = adj[u].size();
  const auto size_v = adj[v].size();

  adj[u].emplace_back(v, size_v, false);
  adj[v].emplace_back(u, size_u, false);
}

void add_edges_to_graph(const vector<pair<ll, ll>> &edges, vector<vector<tuple<ll, ll, bool>>> &adj) {
	for (const auto [u, v] : edges) {
    insert_edge_enhanced(u, v, adj);
	}
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