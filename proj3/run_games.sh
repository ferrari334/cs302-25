#!/bin/bash
# run_games.sh
# This script runs 100 games using sb-player.
# Each game gets a unique random seed, and we extract the final score
# from the output and compute an average.

NUM_GAMES=100
total=0

for i in $(seq 1 $NUM_GAMES); do
    # Generate a random seed. One simple way is to use $RANDOM.
    seed=$RANDOM

    # Run the game. Adjust the arguments as needed.
    # For example, assuming:
    #   ./sb-player <rows> <cols> <min-score-size> <colors> <path-to-sb-play> <interactive> <output> <seed>
    output=$(./sb-player 8 10 5 pbyrg ./sb-play n n $seed)

    # Assume the output contains a line like "Game over. Final score = 505"
    # Extract the score using grep with a Perl regex.
    score=$(echo "$output" | grep -oP 'Final score = \K\d+')
    echo "Game $i: Score = $score"
    total=$(( total + score ))
done

# Calculate the average (using bc for floating-point division).
avg=$(echo "scale=2; $total / $NUM_GAMES" | bc)
echo "Average score over $NUM_GAMES games: $avg"

