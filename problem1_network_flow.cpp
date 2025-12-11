#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <random>
#include <chrono>
#include <fstream>
#include <cmath>
#include <map>
#include <set>

using namespace std;

// Maximum Flow using Edmonds-Karp (BFS-based Ford-Fulkerson)
class MaxFlow {
private:
    int n;
    vector<vector<int>> capacity;
    vector<vector<int>> adj;
    
    bool bfs(int source, int sink, vector<int>& parent) {
        fill(parent.begin(), parent.end(), -1);
        parent[source] = source;
        queue<pair<int, int>> q;
        q.push(make_pair(source, INT_MAX));
        
        while (!q.empty()) {
            int u = q.front().first;
            int flow = q.front().second;
            q.pop();
            
            for (int v : adj[u]) {
                if (parent[v] == -1 && capacity[u][v] > 0) {
                    parent[v] = u;
                    int new_flow = min(flow, capacity[u][v]);
                    if (v == sink) {
                        return true;
                    }
                    q.push(make_pair(v, new_flow));
                }
            }
        }
        return false;
    }
    
public:
    MaxFlow(int n) : n(n), capacity(n, vector<int>(n, 0)), adj(n) {}
    
    void addEdge(int u, int v, int cap) {
        if (capacity[u][v] == 0 && capacity[v][u] == 0) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        capacity[u][v] += cap;
    }
    
    int maxflow(int source, int sink) {
        int flow = 0;
        vector<int> parent(n);
        
        while (bfs(source, sink, parent)) {
            int path_flow = INT_MAX;
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                path_flow = min(path_flow, capacity[u][v]);
            }
            
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                capacity[u][v] -= path_flow;
                capacity[v][u] += path_flow;
            }
            
            flow += path_flow;
        }
        
        return flow;
    }
    
    vector<pair<pair<int,int>, int>> getUsedCorridors(int numHabitats) {
        vector<pair<pair<int,int>, int>> corridors;
        for (int i = 0; i < numHabitats; i++) {
            for (int j = i + 1; j < numHabitats; j++) {
                // Check reverse capacity (used flow)
                if (capacity[j][i] > 0) {
                    corridors.push_back({{i, j}, capacity[j][i]});
                }
            }
        }
        return corridors;
    }
};

// Wildlife Corridor Network Design Problem
class WildlifeCorridorNetwork {
private:
    int numHabitats;
    vector<pair<double, double>> habitatLocations; // (x, y) coordinates
    map<pair<int,int>, int> corridorCapacity; // Terrain suitability
    int sourceHabitat;
    int targetHabitat;
    
    // Calculate distance between habitats
    double distance(int h1, int h2) {
        double dx = habitatLocations[h1].first - habitatLocations[h2].first;
        double dy = habitatLocations[h1].second - habitatLocations[h2].second;
        return sqrt(dx * dx + dy * dy);
    }
    
    // Calculate corridor capacity based on terrain suitability
    int calculateCorridorCapacity(int h1, int h2, double maxDist) {
        double dist = distance(h1, h2);
        if (dist > maxDist) return 0; // Too far for corridor
        
        // Capacity decreases with distance (terrain difficulty)
        // Formula: capacity = max_capacity * (1 - dist/maxDist)^2
        double normalized = 1.0 - (dist / maxDist);
        int capacity = (int)(100 * normalized * normalized);
        
        return max(1, capacity); // Minimum capacity of 1 if corridor exists
    }
    
public:
    WildlifeCorridorNetwork(int habitats, int source, int target) 
        : numHabitats(habitats), sourceHabitat(source), targetHabitat(target) {
        habitatLocations.resize(habitats);
    }
    
    void setHabitatLocation(int habitat, double x, double y) {
        habitatLocations[habitat] = {x, y};
    }
    
    void buildCorridorNetwork(double maxCorridorDistance) {
        // Build all possible corridors based on distance
        for (int i = 0; i < numHabitats; i++) {
            for (int j = i + 1; j < numHabitats; j++) {
                int capacity = calculateCorridorCapacity(i, j, maxCorridorDistance);
                if (capacity > 0) {
                    corridorCapacity[{i, j}] = capacity;
                }
            }
        }
    }
    
