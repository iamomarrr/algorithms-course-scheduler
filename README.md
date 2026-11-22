# Algorithms Assignment 1

## Team members

- Omar Kadan
- Basil Haj Yahia

## Description

A C++ program that solves three course scheduling problems:

- COUNT: maximize the number of nonoverlapping courses in one room.
- ROOMS: schedule every course using the minimum number of rooms.
- ENROLLMENTS: maximize total enrollments in one room.

Times are integer minutes from the beginning of the day.

## Files

- Course.h: course data structure.
- Algorithms.h: required function declarations.
- Algorithms.cpp: merge sort, scheduling algorithms, and exhaustive checkers.
- main.cpp: has our entry point to input and output.
- tests.cpp: doctest test cases and validation helpers.
- doctest.h: doctest version 2.4.11
- testdata/: the three supplied input/output examples.

## Build and run

And this is how to compile and run the code :

```cmd
g++ -Wall -Wextra main.cpp Algorithms.cpp -o courses.exe
g++ -Wall -Wextra tests.cpp Algorithms.cpp -o tests.exe
tests.exe
courses.exe < testdata\sample.in
courses.exe < testdata\empty.in
courses.exe < testdata\ties.in
courses.exe < testdata\sample.in > actual.out
```

The program reads standard input and writes standard output.
The .out files contain the expected outputs.

## Compiler

here is what i used : g++ 15.2.0, MSYS2.

## Tests

There are 13 doctest test cases, including 120 deterministically
generated inputs containing zero to five courses.

Generated start times range from 0 to 70 minutes.
Durations range from 10 to 40 minutes.
Enrollment counts range from 1 to 30.
Generation uses fixed arithmetic formulas, so no random seed is needed.

For inputs of up to five courses, results are compared with
independent exhaustive solutions.

Tests also check sorting, preservation of course fields, unchanged
inputs, absence of overlaps and duplicate identifiers, room coverage,
tie handling, large enrollment totals, and the predecessor regression.

An eight-course case checks feasibility and known results without
running exhaustive search.
