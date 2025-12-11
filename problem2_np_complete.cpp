#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <random>
#include <chrono>
#include <fstream>
#include <set>
#include <map>

using namespace std;

// DNA Fragment Assembly Problem
class DNAFragmentAssembly {
private:
    int numFragments;
    vector<string> fragments;
    vector<vector<int>> overlapGraph; // overlapGraph[i][j] = overlap length
    int minOverlap;
    
    // Calculate overlap between two fragments
    int calculateOverlap(const string& frag1, const string& frag2) {
        int maxOverlap = min(frag1.length(), frag2.length());
        
        for (int overlap = maxOverlap; overlap >= minOverlap; overlap--) {
            // Check if suffix of frag1 matches prefix of frag2
            bool match = true;
            for (int i = 0; i < overlap; i++) {
                if (frag1[frag1.length() - overlap + i] != frag2[i]) {
                    match = false;
                    break;
                }
            }
            if (match) return overlap;
        }
        return 0;
    }
    
public:
    DNAFragmentAssembly(vector<string> frags, int minOverlap = 3) 
        : fragments(frags), numFragments(frags.size()), minOverlap(minOverlap) {
        // Build overlap graph
        overlapGraph.resize(numFragments, vector<int>(numFragments, 0));
        
        for (int i = 0; i < numFragments; i++) {
            for (int j = 0; j < numFragments; j++) {
                if (i != j) {
                    overlapGraph[i][j] = calculateOverlap(fragments[i], fragments[j]);
                }
            }
        }
    }
    
    // Greedy assembly: always pick fragment with maximum overlap
    pair<string, vector<int>> greedyAssemble() {
        vector<bool> used(numFragments, false);
        vector<int> order;
        
        // Start with first fragment
        int current = 0;
        used[current] = true;
        order.push_back(current);
        
        // Greedily add fragments
        for (int step = 1; step < numFragments; step++) {
            int bestNext = -1;
            int bestOverlap = -1;
            
            // Find unused fragment with maximum overlap
            for (int j = 0; j < numFragments; j++) {
                if (!used[j] && overlapGraph[current][j] > bestOverlap) {
                    bestOverlap = overlapGraph[current][j];
                    bestNext = j;
                }
            }
            
            if (bestNext == -1) {
                // No valid next fragment, pick any unused
                for (int j = 0; j < numFragments; j++) {
                    if (!used[j]) {
                        bestNext = j;
                        bestOverlap = 0;
                        break;
                    }
                }
            }
            
            used[bestNext] = true;
            order.push_back(bestNext);
            current = bestNext;
        }
        
        // Construct assembled sequence
        string assembled = fragments[order[0]];
        for (size_t i = 1; i < order.size(); i++) {
            int prevIdx = order[i-1];
            int currIdx = order[i];
            int overlap = overlapGraph[prevIdx][currIdx];
            
            // Append non-overlapping part
            assembled += fragments[currIdx].substr(overlap);
        }
        
        return {assembled, order};
    }
    
    // Nearest neighbor heuristic
    pair<string, vector<int>> nearestNeighborAssemble() {
        vector<bool> used(numFragments, false);
        vector<int> order;
        
        // Start with fragment that has highest total overlap
        int start = 0;
        int maxTotalOverlap = 0;
        for (int i = 0; i < numFragments; i++) {
            int total = 0;
            for (int j = 0; j < numFragments; j++) {
                total += overlapGraph[i][j];
            }
            if (total > maxTotalOverlap) {
                maxTotalOverlap = total;
                start = i;
            }
        }
        
        int current = start;
        used[current] = true;
        order.push_back(current);
        
        // Add nearest neighbors
        for (int step = 1; step < numFragments; step++) {
            int bestNext = -1;
            int bestOverlap = -1;
            
            for (int j = 0; j < numFragments; j++) {
                if (!used[j] && overlapGraph[current][j] > bestOverlap) {
                    bestOverlap = overlapGraph[current][j];
                    bestNext = j;
                }
            }
            
            if (bestNext == -1) {
                for (int j = 0; j < numFragments; j++) {
                    if (!used[j]) {
                        bestNext = j;
                        break;
                    }
                }
            }
            
            used[bestNext] = true;
            order.push_back(bestNext);
            current = bestNext;
        }
        
        // Construct sequence
        string assembled = fragments[order[0]];
        for (size_t i = 1; i < order.size(); i++) {
            int prevIdx = order[i-1];
            int currIdx = order[i];
            int overlap = overlapGraph[prevIdx][currIdx];
            assembled += fragments[currIdx].substr(overlap);
        }
        
        return {assembled, order};
    }
    
