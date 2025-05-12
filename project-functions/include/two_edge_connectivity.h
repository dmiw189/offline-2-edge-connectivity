#include "..//GRAPH_UTILS/Pruning.h"
#include "time_utils.h"
#include "test_utils.h"
#include <chrono>

using ll = long long;

inline chrono::nanoseconds time_query_exists{0}, time_clear_events{0},
                            time_set_up_graph{0}, time_active_nodes{0},
                            time_active_component_nodes{0}, time_static_edges{0},
                            time_static_component_edges{0}, time_add_edges{0},
                            time_bridges{0}, time_pruning{0}, time_clear_graph{0};

                            
void clear_time_measurements() {
    time_query_exists = time_clear_events = time_set_up_graph = time_active_nodes =
    time_active_component_nodes = time_static_edges = time_static_component_edges =
    time_add_edges = time_bridges = time_pruning = time_clear_graph = chrono::nanoseconds{0};
}

/**
 * Computes the 2-edge connectivity of a dynamic graph by recursively processing event ranges.
 * This method divides the event range into left and right halves. For each half:
 *  0.  clear_active_nodes: clear the old active vertices.
 *  1a. find_active_nodes: finds the original/first active vertices and then 
 *  1b. compute_active_component_nodes: computes the current representative active vertices
 *  2a. find_static_edges: finds static edges of the original/first edges and then 
 *  2b. compute_static_component_edges: compute the representative current ones
 *  2c. add_edges_to_graph: it adds the representative edges to the correct graph 
 *  3.  Identical graph routine: shrinking the graph with:
 *      3a. construct bridges, make the bridge tree graph: 
 *          - find the bridges of the graph
 *          - total_bridge_neighbours: the new representative graph representation
 *          (from the bridges find the bridge tree graph that has united all the 2-edge-components into one representative vertex)
 *          - bridge_tree_map: a map that maps the current vertices with the new representative ones 
 *      3b. final pruning:
 *          - prune the inactive vertices with degree 2, 1 and 0:
 *              the only challenging is the degree 1 which is the only one that change 
 *              the degree of the neighbor's vertex
 *              Solution: starts from the leaves of the graph as a queue. 
 *              It manages an optimal linear time complexity  
 *          - update_graph: we update the old-current graph with the newest representative graph and 
 *          - update_components: its new map (=the component representations)      
 *  4. graph.clear: clear all the temporal structures at graph object. 
 *          it is optimal due to the fact that all the fields 
 *          that are being cleared have the current-recuirsive size of the graph

 * @param bridge_graph The bridge graph representing the current state of the graph with:
 *  1. total_neighbours: the current graph representation
 *  2. total_vertices: the current number of vertices
 *  All the others are temporal structures 
 * @param ev The event manager that has:
 *  1. total_augmented_events: the events/queue of fully dynamic queries from input 
 *    -> static: potential problem
 *          we keep it and pass it through each recuirsion. 
 *          Maybe here is the problem???
 *          because we don't pass it as reference? 
 *  2. static_left, static_right: the permanent edges at each half (temporal)
 *  3. left_active_nodes, right_active_nodes: for each current vertex what is active or not at each half
 *      -> static: potential problem:
 *          they have the size of the first original number of vertices
 *          but the current clear function is optimal from time perspective 
 *  4. left_active_nodes_list, right_active_nodes_list: a list for all the current active vertices (temporal)     
 * @param start_index The starting index of the event range to process.
 * @param end_index The ending index of the event range to process.
 * @param results A boolean vector storing the connectivity results.
 * @param umap_left A mapping of vertex IDs for the left partition:
 * @param umap_right A mapping of vertex IDs for the right partition
 *      umap_left, umap_right: they are the component_map representations as maps and not as vector, 
 *      because the size of this vector/array can be very large and instead of pass it as a reference, 
 *      we make a map to reduce the size. The size of the map must be at the size of the active nodes at each partition, 
 *      so the size difference is significant, and its clearer for perspective reasons.
 *      Potential problem: The Map representation is not theoritical optimal, 
 *                  but I have seen that the theoritical optiomal, a reference to a vector/array of 
 *                  the original/first size (the original total vertices) can be very large and it will delay the program
 *      Solution: I think this is not the problem, as it was an optimization to the large size 
 *                  of the original component_map representation   
 */
