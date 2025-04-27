# Queens solver


Still only uses the following techniques:
- Check if a set has only 1 cell. Mark it as queen if it does.
- When a cell is marked as queen, the cells in its sets are crossed, as well as the cells adjacent to the queen.
- Check, for every cell, if it would completely block a set if it were a queen.


For now, it has a lot of difficulty with [game 333](games/game333.txt) (me too).


## Implemented rules
- Rule of 1 celled set = Queen
- Rule of cell blocking a set = not a Queen

## Rules to be implemented
- If n columns or rows contain cells in exactly n groups,
  the queens of said groups can only land within these columns/rows.
- If n groups have cells within exactly n columns or rows,
  the queens of said columns/rows can only land within said groups.

- Rule of only 1 solution. (Not sure if I will add this one. It's basically, if there is the *possibility* of a stalemate that has only 1 resolution, that resolution is correct.)



## Unaccounted for situations
This situation is unaccounted for:

   0 1 2 3 4
0  . A A . .
1  B . . . .
2  B . C C .
3  . . C C .
4  . . . . .

The top-right cell of C blocks B[0, 2] and A[2, 0]. This results in:

   0 1 2 3 4
0  . A x . .
1  B . . . .
2  x . Q x .
3  . . x x .
4  . . . . .

Which is an impossible situation. Thus, C[2, 2] should be crossed off.
