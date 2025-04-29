# Queens solver

This is a solver that solves the Queens game on a certain business-oriented social media platform. It is going to be capable of the following things:

- Read the board from a browser window
- Find the queen positions by solving the puzzle
- Placing the queens on the browser window

The game is also called "Star Battle". You can play it on [this website](https://www.puzzle-star-battle.com).


## Types
- A `board_t` holds the pointers to the arrays of `cell_t`s and `cellSet_t`s. It also stores the size of the board (the length of a column or row (which are always equal)).
- A `cellSet_t` is a collection of cells. This is needed, because a queens board is basically just three collections of cells: the columns, rows, and groups. Here, "group" refers to a certain color on the board.
  A `cellSet_t`s cell array is an array of *pointers* to cells in the *board's* cell array. This array gets emptier over time; when the solver finds cells that cannot hold a queen, it removes that cell from all of its sets.
- A `cell_t` is literally just that: a cell. It holds an x, y, and color value. These refer to the cell's x and y coordinates, and its color (obviously). They are also indices in the column, row, and group arrays of the board the cell is in, respectively. It also holds three `cellSet_t` pointers: one for every set it is in (column, row, and group).

I made a mermaid diagram to explain this a bit better maybe. Here is an example of my notation, because if there is an official standard to this, I haven't heard of it:
```mermaid
flowchart LR
subgraph "struct type"
    field[coolType_t *field]
end
field --main pointer (has [0] as label)--> 
coolArray@{ shape: procs, label: "coolType_t array[array_size]"}
subgraph "other struct type"
    otherField[coolType_t *other_field]
end
otherField -.secondary pointer (has [index] as label).-> coolArray

coolArray -."is an instance of".-o coolType_t



subgraph "coolType_t"
    num[int number]
end
```

Ok so here's how the `board_t` structure is set up, after creating one with `createBoard()` and initializing it with `colorBoard()` (it is missing a few fields that aren't important right now):

```mermaid
flowchart LR
    subgraph cell_t
        direction BT
        x[int x]
        y[int y]
        color[int color]
        col[cellSet_t *column]
        row[cellSet_t *row]
        grp[cellSet_t *group]
    end
    subgraph board_t 
        cells[cell_t *cells]
        cols[cellSet_t *columns]
        rows[cellSet_t *rows]
        grps[cellSet_t *groups]
        size[int size]
    end

    cellPointers@{ shape: procs, label: "cell_t *cells[size]"}
    setCells --[0]--> cellPointers
    cellPointers -.[arbitrary].-> cellArray
    %% cellPointers --> cellArray

    cellArray@{ shape: procs, label: "cell_t cells[size * size]"}
    colsArray@{ shape: procs, label: "cellSet_t columns[size]"}
    rowsArray@{ shape: procs, label: "cellSet_t rows[size]"}
    grpsArray@{ shape: procs, label: "cellSet_t groups[size]"}
    
    colsArray -.-o cellSet_t
    rowsArray -.-o cellSet_t
    grpsArray -.-o cellSet_t
    cellArray -.-o cell_t

    cells --[0]--> cellArray
    
    col -.[x].-> colsArray
    row -.[y].-> rowsArray
    grp -.[color].-> grpsArray

    cols --[0]--> colsArray
    rows --[0]--> rowsArray
    grps --[0]--> grpsArray



    subgraph "cellSet_t"
        id[int identifier]
        setCells[cell_t **cells]
        cellCount[int cellCount]
        solved[bool solved]
    end


```

## Files

- [Makefile](./Makefile) is the makefile used to build the project
- [looker.c](looker.c)/[looker.h](looker.h) gets the browser window and puts it into an array. Currently only works for X11 GNU/Linux systems.
- [seeer.c](seeer.c)/[seeer.h](seeer.h) uses the array retrieved by the looker, and detects the queens board on it.
- [types.c](types.c)/[types.h](types.h) defines a `board_t` object, which holds `cell_t` and `cellSet_t` objects. These have a *lot* of pointer bs going on.
- [solver.c](solver.c)/[solver.h](solver.h) uses a `board_t` object and solves it (finds the queens).
- [main.c](main.c) is the main file. Parses arguments and runs the functions from the other files.
- [games](./games) is a folder that holds a bunch of predefined games to test the solver on.




## The solver
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
