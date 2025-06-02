#pragma once

#include <bits/stdc++.h>
#include "../utils-functions/read_utils.h"
#include "../utils-functions/search.h"

using namespace std;
using ll = long long;
using cll = const long long;
using cb = const bool;
using c_str = const string&;
using List = vector<ll>;
using cList = const vector<ll>;
using AugmentedEventsList = vector<tuple<char, ll, ll, ll>>;

class Graph {
	public: 
		Graph(c_str z): total_vertices(read_total_vertices(z)), graph(total_vertices), components_map_ptr(nullptr) {}

		Graph(cll total_vertices, const vector<vector<tuple<ll, ll, bool>>>& graph, const unordered_map<ll, ll>* map_ref)
			: total_vertices(total_vertices), graph(graph), components_map_ptr(map_ref) {}

		Graph(cll total_vertices, const vector<vector<tuple<ll, ll, bool>>>& graph)
			: Graph(total_vertices, graph, nullptr) {}


		cll get_total_vertices() const {
			return total_vertices;
		}

		const vector<vector<tuple<ll, ll, bool>>>& get_graph() const {
			return graph;
		}

		const unordered_map<ll, ll>& get_components_map() const {
			if (components_map_ptr == nullptr) {
				throw runtime_error("component is null!");
			}
			return *components_map_ptr;
		}

		void set_components_map(const unordered_map<ll, ll>& map_ref) {
        	components_map_ptr = &map_ref;
    	}

		void clear() {
			total_vertices = 0;
			graph.clear();
			components_map_ptr = nullptr;
		}

	private:
		ll total_vertices; 
		vector<vector<tuple<ll, ll, bool>>> graph;
		const unordered_map<ll, ll>* components_map_ptr;
};

class Graph_Details {  

  public:           	
	Graph_Details(cll total_vertices, const vector<vector<tuple<ll, ll, bool>>>& graph) {
		this->total_vertices = total_vertices;
		this->graph = graph;
		this->total_bridge_vertices = 0;
	}

	cll get_total_vertices() const {
		return total_vertices;
	}

	vector<vector<tuple<ll, ll, bool>>>& get_graph() {
		return graph;
	}

	void set_total_vertices(cll total_vertices) {
		this->total_vertices = total_vertices;
	}

	void set_graph(const vector<vector<tuple<ll, ll, bool>>>& graph) {
		this->graph = move(graph);
	}

	void set_graph(cll total_vertices) {
		this->total_vertices = total_vertices;
		this->graph = vector<vector<tuple<ll, ll, bool>>> (total_vertices);;
	}
	
	void add_edge(cll u, cll v) {
		insert_edge(u, v, graph);
	}

	void add_edges(const vector<pair<ll, ll>>& edges) {
		add_edges_to_graph(edges, graph);
	}

	cll get_total_bridge_vertices() const {
		return total_bridge_vertices;
	}

	const vector<ll> get_bridge_map()& {
		return bridge_map;
	}

	vector<ll> get_bridge_graph(cll v)& {
		return bridge_graph[v];
	}

	void clear_bridge_graph(cll v) {
		bridge_graph[v].clear();
	}

	void add_edge_at_bridge_graph(cll u, cll v) {
		insert_edge(u, v, bridge_graph);
	}


	cb get_bridge_active_node(cll v) const {
		return bridge_active_nodes[v];
	}

	cll get_bridge_tree_degree(cll v) const {
		return bridge_tree_degrees[v];
	}

	void set_bridge_tree_degree(cll v, cll value) {
		bridge_tree_degrees[v] = value;
	}
	
	const queue<ll> get_leaves()& {
		return leaves;
	}

	void construct_bridges(const vector<bool>& active_component_nodes) {	
		vector<pair<ll, ll>> bridge_edges; 
		mark_bridge_edges(total_vertices, graph, bridge_edges);
		calculate_bridge_map(total_vertices, graph, bridge_map);
		process_before_pruning(bridge_edges, active_component_nodes);
		// print_debug_info(bridge_edges);
	}

	void print_graph(c_str text) {
		cout << "-------Graph - " << text << ".-------" << endl;
		for (auto i = 0; i < graph.size(); i++){
			cout << i << ": ";
			for (const auto u : graph[i]) 
				cout << get<0>(u) << " ";
			cout << endl;		  
		  }
		cout << "-----------" << endl;
	}

	void clear() {
		total_vertices = 0;
		graph.clear();
		bridge_map.clear();
		bridge_graph.clear();
		total_bridge_vertices = 0;
		bridge_active_nodes.clear();
		bridge_tree_degrees.clear();
		leaves = queue<ll>();
	}

	private:
		ll total_vertices; 
		vector<vector<tuple<ll, ll, bool>>> graph;

		vector<ll> bridge_map; 
		vector<vector<ll>> bridge_graph;
		ll total_bridge_vertices;
		vector<bool> bridge_active_nodes;

		vector <ll> bridge_tree_degrees;
		queue<ll> leaves;

		void process_before_pruning(const vector<pair<ll, ll>>& bridge_edges, const vector<bool>& active_component_nodes) {
			total_bridge_vertices = *max_element(bridge_map.begin(), bridge_map.end()) + 1;
			//create bridge graph
			bridge_graph = vector<vector<ll>> (total_bridge_vertices);
			for (const auto& [u, v] : bridge_edges) {
				auto c_u = bridge_map[u];
				auto c_v = bridge_map[v];  
				if (c_u != c_v) 
					insert_edge(c_u, c_v, bridge_graph);
			}
			//create degrees and leaves
			for (auto v = 0; v < bridge_graph.size(); v++) {
				cll degree = bridge_graph[v].size();			
				bridge_tree_degrees.push_back(degree);
				if (degree == 1) 
					leaves.push(v);
			}

			//create bridge vertices activity
			bridge_active_nodes.assign(total_bridge_vertices, false);
			for (auto v = 0; v < total_vertices; v++) 
				if (active_component_nodes[v]) 
					bridge_active_nodes[bridge_map[v]] = true;		
		}

		void print_debug_info(const vector<pair<ll, ll>>& bridge_edges) const {
			cout << "--------------Construct Bridges----------------------\n";
			cout << "-------Graph - before bridge construction------------\n";
			for (auto i = 0; i < graph.size(); i++){
				cout << i << ": ";
				for (const auto u : graph[i]) 
					cout << get<0>(u) << " ";
				cout << endl;		  
			}
			cout << "-----------------------------------------------------\n";

			cout << "-----------------Bridge Edges------------------------\n";
			for (const auto& [u, v] : bridge_edges)
				cout << "(" << u << ", " << v << ")\n";
			cout << "-----------------------------------------------------\n";
			cout << "-----------------Bridge Graph------------------------\n";
			for (ll i = 0; i < bridge_graph.size(); ++i) {
				cout << i << ": ";
				for (ll u : bridge_graph[i]) cout << u << " ";
				cout << "\n";
			}
			cout << "-----------------------------------------------------\n";
			cout << "------------------Bridge Map-------------------------\n";
			for (ll i = 0; i < bridge_map.size(); ++i)
				cout << i << ": " << bridge_map[i] << "\n";
			cout << "-----------------------------------------------------\n";
		}

};