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
extern chrono::nanoseconds time_bridges, time_pruning;

template<typename Func>
inline void measure_time(chrono::nanoseconds& t, Func&& f) {
    auto s = chrono::high_resolution_clock::now();
    f();
    t += chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - s);
}

#define MEASURE(t, block) measure_time(t, [&]{ block; })


class Sub_Routines {
    public: 
        static void process_side_connectivity(cll start, cll end, cb is_left, cll ev_start, cll ev_end, Graph_Details& g_details, 
            const unordered_map<ll, ll>& old_components_map, unordered_map<ll, ll>& new_components_map) {
            
            auto& active_nodes = is_left ? ev_handler.left_active_list : ev_handler.right_active_list;
            auto& static_edges = is_left ? ev_handler.static_left_edges : ev_handler.static_right_edges;
            vector<bool> active_component_nodes(g_details.get_total_vertices(), false);
            vector<pair<ll, ll>> static_component_edges;
            // cout << "We are at: [" << start << ", " << end << "] - processing" << endl;
            ev_handler.findActiveNodes(start, end, is_left);
            Sub_Routines::compute_active_component_nodes(active_nodes, active_component_nodes, old_components_map);
            ev_handler.findStaticEdges(ev_start, ev_end, is_left);
            Sub_Routines::compute_static_component_edges(static_edges, static_component_edges, is_left, old_components_map);
            g_details.add_edges(static_component_edges);
            g_details.construct_bridges(active_component_nodes);
            Graph_Pruner::prune(g_details, active_nodes, old_components_map, new_components_map);
            ev_handler.clear();
        }

        static void process_single_event(const tuple<char, ll, ll, ll> &value, vector<bool>& results, const unordered_map<ll, ll>& umap) {
            const auto& [type, x, y, _] = value;
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

        static void compute_active_component_nodes(const vector<ll>& active_nodes, vector<bool>& active_component_nodes, 
            const unordered_map<ll, ll>& umap) {
            for (const auto& u : active_nodes) {
                const auto& it = umap.find(u);
                if (it != umap.end()) 
                    active_component_nodes[it->second] = true;
            }
        }
};





