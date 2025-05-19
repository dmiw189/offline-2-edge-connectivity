#pragma once
#include <bits/stdc++.h>

using namespace std;
using ll = long long;
using c_str = const string&;

struct hashFunction{
   size_t operator()(const pair<int ,int> &x) const{return x.first ^ x.second;}
};

void find_neighbours(ll v, unordered_set<pair<ll, ll>, hashFunction> &edges, vector <ll> &total_neighbours){
  for (auto i = edges.begin(); i != edges.end(); i++) {
    if ((*i).first == v)
      total_neighbours.push_back((*i).second);
    
    if ((*i).second == v)
      total_neighbours.push_back((*i).first);
    
  }
  
}

ll read_total_vertices(c_str z){
  cout << "Read graph" << endl;	
  ll total_vertices, total_edges;
  
  ifstream infile("..\\graphs\\graph_"+z+".txt");
  if(infile.is_open()) 
    infile >> total_vertices; 
  
  infile >> total_vertices >> total_edges;
  return total_vertices;
}

void read_graph(c_str z, vector<vector<ll>> &total_neighbours) {
  ifstream infile("..\\graphs\\graph_"+z+".txt");

  int T;
  if(infile.is_open())
    infile>>T; 
  
  ll v1, v2, total_edges, total_vertices;
  infile >> total_vertices >> total_edges;

  while ((infile >> v1 >> v2)) {	  
    total_neighbours[v1].push_back(v2);
    total_neighbours[v2].push_back(v1);
  }

  infile.close();
}

vector<bool> read_results_from_file(c_str file_name) {
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
