#pragma once

using namespace std;
using ll = long long;
using cll = const long long;
using cb = const bool;
using List = vector<ll>;
using cList = const vector<ll>;
using AugmentedEventsList = vector<tuple<char, ll, ll, ll>>;

extern AugmentedEventsList eventsList;

class EventsHandler {
    public:

        EventsHandler() {}

        EventsHandler(cll totalVertices): left_active(totalVertices, false), right_active(totalVertices, false) {
        }

        vector<pair<ll, ll>> static_left_edges;
        vector<pair<ll, ll>> static_right_edges;

        //TODO CHANGE IT to HASH MAP?? (or all the calls as reference)    
        vector<ll> left_active_list;
        vector<ll> right_active_list;

        // Finds static edges on either the left or right side of the event range
        void findStaticEdges(cll start, cll end, cb is_left) {
            auto mid = (start + end) / 2;
            auto from = is_left ? mid + 1 : start;
            auto to   = is_left ? end     : mid;
            auto& edge_store = is_left ? static_left_edges : static_right_edges;

            for (auto i = from; i <= to; i++) {
                const auto& [type, x, y, ref] = eventsList[i];

                // A 'D' with no valid ref on the left or an 'I' with no valid ref on the right is static
                if ((is_left && type == 'D' && (ref == -1 || ref < start))  
                || (!is_left && type == 'I' && (ref == -1 || ref > end))) {
                    edge_store.emplace_back(x, y);
                }
            }
            // printStaticEdges(is_left);
        }


        void findActiveNodes(cll start, cll end, cb is_left) {
            auto& active = is_left ? left_active : right_active;
            auto& list = is_left ? left_active_list : right_active_list;

            for (ll i = start; i <= end; ++i) {
                const auto& [_, u, v, __] = eventsList[i];
                for (ll node : {u, v}) {
                    if (!active[node]) {
                        active[node] = true;
                        list.push_back(node);
                    }
                }
            }
        }

        bool hasQueryInRange(cll start, cll end) const {
            for (auto i = start; i <= end; i++)
                if (get<0>(eventsList[i]) == 'Q') 
                    return true;
            return false;
        }

        void clear() {
            for (auto u : left_active_list) left_active[u] = false;
            for (auto u : right_active_list) right_active[u] = false;
            left_active_list.clear();
            right_active_list.clear();
            static_left_edges.clear();
            static_right_edges.clear();
        }

    private:
        // Active node tracking
        vector<bool> left_active;
        vector<bool> right_active;

        void printStaticEdges(cb is_left) {
            auto& edges = is_left ? static_left_edges : static_right_edges;
            const char* side = is_left ? "Left" : "Right";
            
            cout << "---- Static Edges for the " << side << " part ----\n";
            for (const auto& [u, v] : edges)
                cout << "(" << u << ", " << v << ")\n";
            cout << "---- End of Static " << side << " Edges ----\n";
        }
};
