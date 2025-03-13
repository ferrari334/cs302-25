// Challenge 06: Repeated DNA sequences
// Matthew Ferrari
// 000664319
// dwp393
//

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

int main(int argc, char *argv[]) {

  string input, holding;

  unordered_map<string, int> ltrs; // 9 char strings and their freq
  vector<string> repeated;         // this is whats printed out.

  // accept input
  while (cin >> input) {

    // clear each input after print
    ltrs.clear();
    repeated.clear();

    // input too short
    if (input.length() < 9) {
      cout << "-1" << endl;
      continue;
    }

    // get substrings of that line
    for (size_t i = 0; i <= input.length() - 9; i++) {
      holding = input.substr(i, 9);
      ltrs[holding]++;
    }

    // add substrings that occur more than once
    //     unordered_map<string, int>::const_iterator mit = ltrs.begin();
    for (unordered_map<string, int>::const_iterator mit = ltrs.begin();
         mit != ltrs.end(); mit++) {
      if (mit->second > 1) {
        repeated.push_back(mit->first);
      }
    }

    sort(repeated.begin(), repeated.end());

    // print out sets
    for (vector<string>::const_iterator it = repeated.begin();
         it != repeated.end(); ++it) {
      cout << *it << endl;
    }
    cout << "-1" << endl;
  }
  return 0;
}