    // Reduce to Maximum Flow and solve
    pair<int, vector<pair<pair<int,int>, int>>> solve() {
        MaxFlow mf(numHabitats);
        
        // Add all corridors as edges
        for (auto& corridor : corridorCapacity) {
            int h1 = corridor.first.first;
            int h2 = corridor.first.second;
            int cap = corridor.second;
            
            // Bidirectional corridors
            mf.addEdge(h1, h2, cap);
            mf.addEdge(h2, h1, cap);
        }
        
        // Compute maximum flow
        int maxFlow = mf.maxflow(sourceHabitat, targetHabitat);
        
        // Get utilized corridors
        auto usedCorridors = mf.getUsedCorridors(numHabitats);
        
        return {maxFlow, usedCorridors};
    }
    
    // Generate random habitat network
    static WildlifeCorridorNetwork generateRandom(int numHabitats, 
                                                   double regionSize,
                                                   int seed) {
        mt19937 gen(seed);
        uniform_real_distribution<> locDist(0.0, regionSize);
        
        // Pick source and target on opposite sides
        int source = 0;
        int target = numHabitats - 1;
        
        WildlifeCorridorNetwork wcn(numHabitats, source, target);
        
        // Place habitats randomly
        for (int i = 0; i < numHabitats; i++) {
            wcn.setHabitatLocation(i, locDist(gen), locDist(gen));
        }
        
        // Ensure source and target are far apart
        wcn.setHabitatLocation(source, 0, 0);
        wcn.setHabitatLocation(target, regionSize, regionSize);
        
        return wcn;
    }
    
    int getNumCorridors() const {
        return corridorCapacity.size();
    }
};

// Experimental timing
void runExperiments() {
    ofstream outfile("data/wildlife_network_flow_results.csv");
    outfile << "n_habitats,corridors,time_ms,max_flow\n";
    
    vector<int> sizes = {10, 15, 20, 25, 30, 35, 40, 45, 50};
    double regionSize = 100.0;
    double maxCorridorDist = 35.0;
    
    for (int n : sizes) {
        auto wcn = WildlifeCorridorNetwork::generateRandom(n, regionSize, 42 + n);
        wcn.buildCorridorNetwork(maxCorridorDist);
        
        auto start = chrono::high_resolution_clock::now();
        auto result = wcn.solve();
        auto end = chrono::high_resolution_clock::now();
        
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        double ms = duration.count() / 1000.0;
        
        outfile << n << "," << wcn.getNumCorridors() << "," 
                << ms << "," << result.first << "\n";
        
        cout << "Habitats=" << n << ", Corridors=" << wcn.getNumCorridors() 
             << ", Time=" << ms << "ms, MaxFlow=" << result.first << "\n";
    }
    
    outfile.close();
    cout << "Results saved to data/wildlife_network_flow_results.csv\n";
}

int main() {
    cout << "==================================================\n";
    cout << "Wildlife Corridor Network Design Problem\n";
    cout << "Domain: Conservation Ecology\n";
    cout << "Reduction to: Maximum Flow\n";
    cout << "==================================================\n\n";
    
    // Example problem
    cout << "Example: 6 habitat patches, connecting endangered species populations\n\n";
    
    WildlifeCorridorNetwork wcn(6, 0, 5);
    
    // Set habitat locations (in km)
    wcn.setHabitatLocation(0, 0, 0);      // Source: Main reserve
    wcn.setHabitatLocation(1, 20, 10);    // Intermediate patch
    wcn.setHabitatLocation(2, 15, 25);    // Intermediate patch
    wcn.setHabitatLocation(3, 40, 15);    // Intermediate patch
    wcn.setHabitatLocation(4, 35, 35);    // Intermediate patch
    wcn.setHabitatLocation(5, 60, 50);    // Target: Secondary reserve
    
    // Build corridor network (max distance 35 km)
    wcn.buildCorridorNetwork(35.0);
    
    cout << "Number of feasible corridors: " << wcn.getNumCorridors() << "\n";
    
    auto result = wcn.solve();
    int maxAnimalFlow = result.first;
    auto usedCorridors = result.second;
    
    cout << "\nMaximum animal movement capacity: " << maxAnimalFlow << " animals/year\n";
    cout << "\nCorridors to construct:\n";
    for (auto& corridor : usedCorridors) {
        int h1 = corridor.first.first;
        int h2 = corridor.first.second;
        int flow = corridor.second;
        cout << "  Habitat " << h1 << " <-> Habitat " << h2 
             << " (capacity: " << flow << " animals/year)\n";
    }
    
    cout << "\n\nRunning experiments for different network sizes...\n";
    runExperiments();
    
    return 0;
}