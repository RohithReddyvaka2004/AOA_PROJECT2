import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# Set style
plt.style.use('seaborn-v0_8-darkgrid')

# Problem 1: Wildlife Corridor Network Flow Results
def plot_wildlife_network_flow():
    try:
        df = pd.read_csv('data/wildlife_network_flow_results.csv')
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(14, 10))
        
        # Time vs n
        ax1.plot(df['n_habitats'], df['time_ms'], 'go-', linewidth=2, markersize=8)
        ax1.set_xlabel('Number of Habitat Patches', fontsize=12)
        ax1.set_ylabel('Time (ms)', fontsize=12)
        ax1.set_title('Wildlife Corridor Network: Running Time', fontsize=14, fontweight='bold')
        ax1.grid(True, alpha=0.3)
        
        # Fit polynomial to show complexity
        z = np.polyfit(df['n_habitats'], df['time_ms'], 2)
        p = np.poly1d(z)
        x_smooth = np.linspace(df['n_habitats'].min(), df['n_habitats'].max(), 100)
        ax1.plot(x_smooth, p(x_smooth), 'r--', linewidth=2, alpha=0.7, label='Quadratic Fit')
        ax1.legend()
        
        # Max flow vs habitats
        ax2.plot(df['n_habitats'], df['max_flow'], 'bs-', linewidth=2, markersize=8)
        ax2.set_xlabel('Number of Habitat Patches', fontsize=12)
        ax2.set_ylabel('Maximum Animal Flow (animals/year)', fontsize=12)
        ax2.set_title('Animal Movement Capacity', fontsize=14, fontweight='bold')
        ax2.grid(True, alpha=0.3)
        
        # Corridors vs habitats
        ax3.plot(df['n_habitats'], df['corridors'], 'm^-', linewidth=2, markersize=8)
        ax3.set_xlabel('Number of Habitat Patches', fontsize=12)
        ax3.set_ylabel('Number of Feasible Corridors', fontsize=12)
        ax3.set_title('Corridor Network Density', fontsize=14, fontweight='bold')
        ax3.grid(True, alpha=0.3)
        
        # Theoretical complexity
        V = df['n_habitats'].values
        E = df['corridors'].values
        complexity = (V**2 * E) / 1e6  # Normalized O(V²E)
        
        ax4.plot(df['n_habitats'], complexity, 'r^-', linewidth=2, markersize=8, label='Theoretical O(V²E)')
        ax4.plot(df['n_habitats'], df['time_ms'] / df['time_ms'].max() * complexity.max(), 
                'go-', linewidth=2, markersize=8, label='Actual Runtime (normalized)')
        ax4.set_xlabel('Number of Habitat Patches', fontsize=12)
        ax4.set_ylabel('Complexity (normalized)', fontsize=12)
        ax4.set_title('Complexity Analysis', fontsize=14, fontweight='bold')
        ax4.legend()
        ax4.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig('graphs/wildlife_corridor_analysis.png', dpi=300, bbox_inches='tight')
        print("Saved: graphs/wildlife_corridor_analysis.png")
        plt.close()
        
    except Exception as e:
        print(f"Error plotting wildlife network flow: {e}")

