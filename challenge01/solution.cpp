// Matthew Ferrari
// Challenge 01: Rotating Arrays; null solution
// this takes in 3 commands length, rotations and direction and rotates length
// acordingly.

#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {

  // take the input 2 and subtract by input 1 if the 3rd is left and add if 3rd
  // is R
  int length, rotations;
  char direction;
  // reading in information about the rotation
  while (cin >> length >> rotations >> direction) {

    // vector to place elements that are read in
    vector<int> order(length);

    // read in elements
    for (int i = 0; i < length; i++) {
      cin >> order[i];
    }

    // check that rotation doesn't repeat starting order
    rotations %= length;

    // left rotation
    if (direction == 'L') {
      // add elements to a vector
      for (int i = 0; i < length; i++) {
        cout << order[(i + rotations) % length];
        if (i < length - 1)
          cout << " ";
      }
    }

    // right rotation
    else {
      for (int i = 0; i < length; i++) {
        cout << order[(i + (length - rotations)) % length];
        if (i < length - 1)
          cout << " ";
      }
    }
    cout << "\n";
  }
  // if 2nd is greater than 1st input then subtract and move left that many
  // values

  return 0;
}
