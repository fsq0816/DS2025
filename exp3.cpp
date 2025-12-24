#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <climits>
#include <set>
#include <map>
#include <unordered_set>

using namespace std;

// ========================
// 通用图类（邻接矩阵）
// ========================
class Graph {
public:
    int n; // 顶点数
    vector<vector<int>> adjMatrix;
    vector<string> labels; // 顶点标签（如 "A", "B" ...）

    Graph(int size, const vector<string>& lbls) : n(size), labels(lbls) {
        adjMatrix.assign(n, vector<int>(n, 0));
    }

    void addEdge(int u, int v, int weight = 1) {
        adjMatrix[u][v] = weight;
        adjMatrix[v][u] = weight; // 无向图
    }

    void printAdjMatrix() {
        cout << "邻接矩阵:\n";
        cout << "   ";
        for (const string& l : labels) cout << l << " ";
        cout << "\n";
        for (int i = 0; i < n; ++i) {
            cout << labels[i] << "  ";
            for (int j = 0; j < n; ++j) {
                if (adjMatrix[i][j] == 0 && i != j)
                    cout << ". ";
                else
                    cout << adjMatrix[i][j] << " ";
            }
            cout << "\n";
        }
    }

    // BFS 从 start 开始
    void BFS(int start) {
        vector<bool> visited(n, false);
        queue<int> q;
        q.push(start);
        visited[start] = true;
        cout << "BFS (" << labels[start] << "): ";
        while (!q.empty()) {
            int u = q.front(); q.pop();
            cout << labels[u] << " ";
            for (int v = 0; v < n; ++v) {
                if (adjMatrix[u][v] > 0 && !visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
        cout << "\n";
    }

    // DFS 从 start 开始（递归）
    void DFSUtil(int u, vector<bool>& visited) {
        visited[u] = true;
        cout << labels[u] << " ";
        for (int v = 0; v < n; ++v) {
            if (adjMatrix[u][v] > 0 && !visited[v]) {
                DFSUtil(v, visited);
            }
        }
    }

    void DFS(int start) {
        vector<bool> visited(n, false);
        cout << "DFS (" << labels[start] << "): ";
        DFSUtil(start, visited);
        cout << "\n";
    }

    // Dijkstra 最短路径（从 start 出发）
    void dijkstra(int start) {
        vector<int> dist(n, INT_MAX);
        vector<bool> visited(n, false);
        dist[start] = 0;

        for (int count = 0; count < n; ++count) {
            int u = -1;
            for (int i = 0; i < n; ++i) {
                if (!visited[i] && (u == -1 || dist[i] < dist[u]))
                    u = i;
            }

            visited[u] = true;
            for (int v = 0; v < n; ++v) {
                if (adjMatrix[u][v] > 0 && !visited[v]) {
                    if (dist[u] + adjMatrix[u][v] < dist[v]) {
                        dist[v] = dist[u] + adjMatrix[u][v];
                    }
                }
            }
        }

        cout << "Dijkstra from " << labels[start] << ":\n";
        for (int i = 0; i < n; ++i) {
            cout << labels[start] << "->" << labels[i] << ": ";
            if (dist[i] == INT_MAX) cout << "INF\n";
            else cout << dist[i] << "\n";
        }
    }

    // Prim 最小生成树
    void primMST() {
        vector<int> key(n, INT_MAX);
        vector<bool> inMST(n, false);
        vector<int> parent(n, -1);

        key[0] = 0;

        for (int count = 0; count < n; ++count) {
            int u = -1;
            for (int i = 0; i < n; ++i) {
                if (!inMST[i] && (u == -1 || key[i] < key[u]))
                    u = i;
            }

            inMST[u] = true;
            if (parent[u] != -1) {
                cout << labels[parent[u]] << " - " << labels[u]
                     << " : " << adjMatrix[u][parent[u]] << "\n";
            }

            for (int v = 0; v < n; ++v) {
                if (adjMatrix[u][v] > 0 && !inMST[v] && adjMatrix[u][v] < key[v]) {
                    parent[v] = u;
                    key[v] = adjMatrix[u][v];
                }
            }
        }
    }
};

// ========================
// Tarjan 双连通分量 & 关节点（针对图2）
// ========================
class Biconnected {
public:
    vector<vector<int>> adj;
    vector<int> disc, low, parent;
    vector<bool> visited;
    int time;
    set<int> articulationPoints;
    vector<vector<pair<int,int>>> bcc; // 每个 BCC 的边
    stack<pair<int,int>> stk;

    Biconnected(int n) {
        adj.resize(n);
        disc.assign(n, -1);
        low.assign(n, -1);
        parent.assign(n, -1);
        visited.assign(n, false);
        time = 0;
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void dfs(int u) {
        int children = 0;
        visited[u] = true;
        disc[u] = low[u] = ++time;

        for (int v : adj[u]) {
            if (!visited[v]) {
                children++;
                parent[v] = u;
                stk.push({u, v});
                dfs(v);
                low[u] = min(low[u], low[v]);

                // 根节点且有多个子树 → 关节点
                if (parent[u] == -1 && children > 1)
                    articulationPoints.insert(u);
                // 非根节点且 low[v] >= disc[u]
                if (parent[u] != -1 && low[v] >= disc[u])
                    articulationPoints.insert(u);

                if (low[v] >= disc[u]) {
                    vector<pair<int,int>> comp;
                    while (true) {
                        auto edge = stk.top(); stk.pop();
                        comp.push_back(edge);
                        if (edge.first == u && edge.second == v) break;
                    }
                    bcc.push_back(comp);
                }
            } else if (v != parent[u] && disc[v] < disc[u]) {
                low[u] = min(low[u], disc[v]);
                stk.push({u, v});
            }
        }
    }

    void findBCC(int start = 0) {
        dfs(start);
        // 处理剩余边（如果图不连通）
        for (int i = 0; i < adj.size(); ++i) {
            if (!visited[i]) dfs(i);
        }
    }

    void printResults(const vector<string>& labels) {
        cout << "关节点 (Articulation Points):\n";
        if (articulationPoints.empty()) {
            cout << "无\n";
        } else {
            for (int ap : articulationPoints) {
                cout << labels[ap] << " ";
            }
            cout << "\n";
        }

        cout << "双连通分量 (Biconnected Components):\n";
        for (size_t i = 0; i < bcc.size(); ++i) {
            cout << "Component " << i+1 << ": ";
            for (auto& e : bcc[i]) {
                cout << "(" << labels[e.first] << "," << labels[e.second] << ") ";
            }
            cout << "\n";
        }
    }
};

// ========================
// 主函数
// ========================
int main() {
    // ========== 图1：带权无向图 ==========
    vector<string> labels1 = {"A", "B", "C", "D", "E"};
    Graph g1(5, labels1);
    // 添加边（图1）
    g1.addEdge(0, 1, 4); // A-B
    g1.addEdge(0, 2, 2); // A-C
    g1.addEdge(1, 2, 1); // B-C
    g1.addEdge(1, 3, 5); // B-D
    g1.addEdge(2, 3, 8); // C-D
    g1.addEdge(2, 4,10); // C-E
    g1.addEdge(3, 4, 2); // D-E

    cout << "=== 图1 ===\n";
    g1.printAdjMatrix();
    g1.BFS(0); // A
    g1.DFS(0);
    g1.dijkstra(0);
    cout << "Prim 最小生成树:\n";
    g1.primMST();

    cout << "\n=== 图2 ===\n";
    // ========== 图2：无向图（用于双连通） ==========
    vector<string> labels2 = {"A", "B", "C", "D", "E", "F"};
    Biconnected bc(6);
    // 添加边（图2）
    bc.addEdge(0,1); // A-B
    bc.addEdge(0,2); // A-C
    bc.addEdge(1,2); // B-C
    bc.addEdge(1,3); // B-D
    bc.addEdge(3,4); // D-E
    bc.addEdge(3,5); // D-F
    bc.addEdge(4,5); // E-F

    // 尝试不同起点（但 Tarjan 应该覆盖全图）
    bc.findBCC(0);
    bc.printResults(labels2);

    // 再次用不同起点测试（应得相同关节点）
    cout << "\n[验证] 使用不同起点（如 D=3）重新计算...\n";
    Biconnected bc2(6);
    bc2.addEdge(0,1); bc2.addEdge(0,2);
    bc2.addEdge(1,2); bc2.addEdge(1,3);
    bc2.addEdge(3,4); bc2.addEdge(3,5);
    bc2.addEdge(4,5);
    bc2.findBCC(3);
    bc2.printResults(labels2);

    return 0;
}
