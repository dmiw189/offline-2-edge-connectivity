#pragma once

#include <bits/stdc++.h>

using namespace std;
using ll = long long;
using cll = const long long;
using c_str = const string&;

class Event {       
	public:                 
		Event(const int graph_id, const int event_id) {
			auto graph_id_str = to_string(graph_id);
			auto event_id_str = to_string(event_id);
			read_graph_events(graph_id_str);
			read_events(graph_id_str + "." + event_id_str);
		}

		// Getters
		cll get_total_vertices() const { return total_vertices; }
		cll get_total_edges() const { return total_edges; }
		cll get_total_events() const { return total_events; }
		const vector<tuple<char, ll, ll>>& get_eventsList() const { return eventsList; }
		// Setters
		void set_total_vertices(cll value) { total_vertices = value; }
		void set_total_edges(cll value) { total_edges = value; }
		void set_total_events(cll value) { total_events = value; }
		void set_eventsList(const vector<tuple<char, ll, ll>>& list) { eventsList = list; }


	private: 
		ll total_vertices, total_edges, total_events;
		vector<tuple<char, ll, ll>> eventsList;

		void read_graph_events(c_str id) {
			int T; 	
			ll v1, v2, total_edges, total_vertices;
			ifstream infile("..\\graphs\\graph_" + id + ".txt");	
			if(infile.is_open()) {
				infile >> T; 
				infile >> total_vertices >> total_edges;
				this->total_vertices = total_vertices;
				this->total_edges = total_edges;
				while ((infile >> v1 >> v2)) 
					eventsList.push_back(make_tuple('I', v1, v2));
				infile.close();
			}
		}

		void read_events(c_str id) {
			// int T; 	
			ll total_events, v1, v2;
			char query_type;
			ifstream infile("..\\events\\event_" + id + ".txt");	
			if(infile.is_open()) {
				// infile>>T; 
				infile >> total_events;
				this->total_events = total_events;
				while ((infile >> query_type >> v1 >> v2)) {	  
					eventsList.push_back(make_tuple(query_type, v1, v2));
				}
				infile.close();
			}
		}
		
};