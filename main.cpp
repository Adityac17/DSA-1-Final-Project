#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <climits>
#include <iomanip>
#include <sstream>

using namespace std;

// DATA STRUCTURES (Core Records)

struct Supplier {
    int id;
    string name;
    string country;
    int riskScore;      
    string status;      

    void display() const {
        cout << "[S-" << setw(3) << setfill('0') << id << setfill(' ') << "] "
             << left << setw(22) << name
             << "| " << setw(12) << country
             << "| Risk: " << setw(3) << riskScore
             << "| " << status << "\n";
    }
};

struct Shipment {
    int id;
    int supplierId;
    string origin;
    string destination;
    string status;      
    int priority;      
    double value;       
    int riskFlag;       

    void display() const {
        const string pLabels[] = {"", "CRITICAL", "HIGH", "NORMAL", "LOW"};
        const string rLabels[] = {"Safe", "At Risk", "Critical"};
        cout << "[SH-" << setw(4) << setfill('0') << id << setfill(' ') << "] "
             << left << setw(12) << origin << "-> " << setw(12) << destination
             << "| " << setw(9) << pLabels[priority]
             << "| $" << setw(10) << fixed << setprecision(2) << value
             << "| " << setw(11) << status
             << "| " << rLabels[riskFlag] << "\n";
    }
};


// ACTION LOG (for Stack-based Undo)
enum ActionType { ADD_SUPPLIER, ADD_SHIPMENT, UPDATE_SHIPMENT_STATUS, PROCESS_SHIPMENT };

struct Action {
    ActionType type;
    string description;
    int entityId;
    string prevValue;   
};

// AVL TREE — Primary In-Memory Database for Shipments

struct AVLNode {
    Shipment data;
    AVLNode* left;
    AVLNode* right;
    int height;
    AVLNode(Shipment s) : data(s), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
    AVLNode* root;

    int height(AVLNode* n) { return n ? n->height : 0; }

    int balanceFactor(AVLNode* n) {
        return n ? height(n->left) - height(n->right) : 0;
    }

    void updateHeight(AVLNode* n) {
        if (n) n->height = 1 + max(height(n->left), height(n->right));
    }

