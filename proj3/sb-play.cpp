#include "disjoint.h"
#include <iostream>

using namespace std;

int main() {
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
  cout << "This program doesn't do anything yet.\n";
  return 0;
}
