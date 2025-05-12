#include "read_graph.h"
#include "search.h"
#include <exception> 

using namespace std;
using ll = long long;

class BridgeGraph {  

  public:           
	string z;  
	unordered_map<ll, ll>* umap_ptr;
	
	ll total_vertices; 
	vector<vector<ll>> total_neighbours; 
	vector<vector<tuple<ll, ll, bool>>> enhanced_total_neighbours;
	
	vector<ll> bridge_tree_map; 
	unordered_set<pair<ll, ll>, hashFunction> total_bridges; 
	vector<pair<ll, ll>> bridge_edges; 
	vector<vector<ll>> total_bridge_neighbours;
	
	vector <ll> total_bridge_degree;
	queue<ll> leaves;
	vector<bool> bridge_active_nodes;
	vector <bool> removed;
	ll total_bridge_vertices;
	
    BridgeGraph(string temp_z) : total_vertices(read_total_vertices(temp_z)), total_neighbours(total_vertices), enhanced_total_neighbours(total_vertices), 
			umap_ptr(nullptr){
	  z= temp_z;
    }
	
	BridgeGraph (ll temp_total_vertices, vector<vector<ll>>  temp_total_neighbours): umap_ptr(nullptr) {
		total_vertices= temp_total_vertices;
		total_neighbours= temp_total_neighbours;
	}
	
	BridgeGraph& operator=(const BridgeGraph& other) {
        if (this != &other) {
            total_vertices= other.total_vertices; 
			z= other.z;
			enhanced_total_neighbours = other.enhanced_total_neighbours;
        }
        return *this;
    }
	
	void set_umap(unordered_map<ll, ll> &that_umap) {
		umap_ptr = &that_umap;
	}

	unordered_map<ll, ll>& get_umap() {
		if (umap_ptr == nullptr) {
			throw runtime_error("component is null!");
		}
		return *umap_ptr;
	}
	
	void clear(){
		// total_bridges.clear();                   
		bridge_edges.clear();                   
		bridge_tree_map.clear();
		total_bridge_neighbours.clear();         
		total_bridge_degree.clear(); 
		leaves = queue<ll>();                          
		bridge_active_nodes.clear();             
		removed.clear();                         
		total_bridge_vertices = 0;  
		umap_ptr = nullptr;           
	}
	
	void test_print_components(string z) {
		cout << "-------components(" << z << ")-------" << endl;
		try {
			if (umap_ptr == nullptr) {
				throw runtime_error("Component pointer is null.");
			}
			for (size_t i = 0; i < umap_ptr->size(); i++) {
				cout << i << ": " << (*umap_ptr)[i] << endl;
			}
			cout << "--------------" << endl;
		} catch (const exception& e) {
			cerr << "Exception caught: " << e.what() << endl;
		} catch (...) {
			cerr << "An unknown error occurred in test_print_components." << endl;
		}
	}
	
	void test_print_bridge_tree_map(const string &z) {
		cout << "-------bridge tree map - " << z << ".-------" << endl;
		for (auto i = 0; i < bridge_tree_map.size(); i++) {
			cout << i << ": " << bridge_tree_map[i] << endl;
		}
		cout << "--------------" << endl;
	}

	void print_enhanced_graph(string z){
		cout << "-------Graph - " << z << ".-------" << endl;
		for (auto i = 0; i < enhanced_total_neighbours.size(); i++){
			cout << i << ": ";
			for (const auto u : enhanced_total_neighbours[i]) {
				cout << get<0>(u) << " ";
			}
			cout << endl;		  
		  }
		cout << "-----------" << endl;
	}

	void test_print_graph(vector<vector<ll>> adj, string z){
		cout << "-------Graph - " << z << ".-------" << endl;
		for (auto i = 0; i < adj.size(); i++){
			cout << i << ": ";
			for (const auto u : adj[i]) {
				cout << u << " ";
			}
			cout << endl;		  
		  }
		cout << "-----------" << endl;
	}

	void print_edges() {
		cout << "--------Bridge Edges -------\n";
		for (const auto e : bridge_edges) {
			cout << "(" << e.first << ", " << e.second << ")" << endl; 
		}
		cout << "------------------------\n";
	}
	
	void construct_bridges(const vector <bool> active_component_nodes){
		
		print_enhanced_graph("before bridge construction");
		test_print_components("before bridge construction");
		
		find_bridges(total_vertices, enhanced_total_neighbours, bridge_edges);

		print_edges();
		find_bridge_tree(total_vertices, enhanced_total_neighbours, bridge_tree_map);
		
		total_bridge_vertices= *max_element(bridge_tree_map.begin(), bridge_tree_map.end())+1;
		
		// insert data in the temp_bridge_neighbours
		vector<vector<ll>> temp_bridge_neighbours(total_bridge_vertices);
		ll first, second;
		for (auto u : bridge_edges){
			first = bridge_tree_map[u.first];
			second = bridge_tree_map[u.second];  
			if (first != second){
				temp_bridge_neighbours[first].push_back(second);
				temp_bridge_neighbours[second].push_back(first);
			}				
		}
		
		ll s= 0;
		for (auto u : temp_bridge_neighbours){
			total_bridge_neighbours.push_back(u);
			total_bridge_degree.push_back(total_bridge_neighbours[s].size());
			if (u.size() == 1){
				leaves.push(s);
			}
			s++;
		}
		
		bridge_active_nodes.assign(total_bridge_vertices, false);
		for (int i = 0; i < total_vertices; i++) {
			if (active_component_nodes[i]) {
				bridge_active_nodes[bridge_tree_map[i]] = true;
			}
		}
		
		test_print_graph(total_bridge_neighbours, "after bridge tree construction");
		test_print_bridge_tree_map("before pruning");
	}
   

};