    // Single right rotation — fixes Left-Left imbalance
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x  = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left  = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    // Single left rotation — fixes Right-Right imbalance
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y  = x->right;
        AVLNode* T2 = y->left;
        y->left  = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    // Rebalance after insert/delete — O(1) per node
    AVLNode* rebalance(AVLNode* n) {
        updateHeight(n);
        int bf = balanceFactor(n);

        if (bf > 1 && balanceFactor(n->left) >= 0)    return rotateRight(n);  
        if (bf > 1 && balanceFactor(n->left) < 0) {                            
            n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        if (bf < -1 && balanceFactor(n->right) <= 0)  return rotateLeft(n);   
        if (bf < -1 && balanceFactor(n->right) > 0) {                          
            n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }

    AVLNode* insert(AVLNode* node, Shipment s) {
        if (!node) return new AVLNode(s);
        if (s.id < node->data.id)      node->left  = insert(node->left, s);
        else if (s.id > node->data.id) node->right = insert(node->right, s);
        else { node->data = s; return node; }   // update on duplicate key
        return rebalance(node);
    }

    AVLNode* minNode(AVLNode* n) {
        return n->left ? minNode(n->left) : n;
    }

    AVLNode* remove(AVLNode* node, int id) {
        if (!node) return nullptr;
        if (id < node->data.id)      node->left  = remove(node->left, id);
        else if (id > node->data.id) node->right = remove(node->right, id);
        else {
            if (!node->left || !node->right) {
                AVLNode* tmp = node->left ? node->left : node->right;
                delete node;
                return tmp;
            }
            AVLNode* successor = minNode(node->right);
            node->data = successor->data;
            node->right = remove(node->right, successor->data.id);
        }
        return rebalance(node);
    }

    AVLNode* search(AVLNode* node, int id) {
        if (!node || node->data.id == id) return node;
        return id < node->data.id ? search(node->left, id) : search(node->right, id);
    }

    void inorder(AVLNode* node, vector<Shipment>& result) {
        if (!node) return;
        inorder(node->left, result);
        result.push_back(node->data);
        inorder(node->right, result);
    }

    void destroy(AVLNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    AVLTree() : root(nullptr) {}
    ~AVLTree() { destroy(root); }

    void insert(Shipment s)   { root = insert(root, s); }
    void remove(int id)       { root = remove(root, id); }

    Shipment* search(int id) {
        AVLNode* n = search(root, id);
        return n ? &n->data : nullptr;
    }

    vector<Shipment> getAllSorted() {
        vector<Shipment> result;
        inorder(root, result);
        return result;
    }
};

// ROUTE GRAPH — Weighted Undirected Graph
class RouteGraph {
    int V;
    map<string, int>    nodeIndex; 
    vector<string>      nodes;      
    vector<vector<pair<int,int>>> adj;  

public:
    RouteGraph() : V(0) {}

    void addNode(const string& city) {
        if (nodeIndex.count(city)) return;
        nodeIndex[city] = V++;
        nodes.push_back(city);
        adj.push_back({});
    }

    void addEdge(const string& u, const string& v, int weight) {
        addNode(u); addNode(v);
        int ui = nodeIndex[u], vi = nodeIndex[v];
        adj[ui].push_back({vi, weight});
        adj[vi].push_back({ui, weight});
    }

    void displayNodes() {
        cout << "  Logistics Hubs (" << V << " nodes):\n";
        for (auto& n : nodes) cout << "    - " << n << "\n";
    }

    // ---- BFS: Finds all reachable nodes + shortest hop path (unweighted) ----
    void bfs(const string& startCity) {
        if (!nodeIndex.count(startCity)) { cout << "  City not found.\n"; return; }
        int start = nodeIndex[startCity];
        vector<bool>   visited(V, false);
        vector<int>    parent(V, -1);
        queue<int>     q;

        visited[start] = true;
        q.push(start);

        cout << "  BFS traversal from " << startCity << ":\n  ";
        while (!q.empty()) {
            int u = q.front(); q.pop();
            cout << nodes[u] << " ";
            for (auto& [v, w] : adj[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    parent[v]  = u;
                    q.push(v);
                }
            }
        }
        cout << "\n";
    }

    // ---- DFS: Explores deep paths, useful for detecting connectivity ----
    void dfsHelper(int u, vector<bool>& visited) {
        visited[u] = true;
        cout << nodes[u] << " ";
        for (auto& [v, w] : adj[u]) {
            if (!visited[v]) dfsHelper(v, visited);
        }
    }

    void dfs(const string& startCity) {
        if (!nodeIndex.count(startCity)) { cout << "  City not found.\n"; return; }
        vector<bool> visited(V, false);
        cout << "  DFS traversal from " << startCity << ":\n  ";
        dfsHelper(nodeIndex[startCity], visited);
        cout << "\n";
    }

    // ---- Dijkstra's Algorithm: Shortest (cheapest) path between two cities ----
    void dijkstra(const string& srcCity, const string& dstCity) {
        if (!nodeIndex.count(srcCity) || !nodeIndex.count(dstCity)) {
            cout << "  City not found.\n"; return;
        }
        int src = nodeIndex[srcCity], dst = nodeIndex[dstCity];

        vector<int> dist(V, INT_MAX);
        vector<int> parent(V, -1);
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d > dist[u]) continue; 

            for (auto& [v, w] : adj[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v]   = dist[u] + w;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        if (dist[dst] == INT_MAX) {
            cout << "  No route found between " << srcCity << " and " << dstCity << ".\n";
            return;
        }

        vector<string> path;
        for (int cur = dst; cur != -1; cur = parent[cur])
            path.push_back(nodes[cur]);
        reverse(path.begin(), path.end());

        cout << "  Optimal Route: ";
        for (int i = 0; i < (int)path.size(); i++) {
            cout << path[i];
            if (i + 1 < (int)path.size()) cout << " -> ";
        }
        cout << "\n  Total Cost: $" << dist[dst] << " per unit\n";
    }
};

void bubbleSortByPriority(vector<Shipment>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j].priority > arr[j+1].priority)
                swap(arr[j], arr[j+1]);
}

void selectionSortByRisk(vector<Shipment>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j].riskFlag > arr[maxIdx].riskFlag)
                maxIdx = j;
        swap(arr[i], arr[maxIdx]);
    }
}

