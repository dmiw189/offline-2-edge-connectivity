#include <iostream>
#include <cmath>
#include "Bridge_Tree.h"

using ll = long long;

class Event {       
	public:                 
		ll total_vertices;
		string z;

		vector<tuple<char, ll, ll, ll>> total_augmented_events;
		vector<pair<ll, ll>> static_left;
		vector<pair<ll, ll>> static_right;

		//TODO (9) 
		vector<bool> left_active_nodes;
		vector<bool> right_active_nodes;
		
		vector<ll> left_active_nodes_list;
		vector<ll> right_active_nodes_list;
    
	Event(const string temp_z, const ll temp_total_vertices): left_active_nodes(temp_total_vertices, false), right_active_nodes(temp_total_vertices, false){     
		z= temp_z;
		total_vertices= temp_total_vertices;
    }

	Event(const string temp_z){     
		z= temp_z;
    }

	void add_to_augmented_events(const vector<tuple<char, ll, ll>> &event_list) {
		for (const auto [type, x, y] : event_list) {
			total_augmented_events.push_back(make_tuple(type, x, y, -1));
		}
	}

	void read_event_file() {
		read_graph_events(z);
		read_events(z);
	  	add_to_augmented_events(graph_events);
		add_to_augmented_events(events);
	  
		ll t, s= 0;
		for (auto& u : total_augmented_events){ 
			auto nodes_pair = make_pair(min(get<1>(u), get<2>(u)), max(get<1>(u), get<2>(u)));
			switch (get<0>(u)){
				case 'D':
					t= event_mapping[nodes_pair];
					get<3>(total_augmented_events[s])= t;
					get<3>(total_augmented_events[t])= s;
					event_mapping[nodes_pair]= -1;
					break;

				case 'I':
					event_mapping[nodes_pair]= s;
					break;
				case 'Q':
					break;
			}
			s++;
		}
	}
	
	void print_static_edges() {
		//cout << "-------------Static Left Edges:" << endl;
		for (const auto& edge : static_left) {
			//cout << "(" << edge.first << ", " << edge.second << ")" << endl;
		}
		//cout << "-----------------------" << endl;

		//cout << "-------------Static Right Edges:" << endl;
		for (const auto& edge : static_right) {
			//cout << "(" << edge.first << ", " << edge.second << ")" << endl;
		}
		//cout << "-----------------------" << endl;
	}
	
	void find_static_edges(const ll event_start, const ll event_end, bool staticEdgesForTheLeft) {
		auto half_value = (event_end + event_start + 1) / 2;
		auto start = staticEdgesForTheLeft ? half_value : event_start;
		auto end = staticEdgesForTheLeft ? event_end : half_value - 1;

		for (ll i = start; i <= end; i++) {
			auto [type, x, y, t] = total_augmented_events[i];
			if ((staticEdgesForTheLeft && type == 'D' && (t == -1 || t < start)) ||
				(!staticEdgesForTheLeft && type == 'I' && (t == -1 || t > end))) {
				(staticEdgesForTheLeft ? static_left : static_right).emplace_back(x, y);
			}
		}
	}


	void find_active_nodes(const ll start_index, const ll end_index, bool isleft) {
		auto &active_nodes = isleft ? left_active_nodes : right_active_nodes;
		auto &active_nodes_list = isleft ? left_active_nodes_list : right_active_nodes_list;

		for (auto v = start_index; v <= end_index; v++) {
			for (const auto u : {get<1>(total_augmented_events[v]), get<2>(total_augmented_events[v])}) {
				if (!active_nodes[u]) {
					active_nodes_list.push_back(u);
					active_nodes[u] = true;
				}
			}
		}

		
	}

	//TODO maybe we have to do the same think with that
	void clear_active_nodes() {
		for (auto u : left_active_nodes_list) {
			left_active_nodes[u] = false;
		}
		
		for (auto u : right_active_nodes_list) {
			right_active_nodes[u] = false;
		}
		
		left_active_nodes_list.clear();
		right_active_nodes_list.clear();
	}

	void clear_static_edges() {
		static_left.clear();
		static_right.clear();
	}

	bool query_exists_in_events(const ll start, const ll end) {
		for (auto v = start; v <= end; v++) {
			if (get<0>(total_augmented_events[v]) == 'Q') {
				return true;
			}
		}
		return false;
	}
	
	private: 
		vector<tuple<char, ll, ll>> events;
		vector<tuple <char, ll, ll>> graph_events;
		map<pair<ll, ll>, ll> event_mapping;


	void read_graph_events(string z) {
		int T; 	
		ll v1, v2, file_edges, total_vertices, s= 0;
		ifstream infile("..\\graphs\\graph_"+z+".txt");	
		if(infile.is_open()){
			infile>>T; 
			infile >> total_vertices >> file_edges;
			while ((infile >> v1 >> v2)){	  
				graph_events.push_back(make_tuple('I', v1, v2));
			}
			infile.close();
		}
	}

	void read_events(string z){
		int T; 	
		ll total_lines, v1, v2, file_edges, total_vertices, s= 0;
		char query;
		ifstream infile("..\\events\\events_"+z+".txt");	
		if(infile.is_open()){
			infile>>T; 
			infile >> total_lines;
			while ((infile >> query >> v1 >> v2)){	  
				events.push_back(make_tuple(query, v1, v2));
			}
			infile.close();
		}
	}
		
};