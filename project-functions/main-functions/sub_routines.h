#include <bits/stdc++.h>

#include "final-pruning.h"
#include "identical-graph.h"
#include "../events-functions/events_handler.h"
#include "../include/print_and_log_utils.h"

using namespace std;
using ll = long long;
using cll = const long long;
using cb = const bool;
using List = vector<ll>;
using cList = const vector<ll>;
using AugmentedEventsList = vector<tuple<char, ll, ll, ll>>;

extern AugmentedEventsList eventsList;
extern EventsHandler ev_handler;

extern bool allMeasuremTime;
extern chrono::nanoseconds time_query_exists, time_clear_events, time_set_up_graph, time_active_nodes,
    time_active_component_nodes, time_static_edges, time_static_component_edges, time_add_edges,
    time_bridges, time_pruning, time_clear_graph, time_rec;

template<typename Func>
inline void measure_time(chrono::nanoseconds& t, Func&& f) {
    auto s = chrono::high_resolution_clock::now();
    f();
    t += chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - s);
}

#define MEASURE(t, block) measure_time(t, [&]{ block; })


class Sub_Routines {
    public: 

        static void process_side_connectivity(cll start, cll end, cb is_left, cll ev_start, cll ev_end, 
            Graph_Details& graph_details, unordered_map<ll, ll>& new_components_map) {

            // cout << "We are at: [" << start << ", " << end << "] - processing" << endl;
            // Logs_and_Printers::test_print_eventsList(start, end);

            auto& active_nodes_list = is_left ? ev_handler.left_active_list : ev_handler.right_active_list;
            vector<bool> active_component_nodes(graph_details.total_vertices, false);
            auto& static_edges = is_left ? ev_handler.static_left_edges : ev_handler.static_right_edges;
            vector<pair<ll, ll>> static_component_edges;

            unordered_map<ll, ll> old_components_map = graph_details.get_umap();

            if (allMeasuremTime) {
                MEASURE(time_active_nodes, ev_handler.findActiveNodes(start, end, is_left));
                MEASURE(time_active_component_nodes, 
                    Sub_Routines::compute_active_component_nodes(active_nodes_list, active_component_nodes, is_left, old_components_map));
                MEASURE(time_static_edges, ev_handler.findStaticEdges(ev_start, ev_end, is_left));
                MEASURE(time_static_component_edges, 
                    Sub_Routines::compute_static_component_edges(static_edges, static_component_edges, is_left, old_components_map));
                MEASURE(time_add_edges, add_edges_to_graph(static_component_edges, graph_details.enhanced_total_neighbours));
                MEASURE(time_bridges, graph_details.construct_bridges(active_component_nodes));
                MEASURE(time_pruning, Graph_Pruner::prune(graph_details, active_nodes_list, new_components_map));
                MEASURE(time_clear_graph, graph_details.clear());
                MEASURE(time_clear_events, ev_handler.clear());
            } else {
                ev_handler.findActiveNodes(start, end, is_left);
                Sub_Routines::compute_active_component_nodes(active_nodes_list, active_component_nodes, is_left, old_components_map);
                ev_handler.findStaticEdges(ev_start, ev_end, is_left);
                Sub_Routines::compute_static_component_edges(static_edges, static_component_edges, is_left, old_components_map);
                add_edges_to_graph(static_component_edges, graph_details.enhanced_total_neighbours);
                graph_details.construct_bridges(active_component_nodes);
                Graph_Pruner::prune(graph_details, active_nodes_list, new_components_map);
                graph_details.clear();
                ev_handler.clear();
            }
        
        }

        static void process_single_event(const tuple<char, ll, ll, ll> &value, vector<bool>& results, const unordered_map<ll, ll>& umap) {
            auto [type, x, y, _] = value;
            if (type == 'Q') 
                results.push_back(umap.at(x) == umap.at(y));
        }

        static void compute_static_component_edges(const vector<pair<ll, ll>>& static_edges, vector<pair<ll, ll>>& component_edges,
            bool isleft, const unordered_map<ll, ll>& umap) {
            for (const auto& [u, v] : static_edges) 
                try {
                    ll comp1 = umap.at(u), comp2 = umap.at(v);
                    if (comp1 != comp2) 
                        component_edges.emplace_back(comp1, comp2);
                } catch (const out_of_range& e) {
                    cerr << "Node not found: (" << u << ", " << v << ") - " << e.what() << endl;
                }
        }

        static void compute_active_component_nodes(const vector<ll>& active_nodes, vector<bool>& component_flags, bool isleft, 
            const unordered_map<ll, ll>& umap) {
            for (auto u : active_nodes) {
                auto it = umap.find(u);
                if (it != umap.end()) component_flags[it->second] = true;
            }
        }
};





