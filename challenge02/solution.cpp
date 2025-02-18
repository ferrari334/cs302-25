// Challenge 02: Closest Numbers
// Name:  Matthew Ferrari

// Brief description
// output pairs of elements with smallest difference,
// iterate through every element, paired with every element and see which
// elements create the smallest difference. if more than one pair of
// elements have the same smallest difference, output all those pairs.

// take all integers and use sorting algorithm to sort integers

// This code solves yyyy based on the following idea/implementation...
// using a set of pairs to avoid repeats and add the vals with smallest
// difference outputting
// with iterator
#include <algorithm>
#include <bits/stdc++.h>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

void printPairs(const set<pair<int, int>> &pairs) {
  size_t count = 0;
  size_t total = pairs.size();
  set<pair<int, int>>::iterator it;
  for (it = pairs.begin(); it != pairs.end(); it++) {
    cout << it->first << " " << it->second;
    ++count;
    if (count != total) {
      cout << " ";
    }
  }
  cout << "\n"; // newline after all pairs
}

int main(int argc, char *argv[]) {

  int num_elements_in_array;
  int element;
  int smallest_difference;

  while (cin >> num_elements_in_array) {
    vector<int> elements;

    for (int i = 0; i < num_elements_in_array; i++) {
      cin >> element;
      elements.push_back(element);
    }

    // add all elements to vector and sort when vector is full
    sort(elements.begin(), elements.end());

    // base case smallest_diff, make sure its absv
    smallest_difference = abs(elements[1] - elements[0]);

    set<pair<int, int>> pairs;
    pairs.insert({elements[0], elements[1]});

    // this is used to go through all elements once.
    // this is used to test the element in the while with all other elements of
    // loop.

    for (int i = 0; i < num_elements_in_array - 1; i++) {
      int current_diff = abs(elements[i + 1] - elements[i]);
      if (current_diff < smallest_difference) {
        // Found a new smaller difference
        smallest_difference = current_diff;
        pairs.clear(); // clear previous pairs
        pairs.insert({elements[i], elements[i + 1]});
      } else if (current_diff == smallest_difference) {
        // make sure two numbers paired are already not in vector
        // Found another pair with the same smallest difference
        pairs.insert({elements[i], elements[i + 1]});
      }
    }
    printPairs(pairs);
  }
  return EXIT_SUCCESS;
}
