#include "disjoint.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>
using namespace std;

/*
 * outputs single move that would be accepted as input for sb-player
    - swap or score
    - looks at state of the board
    - fewer than 5 empty slots, game is lost
    - analyze board
    - pick move (swap or score)
    - print move
    - create algorithm
    - augmenting superball class?
  * have to average over 100 points on 100 runs for full credit
- Rows(int): the number of rows on the game board
- cols(int): the number of columns on the game board
- min-score-size (int): the number of contiguous cells that must be touching to
score them
- colors (string of chars -lowercase): this must be a string of distinct
lower-case letters that represent the possible colors a cell can have. The point
value of the first is 2 and goes up by 1 for each new letter in the string.
- player (char): if not interactive then your executable '-'
- interactive: (y/n) if you want to hit enter after every move
- output (char or int): (y/n) if you want to see the board state and the moves
you make
- seed (int): numeric starting seed. input ex '-'

 */

// Superball class: reads board input and holds board state.
class Superball {
public:
  int r, c, mss, empty;
  vector<int> board;  // Each cell stored as its character value.
  vector<int> goals;  // 1 if cell is a goal cell; 0 otherwise.
  vector<int> colors; // Mapping for valid colors (e.g., 'pbyrg').

  // Constructor parses first 4 arguments and then reads the board.
  Superball(int argc, char **argv) {
    if (argc < 5) {
      fprintf(stderr, "usage: sb-play rows cols min-score-size colors player "
                      "interactive output seed\n");
      exit(1);
    }
    if (sscanf(argv[1], "%d", &r) == 0 || r <= 0)
      exit(1);
    if (sscanf(argv[2], "%d", &c) == 0 || c <= 0)
      exit(1);
    if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0)
      exit(1);

    colors.resize(256, 0);
    for (int i = 0; i < (int)strlen(argv[4]); i++) {
      char ch = argv[4][i];
      if (!isalpha(ch) || !islower(ch) || colors[ch] != 0) {
        fprintf(stderr, "Invalid colors specification.\n");
        exit(1);
      }
      colors[ch] = 2 + i;
      colors[toupper(ch)] = 2 + i;
    }

    board.resize(r * c);
    goals.resize(r * c, 0);
    empty = 0;
    string line;
    for (int i = 0; i < r; i++) {
      if (!(cin >> line)) {
        fprintf(stderr, "Not enough rows on input.\n");
        exit(1);
      }
      if ((int)line.size() != c) {
        fprintf(stderr, "Row %d has wrong number of columns.\n", i);
        exit(1);
      }
      for (int j = 0; j < c; j++) {
        char ch = line[j];
        if (ch != '*' && ch != '.' && colors[ch] == 0) {
          fprintf(stderr, "Bad character %c on row %d.\n", ch, i);
          exit(1);
        }
        board[i * c + j] = ch;
        if (ch == '.' || ch == '*')
          empty++;
        // Mark goal cells (either '*' or uppercase letter).
        if (isupper(ch) || ch == '*') {
          goals[i * c + j] = 1;
          board[i * c + j] = tolower(ch);
        }
      }
    }
  }

  // Use disjoint sets to find a scoring set.
  // Returns true if a component meeting criteria (size >= mss and has at
  // least one goal cell) is found. Outputs one goal cell from that component
  // in score_row and score_col.
  bool find_scoring_set(int &score_row, int &score_col) {
    DisjointSetByRankWPC ds(r * c);
    // Process adjacent cells (only right and down, since unions are
    // symmetric).
    for (int i = 0; i < r; i++) {
      for (int j = 0; j < c; j++) {
        int idx = i * c + j;
        char cur = board[idx];
        if (cur == '.' || cur == '*')
          continue;
        // Check right neighbor.
        if (j + 1 < c) {
          int rightIdx = i * c + (j + 1);
          if (board[rightIdx] == cur) {
            int root1 = ds.Find(idx);
            int root2 = ds.Find(rightIdx);
            if (root1 != root2)
              ds.Union(root1, root2);
          }
        }
        // Check down neighbor.
        if (i + 1 < r) {
          int downIdx = (i + 1) * c + j;
          if (board[downIdx] == cur) {
            int root1 = ds.Find(idx);
            int root2 = ds.Find(downIdx);
            if (root1 != root2)
              ds.Union(root1, root2);
          }
        }
      }
    }

    // Create a component summary.
    struct Component {
      int size;
      char color;
      bool has_goal;
      int goal_row;
      int goal_col;
    };
    vector<Component> comp(r * c, {0, ' ', false, -1, -1});

    for (int i = 0; i < r; i++) {
      for (int j = 0; j < c; j++) {
        int idx = i * c + j;
        char cur = board[idx];
        if (cur == '.')
          continue;
        int root = ds.Find(idx);
        if (comp[root].size == 0) {
          comp[root].size = 1;
          comp[root].color = cur;
          if (goals[idx] == 1) {
            comp[root].has_goal = true;
            comp[root].goal_row = i;
            comp[root].goal_col = j;
          }
        } else {
          comp[root].size++;
          if (!comp[root].has_goal && goals[idx] == 1) {
            comp[root].has_goal = true;
            comp[root].goal_row = i;
            comp[root].goal_col = j;
          }
        }
      }
    }

    // Look for a component that can be scored.
    for (int i = 0; i < r * c; i++) {
      if (comp[i].size >= mss && comp[i].has_goal) {
        score_row = comp[i].goal_row;
        score_col = comp[i].goal_col;
        return true;
      }
    }
    return false;
  }
};

int main(int argc, char **argv) {
  // Expect at least 8 arguments:
  // rows, cols, min-score-size, colors, player, interactive, output, seed
  if (argc < 8) {
    // cerr
    fprintf(stderr, "usage: sb-play rows cols min-score-size colors player "
                    "interactive output seed\n");
    exit(1);
  }

  // Seed random number generator.
  if (argc >= 9) {
    if (strcmp(argv[8], "-") == 0)
      srand(time(NULL));
    else
      srand(atoi(argv[8]));
  } else {
    srand(time(NULL));
  }

  // Construct Superball to read the board.
  Superball s(argc, argv);

  // Decision strategy:
  // If there are at least 6 empty cells, simply choose a
  // random swap and print that SWAP, else choose a
  // color with the most connections to a
  // score and output a SCORE move
  if (s.empty >= 6) {
    // Random swap move.
    int r1 = rand() % s.r;
    int c1 = rand() % s.c;
    int r2 = rand() % s.r;
    int c2 = rand() % s.c;
    printf("SWAP %d %d %d %d\n", r1, c1, r2, c2);
  } else {
    int score_r, score_c;
    if (s.find_scoring_set(score_r, score_c)) {
      // Output a SCORE move on a cell within a scoring set.
      printf("SCORE %d %d\n", score_r, score_c);
    } else {
      // Fallback: perform a swap move so the game can end.
      int r1 = rand() % s.r;
      int c1 = rand() % s.c;
      int r2 = rand() % s.r;
      int c2 = rand() % s.c;
      printf("SWAP %d %d %d %d\n", r1, c1, r2, c2);
    }
  }
  return 0;
}
