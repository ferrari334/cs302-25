//
//
#include "disjoint.h" // This header comes from disjoint-rank.cpp
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace std;

// Simple Moves class to hold a swap move (pair of indices)
class Moves {
public:
  int first;
  int second;
};

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

// Helper function: returns true if the cell at (row, col) is adjacent to a goal
// cell.
bool has_goal_adjacent(int row, int col, const vector<int> &goals, int r,
                       int c) {
  if (row > 0 && goals[(row - 1) * c + col] == 1)
    return true;
  if (row < r - 1 && goals[(row + 1) * c + col] == 1)
    return true;
  if (col > 0 && goals[row * c + (col - 1)] == 1)
    return true;
  if (col < c - 1 && goals[row * c + (col + 1)] == 1)
    return true;
  return false;
}

class Superball {
public:
  // Constructor: reads command-line arguments and board from standard input.
  Superball(int argc, char **argv);
  int r, c, mss, empty;
  vector<int> board;  // Board cells (each stored as a char value)
  vector<int> goals;  // 1 if cell is a goal cell; 0 otherwise.
  vector<int> colors; // Color mapping (e.g., "pbyrg")

  // New members for move generation:
  vector<int> moves;  // Indices of valid colored cells for swapping.
  vector<int> pmoves; // Temporary vector for generating combinations.
  map<int, vector<Moves *>>
      movemap; // Map from candidate score to candidate moves.
  map<int, int>
      score; // Map from a component representative to a scoring cell index.
  DisjointSetByRankWPC
      *d; // Pointer to our disjoint set (from disjoint-rank.cpp)

  // Functions used for move selection:
  string
  analyze_superball();    // Returns a move string ("SCORE ..." or "SWAP ...")
  string end_game_move(); // Returns a swap move based on candidate evaluation.
  void gen_moves();       // Populate the moves vector from board cells.
  void nchoosek(int index, int n); // Generate combinations of 2 moves.
  void move_ranker();              // Simulate a swap and rank it.
  int score1(); // Check for a scoring move; return cell index or -1.
  void usage(const char *msg);
};

struct metadata {
  int size;
  char color;
  int scoring_row;
  int scoring_col;
  bool has_scoring;
};

// ----------------- Helper Functions for Move Generation -----------------

// Populate the moves vector with indices of cells that contain colored pieces.
void Superball::gen_moves() {
  moves.clear();
  for (int i = 0; i < (int)board.size(); i++) {
    if (board[i] != '.' && board[i] != '*') {
      moves.push_back(i);
    }
  }
}

// Recursively generate all 2-combinations from moves; for each combination,
// call move_ranker().
void Superball::nchoosek(int index, int n) {
  if (n == 0) {
    move_ranker();
    return;
  }
  if (n > (int)moves.size() - index)
    return;
  pmoves.push_back(moves[index]);
  nchoosek(index + 1, n - 1);
  pmoves.pop_back();
  nchoosek(index + 1, n);
}

// Simulate a swap of the two indices in pmoves, compute connectivity
// improvement, and rank the candidate.
void Superball::move_ranker() {
  int idx1 = pmoves[0];
  int idx2 = pmoves[1];
  int r1 = idx1 / c, c1 = idx1 % c;
  int r2 = idx2 / c, c2 = idx2 % c;

  // Compute original connectivity scores.
  int oldScore1 = connectivity(board, r1, c1, r, c);
  int oldScore2 = connectivity(board, r2, c2, r, c);

  // Simulate the swap on a temporary board.
  vector<int> tempBoard = board;
  swap(tempBoard[idx1], tempBoard[idx2]);
  int newScore1 = connectivity(tempBoard, r1, c1, r, c);
  int newScore2 = connectivity(tempBoard, r2, c2, r, c);

  // Candidate score: connectivity improvement plus bonus if adjacent to a goal.
  int candidateScore = (newScore1 - oldScore1) + (newScore2 - oldScore2);
  if (has_goal_adjacent(r1, c1, goals, r, c))
    candidateScore += 3;
  if (has_goal_adjacent(r2, c2, goals, r, c))
    candidateScore += 3;

  // Only record moves that yield a positive improvement.
  if (candidateScore <= 0)
    return;

  Moves *m = new Moves;
  m->first = idx1;
  m->second = idx2;
  if (movemap.find(candidateScore) != movemap.end()) {
    movemap[candidateScore].push_back(m);
  } else {
    vector<Moves *> tmp;
    tmp.push_back(m);
    movemap.insert(make_pair(candidateScore, tmp));
  }
}

