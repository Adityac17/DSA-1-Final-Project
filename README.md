# 🌍 Smart Global Supply Chain Risk Management System

A robust, console-based C++ application designed to simulate and manage a global logistics and supply chain network. Built as a B.Tech Semester II Data Structures and Algorithms (DSA) project, this system efficiently handles supplier tracking, shipment processing, route optimization, and risk assessment using a variety of core computer science data structures.

## 🚀 Features

* **Shipment & Supplier Management:** Add, search, and view shipments and suppliers with real-time risk evaluation.
* **Intelligent Route Optimization:** Calculate the most cost-effective shipping routes between global logistics hubs.
* **Risk & Analytics Dashboard:** Monitor pending, transit, and delayed shipments alongside critical risk indicators.
* **Action Tracking (Undo System):** Revert previous actions like adding shipments or suppliers safely.
* **Event Queue Processing:** Handle pending shipments systematically via an automated FIFO pipeline.
* **Prioritization Engine:** Sort shipments based on operational priority or critical risk scores.

## 🧠 Data Structures & Algorithms Utilized

This project acts as a practical implementation of fundamental and advanced DSA concepts to solve real-world logistical problems:

### Data Structures
* **AVL Tree:** Acts as the primary in-memory database for Shipments. Ensures `O(log n)` time complexity for insertions, deletions, and searches, with an in-order traversal to retrieve sorted data.
* **Graphs (Weighted & Undirected):** Models the global transport network where nodes are cities/hubs and edges are the shipping costs between them.
* **Hash Maps (`std::unordered_map`):** Provides `O(1)` time complexity for direct supplier and shipment lookups without needing to traverse arrays.
* **Stack:** Powers the **Undo** functionality, utilizing a Last-In-First-Out (LIFO) approach to reverse the most recent state changes.
* **Queue:** Manages the Shipment Processing pipeline using a First-In-First-Out (FIFO) approach.

### Algorithms
* **Dijkstra's Algorithm:** Calculates the optimal (cheapest) route between two geographical logistics hubs.
* **Breadth-First Search (BFS) & Depth-First Search (DFS):** Explores network connectivity and pathfinding across the supply chain graph.
* **Binary Search:** Executes `O(log n)` lookups on sorted shipment arrays.
* **Linear Search:** Acts as a fallback mechanism for unsorted or small datasets.
* **Sorting Algorithms:** * *Bubble Sort:* Sorts shipments by Priority (`O(n²)`).
    * *Selection Sort:* Sorts shipments by Risk Score in descending order (`O(n²)`).

## 🛠️ Getting Started

### Prerequisites
* A standard C++ compiler (e.g., GCC/G++)
* C++11 or higher

### Compilation & Execution

1. Clone the repository:
   ```bash
   git clone [https://github.com/Adityac17/DSA-1-Final-Project.git](https://github.com/Adityac17/DSA-1-Final-Project.git)
   cd supply-chain-risk-management
Compile the source code:

Bash
g++ -std=c++11 main.cpp -o supply_chain
Run the application:

Bash
./supply_chain
(Note: On Windows, use supply_chain.exe)

## 🖥️ System Interface (CLI)
Upon launching, the system loads a set of dummy data (6 suppliers, 8 shipments, 12 routes) to immediately demonstrate functionality. The Main Menu provides the following operational choices:

View All Shipments (AVL Tree In-Order Traversal)

View All Suppliers

Add New Supplier

Add New Shipment (Evaluates risk based on supplier)

Search Shipment (Binary Search / BST)

Search Supplier (Hash Map + Linear fallback)

Sort & Prioritize (Bubble/Selection sort by risk/priority)

Process Shipment Queue (FIFO Queue)

Undo Last Action (Stack-based reversion)

Route Optimizer (Graph algorithms: BFS, DFS, Dijkstra)

Analytics Dashboard (Aggregated system insights)

Exit

## 👨‍💻 Author
Aditya Sunil Chouksey B.Tech Computer Science, School of Future Tech | ITM Skills University ---
Developed as part of the Semester II C++ / Data Structures & Algorithms curriculum.
