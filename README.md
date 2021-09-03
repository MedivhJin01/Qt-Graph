# Qt-Graph
The project is an extended version of ECE250 Project3 which visualizes graph data structure using Qt libs.

# Introduction
The Project is developed based on undirected weighted graph structure and can be used to help UW CompEng 2A students on their ECE250 Project3. The project is written in C++ with Qt libraries. The project provides a simple UI and instruction mannual. It also provides save and open functions that allow users to save a graph or read a graph. In addition, basic graph functions have been included in the project such as insert vertex, adding edge, updating weight, etc. Advanced algorithms are also implemented in the project including Depth First Search, Breadth First Search, Minimum Spanning Tree Prim Method, Floyd-Warshall Algorithm. 

# User Interface
<img width="690" alt="Screen Shot 2021-09-03 at 1 58 09 PM" src="https://user-images.githubusercontent.com/55257057/131957567-8acd5edb-e412-4d10-88c6-76ed7799f1d6.png">

# Vertex
Add Vertex: Inserting a vertex in QGraphicsView, the position of the inserted vertex will be automatically set to the center, and the index of the vertex is the current number of vertices exist in the graph.

<img width="500" alt="Screen Shot 2021-09-03 at 2 19 37 PM" src="https://user-images.githubusercontent.com/55257057/131959958-fafc0bf5-dd09-4f4c-b907-176435b3d8af.png">

Erase Vertex: When user proceed this operation, a window will be popped out asking for the target vertex index that will be erased. When the target vertex is deleted, all the vertex index that greater than it will be updated for tracking purpose. 

<img width="500" alt="Screen Shot 2021-09-03 at 2 19 51 PM" src="https://user-images.githubusercontent.com/55257057/131960836-058e1459-5df2-46fe-a9ca-add5ead9966c.png">

Set Position: Users can change the position of a vertex through this function, the range of the QGraphicsView is top-left(-390, -305) and bottom-right(390, 440). Moreover, vertex position can be changed through dragging.

<img width="300" alt="Screen Shot 2021-09-03 at 2 20 01 PM" src="https://user-images.githubusercontent.com/55257057/131960857-3ad4d06b-1d3a-430e-b9ed-c49b90795222.png">

Vertex Info: To check all the information of a vertex, including coordinates, degree, adjacent vertices, and weight.

<img width="500" alt="Screen Shot 2021-09-03 at 2 20 18 PM" src="https://user-images.githubusercontent.com/55257057/131960867-d875be88-e0bf-4cde-9e2d-aa0d9ef76ebf.png">

*an algorithm is written to create repulsion between vertices in order to clearly display the weight of an edge. Thus, when two vertices are too close, they will be pushed away automatically.

<img width="60" alt="Screen Shot 2021-09-03 at 2 20 28 PM" src="https://user-images.githubusercontent.com/55257057/131960879-a7763f08-0095-46bd-9ed7-81c086a597c4.png">

# Edge
Add Edge: Inserting an edge between two vertices with weight. The edge will be undirected(two ways) and having a weight greater than 0.

<img width="500" alt="Screen Shot 2021-09-03 at 2 39 16 PM" src="https://user-images.githubusercontent.com/55257057/131961565-7bbac36c-5b1a-4165-ba5b-8eda4ccff7a3.png">

Update Weight: Updating the weight of an existed edge.

<img width="500" alt="Screen Shot 2021-09-03 at 2 39 25 PM" src="https://user-images.githubusercontent.com/55257057/131961576-d4068038-8b64-4dc4-bd82-cee0e7072ffe.png">

Erase Edge: Erase an edge.

<img width="500" alt="Screen Shot 2021-09-03 at 2 39 33 PM" src="https://user-images.githubusercontent.com/55257057/131961591-91b62e7b-0457-41ae-bfb3-9ac1f5c5e0e9.png">

# Graph
Function: 
1.  Depth First Search

![dfs](https://user-images.githubusercontent.com/55257057/132029997-19887d30-707d-4407-9e08-3a7095f88997.gif)

2.  Breadth First Search

![bfs](https://user-images.githubusercontent.com/55257057/132030062-8131dcde-1e02-480b-b573-55b0bb486c03.gif)

3.  Minimum Spanning Tree (Prim Method)

![mst](https://user-images.githubusercontent.com/55257057/132030092-618249d6-220c-4fad-99b3-472735e3b1e3.gif)

4.  Optimal Path (Floyd-Warshall Algorithm)

![optimal](https://user-images.githubusercontent.com/55257057/132030106-cbd5467e-9684-46e8-913a-bc79f947032e.gif)




