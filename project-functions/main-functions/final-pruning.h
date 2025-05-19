#pragma once 
#include <bits/stdc++.h>
#include "identical-graph.h"

using ll = long long;
using cll = const long long;
using cb = const bool;
using List = vector<ll>;
using cList = const vector<ll>;
using AugmentedEventsList = vector<tuple<char, ll, ll, ll>>;

class Graph_Pruner {
	public:
		static void prune(Graph_Details& g, cList& original_active_nodes_list, unordered_map<ll, ll>& new_components_map) {
			prune_inactive_with_degree_1(g);
			prune_inactive_with_degree_2(g);
			prune_inactive_with_degree_0(g);
			List pruned_map(g.total_bridge_vertices);
			update_graph(g, pruned_map);	
			update_components(original_active_nodes_list, g.bridge_tree_map, pruned_map, g.get_umap(), new_components_map);
			// g.test_print_components("Updated Components");
			// g.print_enhanced_graph("Pruned Graph)");
		}

	private:
		static void prune_inactive_with_degree_1(Graph_Details& g) {
			g.removed.resize(g.total_bridge_vertices, false);
			queue<ll> leaves = g.leaves;
			while (!leaves.empty()) {
				const auto leaf = leaves.front();
				leaves.pop();
				if (!g.bridge_active_nodes[leaf]) {
					for (const auto& neighboor : g.total_bridge_neighbours[leaf]) 
						if (!g.removed[neighboor]) { //parent found
							auto parent = neighboor; 
							g.total_bridge_degree[parent] -= 1;
							if (g.total_bridge_degree[parent] == 1 
								&& !g.bridge_active_nodes[parent])
								leaves.push(parent);
							break;
						}
					pruning(g, leaf);
				}	  
			}
		}

		static void prune_inactive_with_degree_2(Graph_Details& g) {
			for (auto v = 0; v < g.total_bridge_vertices; v++)
				if (!g.bridge_active_nodes[v] && g.total_bridge_degree[v] == 2) {
					vector<ll> neighbors;
					for (auto& u : g.total_bridge_neighbours[v])
						if (!g.removed[u])
							neighbors.push_back(u);
					assert(neighbors.size() == 2 && "Inactive vertex with degree 2 must have exactly two unremoved neighbors");
					pruning(g, v);
					g.total_bridge_neighbours[neighbors[0]].push_back(neighbors[1]);
					g.total_bridge_neighbours[neighbors[1]].push_back(neighbors[0]);
				}
		}

		static void prune_inactive_with_degree_0(Graph_Details& g){
			for (auto v = 0; v < g.total_bridge_vertices; v++)
				if (!g.bridge_active_nodes[v] && g.total_bridge_degree[v] == 0)
					pruning(g, v);
		}

		static void pruning(Graph_Details& g, cll v) {
			g.removed[v] = true;
			g.total_bridge_neighbours[v].clear();
			g.total_bridge_degree[v] = -1;
		}

		static void extract_edges(const vector<List>& graph, vector<pair<ll, ll>>& edges) {
			for (auto v = 0; v < graph.size(); v++)  
				for (const auto& u : graph[v])       
					if (v < u)
						edges.emplace_back(v, u);
		}

		static void update_graph(Graph_Details& g, List& removed_map){
			ll counter_non_removed_nodes = 0, u_map, v_map;
			
			for (auto v = 0; v < g.total_bridge_vertices; v++) {
				removed_map[v] = !g.removed[v] ? counter_non_removed_nodes++ : -1;
			}

			vector<List> temp_bridge_pruned_neighbours(counter_non_removed_nodes); 
			for (ll v = 0; v < g.total_bridge_vertices; v++) {
				if (g.removed[v]) 
					continue;

				v_map = removed_map[v];
				for (auto u : g.total_bridge_neighbours[v]) {
					if (removed_map[u] == -1) continue;
					u_map = removed_map[u];
					if (u_map != -1) {
						temp_bridge_pruned_neighbours[v_map].push_back(u_map);
					}
				}
			}

			g.total_vertices= counter_non_removed_nodes;
			vector<pair<ll, ll>> edges;
			extract_edges(temp_bridge_pruned_neighbours, edges);
			g.enhanced_total_neighbours = vector<vector<tuple<ll, ll, bool>>>(counter_non_removed_nodes);
			add_edges_to_graph(edges, g.enhanced_total_neighbours);
		}

		static ll original_res_mapping(cll component_node, cList& bridge_tree_map, cList& pruned_map) {  
			return pruned_map[bridge_tree_map[component_node]]; 
		}

		static void update_components(cList& active_vertices, cList& bridge_tree_map, cList& pruned_map, 
			const unordered_map<ll, ll>& old_components_map, unordered_map<ll, ll>& new_components_map) {
			for (const auto& u : active_vertices) 
				new_components_map[u] = original_res_mapping(old_components_map.at(u), bridge_tree_map, pruned_map);
		}

};