// ----------------- End Game Move: Candidate Swap Selection -----------------

string Superball::end_game_move() {
  gen_moves();
  pmoves.clear();
  movemap.clear();
  nchoosek(0, 2);

  // If no candidate moves were generated, fallback to a random swap.
  if (movemap.empty()) {
    int idx1 = moves[rand() % moves.size()];
    int idx2 = moves[rand() % moves.size()];
    while (idx1 == idx2)
      idx2 = moves[rand() % moves.size()];
    int r1 = idx1 / c, c1 = idx1 % c;
    int r2 = idx2 / c, c2 = idx2 % c;
    ostringstream oss;
    oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
    return oss.str();
  }

  // Otherwise, select the candidate move with the highest score.
  auto it = movemap.end();
  it--;
  vector<Moves *> best = it->second;
  Moves *chosen = best[0];
  int r1 = chosen->first / c, c1 = chosen->first % c;
  int r2 = chosen->second / c, c2 = chosen->second % c;
  ostringstream oss;
  oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
  return oss.str();
}

// ----------------- Scoring Move Evaluation -----------------

// score1() uses our disjoint-set to compute connected component sizes and
// checks if any component is large enough and contains a goal cell.
// If yes, returns the index of a scoring cell; otherwise, returns -1.
int Superball::score1() {
  int total = r * c;
  vector<int> sizes(total, 0);
  for (int i = 0; i < total; i++) {
    int root = d->Find(i);
    sizes[root]++;
  }
  map<int, int> scoreboard;
  for (auto it = score.begin(); it != score.end(); it++) {
    if (sizes[it->first] >= mss && board[it->second] != '*') {
      scoreboard.insert(make_pair(sizes[it->first], it->second));
    }
  }
  if (scoreboard.empty())
    return -1;
  else {
    auto it = scoreboard.end();
    it--;
    return it->second;
  }
}

// ----------------- Board Analysis -----------------

// analyze_superball() performs union–find on the board (using left and up
// neighbors) to group connected same-colored cells, then builds component
// metadata. If a component qualifies (size >= mss and contains a goal cell), it
// returns a SCORE move; otherwise, it returns a swap move from end_game_move().
string Superball::analyze_superball() {
  ostringstream oss;

  // If nearly full (fewer than 5 empty cells), return a swap move.
  if (empty < 5)
    return end_game_move();

  // Create a new disjoint set using DisjointSetByRankWPC.
  d = new DisjointSetByRankWPC(r * c);
  score.clear();

  // Perform union operations using left and up neighbors.
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      int idx = i * c + j;
      int left = j - 1;
      int up = i - 1;
      if (left >= 0) {
        if ((board[idx] == board[i * c + left]) && (board[idx] != '*') &&
            (board[idx] != '.')) {
          if (d->Find(idx) != d->Find(i * c + left))
            d->Union(d->Find(idx), d->Find(i * c + left));
        }
      }
      if (up >= 0) {
        if ((board[idx] == board[up * c + j]) && (board[idx] != '*') &&
            (board[idx] != '.')) {
          if (d->Find(idx) != d->Find(up * c + j))
            d->Union(d->Find(idx), d->Find(up * c + j));
        }
      }
    }
  }

  // Build component metadata and score map using goal cells.
  vector<metadata> comp(r * c, {0, ' ', -1, -1, false});
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < c; j++) {
      int idx = i * c + j;
      char current = board[idx];
      if (current == '.' || current == '*')
        continue;
      int root = d->Find(idx);
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

  // If any component qualifies as a scoring move, return a SCORE move.
  for (int i = 0; i < r * c; i++) {
    if (comp[i].size >= mss && comp[i].has_scoring) {
      oss << "SCORE " << comp[i].scoring_row << " " << comp[i].scoring_col;
      return oss.str();
    }
  }

  // Otherwise, return a swap move from our candidate generation.
  return end_game_move();
}

// ----------------- Usage Function -----------------

void Superball::usage(const char *msg) {
  cerr << "usage: sb-analyze rows cols min-score-size colors" << endl;
  if (msg)
    cerr << msg << endl;
  exit(1);
}

// ----------------- Constructor -----------------

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

// ----------------- Main -----------------

int main(int argc, char **argv) {
  if (argc != 5) {
    cerr << "usage: sb-analyze rows cols min-score-size colors" << endl;
    exit(1);
  }
  srand(time(NULL));
  Superball *s = new Superball(argc, argv);
  string move = s->analyze_superball();
  cout << move << endl;
  return 0;
}
