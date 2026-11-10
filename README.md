# Algorithm Battle: Employee ID Search

## Part A: Problem
Companies look up employees by ID all the time (HR, payroll, attendance),
so the search must be fast. This project compares three structures:
Binary Search Tree (BST), AVL Tree and Hash Table (linear probing, size 53).

## How to run
gcc algorithm_battle.c -o battle
./battle > output.txt
./battle 1234

## Part D: Comparison (hiring-order dataset, 40 records)

| Parameter | BST | AVL | Hashing |
|---|---|---|---|
| Successful search (avg comparisons) | 22.25 | 4.75 | 1.50 |
| Unsuccessful search (avg comparisons) | 21.50 | 5.625 | 3.75 |
| Insertion, 40 records (total) | 780 | 177 | 61 |
| Insert one new key (1400) | 40 | 6 | 1 probe |
| Height | 40 | 6 | - |
| Rotations / collisions | - | 34 rotations | 21 collisions |

Shuffled-order run: BST successful 5.75, height 9; AVL successful 5.125, height 7; Hashing successful 1.75.

## Part E: Conclusion
Hashing is the best for Employee ID search. It needed only 1.50 comparisons
on average, against 4.75 for AVL and 22.25 for BST. The BST became a linked
list (height 40) because IDs arrive in ascending order. AVL fixed this with
rotations and kept height 6. Hashing's weaknesses are slower unsuccessful
searches (up to 9 probes, from 21 collisions) and no sorted order, but this
application only needs exact lookup.
