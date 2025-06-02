#include <bits/stdc++.h>

using namespace std;
namespace fs = std::filesystem;
using ll = long long;

struct hashFunction {
    size_t operator()(const pair<int, int> &x) const {
        return x.first ^ x.second;
    }
};

void open_graph_file(const string& graphFile, ifstream& infile) {
    infile.open(graphFile);
    if (!infile.is_open()) {
        cout << "Unable to open " << graphFile << endl;
    }
}

void read_graph_data(ifstream& infile, ll& numNodes, ll& numEdges, unordered_set<pair<ll, ll>, hashFunction>& edges) {
    infile >> numNodes >> numEdges;
    for (ll i = 0; i < numEdges; i++) {
        ll u, v;
        infile >> u >> v;
        edges.insert({u, v});
    }
}

void generate_insert_event(const ll numNodes, unordered_set<pair<ll, ll>, hashFunction>& edges, 
    vector<tuple<char, ll, ll>>& events) {
    ll u, v;
    do {
        u = rand() % numNodes;
        v = rand() % numNodes;
    } while (u == v || edges.count({u, v}) || edges.count({v, u}));
    edges.insert({u, v});
    events.emplace_back('I', u, v);
}

void generate_delete_event(unordered_set<pair<ll, ll>, hashFunction>& edges, 
    vector<tuple<char, ll, ll>>& events) {
    auto it = edges.begin();
    advance(it, rand() % edges.size());  
    ll u = it->first, v = it->second;
    edges.erase(it);
    events.emplace_back('D', u, v);
}

void generate_query_event(ll numNodes, vector<tuple<char, ll, ll>>& events) {
    ll u, v;
    do {
        u = rand() % numNodes;
        v = rand() % numNodes;
    } while (u == v);
    events.emplace_back('Q', u, v);
}

ll generateNumEvents(const ll numNodes, const int fileIdx) {
    return numNodes * (fileIdx + 1);
}

void create_events_directory(const string& eventsFolder) {
    if (!fs::exists(eventsFolder)) {
        if (!fs::create_directories(eventsFolder)) {
            cerr << "Error: Unable to create directory " << eventsFolder << '\n';
        }
    }
}

ofstream open_event_file(const string& eventFile) {
    ofstream outfile(eventFile);
    if (!outfile.is_open()) {
        cout << "Unable to open " << eventFile << endl;
    }
    return outfile;
}

ll calculate_num_events(const ll numNodes, const int fileIdx) {
    return fileIdx  <= 18 ? numNodes * (fileIdx + 1) : numNodes * 2;
}

void write_event_header(ofstream& outfile, ll numEvents) {
    outfile << numEvents << endl;
}

void generate_event(ll numNodes, unordered_set<pair<ll, ll>, hashFunction>& edges, 
    vector<tuple<char, ll, ll>>& events) {
    
    bool generateQuery = rand() % 2;
    if (generateQuery) {
        generate_query_event(numNodes, events);
    } else {
        bool generateInsert = rand() % 2;
        if (generateInsert && edges.size() < numNodes * (numNodes - 1) / 2) {
            generate_insert_event(numNodes, edges, events);
        } else if (!generateInsert && edges.size() > 0) {
            generate_delete_event(edges, events);
        } else {
            generate_query_event(numNodes, events);
        }
    }
}

void log_progress(const ll eventsSize, const ll numEvents) {
    if (eventsSize % 100 == 0) {
        cout << eventsSize << "/" << numEvents << endl;
    }
}

void write_events_to_file(ofstream& outfile, const vector<tuple<char, ll, ll>>& events) {
    for (const auto& event : events) {
        outfile << get<0>(event) << " " << get<1>(event) << " " << get<2>(event) << endl;
    }
}

void generate_events_for_file(const string graphID, const ll numNodes, const int fileIdx, 
    const int numEventFiles, unordered_set<pair<ll, ll>, hashFunction>& edges) {

    string eventsFolder = "../events";
    string eventFile = eventsFolder + "/event_" + graphID + "." + to_string(fileIdx) + ".txt";
    create_events_directory(eventsFolder);
    ofstream outfile = open_event_file(eventFile);
    if (!outfile.is_open()) return;

    ll numEvents = calculate_num_events(numNodes, fileIdx);
    write_event_header(outfile, numEvents);
    cout << "Generated event_" << graphID << "." + to_string(fileIdx) + " with " << numEvents << " events" << endl << flush;

    ll eventCount = 0;
    vector<tuple<char, ll, ll>> events;
    while (eventCount < numEvents) {
        generate_event(numNodes, edges, events);
        log_progress(events.size(), numEvents);
        eventCount++;
    }

    write_events_to_file(outfile, events);
    outfile.close();
}

void generate_events_with_dense_queries(const string& graphID, int numEventFiles) {
    string graphFile = "../graphs/graph_" + graphID + ".txt";
    ifstream infile;
    open_graph_file(graphFile, infile);
    if (!infile.is_open()) return;

    int numTestCases;
    infile >> numTestCases;

    if (numTestCases == 1) {
        for (int fileIdx = 0; fileIdx < numEventFiles; fileIdx++) {
            ll numNodes, numEdges;
            unordered_set<pair<ll, ll>, hashFunction> edges;
    
            read_graph_data(infile, numNodes, numEdges, edges);
            generate_events_for_file(graphID, numNodes, fileIdx, numEventFiles, edges);
        }
        infile.close();
    }
}

int main() {
    ios::sync_with_stdio(false);
    srand(time(nullptr)); // Seed the random generator once here

    int from, to, numEventFiles;
    cout << "Enter from (0-18), to (from-18), and number of event files (1-5): ";
    while (!(cin >> from >> to >> numEventFiles) || from < 0 || from > 18 || to < from || to > 18 || numEventFiles < 1 || numEventFiles > 5) {
        cout << "Invalid input. Please enter from (0-18), to (from-18), and number of event files (1-5): ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    for (auto graph_id = from; graph_id <= to; graph_id++) {
        generate_events_with_dense_queries(to_string(graph_id), numEventFiles);
    }

    generate_events_with_dense_queries(to_string(256), numEventFiles);
    generate_events_with_dense_queries(to_string(257), numEventFiles);

    return 0;
}
