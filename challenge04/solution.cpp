// Challenge 04: Graph Paths
// Matthew Ferrari
// sources:
// https://www.geeksforgeeks.org/2d-vector-in-cpp-with-user-defined-size/
// https://www.geeksforgeeks.org/cpp-program-to-implement-adjacency-matrix/

// represent the graph using either adjacency list or adjacency matrix
//  1) first line specifies the number of edges in the directed graph
//  2) followed by NEDGES pairs of nodes where the first string is the source
//  and the second string is the destination
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

// read in input and build graph
// first number is the edges, 2nd number is the paths
// 3rd number is edges, 4th number is paths

class Graph {
  int V; // num verticies in graph
  vector<vector<int>> adjList;

public:
  // Constructor for initializing the graph with a given
  // number of vertex
  Graph(int vertices) {
    V = vertices;
    adjList.resize(vertices);
  }
  // adding the pairs to matrix
  void addEdge(vector<vector<int>> &matrix, char src, char dest) {
    // matrix[src][dest] = 1;
    // matrix[dest][src] = 1;
    // add dest vertex to serc vertex list
    adjList[src].push_back(dest);
  }

  // check if path exists
  // need to add BFS or DFS search for longer path
  /*
    void findPath(vector<vector<int>> &matrix, char src, char dest) {
      if (matrix[src][dest] == 1) {
        cout << "The path: " << src << dest << " exists." << endl;
      } else {
        cout << "The path: " << src << dest << " does not exist." << endl;
      }
    }
  */

  // Function to perform Breadth-First Traversal (BFS)
  // starting from a given vertex.
  void bfs(int startVertex) {
    // Vector to keep track of visited vertices.
    vector<bool> visited(V, false);
    // Queue to help with the BFS traversal.
    queue<int> q;
    // Mark the start vertex as visited.
    visited[startVertex] = true;
    // Enqueue the start vertex.
    q.push(startVertex);

    while (!q.empty()) {
      // point currentVertex at front vertex from the queue.
      int currentVertex = q.front();
      // Print the current vertex.
      cout << currentVertex << " ";
      // Remove the front vertex from the queue.
      q.pop();

      // Iterate through all the neighbors of the
      // current vertex.
      for (int neighbor : adjList[currentVertex]) {
        // If the neighbor vertex has not been
        // visited yet.
        if (!visited[neighbor]) {
          // Mark the neighbor vertex as visited.
          visited[neighbor] = true;
          // Enqueue the neighbor vertex.
          q.push(neighbor);
        }
      }
    }
  }
};

int main(int argc, char *argv[]) {
  // while we have edges to read in
  while (true) {
    int edges, paths;
    char src, dest;
    // break case
    if (!(cin >> edges)) // reads in edge
      break;

    // EDGES
    // init 2d vector edges x edges. all 0s
    vector<vector<int>> vec(edges, vector<int>(edges, 0));
    // add the edges to matrix
    for (int i = 0; i < edges; i++) {
      // read in what edges we have and adjust out matrix
      cin >> src >> dest;      // read in AB
      addEdge(vec, src, dest); // add AB to matrix
    }

    // PATHS
    cin >> paths; // read how many paths - 2
    // we need certain amount of paths read, that amout is output after edges
    for (int i = 0; i < paths; i++) {
      // read in what pairs we have and adjust out matrix
      cin >> src >> dest; // read in what paths to check
      findPath(vec, src, dest);
    }
    vec.clear(); // clear vector for next graph
  }

  return (0);
}
