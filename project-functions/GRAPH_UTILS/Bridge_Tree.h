#include "read_graph.h"
#include "search.h"
#include <exception> 

using namespace std;
using ll = long long;

class BridgeGraph {  

  public:           
	string z;  
	vector<ll>* component; 
	ll total_vertices; 
	vector <vector <ll>> total_neighbours; 
	
	vector<ll> bridge_tree_map; 
	unordered_set<pair<ll, ll>, hashFunction> total_bridges; 
	vector<vector<ll>> total_bridge_neighbours;
	
	vector <ll> total_bridge_degree;
	vector <ll> leaves;
	vector<bool> bridge_active_nodes;
	vector <bool> removed;
	ll total_bridge_vertices;
	
    BridgeGraph(string temp_z) : total_vertices(read_total_vertices(temp_z)), total_neighbours(total_vertices), component(nullptr){
	  z= temp_z;
    }
	
	//TODO is it neccessary (1)?
	BridgeGraph (ll temp_total_vertices, vector<vector<ll>>  temp_total_neighbours): component(nullptr){
		total_vertices= temp_total_vertices;
		total_neighbours= temp_total_neighbours;
	}
	
	BridgeGraph& operator=(const BridgeGraph& other) {
        if (this != &other) {
            total_vertices= other.total_vertices; 
			z= other.z;
			total_neighbours= other.total_neighbours; 
        }
        return *this;
    }
	
	void set_component(vector<ll> &that_component) {
		component= &that_component;
	}
	
	vector<ll>& get_component() {
		if (component == nullptr) {
			throw runtime_error("component is null!");
		}
		return *component;
	}

	
	void clear(){
		total_bridges.clear();                   
		bridge_tree_map.clear();
		total_bridge_neighbours.clear();         
		total_bridge_degree.clear();             
		leaves.clear();                          
		bridge_active_nodes.clear();             
		removed.clear();                         
		total_bridge_vertices = 0;  
		component = nullptr;             
	}
	
	void test_print_components(string z) {
		cout << "-------components(" << z << ")-------" << endl;
		try {
			if (component == nullptr) {
				throw runtime_error("Component pointer is null.");
			}
			// for (size_t i = 0; i < component->size(); i++) {
			// 	cout << i << ": " << (*component)[i] << endl;
			// }
			// cout << "--------------" << endl;
		} catch (const exception& e) {
			cerr << "Exception caught: " << e.what() << endl;
		} catch (...) {
			cerr << "An unknown error occurred in test_print_components." << endl;
		}
	}
	
	void test_print_bridge_tree_map(const string &z) {
		cout << "-------bridge tree map - " << z << ".-------" << endl;
		// for (auto i = 0; i < bridge_tree_map.size(); i++) {
		// 	cout << i << ": " << bridge_tree_map[i] << endl;
		// }
		// cout << "--------------" << endl;
	}

	void test_print_graph(vector<vector<ll>> adj, ll size, string z){
		cout << "-------Graph - " << z << ".-------" << endl;
		// for (auto i = 0; i < size; i++){
		// 	cout << i << ": ";
		// 	for (const auto& u : adj[i]) {
		// 		cout << u << " ";
		// 	}
		// 	cout << endl;		  
		//   }
		// cout << "-----------" << endl;
	}
	
	void construct_bridges(const vector <bool> active_component_nodes){
		
		test_print_graph(total_neighbours, total_vertices, "before bridge construction");
		test_print_components("before bridge construction");
		
		find_bridges(total_vertices, total_neighbours, total_bridges);
		find_bridge_tree();
		
		total_bridge_vertices= *max_element(bridge_tree_map.begin(), bridge_tree_map.end())+1;
		
		vector<vector<ll>> temp_bridge_neighbours(total_bridge_vertices);
		vector<ll> temp_degree(total_bridge_vertices);

		// insert data in the temp_bridge_neighbours
		for (auto u = total_bridges.begin(); u != total_bridges.end(); u++){
			if (bridge_tree_map[u->first] != bridge_tree_map[u->second]){
				temp_bridge_neighbours[bridge_tree_map[u->first]].push_back(bridge_tree_map[u->second]);
				temp_bridge_neighbours[bridge_tree_map[u->second]].push_back(bridge_tree_map[u->first]);
			}				
		}
		
		ll s= 0;
		for (auto u = temp_bridge_neighbours.begin(); u != temp_bridge_neighbours.end(); u++){
			total_bridge_neighbours.push_back(*u);
			total_bridge_degree.push_back(total_bridge_neighbours[s].size());
			if (u->size() == 1){
				leaves.push_back(s);
			}
			s++;
		}
		
		bridge_active_nodes.assign(total_bridge_vertices, false);
		for (int i = 0; i < total_vertices; i++) {
			if (active_component_nodes[i]) {
				bridge_active_nodes[bridge_tree_map[i]] = true;
			}
		}
		
		test_print_graph(total_bridge_neighbours, total_bridge_vertices, "after bridge tree construction");
		test_print_bridge_tree_map("before pruning");
	}
   
   void find_bridge_tree(){
		//remove the edges!! (its with wrong complexity)
		for (auto u = total_bridges.begin(); u != total_bridges.end(); u++){ 
			remove_edge(u->first, u->second, total_neighbours);
		}
	  
		ll k= 0;
		vector <bool> visited(total_vertices, false);
		bridge_tree_map = vector<ll>(total_vertices, 0);
		for (int i= 0; i< total_vertices; i++) {  
			if (!visited[i]) {
				bridge_tree_dfs(i, total_vertices, visited, total_neighbours, bridge_tree_map, k);	 
				k++;
			}
		}
   }
   
};