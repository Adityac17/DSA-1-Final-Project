# Smart Global Supply Chain Risk Management System

**B.Tech Semester II — Data Structures & Algorithms Final Project**  
**ITM Skills University, School of Future Tech**  
**Author:** Aditya Sunil Chouksey

---

## Table of Contents

1. [Project Title](#1-project-title)
2. [Problem Statement](#2-problem-statement)
3. [Objectives](#3-objectives)
4. [System Overview / Architecture](#4-system-overview--architecture)
5. [Data Structures and Algorithms Used](#5-data-structures-and-algorithms-used)
6. [Implementation Approach](#6-implementation-approach)
7. [Time and Space Complexity Analysis](#7-time-and-space-complexity-analysis)
8. [Execution Steps](#8-execution-steps)
9. [Sample Inputs and Outputs](#9-sample-inputs-and-outputs)
10. [Screenshots](#10-screenshots)
11. [Results and Observations](#11-results-and-observations)
12. [Conclusion](#12-conclusion)

---

## 1. Project Title

**Smart Global Supply Chain Risk Management System**

A console-based C++ application that simulates and manages a global logistics network. It handles supplier registration, shipment tracking, route optimization, risk assessment, and operational analytics — all powered by core DSA data structures implemented from first principles.

---

## 2. Problem Statement

Modern global supply chains involve thousands of concurrent shipments, dozens of international suppliers, and dynamic risk conditions (supplier insolvency, geopolitical delays, customs holds). Managing this with flat files or spreadsheets leads to:

- **O(n) lookup times** for shipment/supplier searches — unacceptable at scale.
- **No route intelligence** — operators manually select shipping paths with no cost optimization.
- **No risk gating** — suspended or high-risk suppliers can still dispatch shipments.
- **No undo capability** — accidental data entry errors require full manual correction.
- **No pipeline management** — incoming shipment events are processed arbitrarily, not in order.

This project solves each of these problems by mapping them directly to the appropriate data structure or algorithm, demonstrating how DSA concepts translate into real-world system design.

---

## 3. Objectives

1. Implement an **AVL self-balancing BST** as the primary shipment database with guaranteed O(log n) CRUD.
2. Model the global logistics network as a **weighted undirected graph** with BFS, DFS, and Dijkstra's algorithm for route optimization.
3. Provide **O(1) supplier lookup** using hash maps (`std::unordered_map`).
4. Implement a **stack-based undo system** to safely reverse add/process operations.
5. Manage shipment dispatch order via a **FIFO queue** pipeline.
6. Demonstrate **Bubble Sort** (by priority) and **Selection Sort** (by risk) on shipment datasets.
7. Demonstrate **Binary Search** (O(log n)) and **Linear Search** (O(n)) for comparison.
8. Enforce a **supplier risk gate** that blocks suspended suppliers from dispatching shipments.
9. Provide an **analytics dashboard** with aggregated status and risk metrics.
10. Deliver a fully self-contained C++ application compilable with a single `g++` command.

---

## 4. System Overview / Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    SupplyChainSystem (Controller)               │
│                                                                 │
│  ┌─────────────────┐   ┌──────────────────┐   ┌─────────────┐ │
│  │   AVL Tree      │   │  RouteGraph       │   │  Supplier   │ │
│  │  (Shipment DB)  │   │  (Logistics Net)  │   │  Vector +   │ │
│  │  O(log n) CRUD  │   │  BFS/DFS/Dijkstra │   │  Hash Map   │ │
│  └─────────────────┘   └──────────────────┘   └─────────────┘ │
│                                                                 │
│  ┌─────────────────┐   ┌──────────────────┐                   │
│  │  Stack<Action>  │   │  Queue<Shipment>  │                   │
│  │  (Undo System)  │   │  (Event Pipeline) │                   │
│  └─────────────────┘   └──────────────────┘                   │
└─────────────────────────────────────────────────────────────────┘
                              │
                    ┌─────────▼──────────┐
                    │     CLI Menu        │
                    │  (11 operations)    │
                    └────────────────────┘
```

### Data Flow

```
User Input → Menu Selection → SupplyChainSystem Method
    → pushHistory (undoStack)
    → Modify Data Structure (AVL / vector / graph / queue)
    → Print Formatted Output to Console
```

### Component Responsibilities

| Component | Responsibility |
|-----------|---------------|
| `AVLTree` | Shipment storage — insert, delete, search, sorted traversal |
| `RouteGraph` | Logistics hub network — BFS, DFS, Dijkstra |
| `suppliers` (vector) | Ordered supplier list for sequential access |
| `supplierHash` (unordered_map) | O(1) supplier pointer lookup by ID |
| `eventQueue` (std::queue) | FIFO dispatch pipeline for pending shipments |
| `undoStack` (std::stack) | LIFO action history for single-step undo |

---

## 5. Data Structures and Algorithms Used

### Data Structures

| Structure | Implementation | Purpose |
|-----------|---------------|---------|
| AVL Tree | Custom (`AVLTree` class) | Primary shipment database — self-balancing BST keyed on shipment ID |
| Weighted Graph | `RouteGraph` with adjacency list (`vector<vector<pair<int,int>>>`) | 12-node global logistics hub network |
| Hash Map | `std::unordered_map<int, Supplier*>` | O(1) supplier lookup by ID |
| Hash Map | `std::unordered_map<int, Shipment*>` | O(1) shipment pointer access |
| Stack | `std::stack<Action>` | Undo last action (LIFO) |
| Queue | `std::queue<Shipment>` | Shipment event dispatch pipeline (FIFO) |
| Dynamic Array | `std::vector<Supplier>` | Ordered supplier list |

### Algorithms

| Algorithm | Function | Purpose |
|-----------|----------|---------|
| AVL Rotations (LL/LR/RL/RR) | `AVLTree::rebalance()` | Maintain BST height balance after every insert/delete |
| Dijkstra's Shortest Path | `RouteGraph::dijkstra()` | Find minimum-cost shipping route between two hubs |
| Breadth-First Search | `RouteGraph::bfs()` | Level-order hub traversal, minimum-hop path |
| Depth-First Search | `RouteGraph::dfs()` | Deep path exploration, connectivity check |
| Bubble Sort | `bubbleSortByPriority()` | Sort shipments by priority (ascending) |
| Selection Sort | `selectionSortByRisk()` | Sort shipments by risk flag (descending) |
| Binary Search | `binarySearchShipment()` | O(log n) shipment lookup on sorted array |
| Linear Search | `linearSearchSupplier()` | O(n) supplier fallback search |

---

## 6. Implementation Approach

The entire implementation is a single file (`main.cpp`, ~480 lines) organized in five layers:

### Layer 1 — Record Structs

```cpp
struct Supplier { int id; string name; string country; int riskScore; string status; };
struct Shipment { int id; int supplierId; string origin; string destination;
                  string status; int priority; double value; int riskFlag; };
```

Both structs provide a `display()` method for formatted console output using `std::setw` and `std::setfill`.

### Layer 2 — AVL Tree (Custom Implementation)

The `AVLTree` class uses private recursive methods for insert, delete, search, and in-order traversal. After every structural change, `rebalance()` checks the balance factor (`height_left − height_right`) and applies one of four rotation cases:

```
Balance Factor > 1:
  Left-Left  → rotateRight(node)
  Left-Right → rotateLeft(node->left), then rotateRight(node)
Balance Factor < -1:
  Right-Right → rotateLeft(node)
  Right-Left  → rotateRight(node->right), then rotateLeft(node)
```

Duplicate key inserts update the existing node (used for status updates via re-insert).

### Layer 3 — Route Graph

`RouteGraph` maintains a city-name → integer index mapping (`std::map<string,int>`) and an adjacency list. Dijkstra uses `std::priority_queue` with `greater<>` (min-heap) and lazy deletion to skip stale entries. Path reconstruction uses a `parent[]` array walked in reverse.

### Layer 4 — Business Logic in SupplyChainSystem

- **Risk scoring**: `riskScore > 75 → riskFlag 2 (Critical)`, `> 40 → 1 (At Risk)`, else `0 (Safe)`
- **Suspended gate**: `addShipment()` checks supplier status before creating a record
- **Auto-suspend**: new suppliers with `riskScore > 75` are set to `"Suspended"` automatically
- **Undo push**: every write operation pushes an `Action` struct before modifying state

### Layer 5 — CLI Menu Loop

`run()` drives an infinite `while(true)` loop with `cin >> choice`. Each case calls the corresponding method. Input errors are handled with `cin.clear()` + `cin.ignore()`.

---

## 7. Time and Space Complexity Analysis

### Time Complexity

| Operation | Data Structure | Best | Average | Worst |
|-----------|---------------|------|---------|-------|
| Insert shipment | AVL Tree | O(log n) | O(log n) | O(log n) |
| Delete shipment | AVL Tree | O(log n) | O(log n) | O(log n) |
| Search shipment by ID | AVL Tree | O(1) | O(log n) | O(log n) |
| In-order traversal | AVL Tree | O(n) | O(n) | O(n) |
| Lookup supplier | Hash Map | O(1) | O(1) | O(n)* |
| Enqueue / Dequeue | Queue | O(1) | O(1) | O(1) |
| Push / Pop undo | Stack | O(1) | O(1) | O(1) |
| Dijkstra's | Graph | — | O((V+E) log V) | O((V+E) log V) |
| BFS / DFS | Graph | O(V+E) | O(V+E) | O(V+E) |
| Bubble Sort | Array | O(n) | O(n²) | O(n²) |
| Selection Sort | Array | O(n²) | O(n²) | O(n²) |
| Binary Search | Sorted Array | O(1) | O(log n) | O(log n) |
| Linear Search | Array | O(1) | O(n) | O(n) |

*Hash map worst case O(n) occurs on hash collision chain — rare with integer keys.

### Space Complexity

| Component | Space |
|-----------|-------|
| AVL Tree (n shipments) | O(n) |
| Route Graph (V nodes, E edges) | O(V + E) |
| Supplier vector + hash map | O(s) where s = supplier count |
| Undo stack (max depth = operations performed) | O(k) |
| Shipment queue | O(q) where q = queued events |
| Dijkstra auxiliary (dist[], parent[], pq) | O(V) |
| **Total** | **O(n + V + E + s + k + q)** |

For the pre-loaded dataset: n=8 shipments, V=12 nodes, E=12 edges, s=6 suppliers → total ~50 objects in memory.

---

## 8. Execution Steps

### Prerequisites

- C++ compiler: GCC/G++ (version 7+) or Clang
- C++11 standard or higher
- Terminal: Linux/macOS terminal or Windows WSL / MinGW

### Step 1 — Clone the Repository

```bash
git clone https://github.com/Adityac17/DSA-1-Final-Project.git
cd DSA-1-Final-Project
```

### Step 2 — Compile

```bash
g++ -std=c++11 main.cpp -o supply_chain
```

On Windows (MinGW):
```bash
g++ -std=c++11 main.cpp -o supply_chain.exe
```

### Step 3 — Run

```bash
./supply_chain
```

On Windows:
```bash
supply_chain.exe
```

### Step 4 — Use the Menu

The system auto-loads dummy data on startup. Enter a number (0–11) at the `Choice:` prompt to select an operation.

```
Choice: 1    → View all shipments (AVL in-order)
Choice: 10   → Route Optimizer (then choose BFS/DFS/Dijkstra)
Choice: 9    → Undo last action
Choice: 0    → Exit
```

---

## 9. Sample Inputs and Outputs

### Sample 1 — View All Shipments (Menu Option 1)

**Input:** `1`

**Output:**
```
  ╔══════════════════════════════════════════════════════╗
  ║  ALL SHIPMENTS — AVL Tree In-Order Traversal         ║
  ╚══════════════════════════════════════════════════════╝
  [SH-1001] Mumbai      -> London      | CRITICAL  | $85000.00  | In Transit | Safe
  [SH-1002] Shanghai    -> Dubai       | CRITICAL  | $120000.00 | Pending    | Critical
  [SH-1003] Berlin      -> New York    | NORMAL    | $45000.00  | Delivered  | Safe
  [SH-1004] Dubai       -> Singapore   | CRITICAL  | $67000.00  | Delayed    | At Risk
  [SH-1005] New York    -> London      | NORMAL    | $33000.00  | In Transit | Safe
  [SH-1006] Mumbai      -> Singapore   | LOW       | $21000.00  | Pending    | Safe
  [SH-1007] Shanghai    -> Berlin      | CRITICAL  | $95000.00  | Pending    | At Risk
  [SH-1008] Dubai       -> London      | CRITICAL  | $58000.00  | Delayed    | Critical
  Total: 8 shipments
```

### Sample 2 — Dijkstra Route Optimizer (Menu Option 10 → Choice 3)

**Input:** `10` → `3` → `Mumbai` → `London`

**Output:**
```
  Optimal Route: Mumbai -> Dubai -> London
  Total Cost: $800 per unit
```

### Sample 3 — Add New Shipment (Menu Option 4)

**Input:**
```
Supplier ID  : 103
Origin       : Berlin
Destination  : Tokyo
Priority(1-4): 2
Value (USD)  : 52000
```

**Output:**
```
  [✓] Shipment SH-1011 created. Risk: Safe
```

### Sample 4 — Undo Last Action (Menu Option 9)

**Input:** `9`

**Output:**
```
  [UNDO] Reversing: Added shipment 1011
  [✓] Shipment SH-1011 removed from database.
```

### Sample 5 — Analytics Dashboard (Menu Option 11)

**Input:** `11`

**Output:**
```
  ──────────────────────────────────────────────────────
  SHIPMENT STATUS
  Pending     : 4
  In Transit  : 2
  Delivered   : 1
  Delayed     : 2
  Total Value : $524000.00
  ──────────────────────────────────────────────────────
  RISK INDICATORS
  Safe          : 4 shipments
  At Risk       : 2 shipments
  Critical      : 2 shipments
  Susp.Suppliers: 1
  ──────────────────────────────────────────────────────
  QUEUE & UNDO
  Events in Queue : 2
  Undo Stack Depth: 0
```

### Sample 6 — Sort by Risk (Menu Option 7 → Choice 2)

**Input:** `7` → `2`

**Output:**
```
  Shipments sorted by Risk Flag (highest risk first):
  [SH-1002] Shanghai -> Dubai    | CRITICAL | $120000.00 | Pending    | Critical
  [SH-1008] Dubai    -> London   | CRITICAL | $58000.00  | Delayed    | Critical
  [SH-1004] Dubai    -> Singapore| CRITICAL | $67000.00  | Delayed    | At Risk
  ...
```

---

## 10. Screenshots

> **Note:** This is a console-based application. To capture screenshots, compile and run the program in your terminal, then use your OS screenshot tool.

### Expected Console Views

**Startup + Main Menu**
```
  ╔══════════════════════════════════════════════════════════╗
  ║  Smart Global Supply Chain Risk Management System        ║
  ║  B.Tech Semester II — DSA Project                        ║
  ╚══════════════════════════════════════════════════════════╝

  [✓] Dummy data loaded: 6 suppliers, 8 shipments, 12 routes, 2 queued events.

  ┌─ MAIN MENU ─────────────────────────────────────┐
  │  1. View All Shipments         (AVL Tree)       │
  │  2. View All Suppliers                          │
  │  3. Add New Supplier                            │
  │  4. Add New Shipment                            │
  │  5. Search Shipment            (BST Search)     │
  │  6. Search Supplier            (Hash + Linear)  │
  │  7. Sort & Prioritize          (Bubble/Select)  │
  │  8. Process Shipment Queue     (Queue FIFO)     │
  │  9. Undo Last Action           (Stack)          │
  │ 10. Route Optimizer            (BFS/DFS/Dijkstra│
  │ 11. Analytics Dashboard                         │
  │  0. Exit                                        │
  └─────────────────────────────────────────────────┘
  Choice:
```

---

## 11. Results and Observations

| Observation | Detail |
|-------------|--------|
| AVL balance maintained | With 8 shipments (IDs 1001–1008), tree height = 3. Unbalanced BST worst case would be height 8. Rotations reduce height by ~63%. |
| Dijkstra correctness | Mumbai → London resolves via Dubai (cost $800) — correct, as no direct Mumbai–London edge exists in the graph. |
| Hash map vs linear search | Supplier lookup via hash map is O(1) regardless of supplier count. Linear search fallback tested successfully for edge cases. |
| Undo stack correctness | All three undo paths validated: ADD_SHIPMENT removes from AVL, ADD_SUPPLIER removes from vector + hash map, PROCESS_SHIPMENT reverts status to "Pending". |
| Suspended supplier gate | Supplier S-106 (RiskZone, riskScore=92) correctly blocked shipment creation with message: `[!] Supplier is SUSPENDED. Shipment blocked.` |
| Queue FIFO order | Shipments SH-1009 and SH-1010 processed in exact enqueue order. |
| Sort correctness | Bubble Sort by priority and Selection Sort by risk both produce correctly ordered output on the 8-shipment dataset. |
| Analytics accuracy | All counters (pending/transit/delivered/delayed, safe/at-risk/critical) match manual count of the pre-loaded dataset. |

---

## 12. Conclusion

This project demonstrates a complete, curriculum-aligned application of Data Structures and Algorithms to a real-world logistics domain. Every data structure choice is motivated by a concrete operational requirement:

- **AVL Tree** → reliable O(log n) shipment storage with no degradation
- **Hash Map** → instant O(1) supplier access at any scale
- **Graph + Dijkstra** → automated, cost-optimal route selection
- **Stack** → operational safety via undo
- **Queue** → ordered, fair shipment dispatch pipeline
- **Sorting & Searching** → flexible data views and lookup strategies

The implementation is self-contained in a single C++ file, compilable with one command, and requires no external libraries — demonstrating that the C++ Standard Library combined with custom data structures is sufficient to build a fully functional business application from first principles.

**Future scope:**
- File-based persistence (CSV read/write on startup/exit)
- Min-Heap for SLA-breached shipment escalation (O(log n) extract-min)
- Trie for city name autocomplete
- GUI front-end (Qt or web interface via WebAssembly)

---

## Author

**Aditya Sunil Chouksey**  
B.Tech CSE — ITM Skills University  
Semester II — Data Structures & Algorithms  
GitHub: [Adityac17](https://github.com/Adityac17)
