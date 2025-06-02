#pragma once 
#include <bits/stdc++.h>
#include "identical-graph.h"

using ll = long long;
using cll = const long long;
using List = vector<ll>;
using cList = const vector<ll>;

class Graph_Pruner {
	public:
		static void prune(Graph_Details& g, cList& active_nodes_list, 
			const unordered_map<ll, ll>& old_components_map, unordered_map<ll, ll>& new_components_map) {
			
			const auto vertices = g.get_total_bridge_vertices();
			vector<bool> removed_bridge_vertices(vertices, false);
			prune_inactive_with_degree_1(g, removed_bridge_vertices);
			prune_inactive_with_degree_2(g, removed_bridge_vertices);
			prune_inactive_with_degree_0(g, removed_bridge_vertices);
			//from bridge -> pruned component vertices
			List pruned_map(vertices);
			update_graph(g, pruned_map, removed_bridge_vertices);	
			update_components(active_nodes_list, g.get_bridge_map(), pruned_map, old_components_map, new_components_map);
		}

	private:
		static void prune_inactive_with_degree_1(Graph_Details& g, vector<bool>& removed_bridge_vertices) {
			queue<ll> leaves = g.get_leaves();
			while (!leaves.empty()) {
				const auto leaf = leaves.front();
				leaves.pop();
				if (!g.get_bridge_active_node(leaf)) {
					for (const auto& u : g.get_bridge_graph(leaf)) 
						if (!removed_bridge_vertices[u]) { //parent found
							auto parent = u; 
							g.set_bridge_tree_degree(parent, g.get_bridge_tree_degree(parent) - 1);
							if (g.get_bridge_tree_degree(parent) == 1 && !g.get_bridge_active_node(parent))
								leaves.push(parent);
							break;
						}
					pruning(g, leaf, removed_bridge_vertices);
				}	  
			}
		}

		static void prune_inactive_with_degree_2(Graph_Details& g, vector<bool>& removed_bridge_vertices) {
			for (auto v = 0; v < g.get_total_bridge_vertices(); v++)
				if (!g.get_bridge_active_node(v) && g.get_bridge_tree_degree(v) == 2) {
					vector<ll> neighbors_list;
					for (const auto& u : g.get_bridge_graph(v))
						if (!removed_bridge_vertices[u])
							neighbors_list.push_back(u);
					
					assert(neighbors_list.size() == 2 && "Inactive vertex with degree 2 must have exactly two unremoved neighbors_list");
					pruning(g, v, removed_bridge_vertices);
					g.add_edge_at_bridge_graph(neighbors_list[0], neighbors_list[1]);
				}
		}

		static void prune_inactive_with_degree_0(Graph_Details& g, vector<bool>& removed_bridge_vertices) {
			for (auto v = 0; v < g.get_total_bridge_vertices(); v++)
				if (!g.get_bridge_active_node(v) && g.get_bridge_tree_degree(v) == 0)
					pruning(g, v, removed_bridge_vertices);
		}

		static void pruning(Graph_Details& g, cll v, vector<bool>& removed_bridge_vertices) {
			removed_bridge_vertices[v] = true;
			g.clear_bridge_graph(v);
			g.set_bridge_tree_degree(v, g.get_bridge_tree_degree(v) - 1);
		}

		static void update_graph(Graph_Details& g, List& pruned_map, vector<bool>& removed_bridge_vertices) {
			ll total_updated_vertices = 0;
			cll total_vertices = g.get_total_bridge_vertices();

			//find total vertices && pruned_map
			for (auto v = 0; v < total_vertices; v++)
				pruned_map[v] = !removed_bridge_vertices[v] ? total_updated_vertices++ : -1;
			vector<List> pruned_graph(total_updated_vertices); 
			
			//find the pruned and filtered graph
			for (auto v = 0; v < total_vertices; v++) {
				if (removed_bridge_vertices[v]) 
					continue;
				auto v_map = pruned_map[v];
				cList v_neighbors = g.get_bridge_graph(v);  
				for (const auto& u : v_neighbors) {
					if (pruned_map[u] == -1) 
						continue;
					auto u_map = pruned_map[u];
					if (u_map != -1) 
						pruned_graph[v_map].push_back(u_map);
				}
			}

			g.set_graph(total_updated_vertices);
			for (auto v = 0; v < pruned_graph.size(); v++)  
				for (const auto& u : pruned_graph[v])       
					if (v < u)
						g.add_edge(v, u);

		}

		static cll calculate_mapping(cll old_component, cList& bridge_map, cList& pruned_map) {  
			return pruned_map[bridge_map[old_component]]; 
		}

		static void update_components(cList& active_vertices, cList& bridge_map, cList& pruned_map, 
			const unordered_map<ll, ll>& old_components_map, unordered_map<ll, ll>& new_components_map) {
			for (const auto& u : active_vertices) 
				new_components_map[u] = calculate_mapping(old_components_map.at(u), bridge_map, pruned_map);
		}

};