//
#include "disjoint.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace std;
// method for analyzing if cells around a cell are same color
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
// tests if cell is next to a goal cell
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

struct metadata {
  int size;
  char color;
  int scoring_row;
  int scoring_col;
  bool has_scoring;
};
// hold possible moves
class Choices {
public:
  int first_choice;
  int second_choice;
};

class Superball {
public:
  Superball(int argc, char **argv);
  int r, c, mss, empty;
  vector<int> board;
  vector<int> goals;
  vector<int> colors;

  vector<int> moves;      // holds good cells
  vector<int> tempCombos; // a temp vector to hold possible swaps
  map<int, vector<Choices *>>
      movemap; // ranking the possible score with possible moves. used to get
               // best next move
  map<int, int> score; // maps possible moves to where scoring cell is
  DisjointSetByRankWPC *d;

  string analyze_superball();
  string end_game_move(); // swap move selection
  void possible_moves();  // fill vector with possible moves of available cells
  void switch_choices(int index, int n); // make swaps
  void best_moves();                     // test swap and rank it
  int score_checker();                   // check if a scoring move is available
};

// adds cells that are colored pieces not dead space
void Superball::possible_moves() {
  moves.clear();
  for (int i = 0; i < (int)board.size(); i++) {
    if (board[i] != '.' && board[i] != '*') {
      moves.push_back(i);
    }
  }
}

// selects 2 cells and tests what is a better move
void Superball::switch_choices(int index, int n) {
  if (n == 0) {
    best_moves();
    return;
  }
  if (n > (int)moves.size() - index)
    return;
  tempCombos.push_back(moves[index]);
  switch_choices(index + 1, n - 1);
  tempCombos.pop_back();
  switch_choices(index + 1, n);
}

// tests a swap move and ranks what swaps are the best option
void Superball::best_moves() {
  // Get the two candidate indices.
  int idx1 = tempCombos[0];
  int idx2 = tempCombos[1];
  int r1 = idx1 / c, c1 = idx1 % c;
  int r2 = idx2 / c, c2 = idx2 % c;

  // test swap on temp board
  int oldScore1 = connectivity(board, r1, c1, r, c);
  int oldScore2 = connectivity(board, r2, c2, r, c);
  vector<int> tempBoard = board;
  swap(tempBoard[idx1], tempBoard[idx2]);
  int newScore1 = connectivity(tempBoard, r1, c1, r, c);
  int newScore2 = connectivity(tempBoard, r2, c2, r, c);

  // ranking the possible swaps, swap is better if its next to a goal cell
  int candidateScore = (newScore1 - oldScore1) + (newScore2 - oldScore2);
  // bonus for cell if adjacent to a goal cell.
  if (has_goal_adjacent(r1, c1, goals, r, c))
    candidateScore += 3;
  if (has_goal_adjacent(r2, c2, goals, r, c))
    candidateScore += 3;

  // only want moves that impove the board
  if (candidateScore <= 0)
    return;

  Choices *choice = new Choices;
  choice->first_choice = idx1;
  choice->second_choice = idx2;

  if (movemap.find(candidateScore) != movemap.end()) {
    movemap[candidateScore].push_back(choice);
  } else {
    vector<Choices *> tmp;
    tmp.push_back(choice);
    movemap.insert(make_pair(candidateScore, tmp));
  }
}

// uses disjoint set to test connected cell sizes. check if set is big enough to
// score
int Superball::score_checker() {
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

// selects swap move from test moves, if nothing then random swap
string Superball::end_game_move() {
  // find all valid moves from board
  possible_moves();
  tempCombos.clear();
  movemap.clear();
  switch_choices(0, 2);

  // If movemap is empty do a random swap
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

  // if movemap is not empty choose the move with the highest ranking
  auto it = movemap.end();
  it--;
  vector<Choices *> best = it->second;
  Choices *chosen = best[0];
  int r1 = chosen->first_choice / c, c1 = chosen->first_choice % c;
  int r2 = chosen->second_choice / c, c2 = chosen->second_choice % c;
  ostringstream oss;
  oss << "SWAP " << r1 << " " << c1 << " " << r2 << " " << c2;
  return oss.str();
}

// finds possible move
string Superball::analyze_superball() {
  ostringstream oss;

  // if the board is almost full, return a swap move
  if (empty < 5)
    return end_game_move();

  d = new DisjointSetByRankWPC(r * c);
  score.clear();

  // union left and up neighbors
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

  // build component metadata and record goal cells
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

  // if component is a scoring move, score
  for (int i = 0; i < r * c; i++) {
    if (comp[i].size >= mss && comp[i].has_scoring) {
      oss << "SCORE " << comp[i].scoring_row << " " << comp[i].scoring_col;
      return oss.str();
    }
  }

  // if its not a scoring move then swap
  return end_game_move();
}

void usage(const char *msg) {
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
  if (argc != 5) {
    cerr << "usage: sb-analyze rows cols min-score-size colors" << endl;
    exit(1);
  }
  // srand(time(NULL));
  Superball *s = new Superball(argc, argv);
  string move = s->analyze_superball();
  cout << move << endl;
  return 0;
}