// SEARCHING ALGORITHMS

int linearSearchSupplier(const vector<Supplier>& suppliers, int id) {
    for (int i = 0; i < (int)suppliers.size(); i++)
        if (suppliers[i].id == id) return i;
    return -1;
}

int binarySearchShipment(const vector<Shipment>& arr, int id) {
    int lo = 0, hi = arr.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid].id == id)      return mid;
        else if (arr[mid].id < id)  lo = mid + 1;
        else                        hi = mid - 1;
    }
    return -1;
}


// SUPPLY CHAIN SYSTEM — Main Controller


class SupplyChainSystem {
    AVLTree shipmentDB;                             
    vector<Supplier> suppliers;                    
    unordered_map<int, Supplier*> supplierHash;     
    unordered_map<int, Shipment*> shipmentHash;     
    queue<Shipment>  eventQueue;                    
    stack<Action>    undoStack;                     
    RouteGraph       graph;                         
    int nextShipmentId = 1000;
    int nextSupplierId = 100;

    void printHeader(const string& title) {
        cout << "\n  ╔══════════════════════════════════════════════════════╗\n";
        cout << "  ║  " << left << setw(52) << title << "║\n";
        cout << "  ╚══════════════════════════════════════════════════════╝\n";
    }

    void printDivider() {
        cout << "  ──────────────────────────────────────────────────────\n";
    }

public:

    // DUMMY DATA LOADER
    void loadDummyData() {
        // --- Suppliers ---
        vector<Supplier> initSuppliers = {
            {101, "Tata Global Logistics",    "India",   15, "Active"},
            {102, "SinoFreight Ltd",          "China",   45, "Active"},
            {103, "EuroSupply GmbH",          "Germany", 20, "Active"},
            {104, "Gulf Trade Co",            "UAE",     60, "Under Review"},
            {105, "AmeriCargo Inc",           "USA",     10, "Active"},
            {106, "RiskZone Suppliers",       "Yemen",   92, "Suspended"},
        };
        for (auto& s : initSuppliers) {
            suppliers.push_back(s);
            supplierHash[s.id] = &suppliers.back();
        }
        sort(suppliers.begin(), suppliers.end(), [](const Supplier& a, const Supplier& b){
            return a.id < b.id;
        });

        vector<Shipment> initShipments = {
            {1001, 101, "Mumbai",   "London",    "In Transit",  2, 85000.0,  0},
            {1002, 102, "Shanghai", "Dubai",     "Pending",     1, 120000.0, 2},
            {1003, 103, "Berlin",   "New York",  "Delivered",   3, 45000.0,  0},
            {1004, 104, "Dubai",    "Singapore", "Delayed",     2, 67000.0,  1},
            {1005, 105, "New York", "London",    "In Transit",  3, 33000.0,  0},
            {1006, 101, "Mumbai",   "Singapore", "Pending",     4, 21000.0,  0},
            {1007, 102, "Shanghai", "Berlin",    "Pending",     1, 95000.0,  1},
            {1008, 104, "Dubai",    "London",    "Delayed",     2, 58000.0,  2},
        };
        for (auto& s : initShipments) {
            shipmentDB.insert(s);
            nextShipmentId = max(nextShipmentId, s.id + 1);
        }

        eventQueue.push({1009, 103, "Berlin",  "Mumbai",  "Pending", 2, 41000.0, 0});
        eventQueue.push({1010, 105, "New York", "Tokyo",  "Pending", 1, 78000.0, 1});

        graph.addEdge("Mumbai",    "Dubai",     300);
        graph.addEdge("Mumbai",    "Singapore", 450);
        graph.addEdge("Dubai",     "London",    500);
        graph.addEdge("Dubai",     "Singapore", 350);
        graph.addEdge("Singapore", "Shanghai",  200);
        graph.addEdge("Singapore", "Tokyo",     250);
        graph.addEdge("Shanghai",  "Tokyo",     150);
        graph.addEdge("Shanghai",  "Berlin",    700);
        graph.addEdge("London",    "Berlin",    200);
        graph.addEdge("London",    "New York",  600);
        graph.addEdge("Berlin",    "New York",  750);
        graph.addEdge("New York",  "Tokyo",     900);

        cout << "\n  [✓] Dummy data loaded: 6 suppliers, 8 shipments, 12 routes, 2 queued events.\n";
    }

