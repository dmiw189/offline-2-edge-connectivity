#include <iostream>
#include <cmath>
#include "Bridge_Tree.h"

using ll = long long;

class Event {       
	public:                 
		ll total_vertices;
		string z;

		vector< tuple <char, ll, ll, ll> > total_augmented_events;
		vector< pair <ll, ll> > static_left;
		vector< pair <ll, ll> > static_right;

		vector <bool> left_active_nodes;
		vector <ll> left_active_nodes_list;
		vector <bool> right_active_nodes;
		vector <ll> right_active_nodes_list;
    
	Event(string temp_z, ll temp_total_vertices): left_active_nodes(temp_total_vertices, false), right_active_nodes(temp_total_vertices, false){     
		z= temp_z;
		total_vertices= temp_total_vertices;
    }

	Event(string temp_z){     
		z= temp_z;
    }

	void add_to_augmented_events(const vector<tuple<char, ll, ll>>& event_list) {
		for (const auto& u : event_list) {
			total_augmented_events.push_back(make_tuple(get<0>(u), get<1>(u), get<2>(u), -1));
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
		cout << "-------------Static Left Edges:" << endl;
		for (const auto& edge : static_left) {
			cout << "(" << edge.first << ", " << edge.second << ")" << endl;
		}
		cout << "-----------------------" << endl;

		cout << "-------------Static Right Edges:" << endl;
		for (const auto& edge : static_right) {
			cout << "(" << edge.first << ", " << edge.second << ")" << endl;
		}
		cout << "-----------------------" << endl;
	}
	
	void find_static_edges(ll event_start, ll event_end) {
		ll half_value = ceil((event_end + event_start) / 2.0);
		for (ll i = event_start; i <= event_end; i++) {
			auto& event = total_augmented_events[i];
			char type = get<0>(event);
			ll x = get<1>(event), y = get<2>(event), t = get<3>(event);
			if (i >= half_value && type == 'D'&& (t == -1 || t < event_start)) {
					static_left.emplace_back(x, y);
			} else if (i < half_value && type == 'I' && (t == -1 || t > event_end)) {
					static_right.emplace_back(x, y); 
			}
		}
	}

	void find_active_nodes(ll start_index, ll end_index, bool isleft) {
		auto &active_nodes = isleft ? left_active_nodes : right_active_nodes;
		auto &active_nodes_list = isleft ? left_active_nodes_list : right_active_nodes_list;

		for (ll v = start_index; v <= end_index; v++) {
			ll u1 = get<1>(total_augmented_events[v]);
			ll u2 = get<2>(total_augmented_events[v]);
			
			for (ll u : {u1, u2}) {
				if (!active_nodes[u]) {
					active_nodes_list.push_back(u);
					active_nodes[u] = true;
				}
			}
		}
	}

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

	bool query_exists_in_events(ll start, ll end) {
		for (ll v = start; v <= end; v++) {
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