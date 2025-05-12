#pragma once

void test_print(const BridgeGraph& bridge_graph, const Event& ev, ll start_index, ll end_index, bool left) {
    const string side = left ? "left" : "right";
    const auto& events = ev.total_augmented_events;
    for (int i = start_index; i <= end_index; ++i) {
        const auto& [type, x, y, z] = events[i];
        // Uncomment for debugging
        // cout << type << " " << x << " " << y << " " << z << endl;
    }

    const auto& active_nodes = left ? ev.left_active_nodes : ev.right_active_nodes;
    for (int i = 0; i < bridge_graph.total_vertices; ++i) {
        // Uncomment for debugging
        // cout << i << ": " << (active_nodes[i] ? "Active" : "Inactive") << endl;
    }
}

void process_single_event(const Event& ev, ll index, vector<bool>& results, const unordered_map<ll, ll>& umap) {
    auto [type, x, y, z] = ev.total_augmented_events[index];
    if (type == 'Q') {
        results.push_back(umap.at(x) == umap.at(y));
    }
}

void compute_static_component_edges(const vector<pair<ll, ll>>& static_edges,
                                           vector<pair<ll, ll>>& component_edges,
                                           bool isleft,
                                           const unordered_map<ll, ll>& umap) {
    for (const auto& [u, v] : static_edges) {
        try {
            ll comp1 = umap.at(u), comp2 = umap.at(v);
            if (comp1 != comp2) {
                component_edges.emplace_back(comp1, comp2);
            }
        } catch (const out_of_range& e) {
            cerr << "Node not found: (" << u << ", " << v << ") - " << e.what() << endl;
        }
    }
}

void compute_active_component_nodes(const vector<ll>& active_nodes, vector<bool>& component_flags, bool isleft, 
    const unordered_map<ll, ll>& umap) {
    for (auto u : active_nodes) {
        auto it = umap.find(u);
        if (it != umap.end()) component_flags[it->second] = true;
    }
}

