#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
using namespace std;

/*
  sb-player.cpp
  -------------
  This is a minimal test harness that acts as the game engine.
  It accepts the following command-line arguments:
    rows cols min-score-size colors move_generator interactive output seed

  It reads a board from standard input, calls the move generator executable
  (e.g., your compiled sb-play2 program) with the board input, and prints the
  move output by the move generator. Finally, it prints a dummy final score.

  For example, you can test it like this:
    ./sb-player 8 10 5 pbyrg ./sb-play2 n y -

  Then provide the board input via standard input (or redirect from a file).
*/

int main(int argc, char **argv) {
  if (argc < 8) {
    cerr << "usage: sb-player rows cols min-score-size colors move_generator "
            "interactive output seed"
         << endl;
    return 1;
  }

  // Extract parameters.
  string rows = argv[1];
  string cols = argv[2];
  string mss = argv[3];
  string colors = argv[4];
  string moveGenPath = argv[5];
  // We ignore the interactive, output, and seed parameters in this simple test.

  // Build the command string that will run the move generator.
  // The move generator (sb-play2) expects exactly 5 arguments: rows, cols, mss,
  // colors. We assume that the board will be provided via standard input.
  ostringstream cmd;
  cmd << moveGenPath << " " << rows << " " << cols << " " << mss << " "
      << colors;

  // Open a pipe to run the move generator.
  // The board should be provided to sb-player via redirection (or typed in
  // manually).
  FILE *pipe = popen(cmd.str().c_str(), "r");
  if (!pipe) {
    cerr << "Error: cannot run move generator." << endl;
    return 1;
  }

  // Read the output from the move generator.
  char buffer[128];
  string moveOutput = "";
  while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
    moveOutput += buffer;
  }
  pclose(pipe);

  // Output the move.
  cout << "Move is: " << moveOutput << endl;

  // For testing, we print a dummy final score.
  cout << "Game over. Final score = 0" << endl;

  return 0;
}
