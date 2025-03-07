// Matthew Ferrari
// 000664319
// dwp393
// Challenge 5: Minimum Spanning Tree
// outline: given undirected graph find the MST

// So lets go ahead and create a adjacency matrix
// create a multi_map
// implement prims algorithm to find MST
// -1 : no edge
// verticies are a,b,c,d, etc..
//
#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
using namespace std;

int main() {
  int num_nodes, edge;
  // store each test output
  vector<string> outputs;

  while (cin >> num_nodes) {
    // read in matrix
    vector<vector<int>> adjMatrix(num_nodes, vector<int>(num_nodes, -1));
    for (int i = 0; i < num_nodes; i++) {
      for (int j = 0; j < num_nodes; j++) {
        cin >> edge;
        if (edge != -1) {
          adjMatrix[i][j] = edge;
        }
      }
    }

    // Prim's
    vector<bool> used(num_nodes, false); // tracks vertices in the MST
    vector<pair<int, int>> mstEdges;     // MST edges
    int totalWeight = 0;
    // multimap as PQ (key = weight, value = (from,
    // to) edge).
    multimap<int, pair<int, int>> edges;

    used[0] = true;
    for (int j = 0; j < num_nodes; j++) {
      if (adjMatrix[0][j] != -1)
        edges.insert(make_pair(adjMatrix[0][j], make_pair(0, j)));
    }

    while (!edges.empty() &&
           mstEdges.size() < static_cast<size_t>(num_nodes - 1)) {
      multimap<int, pair<int, int>>::iterator it = edges.begin();
      int w = it->first;
      int u = it->second.first;
      int v = it->second.second;
      edges.erase(it);

      if (used[v])
        continue;

      used[v] = true;
      totalWeight += w;
      mstEdges.push_back(make_pair(u, v));

      // add all new edges from vertex v
      for (int j = 0; j < num_nodes; j++) {
        if (adjMatrix[v][j] != -1 && !used[j])
          edges.insert(make_pair(adjMatrix[v][j], make_pair(v, j)));
      }
    }

    // output
    vector<string> edgeStr;
    for (vector<pair<int, int>>::iterator it = mstEdges.begin();
         it != mstEdges.end(); it++) {
      int a = it->first, b = it->second;
      if (a > b) {
        int temp = a;
        a = b;
        b = temp;
      }
      string s;
      s.push_back('A' + a);
      s.push_back('A' + b);
      edgeStr.push_back(s);
    }
    // sort lexicographically
    sort(edgeStr.begin(), edgeStr.end());

    ostringstream oss;
    oss << totalWeight << "\n";
    for (vector<string>::iterator it = edgeStr.begin(); it != edgeStr.end();
         it++) {
      oss << *it << "\n";
    }

    outputs.push_back(oss.str());
  }

  for (size_t i = 0; i < outputs.size(); i++) {
    cout << outputs[i];
    if (i != outputs.size() - 1)
      cout << "\n";
  }

  return 0;
}
