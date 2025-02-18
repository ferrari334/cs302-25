// Matthew Ferrari
//
//  Challenge 03: Palindrome Permutation

#include <bits/stdc++.h>
#include <cctype> // for ispunct()
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;
// determines if string s  is a palindrome in O(n) time using a unordered_set or
// unordered_map
// take string that has punctuation removed
// even strings need to have all even characters and odd strings can have one
// odd character
bool is_palindrome(string s) {
  unordered_map<char, int> char_tracker;
  // string iterator doc:https://www.geeksforgeeks.org/introduction-iterators-c/
  string::iterator it;
  for (it = s.begin(); it != s.end(); it++) {
    char ch = *it;

    // easy way to check if character is a letter
    // documentation says unsigned char is needed
    // https://en.cppreference.com/w/cpp/string/byte/isalpha
    if (isalpha(static_cast<unsigned char>(ch))) {
      char_tracker[ch]++;
    }
  }

  int oddCount = 0;
  unordered_map<char, int>::iterator mit;
  for (mit = char_tracker.begin(); mit != char_tracker.end(); mit++) {
    if (mit->second % 2 != 0) {
      oddCount++;
    }
  }
  return (oddCount <= 1);
}

int main(int argc, char *argv[]) {

  string input;
  // use map to have char and how many times that char appears
  // read in elements with characters and spaces
  // ignore numbers, spaces, and punctuation
  while (getline(cin, input)) {
    //  input.lowercase();
    if (is_palindrome(input)) {
      cout << '"' << input << '"' << " is a palindrome permutation" << endl;
    } else {
      cout << '"' << input << '"' << " is not a palindrome permutation" << endl;
    }
  }
  // output if phrase is palindrome or not
  return (0);
}