# Problem 2: DNA Assembly Results
def plot_dna_assembly():
    try:
        df = pd.read_csv('data/dna_assembly_results.csv')
        
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(14, 10))
        
        # Time comparison
        ax1.plot(df['n_fragments'], df['greedy_time_ms'], 'bo-', linewidth=2, markersize=6, label='Greedy')
        ax1.plot(df['n_fragments'], df['nn_time_ms'], 'rs-', linewidth=2, markersize=6, label='Nearest Neighbor')
        ax1.plot(df['n_fragments'], df['savings_time_ms'], 'g^-', linewidth=2, markersize=6, label='Savings')
        ax1.set_xlabel('Number of DNA Fragments', fontsize=12)
        ax1.set_ylabel('Time (ms)', fontsize=12)
        ax1.set_title('DNA Assembly: Running Time Comparison', fontsize=14, fontweight='bold')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Overlap quality comparison
        ax2.plot(df['n_fragments'], df['greedy_overlap'], 'bo-', linewidth=2, markersize=6, label='Greedy')
        ax2.plot(df['n_fragments'], df['nn_overlap'], 'rs-', linewidth=2, markersize=6, label='Nearest Neighbor')
        ax2.plot(df['n_fragments'], df['savings_overlap'], 'g^-', linewidth=2, markersize=6, label='Savings')
        ax2.set_xlabel('Number of DNA Fragments', fontsize=12)
        ax2.set_ylabel('Total Overlap (base pairs)', fontsize=12)
        ax2.set_title('Assembly Quality Comparison', fontsize=14, fontweight='bold')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        
        # Edges vs fragments
        ax3.scatter(df['n_fragments'], df['edges'], c=df['n_fragments'], cmap='viridis', 
                   s=100, alpha=0.6, edgecolors='black')
        ax3.set_xlabel('Number of DNA Fragments', fontsize=12)
        ax3.set_ylabel('Number of Overlap Edges', fontsize=12)
        ax3.set_title('Overlap Graph Density', fontsize=14, fontweight='bold')
        ax3.grid(True, alpha=0.3)
        
        # Time scaling (log scale)
        ax4.semilogy(df['n_fragments'], df['greedy_time_ms'], 'bo-', linewidth=2, markersize=6, label='Greedy')
        ax4.semilogy(df['n_fragments'], df['nn_time_ms'], 'rs-', linewidth=2, markersize=6, label='Nearest Neighbor')
        ax4.semilogy(df['n_fragments'], df['savings_time_ms'], 'g^-', linewidth=2, markersize=6, label='Savings')
        
        # Add polynomial complexity references
        n_vals = df['n_fragments'].values
        ax4.semilogy(n_vals, n_vals**2 / 1000, 'k--', alpha=0.5, linewidth=1.5, label='O(n²)')
        
        ax4.set_xlabel('Number of DNA Fragments', fontsize=12)
        ax4.set_ylabel('Time (ms) - log scale', fontsize=12)
        ax4.set_title('Complexity Analysis (Log Scale)', fontsize=14, fontweight='bold')
        ax4.legend()
        ax4.grid(True, alpha=0.3, which='both')
        
        plt.tight_layout()
        plt.savefig('graphs/dna_assembly_analysis.png', dpi=300, bbox_inches='tight')
        print("Saved: graphs/dna_assembly_analysis.png")
        plt.close()
        
    except Exception as e:
        print(f"Error plotting DNA assembly: {e}")

# Algorithm comparison for DNA assembly
def plot_algorithm_comparison():
    try:
        df = pd.read_csv('data/dna_assembly_results.csv')
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))
        
        # Average overlap by algorithm
        greedy_overlap = df['greedy_overlap'].values
        nn_overlap = df['nn_overlap'].values
        savings_overlap = df['savings_overlap'].values
        
        x = df['n_fragments'].values
        ax1.plot(x, greedy_overlap, 'bo-', linewidth=2, markersize=8, label='Greedy')
        ax1.plot(x, nn_overlap, 'rs-', linewidth=2, markersize=8, label='Nearest Neighbor')
        ax1.plot(x, savings_overlap, 'g^-', linewidth=2, markersize=8, label='Savings')
        ax1.set_xlabel('Number of DNA Fragments', fontsize=12)
        ax1.set_ylabel('Total Overlap (base pairs)', fontsize=12)
        ax1.set_title('Heuristic Quality Comparison', fontsize=14, fontweight='bold')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Overlap distribution
        positions = [1, 2, 3]
        data = [greedy_overlap, nn_overlap, savings_overlap]
        bp = ax2.boxplot(data, positions=positions, tick_labels=['Greedy', 'Nearest Neighbor', 'Savings'],
                        patch_artist=True, showmeans=True)
        
        colors = ['lightblue', 'lightcoral', 'lightgreen']
        for patch, color in zip(bp['boxes'], colors):
            patch.set_facecolor(color)
        
        ax2.set_ylabel('Total Overlap (base pairs)', fontsize=12)
        ax2.set_title('Solution Quality Distribution', fontsize=14, fontweight='bold')
        ax2.grid(True, alpha=0.3, axis='y')
        
        plt.tight_layout()
        plt.savefig('graphs/algorithm_comparison.png', dpi=300, bbox_inches='tight')
        print("Saved: graphs/algorithm_comparison.png")
        plt.close()
        
    except Exception as e:
        print(f"Error plotting comparison: {e}")

if __name__ == "__main__":
    print("Generating experimental graphs...")
    plot_wildlife_network_flow()
    plot_dna_assembly()
    plot_algorithm_comparison()
    print("All graphs generated successfully!")