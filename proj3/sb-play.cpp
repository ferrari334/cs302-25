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

/*
bool has_adjacent(int row, int col, char color, const vector<int> &board, int r,
                  int c) {
  // Up
  if (row > 0 && board[(row - 1) * c + col] == color)
    return true;
  // Down
  if (row < r - 1 && board[(row + 1) * c + col] == color)
    return true;
  // Left
  if (col > 0 && board[row * c + (col - 1)] == color)
    return true;
  // Right
  if (col < c - 1 && board[row * c + (col + 1)] == color)
    return true;
  return false;
}
*/

// Helper function: returns true if the cell at (row, col) is adjacent to a goal
// cell.
bool has_goal_adjacent(int row, int col, const vector<int> &goals, int r,
                       int c) {
  // Up
  if (row > 0 && goals[(row - 1) * c + col] == 1)
    return true;
  // Down
  if (row < r - 1 && goals[(row + 1) * c + col] == 1)
    return true;
  // Left
  if (col > 0 && goals[row * c + (col - 1)] == 1)
    return true;
  // Right
  if (col < c - 1 && goals[row * c + (col + 1)] == 1)
    return true;
  return false;
}

// Helper function: returns the connectivity score (number of adjacent cells
// with the same color) for the cell at (row, col) in the given board.
int connectivity(const vector<int> &board, int row, int col, int r, int c) {
  int score = 0;
  char cell = board[row * c + col];
  // Up
  if (row > 0 && board[(row - 1) * c + col] == cell)
    score++;
  // Down
  if (row < r - 1 && board[(row + 1) * c + col] == cell)
    score++;
  // Left
  if (col > 0 && board[row * c + (col - 1)] == cell)
    score++;
  // Right
  if (col < c - 1 && board[row * c + (col + 1)] == cell)
    score++;
  return score;
}

string Superball::end_game_move() {
  vector<int> valid;
  // Build list of indices that have actual colored pieces.
  for (int i = 0; i < r * c; i++) {
    if (board[i] != '.' && board[i] != '*')
      valid.push_back(i);
  }
  if (valid.size() < 2) {
    // Fallback: choose any two board indices.
    int idx1 = rand() % (r * c);
    int idx2 = rand() % (r * c);
    while (idx1 == idx2)
      idx2 = rand() % (r * c);
    int r1 = idx1 / c, c1 = idx1 % c;
    int r2 = idx2 / c, c2 = idx2 % c;
    ostringstream oss;
    oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
    return oss.str();
  }

  string bestCandidate = "";
  string oneCandidate = "";
  int bestSum = -1000;
  int secondBestSum = -1000;

  // Try up to 100 trials for a candidate swap.
  for (int trial = 0; trial < 100; trial++) {
    int idx1 = valid[rand() % valid.size()];
    int idx2 = valid[rand() % valid.size()];
    while (idx1 == idx2)
      idx2 = valid[rand() % valid.size()];
    int r1 = idx1 / c, c1 = idx1 % c;
    int r2 = idx2 / c, c2 = idx2 % c;
    // Ensure cells are not adjacent.
    if (abs(r1 - r2) + abs(c1 - c2) == 1)
      continue;
    // Ensure cells are not the same color.
    if (board[idx1] == board[idx2])
      continue;

    // make sure cells are not swapped once moved next to neighbor
    //  If either cell already has a same-colored neighbor, skip it.
    if (connectivity(board, r1, c1, r, c) > 0 ||
        connectivity(board, r2, c2, r, c) > 0)
      continue;

    // Create a temporary copy and simulate swap.
    vector<int> tempBoard = board;
    swap(tempBoard[idx1], tempBoard[idx2]);
    int newScore1 = connectivity(tempBoard, r1, c1, r, c);
    int newScore2 = connectivity(tempBoard, r2, c2, r, c);

    int candidateScore = newScore1 + newScore2;
    // Add a bonus if the new cell has an adjacent goal cell.
    if (has_goal_adjacent(r1, c1, goals, r, c))
      candidateScore += 3;
    if (has_goal_adjacent(r2, c2, goals, r, c))
      candidateScore += 3;

    // Candidate qualifies best if both cells get a positive connectivity score.
    if (newScore1 > 0 && newScore2 > 0) {
      int sum = newScore1 + newScore2;
      if (sum > bestSum) {
        bestSum = sum;
        ostringstream oss;
        oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
        bestCandidate = oss.str();
      }
    }
    // Otherwise, if at least one cell gets a positive connectivity score.
    else if ((newScore1 > 0 || newScore2 > 0)) {
      int sum = newScore1 + newScore2;
      if (sum > secondBestSum) {
        secondBestSum = sum;
        ostringstream oss;
        oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
        oneCandidate = oss.str();
      }
    }
  }

  if (!bestCandidate.empty())
    return bestCandidate;
  if (!oneCandidate.empty())
    return oneCandidate;

  // Fallback: return a random swap move (swapping two colored cells).
  int idx1 = valid[rand() % valid.size()];
  int idx2 = valid[rand() % valid.size()];
  while (idx1 == idx2)
    idx2 = valid[rand() % valid.size()];
  int r1 = idx1 / c, c1 = idx1 % c;
  int r2 = idx2 / c, c2 = idx2 % c;
  ostringstream oss;
  oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
  return oss.str();
}

