#include "sub_routines.h"
#include "../events-functions/events_handler.h"
#include "../include/print_and_log_utils.h"
#include "../include/memory_tracker.h"

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
extern chrono::time_point<std::chrono::high_resolution_clock> rec_start_t;
extern chrono::time_point<std::chrono::high_resolution_clock> rec_end_t;

#define MEASURE(t, block) measure_time(t, [&]{ block; })

inline chrono::nanoseconds time_query_exists{0}, time_clear_events{0}, time_set_up_graph{0}, time_active_nodes{0},
    time_active_component_nodes{0}, time_static_edges{0}, time_static_component_edges{0}, time_add_edges{0},
    time_bridges{0}, time_pruning{0}, time_clear_graph{0}, time_rec{0};

void clear_time_measurements() {
    time_query_exists = time_clear_events = time_set_up_graph = time_active_nodes =
    time_active_component_nodes = time_static_edges = time_static_component_edges =
    time_add_edges = time_bridges = time_pruning = time_clear_graph = time_rec = chrono::nanoseconds{0};
}

void compute_2_edge_connectivity(Graph_Details left_graph_details, cll ev_start, cll ev_end, 
    vector<bool>& results, const unordered_map<ll, ll>& components_map) {

    MEM_LOG("recursion", left_graph_details, components_map);

    rec_end_t = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(rec_end_t - rec_start_t);
    time_rec += duration;

    // cout << "We are at: [" << ev_start << ", " << ev_end << "] - recursion" << endl;
    if (!ev_handler.hasQueryInRange(ev_start, ev_end)) return;

    if (ev_start == ev_end) {
        Sub_Routines::process_single_event(eventsList[ev_start], results, components_map);
        return;
    }

    // Logs_and_Printers::test_print_eventsList(ev_start, ev_end);
    // left_graph_details.print_enhanced_graph("At the beginning----");

    Graph_Details right_graph_details = left_graph_details;

    if (allMeasuremTime) {
        MEASURE(time_set_up_graph, {
            left_graph_details.set_umap(components_map);
            right_graph_details.set_umap(components_map);
        });
    } else {
        left_graph_details.set_umap(components_map);
        right_graph_details.set_umap(components_map);
    }

    cll mid = (ev_start + ev_end) / 2;
    cll l0 = ev_start, l1 = mid, r0 = mid + 1, r1 = ev_end;

    bool ql, qr;
    if (allMeasuremTime) {
        MEASURE(time_query_exists, {
            ql = ev_handler.hasQueryInRange(l0, l1);
            qr = ev_handler.hasQueryInRange(r0, r1);
        });
    } else {
        ql = ev_handler.hasQueryInRange(l0, l1);
        qr = ev_handler.hasQueryInRange(r0, r1);
    }
    
    if (ql) {
        unordered_map<ll, ll> new_components_map;
        Sub_Routines::process_side_connectivity(l0, l1, true, ev_start, ev_end, left_graph_details, new_components_map);
        rec_start_t = chrono::high_resolution_clock::now();
        compute_2_edge_connectivity(left_graph_details, l0, l1, results, new_components_map);
    }
    if (qr) {
        unordered_map<ll, ll> new_components_map;
        Sub_Routines::process_side_connectivity(r0, r1, false, ev_start, ev_end, right_graph_details, new_components_map);
        compute_2_edge_connectivity(right_graph_details, r0, r1, results, new_components_map);
    }
  
}