    // Savings algorithm (look-ahead)
    pair<string, vector<int>> savingsAssemble() {
        vector<bool> used(numFragments, false);
        vector<int> order;
        
        // Calculate "savings" for each fragment based on forward-looking overlap
        vector<int> savings(numFragments);
        for (int i = 0; i < numFragments; i++) {
            int maxOut = 0;
            for (int j = 0; j < numFragments; j++) {
                if (i != j) {
                    maxOut = max(maxOut, overlapGraph[i][j]);
                }
            }
            savings[i] = maxOut;
        }
        
        // Start with fragment with maximum savings
        int current = max_element(savings.begin(), savings.end()) - savings.begin();
        used[current] = true;
        order.push_back(current);
        
        // Continue assembly
        for (int step = 1; step < numFragments; step++) {
            int bestNext = -1;
            int bestScore = -1;
            
            for (int j = 0; j < numFragments; j++) {
                if (!used[j]) {
                    // Score = current overlap + future potential
                    int score = overlapGraph[current][j] + savings[j];
                    if (score > bestScore) {
                        bestScore = score;
                        bestNext = j;
                    }
                }
            }
            
            if (bestNext == -1) {
                for (int j = 0; j < numFragments; j++) {
                    if (!used[j]) {
                        bestNext = j;
                        break;
                    }
                }
            }
            
            used[bestNext] = true;
            order.push_back(bestNext);
            current = bestNext;
        }
        
        // Construct sequence
        string assembled = fragments[order[0]];
        for (size_t i = 1; i < order.size(); i++) {
            int prevIdx = order[i-1];
            int currIdx = order[i];
            int overlap = overlapGraph[prevIdx][currIdx];
            assembled += fragments[currIdx].substr(overlap);
        }
        
        return {assembled, order};
    }
    
    // Verify solution quality
    pair<int, double> evaluateSolution(const vector<int>& order, 
                                        const string& original) {
        int totalOverlap = 0;
        for (size_t i = 0; i < order.size() - 1; i++) {
            totalOverlap += overlapGraph[order[i]][order[i+1]];
        }
        
        // Calculate accuracy if original is known
        double accuracy = 0.0;
        if (!original.empty()) {
            // Reconstruct sequence
            string assembled = fragments[order[0]];
            for (size_t i = 1; i < order.size(); i++) {
                int overlap = overlapGraph[order[i-1]][order[i]];
                assembled += fragments[order[i]].substr(overlap);
            }
            
            // Compute similarity
            int matches = 0;
            int len = min(assembled.length(), original.length());
            for (int i = 0; i < len; i++) {
                if (assembled[i] == original[i]) matches++;
            }
            accuracy = 100.0 * matches / max(assembled.length(), original.length());
        }
        
        return {totalOverlap, accuracy};
    }
    
    // Generate random DNA fragments from a sequence
    static pair<vector<string>, string> generateRandomFragments(
        int numFragments, int fragmentLength, int sequenceLength, int seed) {
        
        mt19937 gen(seed);
        uniform_int_distribution<> nucDist(0, 3);
        uniform_int_distribution<> posDist(0, sequenceLength - fragmentLength);
        
        const char nucleotides[] = {'A', 'C', 'G', 'T'};
        
        // Generate original sequence
        string original;
        for (int i = 0; i < sequenceLength; i++) {
            original += nucleotides[nucDist(gen)];
        }
        
        // Extract overlapping fragments
        vector<string> fragments;
        set<int> positions;
        
        while ((int)fragments.size() < numFragments) {
            int pos = posDist(gen);
            if (positions.count(pos) == 0 && pos + fragmentLength <= sequenceLength) {
                positions.insert(pos);
                fragments.push_back(original.substr(pos, fragmentLength));
            }
        }
        
        // Shuffle fragments to remove order
        shuffle(fragments.begin(), fragments.end(), gen);
        
        return {fragments, original};
    }
    
    int getNumFragments() const { return numFragments; }
    int getNumEdges() const {
        int edges = 0;
        for (int i = 0; i < numFragments; i++) {
            for (int j = 0; j < numFragments; j++) {
                if (i != j && overlapGraph[i][j] > 0) edges++;
            }
        }
        return edges;
    }
};