    // 1. ADD SUPPLIER

    void addSupplier() {
        Supplier s;
        s.id = ++nextSupplierId;
        cout << "  Supplier Name   : "; cin.ignore(); getline(cin, s.name);
        cout << "  Country         : "; getline(cin, s.country);
        cout << "  Risk Score(0-100): "; cin >> s.riskScore;
        s.status = (s.riskScore > 75) ? "Suspended" : "Active";

        suppliers.push_back(s);
        supplierHash[s.id] = &suppliers.back();
        sort(suppliers.begin(), suppliers.end(), [](const Supplier& a, const Supplier& b){
            return a.id < b.id;
        });

        undoStack.push({ADD_SUPPLIER, "Added supplier " + s.name, s.id, ""});
        cout << "  [✓] Supplier added with ID: S-" << s.id
             << " | Status: " << s.status << "\n";
    }

    // 2. ADD SHIPMENT

    void addShipment() {
        Shipment s;
        s.id = nextShipmentId++;
        cout << "  Supplier ID  : "; cin >> s.supplierId;

        if (!supplierHash.count(s.supplierId)) {
            cout << "  [!] Supplier not found. Aborted.\n"; return;
        }
        if (supplierHash[s.supplierId]->status == "Suspended") {
            cout << "  [!] Supplier is SUSPENDED. Shipment blocked.\n"; return;
        }

        cin.ignore();
        cout << "  Origin       : "; getline(cin, s.origin);
        cout << "  Destination  : "; getline(cin, s.destination);
        cout << "  Priority(1-4): "; cin >> s.priority;
        cout << "  Value (USD)  : "; cin >> s.value;

        int supplierRisk = supplierHash[s.supplierId]->riskScore;
        s.riskFlag = (supplierRisk > 75) ? 2 : (supplierRisk > 40) ? 1 : 0;
        s.status   = "Pending";

        shipmentDB.insert(s);

        undoStack.push({ADD_SHIPMENT, "Added shipment " + to_string(s.id), s.id, ""});
        cout << "  [✓] Shipment SH-" << s.id << " created. Risk: "
             << (s.riskFlag == 0 ? "Safe" : s.riskFlag == 1 ? "At Risk" : "Critical") << "\n";
    }

    // 3. VIEW ALL SHIPMENTS (AVL In-order = sorted by ID)

    void viewAllShipments() {
        printHeader("ALL SHIPMENTS — AVL Tree In-Order Traversal");
        auto shipments = shipmentDB.getAllSorted();
        if (shipments.empty()) { cout << "  No shipments found.\n"; return; }
        for (auto& s : shipments) { cout << "  "; s.display(); }
        cout << "  Total: " << shipments.size() << " shipments\n";
    }

    // 4. VIEW ALL SUPPLIERS

    void viewAllSuppliers() {
        printHeader("ALL SUPPLIERS");
        for (auto& s : suppliers) { cout << "  "; s.display(); }
    }

    // 5. SEARCH SHIPMENT (Binary Search on sorted array)

    void searchShipment() {
        int id;
        cout << "  Enter Shipment ID: "; cin >> id;
        Shipment* s = shipmentDB.search(id);
        if (s) { cout << "  Found:\n  "; s->display(); }
        else   { cout << "  [!] Shipment SH-" << id << " not found.\n"; }
    }

    // 6. SEARCH SUPPLIER (Hash Map O(1) lookup + Linear Search fallback)

