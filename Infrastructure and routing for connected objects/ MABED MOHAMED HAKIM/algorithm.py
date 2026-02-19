import random
import time
import math
import itertools
import networkx as nx
import matplotlib.pyplot as plt
import sys

# Increase recursion depth for stability, especially for Held-Karp's DP structure
sys.setrecursionlimit(3000)


# --- Part 1: Graph Preparation (Q1, Q2, Q3) ---

def generate_random_graph(n, edge_probability=0.7, max_weight=10):
    """
    Q1: Define a function that can generate a random graph.
    Generates a random, weighted, undirected graph using NetworkX.
    """
    if n <= 1:
        G = nx.Graph()
        if n == 1: G.add_node(0)
        return G
        
    G = nx.Graph()
    nodes = range(n)
    G.add_nodes_from(nodes)

    for i in range(n):
        for j in range(i + 1, n):
            if random.random() < edge_probability:
                # Add an edge with a random integer weight (distance)
                weight = random.randint(1, max_weight)
                G.add_edge(i, j, weight=weight)
    return G

def is_connected(G):
    """
    Q2: Define a function that checks whether the graph of cities is connected.
    """
    if G.number_of_nodes() <= 1:
        return True
    return nx.is_connected(G)

def compute_distance_matrix(G):
    """
    Q3: Calculate all the lengths of the shortest paths between all the cities.
    Uses NetworkX's single-source shortest path function (Dijkstra's) iteratively.
    """
    n = G.number_of_nodes()
    if n == 0:
        return []
    if n == 1:
        return [[0]]
        
    dist_matrix = [[float('inf')] * n for _ in range(n)]

    # Use single-source shortest path (Dijkstra's) iteratively
    for u in range(n):
        # The result is a dictionary {target: distance}
        shortest_paths_from_u = nx.shortest_path_length(G, source=u, weight='weight')

        for v in range(n):
            if u == v:
                dist_matrix[u][v] = 0
            elif v in shortest_paths_from_u: 
                dist_matrix[u][v] = shortest_paths_from_u[v]

    return dist_matrix

# --- Utility Function ---

def calculate_tour_length(tour, dist_matrix):
    """Calculates the total length of a given tour."""
    if not tour or len(tour) < 2:
        return 0
    length = 0
    for i in range(len(tour) - 1):
        u, v = tour[i], tour[i+1]
        length += dist_matrix[u][v]
    return length


# --- Part 2: TSP Solutions (Q4, Q5) ---

# Q4: Simple approach (nearest neighbor first)
def nearest_neighbor_tsp(dist_matrix, start_node=0):
    """
    Implements the Nearest Neighbor heuristic. O(N^2) complexity.
    """
    n = len(dist_matrix)
    if n <= 1:
        return 0, [0] if n==1 else []

    current_node = start_node
    unvisited = set(range(n))
    unvisited.remove(start_node)
    tour = [start_node]
    tour_length = 0

    while unvisited:
        min_dist = float('inf')
        next_node = -1

        for neighbor in unvisited:
            distance = dist_matrix[current_node][neighbor]
            if distance != float('inf') and distance < min_dist:
                min_dist = distance
                next_node = neighbor

        if next_node == -1:
            raise ValueError("Graph became disconnected during NN path search.")

        tour_length += min_dist
        current_node = next_node
        unvisited.remove(current_node)
        tour.append(current_node)

    # Return to the starting city
    return_dist = dist_matrix[current_node][start_node]
    tour_length += return_dist
    tour.append(start_node)

    return tour_length, tour

# Brute Force approach (Exact, O(N!))
def brute_force_tsp(dist_matrix, start_node=0):
    """
    Implements the Brute Force exact solution. O(N!) complexity.
    """
    n = len(dist_matrix)
    if n <= 1:
        return 0, [0] if n==1 else []
    
    cities = [i for i in range(n) if i != start_node]
    min_tour_length = float('inf')
    best_tour = []
    
    for perm in itertools.permutations(cities):
        current_tour = [start_node] + list(perm) + [start_node]
        current_length = calculate_tour_length(current_tour, dist_matrix)
        
        if current_length < min_tour_length:
            min_tour_length = current_length
            best_tour = current_tour
            
    return min_tour_length, best_tour