void compute_2_edge_connectivity(BridgeGraph bridge_graph, Event ev, const ll start_index, const ll end_index, vector<bool>& results, 
                                 unordered_map<ll, ll> umap_left, unordered_map<ll, ll> umap_right) {
    
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    // Base check: if no events exist in the range, return
    auto start = chrono::high_resolution_clock::now();
    if (!ev.query_exists_in_events(start_index, end_index)) return;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
    time_query_exists += duration;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // cout << "[" << start_index << ", " << end_index << "]" << endl;

    // Handle single-event case
    if (start_index == end_index) {
        process_single_event(ev, start_index, results, umap_left);
        return;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // Initial setup
    bridge_graph.print_enhanced_graph("At the beginning----");
    start = chrono::high_resolution_clock::now();
    ev.clear_active_nodes();
    ev.clear_static_edges();
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
    time_clear_events += duration;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    start = chrono::high_resolution_clock::now();
    BridgeGraph right_bridge_graph = bridge_graph;
    bridge_graph.set_umap(umap_left);
    right_bridge_graph.set_umap(umap_right);

    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
    time_set_up_graph += duration;

    // Determine left and right ranges
    ll left_start = start_index;
    ll left_end = left_start + floor((end_index - start_index) / 2);
    ll right_start = left_end + 1;
    ll right_end = end_index;

    // Initialize static component edges for left and right
    vector<pair<ll, ll>> left_static_component_edges, right_static_component_edges;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Query event existence for left and right ranges

    start = chrono::high_resolution_clock::now();
    bool leftExists = ev.query_exists_in_events(left_start, left_end);
    bool rightExists = ev.query_exists_in_events(right_start, right_end);

    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
    time_query_exists += duration;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // Lambda to handle processing for each side
    auto process_side = [&](bool exists, auto& ev_nodes_list, auto& static_edges, auto& static_component_edges,
                            auto& umap, BridgeGraph& graph, ll start, ll end, bool isLeft) {
        if (!exists) return;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        
        auto start_t = chrono::high_resolution_clock::now();
        // Find active nodes and compute active component nodes
        ev.find_active_nodes(start, end, isLeft);

        auto end_t = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(end_t - start_t);
        time_active_nodes += duration;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        start_t = chrono::high_resolution_clock::now();
        vector<bool> active_component_nodes(graph.total_vertices, false);
        compute_active_component_nodes(ev_nodes_list, active_component_nodes, isLeft, umap);

        end_t = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::nanoseconds>(end_t - start_t);
        time_active_component_nodes += duration;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Find and compute static component edges

        start_t = chrono::high_resolution_clock::now();
        ev.find_static_edges(start_index, end_index, isLeft);
        end_t = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::nanoseconds>(end_t - start_t);
        time_static_edges += duration;


        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        start_t = chrono::high_resolution_clock::now();
        compute_static_component_edges(static_edges, static_component_edges, isLeft, umap);
        end_t = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::nanoseconds>(end_t - start_t);
        time_static_component_edges += duration;


        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        start_t = chrono::high_resolution_clock::now();
        // Add static component edges to graph
        add_edges_to_graph(static_component_edges, graph.enhanced_total_neighbours);
        end_t = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::nanoseconds>(end_t - start_t);
        time_add_edges += duration;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Construct bridges and prune

        start_t = chrono::high_resolution_clock::now();
        graph.construct_bridges(active_component_nodes);
        end_t = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::nanoseconds>(end_t - start_t);
        time_bridges += duration;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        start_t = chrono::high_resolution_clock::now();
        final_pruning(graph, ev_nodes_list);
        end_t = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::nanoseconds>(end_t - start_t);
        time_pruning += duration;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Clear the graph

        start_t = chrono::high_resolution_clock::now();
        graph.clear();
        end_t = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::nanoseconds>(end_t - start_t);
        time_clear_graph += duration;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        // Recursive call for further subdivisions
        compute_2_edge_connectivity(graph, ev, start, end, results, umap, umap);
    };

    // Process left and right sides
    process_side(leftExists, ev.left_active_nodes_list, ev.static_left, left_static_component_edges,
                 umap_left, bridge_graph, left_start, left_end, true);

    process_side(rightExists, ev.right_active_nodes_list, ev.static_right, right_static_component_edges,
                 umap_right, right_bridge_graph, right_start, right_end, false);
}

