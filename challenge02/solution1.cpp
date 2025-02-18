#include <algorithm>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <set>
#include <utility> // For std::pair
#include <vector>

using namespace std;

// Function to print pairs without trailing space and followed by a newline
void printPairs(const set<pair<int, int>> &pairs) {
  size_t count = 0;
  for (const auto &p : pairs) {
    cout << p.first << " " << p.second;
    if (++count != pairs.size()) {
      cout << " ";
    }
  }
  cout << "\n"; // Newline after all pairs
}

int main() {
  // Optimize I/O operations

  int num_elements_in_array;
  int element;

  // Process multiple test cases until EOF
  while (cin >> num_elements_in_array) {
    // Handle cases where the number of elements is less than 2

    // Read exactly num_elements_in_array integers
    vector<int> elements;

    for (int i = 0; i < num_elements_in_array; ++i) {
      cin >> element;
      elements.push_back(element);
    }

    // Sort the elements
    sort(elements.begin(), elements.end());

    // Initialize smallest_difference with the first pair
    int smallest_difference = elements[1] - elements[0];
    set<pair<int, int>> pairs;
    pairs.insert({elements[0], elements[1]});

    // Iterate through the sorted vector to find smallest differences
    for (int i = 1; i < num_elements_in_array - 1; ++i) {
      int current_diff =
          elements[i + 1] - elements[i]; // No need for abs since sorted
      if (current_diff < smallest_difference) {
        // Found a new smaller difference
        smallest_difference = current_diff;
        pairs.clear(); // Clear previous pairs
        pairs.insert({elements[i], elements[i + 1]});
      } else if (current_diff == smallest_difference) {
        // Found another pair with the same smallest difference
        pairs.insert({elements[i], elements[i + 1]});
      }
    }

    // Output the pairs for the current test case
    printPairs(pairs);
  }

  return EXIT_SUCCESS;
}