# Q5: Exact approach that returns an optimal solution (Held-Karp Dynamic Programming)
def held_karp_tsp(dist_matrix, start_node=0):
    """
    Implements the Held-Karp Dynamic Programming exact solution. O(N^2 * 2^N) complexity.
    
    """
    n = len(dist_matrix)
    if n <= 1:
        return 0, [0] if n==1 else []
    if n == 2:
          return dist_matrix[0][1] + dist_matrix[1][0], [0, 1, 0]

    # DP table: C[mask, j] = min cost path visiting nodes in mask, ending at j.
    C = {}
    P = {} # Predecessor table for reconstruction

    # 1. Initialization (Subset size 2: {start_node, j})
    for j in range(n):
        if j == start_node: continue
        mask = (1 << start_node) | (1 << j) 
        
        cost = dist_matrix[start_node][j]
        if cost != float('inf'):
              C[(mask, j)] = cost
              P[(mask, j)] = start_node 

    # 2. DP Iteration
    for size in range(3, n + 1):
        for subset_nodes in itertools.combinations([x for x in range(n) if x != start_node], size - 1):
            
            current_mask = (1 << start_node)
            for k in subset_nodes:
                current_mask |= (1 << k)

            for j in subset_nodes:
                if j == start_node: continue

                prev_mask = current_mask ^ (1 << j)
                min_cost = float('inf')
                best_predecessor = -1

                for k in subset_nodes:
                    if k == start_node or k == j: continue

                    cost_to_k = C.get((prev_mask, k), float('inf'))
                    
                    cost = cost_to_k + dist_matrix[k][j]
                    
                    if cost < min_cost:
                        min_cost = cost
                        best_predecessor = k
                
                if min_cost != float('inf'):
                    C[(current_mask, j)] = min_cost
                    P[(current_mask, j)] = best_predecessor


    # 3. Final Step: Complete the tour
    final_mask = (1 << n) - 1 
    min_tour_length = float('inf')
    best_last_node = -1

    for j in range(n):
        if j == start_node: continue

        cost_to_j = C.get((final_mask, j), float('inf')) 
        
        tour_length = cost_to_j + dist_matrix[j][start_node]

        if tour_length < min_tour_length:
            min_tour_length = tour_length
            best_last_node = j

    # 4. Tour Reconstruction
    tour = []
    if min_tour_length != float('inf'):
        curr_mask = final_mask
        curr_node = best_last_node
        
        while curr_node != start_node:
            tour.insert(0, curr_node) 
            pred_node = P.get((curr_mask, curr_node), start_node)
            curr_mask = curr_mask ^ (1 << curr_node)
            curr_node = pred_node
            
            if pred_node == start_node:
                tour.insert(0, start_node)
                break
            
        if not tour or tour[0] != start_node:
            tour = [start_node] + tour
        tour.append(start_node)

    return min_tour_length, tour

# --- Comparison (Q6) ---

def run_comparison(min_n=1, max_n=20, num_trials=1):
    """
    Q6: Compare the execution times and the obtained length tour of the three variants.
    (Nearest Neighbor, Brute Force, Held-Karp)
    """
    print(f"Running comparison for n={min_n} to {max_n} (Avg over {num_trials} trials per n)...")

    sizes = list(range(min_n, max_n + 1))
    nn_times, hk_times, bf_times = [], [], [] # Removed opt2_times
    nn_lengths, hk_lengths, bf_lengths = [], [], [] # Removed opt2_lengths

    for n in sizes:
        print(f"Processing n = {n} cities...")
        
        avg_nn_time, avg_hk_time, avg_bf_time = 0, 0, 0 # Removed avg_opt2_time
        avg_nn_length, avg_hk_length, avg_bf_length = 0, 0, 0 # Removed avg_opt2_length

        for _ in range(num_trials):
            # 1. Generate & ensure connected
            while True:
                G = generate_random_graph(n, edge_probability=0.7)
                if is_connected(G):
                    break
            
            # 2. Compute Distance Matrix (Q3)
            dist_matrix = compute_distance_matrix(G)
            if not dist_matrix: continue
            
            # --- Nearest Neighbor (Q4) ---
            start_time = time.time()
            nn_length, _ = nearest_neighbor_tsp(dist_matrix) # Removed nn_tour since 2-opt is gone
            end_time = time.time()
            avg_nn_time += (end_time - start_time)
            avg_nn_length += nn_length
            
            # --- Brute Force ---
            if n <= 10:
                start_time = time.time()
                bf_length, _ = brute_force_tsp(dist_matrix)
                end_time = time.time()
                avg_bf_time += (end_time - start_time)
                avg_bf_length += bf_length
            else:
                bf_length = float('nan')

            # --- Held-Karp (Q5) ---
            if n <= 20: 
                start_time = time.time()
                hk_length, _ = held_karp_tsp(dist_matrix)
                end_time = time.time()
                avg_hk_time += (end_time - start_time)
                avg_hk_length += hk_length
            else:
                hk_length = float('nan')
                avg_hk_time += 0
                avg_hk_length += 0


        nn_times.append(avg_nn_time / num_trials)
        bf_times.append(avg_bf_time / num_trials)
        hk_times.append(avg_hk_time / num_trials)
        nn_lengths.append(avg_nn_length / num_trials)
        bf_lengths.append(avg_bf_length / num_trials if not math.isnan(bf_length) else float('nan'))
        hk_lengths.append(avg_hk_length / num_trials)

    # Removed opt2_times and opt2_lengths from return
    return sizes, nn_times, hk_times, bf_times, nn_lengths, hk_lengths, bf_lengths

