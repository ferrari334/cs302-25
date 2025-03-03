
/*
 * outputs single move that would be accepted as input for sb-player
    - swap or score
    - looks at state of the board
    - fewer than 5 empty slots, game is lost
    - analyze board
    - pick move (swap or score)
    - print move
    - create algorithm
  * have to average over 100 points on 100 runs for full credit
- Rows(int): the number of rows on the game board
- cols(int): the number of columns on the game board
- min-score-size (int): the number of contiguous cells that must be touching to
score them
- colors (string of chars -lowercase): this must be a string of distinct
lower-case letters that represent the possible colors a cell can have. The point
value of the first is 2 and goes up by 1 for each new letter in the string.

 */
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

#include "disjoint.h"
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

  string analyze_superball();
  string end_game_move();
};

// metadata struct
struct metadata {
  int size;
  char color;
  int scoring_row;
  int scoring_col;
  bool has_scoring;
};

string Superball::end_game_move() {
  vector<int> valid;
  // get all valid cells
  for (int i = 0; i < r * c; i++) {
    if (board[i] != '.' && board[i] != '*') {
      valid.push_back(i);
    }
  }
  // *****choose a cell where one of the swaps neighbors is a like color
  //  choose a random valid cell on board
  int idx1 = valid[rand() % valid.size()];
  int idx2 = valid[rand() % valid.size()];
  while (idx1 == idx2) {
    idx2 = valid[rand() % valid.size()];
  }
  // choose a random swap move
  int r1 = idx1 / c;
  int c1 = idx1 % c;
  int r2 = idx2 / c;
  int c2 = idx2 % c;
  ostringstream oss;
  oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
  return oss.str();
}

string Superball::analyze_superball() {
  ostringstream oss;

  if (empty < 5) {
    return end_game_move();
  }

  DisjointSetByRankWPC ds(r * c);
  // UNION cells with same-color cells next to non empty cell
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      int idx = i * c + j;
      char current = board[idx];
      // ignore empty cells and goal markers "*"
      if (current == '.' || current == '*')
        continue;
      // check right cell
      if (j + 1 < c) {
        int rightIdx = i * c + (j + 1);
        if (board[rightIdx] == current) {
          int root1 = ds.Find(idx);
          int root2 = ds.Find(rightIdx);
          if (root1 != root2)
            ds.Union(root1, root2);
        }
      }
      // check down cell
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
      // ignore empty
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

  // Now, according to our algorithm, if any component meets the criteria (size
  // >= mss and has a goal cell) then we want to score immediately.
  for (int i = 0; i < r * c; i++) {
    if (comp[i].size >= mss && comp[i].has_scoring) {
      // Build the move string for a SCORE move.
      oss << "SCORE " << comp[i].scoring_row << " " << comp[i].scoring_col;
      return oss.str();
    }
  }

  return end_game_move();
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
}

int main(int argc, char **argv) {
  Superball *s;

  s = new Superball(argc, argv);

  DisjointSetByRankWPC ds(s->r * s->c);
  string move = s->analyze_superball();
  cout << move << endl;
  //  ds.Print();
}
