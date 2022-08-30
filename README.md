# NonogramSolver
A package built to solve nonograms
How this works:
  This works using the idea that the basic probability of a space being filled (just using the blocks given) is equal to total_block_len * (dist_to_empty_square) / len

the basic board is constructed by barriers surrounding the actual length (I.e.):

XXXXXXX
X00000X
X00000X
X00000X
X00000X
X00000X
XXXXXXX
(the reason will be given later)

Inside the array, these are actually given by -1 (EMPTY), and Filled in squares are given by -2 (FILLED)

So, we have 2 verifiable rules for nonogram that we can use to have a computer solve a nonogram.
1. The probability that a square is filled is given by block_len(dist_to_empty_square) / len
2. If the sum of all spaces in a row / column equals the sum of all blocks in that row / column, then all other spaces in that range must be empty
3. No two blocks can touch, so each must have an empty space at the end.


Important things to look for:
 1. mark spaces empty when they can't be reached
 2. Mark end caps
 3. if empty + sum_blocks == len fill in all remaining spaces as full.
    if sum_full_spaces == sum_blocks fill in all remaining spaces as empty.
 4. if a block is known to be in two spaces that aren't next to each other, all middle spaces must be filled.

Using those rules, we can construct a loop for a computer to follow to solve a nonogram:

FIRST RUN:
1. Check for full rows (total_block_len = sum_blocks + num_blocks - 1)
2. General possibility run (don't devide by len to conserve computing power)

LATER RUNS:
1. For each block:
  a. Itteritivly check that for a spot it can fit in. Add 1 to each of their numerators.
     Also Set their denominators to the current length.
  b. if there is a filled space (without end caps and isn't the proper length), start a special calculation:
      number of possibilities = dist to closest empty space + 1. subtract that from the block length and fill in that many blocks in the opposite direction.
  c. if an empty space is found that the block can't enter within the region that encloses the block's space between where it started and where it has to be, fill in said space.
  d. if that block is found to be full, add end caps, and end calcualtions for that block.

Most effecient way to fill blocks:

i 0 -> blocks[0]
| empty -> move on
| full -> constrain length
| other -> add 1
| i > length - total_length -> make full and constrain
j blocks[0] -> len
| empty -> constrain
