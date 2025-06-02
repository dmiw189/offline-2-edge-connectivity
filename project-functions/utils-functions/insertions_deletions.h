#pragma once
#include <bits/stdc++.h>

using namespace std;
using ll = long long;
using cll = const long long;

void insert_edge(cll u, cll v, vector<vector<ll>>& graph) {
  graph[u].push_back(v);
  graph[v].push_back(u);
}

void insert_edge(cll u, cll v, vector<vector<tuple<ll, ll, bool>>>& graph) {
  const auto size_u = graph[u].size();
  const auto size_v = graph[v].size();
  graph[u].emplace_back(v, size_v, false);
  graph[v].emplace_back(u, size_u, false);
}

void add_edges_to_graph(const vector<pair<ll, ll>>& edges, vector<vector<tuple<ll, ll, bool>>>& graph) {
  for (const auto& [u, v] : edges)
    insert_edge(u, v, graph);
}

void remove_edge(cll u, cll v, vector<vector<ll>>& graph) {
  auto it = find(graph[u].begin(), graph[u].end(), v);
  if (it != graph[u].end())
    graph[u].erase(it);
  
  it = find(graph[v].begin(), graph[v].end(), u);
  if (it != graph[v].end())
    graph[v].erase(it);
}