    void searchSupplier() {
        int id;
        cout << "  Enter Supplier ID: "; cin >> id;

        if (supplierHash.count(id)) {
            cout << "  Found (via Hash Map):\n  ";
            supplierHash[id]->display();
        } else {
            int idx = linearSearchSupplier(suppliers, id);
            if (idx != -1) { cout << "  Found (via Linear Search):\n  "; suppliers[idx].display(); }
            else            cout << "  [!] Supplier S-" << id << " not found.\n";
        }
    }

    // 7. PROCESS SHIPMENT QUEUE (FIFO)

    void processQueue() {
        printHeader("PROCESS SHIPMENT QUEUE");
        if (eventQueue.empty()) { cout << "  Queue is empty. No events to process.\n"; return; }

        cout << "  Queue size: " << eventQueue.size() << " event(s)\n\n";
        Shipment s = eventQueue.front();
        eventQueue.pop();

        s.status = "In Transit";
        shipmentDB.insert(s); 

        undoStack.push({PROCESS_SHIPMENT,
                        "Processed shipment " + to_string(s.id) + " from queue",
                        s.id, "Pending"});

        cout << "  [✓] Processed: "; s.display();
        cout << "  Queue remaining: " << eventQueue.size() << "\n";
    }

    // 8. UNDO LAST ACTION (Stack)

    void undoLastAction() {
        if (undoStack.empty()) { cout << "  [!] Nothing to undo.\n"; return; }

        Action last = undoStack.top();
        undoStack.pop();

        cout << "  [UNDO] Reversing: " << last.description << "\n";

        if (last.type == ADD_SHIPMENT) {
            shipmentDB.remove(last.entityId);
            cout << "  [✓] Shipment SH-" << last.entityId << " removed from database.\n";
        } else if (last.type == ADD_SUPPLIER) {
            auto it = find_if(suppliers.begin(), suppliers.end(),
                              [&](const Supplier& s){ return s.id == last.entityId; });
            if (it != suppliers.end()) {
                supplierHash.erase(last.entityId);
                suppliers.erase(it);
            }
            cout << "  [✓] Supplier S-" << last.entityId << " removed.\n";
        } else if (last.type == PROCESS_SHIPMENT) {
            // Revert status to Pending
            Shipment* s = shipmentDB.search(last.entityId);
            if (s) { s->status = "Pending"; }
            cout << "  [✓] Shipment SH-" << last.entityId << " status reverted to Pending.\n";
        }
    }

    // 9. ROUTE OPTIMIZER (Graph Menu)

    void routeMenu() {
        printHeader("LOGISTICS ROUTE NETWORK");
        graph.displayNodes();

        cout << "\n  [1] BFS from a city\n"
             << "  [2] DFS from a city\n"
             << "  [3] Dijkstra (Optimal Route)\n"
             << "  Choice: ";
        int c; cin >> c;
        string city1, city2;

        if (c == 1) {
            cout << "  Start city: "; cin.ignore(); getline(cin, city1);
            graph.bfs(city1);
        } else if (c == 2) {
            cout << "  Start city: "; cin.ignore(); getline(cin, city1);
            graph.dfs(city1);
        } else if (c == 3) {
            cout << "  Source      : "; cin.ignore(); getline(cin, city1);
            cout << "  Destination : "; getline(cin, city2);
            graph.dijkstra(city1, city2);
        }
    }

    // 10. SORT & PRIORITIZE SHIPMENTS

    void sortShipments() {
        printHeader("SORTED SHIPMENT VIEW");
        cout << "  [1] Sort by Priority (Critical first)\n"
             << "  [2] Sort by Risk Flag (Highest risk first)\n"
             << "  Choice: ";
        int c; cin >> c;

        auto shipments = shipmentDB.getAllSorted();

        if (c == 1) {
            bubbleSortByPriority(shipments);  
            cout << "\n  Shipments sorted by Priority:\n";
        } else {
            selectionSortByRisk(shipments);   
            cout << "\n  Shipments sorted by Risk Flag:\n";
        }

        for (auto& s : shipments) { cout << "  "; s.display(); }
    }

    // 11. ANALYTICS DASHBOARD

