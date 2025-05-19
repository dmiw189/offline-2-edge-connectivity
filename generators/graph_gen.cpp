#include <bits/stdc++.h>

using namespace std;
namespace fs = std::filesystem;
using ll = long long;

// Hash function for pairs to use in unordered_set
struct pair_hash {
    template<typename T1, typename T2>
    size_t operator()(const pair<T1, T2>& p) const {
        return hash<T1>{}(p.first) ^ (hash<T2>{}(p.second) << 1);
    }
};

// Faster random integer generation
inline ll randomInt(ll a, ll b) {
    return a + rand() % (b - a + 1);
}

// Generates number of test cases
int generateNumTestCases(bool isRandom, int z) {
    return isRandom ? randomInt(2, 5 * z + 2) : 1;
}

// Predefined vertices
ll generateVertices(int z) {
    static const ll vertices[] = {
        10, 40, 100, 400, 1000, 3000, 6000, 10000, 30000,
        80000, 100000, 300000, 600000, 1000000,
        5000000, 10000000, 40000000, 90000000, 130000000
    };
    return vertices[max(0, min(18, z))];
}

// Generate a high-density number of edges
ll generateHighDensityEdges(const ll V, const int T, const int z) {
   ll numEdges;
   do {
       // The number of edges is between 0 and V * 2 * T + (1LL << z) + 2 + 2 * T, ensuring high density
       numEdges = randomInt(0, V * 2 * T + (1LL << z) + 2 + 2 * T);
   } while (numEdges < V || numEdges >= V * V / 2);  // Ensure numEdges is within a valid range
   return numEdges;
}

// Generate a low-density number of edges
ll generateLowDensityEdges(const ll V) {
   return randomInt(V / 5, V);
}

ll generateEdges(const ll V, const int T, const int z) {
   return T == 1 ? generateLowDensityEdges(V) : generateHighDensityEdges(V, T, z);
}

unordered_set<pair<ll, ll>, pair_hash> generateUniqueEdges(ll V, ll M) {
   unordered_set<pair<ll, ll>, pair_hash> edges;
   while (edges.size() < M) {
       ll x = randomInt(0, V - 1);
       ll y = randomInt(0, V - 1);
       // Ensure no self-loops and no duplicate edges
       if (x != y) {
           if (edges.count({x, y}) == 0) {
               edges.insert({x, y});
           }
       }
       // Log the current edge count for every 100 edges
       if (edges.size() % 1000 == 0) {
           cout << "Current edge count: " << edges.size() << "/" << M << endl;
       }
   }
   return edges;
}


// Write graph to file
void writeGraph(ofstream& outfile, int T, ll V, const unordered_set<pair<ll, ll>, pair_hash>& edges) {
    outfile << T << '\n';
    outfile << V << ' ' << edges.size() << '\n';
    for (const auto& [u, v] : edges) {
        outfile << u << ' ' << v << '\n';
    }
}

// Generate one graph file
void generateGraphFile(int z) {
    string graphsFolder = "../graphs";
    string outputPath = graphsFolder + "/graph_" + to_string(z) + ".txt";

    if (!fs::exists(graphsFolder)) {
        if (!fs::create_directories(graphsFolder)) {
            cerr << "Error: Unable to create directory " << graphsFolder << '\n';
            return;
        }
    }

    ofstream outfile(outputPath);
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open file " << outputPath << '\n';
        return;
    }

    int T = generateNumTestCases(false, 0);
    ll V = generateVertices(z);
    ll M = generateEdges(V, T, z);
    auto edges = generateUniqueEdges(V, M);

    writeGraph(outfile, T, V, edges);

    cout << "Generated graph file: " << outputPath << " with V=" << V << "and M=" << M << '\n' << flush;
}

int main() {
    ios::sync_with_stdio(false);
    srand(time(nullptr)); // Seed the random generator once here

    int from, to;
    do {
        cout << "Please give from (0-18): ";
        cin >> from;
    } while (from < 0 || from > 18);

    do {
        cout << "Please give to (from-18): ";
        cin >> to;
    } while (to < from || to > 18);

    for (int z = from; z <= to; ++z) {
        generateGraphFile(z);
    }

    return 0;
}