/*
string Superball::end_game_move() {
  vector<int> valid;
  // Build a list of indices that contain actual colored pieces.
  for (int i = 0; i < r * c; i++) {
    if (board[i] != '.' && board[i] != '*')
      valid.push_back(i);
  }
  if (valid.size() < 2) {
    // Fallback: choose any two board indices.
    int idx1 = rand() % (r * c);
    int idx2 = rand() % (r * c);
    while (idx1 == idx2)
      idx2 = rand() % (r * c);
    int r1 = idx1 / c, c1 = idx1 % c;
    int r2 = idx2 / c, c2 = idx2 % c;
    ostringstream oss;
    oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
    return oss.str();
  }

  string bestCandidate = "";
  string oneCandidate = "";
  // Try a fixed number of trials (e.g., 100) to find a swap meeting our
  // criteria.
  for (int trial = 0; trial < 100; trial++) {
    int idx1 = valid[rand() % valid.size()];
    int idx2 = valid[rand() % valid.size()];
    while (idx1 == idx2)
      idx2 = valid[rand() % valid.size()];
    int r1 = idx1 / c, c1 = idx1 % c;
    int r2 = idx2 / c, c2 = idx2 % c;
    if (abs(r1 - r2) + abs(c1 - c2) == 1) {
      // They are adjacent; reject this candidate.
      continue; // or pick another candidate
    }
    // Ensure cells are not the same color.
    if (board[idx1] == board[idx2])
      continue;

    // Make a temporary copy of the board and simulate the swap.
    vector<int> tempBoard = board;
    swap(tempBoard[idx1], tempBoard[idx2]);
    char newColor1 = tempBoard[idx1];
    char newColor2 = tempBoard[idx2];
    bool cond1 = has_adjacent(r1, c1, newColor1, tempBoard, r, c);
    bool cond2 = has_adjacent(r2, c2, newColor2, tempBoard, r, c);
    if (cond1 && cond2) {
      ostringstream oss;
      oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
      bestCandidate = oss.str();
      break; // Found the best candidate.
    } else if ((cond1 || cond2) && oneCandidate.empty()) {
      ostringstream oss;
      oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
      oneCandidate = oss.str();
    }
  }
  if (!bestCandidate.empty())
    return bestCandidate;
  if (!oneCandidate.empty())
    return oneCandidate;
  // Fallback: return a random swap (swapping two colored cells).
  int idx1 = valid[rand() % valid.size()];
  int idx2 = valid[rand() % valid.size()];
  while (idx1 == idx2)
    idx2 = valid[rand() % valid.size()];
  int r1 = idx1 / c, c1 = idx1 % c;
  int r2 = idx2 / c, c2 = idx2 % c;
  ostringstream oss;
  oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
  return oss.str();
}
*/
//
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
/*
#include "disjoint.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// Helper function: check if the cell at (row, col) in board has an adjacent
// cell (up, down, left, or right) with the specified color.
bool has_adjacent(int row, int col, char color, const vector<int> &board, int r,
                  int c) {
  // Up
  if (row > 0 && board[(row - 1) * c + col] == color)
    return true;
  // Down
  if (row < r - 1 && board[(row + 1) * c + col] == color)
    return true;
  // Left
  if (col > 0 && board[row * c + (col - 1)] == color)
    return true;
  // Right
  if (col < c - 1 && board[row * c + (col + 1)] == color)
    return true;
  return false;
}

class Superball {
public:
  Superball(int argc, char **argv);
  int r;              // number of rows
  int c;              // number of columns
  int mss;            // minimum score size
  int empty;          // number of empty cells ('.' or '*')
  vector<int> board;  // board cells (stored as char values)
  vector<int> goals;  // 1 if cell is a goal cell; 0 otherwise.
  vector<int> colors; // mapping for valid colors (e.g., "pbyrg")

  // analyze_superball() returns a move string ("SCORE ..." or "SWAP ...")
  string analyze_superball();
  // end_game_move() returns a swap move using our new algorithm.
  string end_game_move();
  void usage(const char *msg);
};

struct metadata {
  int size;
  char color;
  int scoring_row;
  int scoring_col;
  bool has_scoring;
};

// New end_game_move() that attempts to swap two colored cells so that after the
// swap, ideally both cells are next to a cell of the same color; if not, then
// at least one; otherwise, random.
string Superball::end_game_move() {
  vector<int> valid;
  // Build a list of indices that contain actual colored pieces.
  for (int i = 0; i < r * c; i++) {
    if (board[i] != '.' && board[i] != '*')
      valid.push_back(i);
  }
  if (valid.size() < 2) {
    // Fallback: choose any two board indices.
    int idx1 = rand() % (r * c);
    int idx2 = rand() % (r * c);
    while (idx1 == idx2)
      idx2 = rand() % (r * c);
    int r1 = idx1 / c, c1 = idx1 % c;
    int r2 = idx2 / c, c2 = idx2 % c;
    ostringstream oss;
    oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
    return oss.str();
  }

  string bestCandidate = "";
  string oneCandidate = "";
  // Try a fixed number of trials (e.g., 100) to find a swap meeting our
  // criteria.
  for (int trial = 0; trial < 100; trial++) {
    int idx1 = valid[rand() % valid.size()];
    int idx2 = valid[rand() % valid.size()];
    while (idx1 == idx2)
      idx2 = valid[rand() % valid.size()];
    int r1 = idx1 / c, c1 = idx1 % c;
    int r2 = idx2 / c, c2 = idx2 % c;
    // Make a temporary copy of the board and simulate the swap.
    vector<int> tempBoard = board;
    swap(tempBoard[idx1], tempBoard[idx2]);
    char newColor1 = tempBoard[idx1];
    char newColor2 = tempBoard[idx2];
    bool cond1 = has_adjacent(r1, c1, newColor1, tempBoard, r, c);
    bool cond2 = has_adjacent(r2, c2, newColor2, tempBoard, r, c);
    if (cond1 && cond2) {
      ostringstream oss;
      oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
      bestCandidate = oss.str();
      break; // Found the best candidate.
    } else if ((cond1 || cond2) && oneCandidate.empty()) {
      ostringstream oss;
      oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
      oneCandidate = oss.str();
    }
  }
  if (!bestCandidate.empty())
    return bestCandidate;
  if (!oneCandidate.empty())
    return oneCandidate;
  // Fallback: return a random swap (swapping two colored cells).
  int idx1 = valid[rand() % valid.size()];
  int idx2 = valid[rand() % valid.size()];
  while (idx1 == idx2)
    idx2 = valid[rand() % valid.size()];
  int r1 = idx1 / c, c1 = idx1 % c;
  int r2 = idx2 / c, c2 = idx2 % c;
  ostringstream oss;
  oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
  return oss.str();
}

string Superball::analyze_superball() {
  ostringstream oss;

  // If there are fewer than 5 empty cells, we're nearly full so return a swap.
  if (empty < 5)
    return end_game_move();

  // Use disjoint sets to group adjacent cells of the same color.
  DisjointSetByRankWPC ds(r * c);
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      int idx = i * c + j;
      char current = board[idx];
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

  // Build component metadata.
  vector<metadata> comp(r * c, {0, ' ', -1, -1, false});
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      int idx = i * c + j;
      char current = board[idx];
      if (current == '.' || current == '*')
        continue;
      int root = ds.Find(idx);
      if (comp[root].size == 0) {
        comp[root].size = 1;
        comp[root].color = current;
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

  // If any component meets the scoring criteria, return a SCORE move.
  for (int i = 0; i < r * c; i++) {
    if (comp[i].size >= mss && comp[i].has_scoring) {
      oss << "SCORE " << comp[i].scoring_row << " " << comp[i].scoring_col;
      return oss.str();
    }
  }
  // Otherwise, return a swap move as chosen by our enhanced algorithm.
  return end_game_move();
}

void Superball::usage(const char *msg) {
  cerr << "usage: sb-analyze rows cols min-score-size colors" << endl;
  if (msg)
    cerr << msg << endl;
  exit(1);
}

Superball::Superball(int argc, char **argv) {
  int i, j;
  string s;
  if (argc != 5)
    usage("Incorrect number of arguments");
  if (sscanf(argv[1], "%d", &r) != 1 || r <= 0)
    usage("Bad rows");
  if (sscanf(argv[2], "%d", &c) != 1 || c <= 0)
    usage("Bad cols");
  if (sscanf(argv[3], "%d", &mss) != 1 || mss <= 0)
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
  // Expect exactly 5 arguments: rows, cols, min-score-size, colors.
  if (argc != 5) {
    cerr << "usage: sb-analyze rows cols min-score-size colors" << endl;
    exit(1);
  }
  srand(time(NULL)); // Seed random number generator.
  Superball s(argc, argv);
  string move = s.analyze_superball();
  cout << move << endl;
  return 0;
}
*/