def plot_results(sizes, nn_times, hk_times, bf_times, nn_lengths, hk_lengths, bf_lengths):
    """
    Generates plots for Q6 comparison.
    """
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6)) 

    # --- Plot 1: Execution Time ---
    ax1.plot(sizes, nn_times, label=r'Nearest Neighbor ($\mathcal{O}(N^2)$)', marker='o', color='blue')
    # Removed 2-Opt
    ax1.plot(sizes, hk_times, label=r'Held-Karp ($\mathcal{O}(N^2 2^N)$)', marker='x', color='red')
    ax1.plot(sizes, bf_times, label=r'Brute Force ($\mathcal{O}(N!)$)', marker='s', color='purple')
    ax1.set_xlabel('Number of Cities (N)')
    ax1.set_ylabel('Execution Time (seconds, logarithmic scale)')
    ax1.set_title('Q6: Execution Time Comparison (Three Algorithms)')
    ax1.set_yscale('log')
    ax1.legend()
    ax1.grid(True, which="both", ls="--")

    # --- Plot 2: Tour Length ---
    ax2.plot(sizes, nn_lengths, label='NN Length (Suboptimal)', marker='o', color='blue')
    # Removed 2-Opt
    ax2.plot(sizes, hk_lengths, label='Held-Karp Length (Optimal)', marker='x', color='red')
    ax2.plot(sizes, bf_lengths, label='Brute Force Length (Optimal)', marker='s', color='purple')
    ax2.set_xlabel('Number of Cities (N)')
    ax2.set_ylabel('Average Tour Length')
    ax2.set_title('Q6: Tour Length Comparison (Suboptimal vs. Optimal)')
    ax2.legend()
    ax2.grid(True, ls="--")
    
    # Diagram showing a sample TSP tour to visually explain the problem
    # 

    plt.tight_layout()
    plt.show()

# --- Main Execution Block ---

if __name__ == '__main__':
    # Set the comparison range based on the user request (1 to 20 cities)
    MIN_N = 1
    MAX_N = 20 
    NUM_TRIALS = 1 
    
    print("--- Exercice 8: TSP Problem Solver ---")

    # Run the full comparison (Q6)
    # Removed opt2_times/lengths from assignment
    sizes, nn_times, hk_times, bf_times, nn_lengths, hk_lengths, bf_lengths = run_comparison(MIN_N, MAX_N, NUM_TRIALS)

    # Print data table
    print("\n--- Q6: Results Table ---")
    # Removed 2Opt columns from header
    print(f"{'n':<4} | {'NN Time':<10} | {'HK Time':<10} | {'BF Time':<10} | {'NN Len':<7} | {'HK Len':<7} | {'BF Len':<7}")
    print("-" * 65)
    
    # Custom print function to handle the 'nan' values when HK is skipped
    def format_output(val, is_time=False):
        if math.isnan(val) or (is_time and val == 0 and val != float('nan')):
            return "N/A"
        if is_time:
            return f"{val:<10.5f}"
        return f"{val:<7.2f}"

    for i, n in enumerate(sizes):
        hk_time_str = format_output(hk_times[i], is_time=True)
        hk_len_str = format_output(hk_lengths[i])
        bf_time_str = format_output(bf_times[i], is_time=True)
        bf_len_str = format_output(bf_lengths[i])
        
        # Removed 2Opt columns from print
        print(f"{n:<4} | {format_output(nn_times[i], True)} | {hk_time_str} | {bf_time_str} | {format_output(nn_lengths[i])} | {hk_len_str} | {bf_len_str}")

    # Plot the results
    # Removed opt2_times/lengths from plot call
    plot_results(sizes, nn_times, hk_times, bf_times, nn_lengths, hk_lengths, bf_lengths)