    void analytics() {
        printHeader("OPERATIONAL ANALYTICS & RISK DASHBOARD");
        auto shipments = shipmentDB.getAllSorted();

        int pending=0, transit=0, delivered=0, delayed=0;
        int safe=0, atRisk=0, critical=0;
        double totalValue=0;

        for (auto& s : shipments) {
            if (s.status == "Pending")     pending++;
            else if (s.status == "In Transit") transit++;
            else if (s.status == "Delivered")  delivered++;
            else if (s.status == "Delayed")    delayed++;
            if (s.riskFlag == 0)  safe++;
            else if (s.riskFlag == 1) atRisk++;
            else critical++;
            totalValue += s.value;
        }

        int suspended = 0;
        for (auto& sup : suppliers)
            if (sup.status == "Suspended") suspended++;

        cout << fixed << setprecision(2);
        printDivider();
        cout << "  SHIPMENT STATUS\n";
        cout << "  Pending     : " << pending   << "\n";
        cout << "  In Transit  : " << transit   << "\n";
        cout << "  Delivered   : " << delivered << "\n";
        cout << "  Delayed     : " << delayed   << "\n";
        cout << "  Total Value : $" << totalValue << "\n";
        printDivider();
        cout << "  RISK INDICATORS\n";
        cout << "  Safe        : " << safe     << " shipments\n";
        cout << "  At Risk     : " << atRisk   << " shipments\n";
        cout << "  Critical    : " << critical << " shipments\n";
        cout << "  Susp.Suppliers: " << suspended << "\n";
        printDivider();
        cout << "  QUEUE & UNDO\n";
        cout << "  Events in Queue : " << eventQueue.size() << "\n";
        cout << "  Undo Stack Depth: " << undoStack.size() << "\n";
        printDivider();
    }

    // MAIN MENU — CLI Dashboard

    void run() {
        cout << "\n";
        cout << "  ╔══════════════════════════════════════════════════════════╗\n";
        cout << "  ║  Smart Global Supply Chain Risk Management System        ║\n";
        cout << "  ║  B.Tech Semester II — DSA Project                        ║\n";
        cout << "  ╚══════════════════════════════════════════════════════════╝\n";

        loadDummyData();

        while (true) {
            cout << "\n  ┌─ MAIN MENU ─────────────────────────────────────┐\n";
            cout << "  │  1. View All Shipments         (AVL Tree)       │\n";
            cout << "  │  2. View All Suppliers                          │\n";
            cout << "  │  3. Add New Supplier                            │\n";
            cout << "  │  4. Add New Shipment                            │\n";
            cout << "  │  5. Search Shipment            (BST Search)     │\n";
            cout << "  │  6. Search Supplier            (Hash + Linear)  │\n";
            cout << "  │  7. Sort & Prioritize          (Bubble/Select)  │\n";
            cout << "  │  8. Process Shipment Queue     (Queue FIFO)     │\n";
            cout << "  │  9. Undo Last Action           (Stack)          │\n";
            cout << "  │ 10. Route Optimizer            (BFS/DFS/Dijkstra│\n";
            cout << "  │ 11. Analytics Dashboard                         │\n";
            cout << "  │  0. Exit                                        │\n";
            cout << "  └─────────────────────────────────────────────────┘\n";
            cout << "  Choice: ";

            int choice;
            if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

            switch (choice) {
                case 1:  viewAllShipments(); break;
                case 2:  viewAllSuppliers();  break;
                case 3:  addSupplier();        break;
                case 4:  addShipment();        break;
                case 5:  searchShipment();     break;
                case 6:  searchSupplier();     break;
                case 7:  sortShipments();      break;
                case 8:  processQueue();       break;
                case 9:  undoLastAction();     break;
                case 10: routeMenu();          break;
                case 11: analytics();          break;
                case 0:
                    cout << "  Exiting system. Goodbye.\n\n";
                    return;
                default:
                    cout << "  Invalid choice.\n";
            }
        }
    }
};

// ENTRY POINT

int main() {
    SupplyChainSystem system;
    system.run();
    return 0;
}
