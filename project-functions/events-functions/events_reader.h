#pragma once

using EventsList = vector<tuple<char, ll, ll>>;
using c_str = const string&;

class EventsLoader {
public:
    AugmentedEventsList getTotalEvents(c_str graphId, c_str eventId) {
        return readAugmentedEvents(graphId, eventId);
    }

private:
    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator() (const std::pair<T1, T2>& p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };

    EventsList readGraphEvents(c_str id) {
        EventsList events;
        ifstream infile("../graphs/graph_" + id + ".txt");
        if (infile) {
            ll v1, v2, vertexCount, edgeCount;
            int dummy;
            infile >> dummy >> vertexCount >> edgeCount;
            while (infile >> v1 >> v2)
                events.emplace_back('I', v1, v2);
        }
        return events;
    }

    EventsList readEventLog(c_str id) {
        EventsList events;
        ifstream infile("../events/event_" + id + ".txt");
        if (infile) {
            char type;
            ll v1, v2, count;
            infile >> count;
            while (infile >> type >> v1 >> v2)
                events.emplace_back(type, v1, v2);
        }
        return events;
    }

    void mapDeletionsToInsertions(AugmentedEventsList& events) {
        unordered_map<pair<ll, ll>, ll, pair_hash> insertMap;
        for (ll i = 0; i < events.size(); ++i) {
            auto& [type, u, v, ref] = events[i];
            auto key = minmax(u, v);
            if (type == 'I') {
                insertMap[key] = i;
            } else if (type == 'D') {
                auto it = insertMap.find(key);
                if (it != insertMap.end()) {
                    ref = it->second;
                    get<3>(events[it->second]) = i;
                    insertMap.erase(it);
                }
            }
        }
    }

    AugmentedEventsList readAugmentedEvents(c_str graphId, c_str eventId) {
        auto graphEvents = readGraphEvents(graphId);
        auto dynamicEvents = readEventLog(graphId + "." + eventId);
        AugmentedEventsList augmented;

        for (const auto& [type, u, v] : graphEvents)
            augmented.emplace_back(type, u, v, -1);
        for (const auto& [type, u, v] : dynamicEvents)
            augmented.emplace_back(type, u, v, -1);

        mapDeletionsToInsertions(augmented);
        return augmented;
    }
};
