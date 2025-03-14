#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

#include "disjoint.h"
/*
- prints out every possible scoring set
- size, scoring color, scoring cell
- read in board and output all sets that are scoring sets
- use disjoint sets to look around and see if colors next to it are same and
union
- create separate container that holds metadata, size, what color, if there is a
scoring cell
*/
using namespace std;

class Superball {
public:
  Superball(int argc, char **argv);
  int r;
  int c;
  int mss;
  int empty;
  vector<int> board; // whole board or chars
  vector<int> goals; // goal cells
  vector<int> colors;

  void analyze_superball();
};

// metadata struct
struct metadata {
  int size;
  char color;
  int scoring_row;
  int scoring_col;
  bool has_scoring;
};

void Superball::analyze_superball() {
  DisjointSetByRankWPC ds(r * c);
  // Union non-empty cells with same-color neighbors.
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      int idx = i * c + j;
      char current = board[idx];
      // Skip empty cells and goal markers (like '*') since they are not actual
      // pieces.
      if (current == '.' || current == '*')
        continue;
      // Check right neighbor.
      if (j + 1 < c) {
        int rightIdx = i * c + (j + 1);
        if (board[rightIdx] == current) {
          int root1 = ds.Find(idx);
          int root2 = ds.Find(rightIdx);
          if (root1 != root2)
            ds.Union(root1, root2);
        }
      }
      // Check down neighbor.
      if (i + 1 < r) {
        int downIdx = (i + 1) * c + j;
        if (board[downIdx] == current) {
          int root1 = ds.Find(idx);
          int root2 = ds.Find(downIdx);
          if (root1 != root2)
            ds.Union(root1, root2);
        }
      }
    }
  }

  // Build component metadata
  // goes through table and writes down info about cells we care about.
  vector<metadata> comp(r * c, {0, ' ', -1, -1, false});
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      int idx = i * c + j;
      char current = board[idx];
      // Skip empty cells and '*' markers.
      if (current == '.' || current == '*')
        continue;
      int root = ds.Find(idx);
      if (comp[root].size == 0) {
        comp[root].color = current;
        comp[root].size = 1;
        if (goals[idx] == 1) {
          comp[root].has_scoring = true;
          comp[root].scoring_row = i;
          comp[root].scoring_col = j;
        }
      } else {
        comp[root].size++;
        if (!comp[root].has_scoring && goals[idx] == 1) {
          comp[root].has_scoring = true;
          comp[root].scoring_row = i;
          comp[root].scoring_col = j;
        }
      }
    }
  }

  // if cell has scoring cell == * then exit, we want cap CHAR
  // Check for a bad scoring cell.
  for (int i = 0; i < r * c; i++) {
    if (comp[i].size > 0 && comp[i].has_scoring) {
      int idx = comp[i].scoring_row * c + comp[i].scoring_col;
      if (board[idx] == '*') {
        cerr << "Error: Component with size " << comp[i].size
             << " has an invalid scoring cell at " << comp[i].scoring_row << ","
             << comp[i].scoring_col << " (cell is '*', not a valid piece)."
             << endl;
        exit(1);
      }
    }
  }

  // Only if no errors, print the scoring sets.
  cout << "Scoring sets:" << endl;
  vector<bool> printed(r * c, false);
  for (int i = 0; i < r * c; i++) {
    if (comp[i].size > 0 && !printed[i]) {
      if (comp[i].size >= mss && comp[i].has_scoring) {
        // Here, we add 1 to the scoring column if needed (depending on your
        // desired output format)
        printf("  Size: %2d  Char: %c  Scoring Cell: %d,%d\n", comp[i].size,
               comp[i].color, comp[i].scoring_row, comp[i].scoring_col);
      }
      printed[i] = true;
    }
  }
}

void usage(const char *s) {
  cerr << "usage: sb-analyze rows cols min-score-size colors" << endl;
  if (s != NULL)
    cerr << s << endl;
  exit(1);
}

Superball::Superball(int argc, char **argv) {
  int i, j;
  string s;

  if (argc != 5)
    usage(NULL);

  if (sscanf(argv[1], "%d", &r) == 0 || r <= 0)
    usage("Bad rows");
  if (sscanf(argv[2], "%d", &c) == 0 || c <= 0)
    usage("Bad cols");
  if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0)
    usage("Bad min-score-size");

  colors.resize(256, 0);
  for (i = 0; i < (int)strlen(argv[4]); i++) {
    if (!isalpha(argv[4][i]))
      usage("Colors must be distinct letters");
    if (!islower(argv[4][i]))
      usage("Colors must be lowercase letters");
    if (colors[argv[4][i]] != 0)
      usage("Duplicate color");
    colors[argv[4][i]] = 2 + i;
    colors[toupper(argv[4][i])] = 2 + i;
  }

  board.resize(r * c);
  goals.resize(r * c, 0);
  empty = 0;

  for (i = 0; i < r; i++) {
    if (!(cin >> s)) {
      cerr << "Bad board: not enough rows on standard input" << endl;
      exit(1);
    }
    if ((int)s.size() != c) {
      cerr << "Bad board on row " << i << " - wrong number of characters."
           << endl;
      exit(1);
    }
    for (j = 0; j < c; j++) {
      if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
        cerr << "Bad board row " << i << " - bad character " << s[j] << "."
             << endl;
        exit(1);
      }
      board[i * c + j] = s[j];
      if (board[i * c + j] == '.' || board[i * c + j] == '*')
        empty++;
      if (isupper(board[i * c + j]) || board[i * c + j] == '*') {
        goals[i * c + j] = 1;
        board[i * c + j] = tolower(board[i * c + j]);
      }
    }
  }

  /*
  // Check for any invalid goal cells.
  for (int i = 0; i < r * c; i++) {
    if (goals[i] == 1 && board[i] == '*') {
      cerr << "Error: Invalid board. Goal cell at (" << i / c << "," << i % c
           << ") is '*' instead of a valid piece." << endl;
      exit(1);
    }
  }
  */
}

int main(int argc, char **argv) {
  Superball *s;

  s = new Superball(argc, argv);

  DisjointSetByRankWPC ds(s->r * s->c);
  s->analyze_superball();
  //  ds.Print();
}
