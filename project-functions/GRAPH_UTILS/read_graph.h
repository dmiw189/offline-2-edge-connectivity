#pragma once

#include <list>
#include<bits/stdc++.h>
#include <chrono>

using namespace std;
using ll = long long;

struct hashFunction{
   size_t operator()(const pair<int ,int> &x) const{return x.first ^ x.second;}
};

void find_neighbours(ll v, unordered_set<pair<ll, ll>, hashFunction> &edges, vector <ll> &total_neighbours){
  for (auto i = edges.begin(); i != edges.end(); i++){
    if ((*i).first == v){
      total_neighbours.push_back((*i).second);
    }
    if ((*i).second == v){
      total_neighbours.push_back((*i).first);
    }
  }
  
}

ll read_total_vertices(string z){
  cout << "Read graph" << endl;	
  ll total_vertices;
  ll file_edges;
  ifstream infile("..\\graphs\\graph_"+z+".txt");
  if(infile.is_open()) {
    infile>>total_vertices; 
  }
  infile >> total_vertices >> file_edges;
  return total_vertices;
}

void read_graph(string z, vector < vector <ll> > &total_neighbours){
  ifstream infile("..\\graphs\\graph_"+z+".txt");
  int T;
  ll v1;
  ll v2;
  ll file_edges;
  ll total_vertices;
  vector<ll> *d;

  if(infile.is_open()){
    infile>>T; 
  }
  infile >> total_vertices >> file_edges;
  while ((infile >> v1 >> v2)){	  
    total_neighbours[v1].push_back(v2);
    total_neighbours[v2].push_back(v1);
  }
    
  infile.close();
}