// Experimental timing
void runExperiments() {
    ofstream outfile("data/dna_assembly_results.csv");
    outfile << "n_fragments,edges,greedy_time_ms,greedy_overlap,nn_time_ms,nn_overlap,savings_time_ms,savings_overlap\n";
    
    vector<int> sizes = {10, 15, 20, 25, 30, 35, 40};
    int fragmentLength = 15;
    int sequenceLength = 200;
    
    for (int n : sizes) {
        auto result = DNAFragmentAssembly::generateRandomFragments(
            n, fragmentLength, sequenceLength, 42 + n);
        vector<string> fragments = result.first;
        string original = result.second;
        
        DNAFragmentAssembly dna(fragments, 3);
        
        // Greedy
        auto start1 = chrono::high_resolution_clock::now();
        auto result1 = dna.greedyAssemble();
        string seq1 = result1.first;
        vector<int> order1 = result1.second;
        auto end1 = chrono::high_resolution_clock::now();
        auto eval1 = dna.evaluateSolution(order1, original);
        int overlap1 = eval1.first;
        double acc1 = eval1.second;
        auto duration1 = chrono::duration_cast<chrono::microseconds>(end1 - start1);
        
        // Nearest Neighbor
        auto start2 = chrono::high_resolution_clock::now();
        auto result2 = dna.nearestNeighborAssemble();
        string seq2 = result2.first;
        vector<int> order2 = result2.second;
        auto end2 = chrono::high_resolution_clock::now();
        auto eval2 = dna.evaluateSolution(order2, original);
        int overlap2 = eval2.first;
        double acc2 = eval2.second;
        auto duration2 = chrono::duration_cast<chrono::microseconds>(end2 - start2);
        
        // Savings
        auto start3 = chrono::high_resolution_clock::now();
        auto result3 = dna.savingsAssemble();
        string seq3 = result3.first;
        vector<int> order3 = result3.second;
        auto end3 = chrono::high_resolution_clock::now();
        auto eval3 = dna.evaluateSolution(order3, original);
        int overlap3 = eval3.first;
        double acc3 = eval3.second;
        auto duration3 = chrono::duration_cast<chrono::microseconds>(end3 - start3);
        
        outfile << n << "," << dna.getNumEdges() << ","
                << duration1.count() / 1000.0 << "," << overlap1 << ","
                << duration2.count() / 1000.0 << "," << overlap2 << ","
                << duration3.count() / 1000.0 << "," << overlap3 << "\n";
        
        cout << "n=" << n << ", overlap: greedy=" << overlap1 
             << ", nn=" << overlap2 << ", savings=" << overlap3 << "\n";
    }
    
    outfile.close();
    cout << "Results saved to data/dna_assembly_results.csv\n";
}

int main() {
    cout << "==================================================\n";
    cout << "DNA Fragment Assembly Problem\n";
    cout << "Domain: Bioinformatics / Genome Sequencing\n";
    cout << "Reduction to: Hamiltonian Path (NP-Complete)\n";
    cout << "==================================================\n\n";
    
    // Example problem
    cout << "Example: Assembling DNA sequence from 5 fragments\n\n";
    
    vector<string> fragments = {
        "ATCGATCGAT",
        "TCGATCGATA",
        "GATCGATACG",
        "ATACGTACGT",
        "CGTACGTACG"
    };
    
    DNAFragmentAssembly dna(fragments, 3);
    
    cout << "Fragments:\n";
    for (size_t i = 0; i < fragments.size(); i++) {
        cout << "  Fragment " << i << ": " << fragments[i] << "\n";
    }
    
    cout << "\nGreedy Assembly:\n";
    auto result1 = dna.greedyAssemble();
    string assembled1 = result1.first;
    vector<int> order1 = result1.second;
    cout << "  Assembled sequence: " << assembled1 << "\n";
    cout << "  Order: ";
    for (int idx : order1) cout << idx << " ";
    cout << "\n";
    
    cout << "\nNearest Neighbor Assembly:\n";
    auto result2 = dna.nearestNeighborAssemble();
    string assembled2 = result2.first;
    vector<int> order2 = result2.second;
    cout << "  Assembled sequence: " << assembled2 << "\n";
    cout << "  Order: ";
    for (int idx : order2) cout << idx << " ";
    cout << "\n";
    
    cout << "\nSavings Algorithm Assembly:\n";
    auto result3 = dna.savingsAssemble();
    string assembled3 = result3.first;
    vector<int> order3 = result3.second;
    cout << "  Assembled sequence: " << assembled3 << "\n";
    cout << "  Order: ";
    for (int idx : order3) cout << idx << " ";
    cout << "\n";
    
    cout << "\n\nRunning experiments...\n";
    runExperiments();
    
    return 0;
}