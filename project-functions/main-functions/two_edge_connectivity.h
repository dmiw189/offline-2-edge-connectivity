#include "sub_routines.h"
#include "../events-functions/events_handler.h"
#include "../include/print_and_log_utils.h"
#include "../include/memory_tracker.h"

using namespace std;
using ll = long long;
using AugmentedEventsList = vector<tuple<char, ll, ll, ll>>;

extern AugmentedEventsList eventsList;
extern EventsHandler ev_handler;

void compute_2_edge_connectivity(ll ev_start, ll ev_end, Graph& graph, vector<bool>& results, const unordered_map<ll, ll>& old_components_map);

void process_range(cll l, cll r, cll ev_start, cll ev_end, cb is_left, const Graph& graph, const unordered_map<ll, ll>& old_components_map, vector<bool>& results) {
    if (!ev_handler.hasQueryInRange(l, r)) return;
    
    Graph_Details details(graph.get_total_vertices(), graph.get_graph());
    // cout << "We are at: [" << l << ", " << r << "] - process range" << endl;
    // Logs_and_Printers::print_components("Old Components", old_components_map);
    unordered_map<ll, ll> new_components_map;
    Sub_Routines::process_side_connectivity(l, r, is_left, ev_start, ev_end, details, old_components_map, new_components_map);
    // Logs_and_Printers::print_components("Updated Components", new_components_map);
    // details.print_graph("Pruned Graph)");
    Graph identical_graph(details.get_total_vertices(), details.get_graph());
    details.clear();
    compute_2_edge_connectivity(l, r, identical_graph, results, new_components_map);
}

void compute_2_edge_connectivity(cll ev_start, cll ev_end, Graph& graph, vector<bool>& results, const unordered_map<ll, ll>& old_components_map) {
    // cout << "We are at: [" << ev_start << ", " << ev_end << "] - recursion" << endl;
    if (ev_start == ev_end) {
        Sub_Routines::process_single_event(eventsList[ev_start], results, old_components_map);
        return;
    }
    if (!ev_handler.hasQueryInRange(ev_start, ev_end)) return;
    
    // Logs_and_Printers::test_print_eventsList(ev_start, ev_end);
    
    cll mid = (ev_start + ev_end) / 2;
    process_range(ev_start, mid, ev_start, ev_end, true, graph, old_components_map, results);
    process_range(mid + 1, ev_end, ev_start, ev_end, false, graph, old_components_map, results);
}
