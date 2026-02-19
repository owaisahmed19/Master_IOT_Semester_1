#include <QCoreApplication>
#include <QDebug>
#include <vector>
#include <algorithm>
#include <numeric>
#include <stack> // Needed for Tarjan's algorithm

using namespace std;

// FLOYD–WARSHALL (All-Pairs Shortest Paths)

const int INF = 1e9;

vector<vector<int>> floydWarshall(const vector<vector<int>>& C,
                                  const vector<vector<int>>& A)
{
    int n = C.size();
    vector<vector<int>> D(n, vector<int>(n, INF));

    // Build D(0)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            if (i == j) D[i][j] = 0;
            else if (A[i][j] == 1) D[i][j] = C[i][j];
        }

    // Floyd–Warshall algorithm
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (D[i][k] < INF && D[k][j] < INF)
                    D[i][j] = min(D[i][j], D[i][k] + D[k][j]);

    return D;
}

// KRUSKAL MINIMUM SPANNING TREE (Undirected)

struct Edge {
    int u, v, w;
    bool operator<(const Edge& o) const { return w < o.w; }
};

struct DSU {
    vector<int> parent, rankv;
    DSU(int n) : parent(n), rankv(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int a) {
        return parent[a] = (parent[a] == a ? a : find(parent[a]));
    }
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (rankv[a] < rankv[b]) swap(a, b);
        parent[b] = a;
        if (rankv[a] == rankv[b]) rankv[a]++;
        return true;
    }
};

vector<Edge> kruskalMST(const vector<vector<int>>& I)
{
    int n = I.size();
    vector<Edge> edges, mst;

    for (int i = 0; i < n; i++)
        for (int j = i+1; j < n; j++)
            edges.push_back({i, j, I[i][j]});

    sort(edges.begin(), edges.end());
    DSU dsu(n);

    for (auto &e : edges)
    {
        if (dsu.unite(e.u, e.v))
            mst.push_back(e);
        if (mst.size() == n - 1) break;
    }
    return mst;
}

// Global/Class variables for Tarjan's DFS state
int current_id;
vector<int> ids, low;
vector<bool> onStack;
stack<int> s;
vector<vector<int>> components;

void dfs_tarjan(int at, const vector<vector<int>>& A) {
    s.push(at);
    onStack[at] = true;
    ids[at] = low[at] = current_id++;

    // Iterate over neighbors
    for (int to = 0; to < A.size(); ++to) {
        if (A[at][to] == 1) { // If there is an edge at -> to
            if (ids[to] == -1) {
                // Case 1: Tree edge (to is unvisited)
                dfs_tarjan(to, A);
                // After returning from recursive call, update low-link
                low[at] = min(low[at], low[to]);
            } else if (onStack[to]) {
                // Case 2: Back edge (to is visited and still on stack)
                // This means 'to' is an ancestor in the current DFS tree
                low[at] = min(low[at], ids[to]);
            }
        }
    }

    // If 'at' is the root of an SCC
    if (ids[at] == low[at]) {
        vector<int> component;
        while (true) {
            int node = s.top();
            s.pop();
            onStack[node] = false;
            component.push_back(node);
            if (node == at) break; // Found the end of the SCC
        }
        components.push_back(component);
    }
}

// STRONGLY CONNECTED COMPONENTS (Tarjan's)

vector<vector<int>> tarjanSCC(const vector<vector<int>>& A) {
    int n = A.size();
    current_id = 0;
    ids.assign(n, -1);
    low.assign(n, -1);
    onStack.assign(n, false);
    components.clear();

    // Clear the stack in case it wasn't empty from a previous call
    while (!s.empty()) s.pop();

    for (int i = 0; i < n; i++) {
        if (ids[i] == -1) {
            dfs_tarjan(i, A);
        }
    }

    // The result is in components
    return components;
}

// CLUSTER ADJACENCY MATRIX (Modified to show only cross-links)

vector<vector<int>> buildCrossClusterMatrix(
    const vector<vector<int>>& A,
    const vector<vector<int>>& clusters)
{
    int k = clusters.size();
    vector<vector<int>> X(k, vector<int>(k, 0)); // Renamed to X for Cross-Cluster

    if (k == 0) return X;

    vector<int> clusterOf(A.size());
    for (int i = 0; i < k; i++)
        for (int node : clusters[i])
            clusterOf[node] = i;

    for (int i = 0; i < A.size(); i++)
        for (int j = 0; j < A.size(); j++)
            if (A[i][j] == 1)
            {
                int cluster_i = clusterOf[i];
                int cluster_j = clusterOf[j];

                // Only mark connections between DIFFERENT clusters
                if (cluster_i != cluster_j)
                {
                    // If an edge exists, mark the connection as 1 (Boolean matrix)
                    X[cluster_i][cluster_j] = 1;
                }
            }

    return X;
}

// MAIN PROGRAM
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    vector<vector<int>> A = {
        {0,1,0,0,0,0},
        {1,0,1,0,0,0},
        {0,0,0,1,0,0},
        {0,0,0,0,1,0},
        {0,0,0,0,0,1},
        {0,0,1,0,0,0}
    };

    vector<vector<int>> C = {
        {0,2,0,0,0,0},
        {1,0,1,0,0,0},
        {0,0,0,6,0,0},
        {0,0,0,0,1,0},
        {0,0,0,0,0,2},
        {0,0,2,0,0,0}
    };

    vector<vector<int>> I = {
        {20, 19, 15,  8, 19, 20},
        {19, 19,  8,  1, 19, 11},
        {15,  8, 20, 11, 10, 12},
        { 8,  1, 11,  6,  9, 10},
        {19, 19, 10,  9,  9,  8},
        {20, 11, 12, 10,  8,  7}
    };


    // Floyd–Warshall
    auto D = floydWarshall(C, A);
    qDebug() << "\nShortest Path Matrix D:";
    for (auto &row : D) {
        QString line;
        for (int x : row) {
            if (x >= INF)
                line += "INF ";
            else
                line += QString::number(x) + "  ";
        }
        qDebug().noquote() << line;
    }

    // MST
    auto mst = kruskalMST(I);
    qDebug() << "\nMST Edges:";
    int totalCost = 0;
    for (auto &e : mst) {
        qDebug() << e.u << "-" << e.v << "(cost" << e.w << ")";
        totalCost += e.w;
    }
    qDebug() << "Total installation cost =" << totalCost;

    // SCC / Clusters (using Tarjan's)
    auto clusters = tarjanSCC(A);
    qDebug() << "\nClusters (SCC) using Tarjan's:";
    for (int i = 0; i < clusters.size(); i++) {
        QString s = "Cluster " + QString::number(i) + ": ";
        for (int node : clusters[i]) s += QString::number(node) + " ";
        qDebug().noquote() << s;
    }




    // Cross-Cluster Adjacency Matrix
    // This matrix (X) shows only connections BETWEEN clusters (1s off the diagonal)
    auto X = buildCrossClusterMatrix(A, clusters);
    qDebug() << "\nCross-Cluster Adjacency Matrix X:";
    for (auto &row : X) {
        QString s;
        for (int x : row) s += QString::number(x) + " ";
        qDebug().noquote() << s;
    }

    return a.exec();
}
