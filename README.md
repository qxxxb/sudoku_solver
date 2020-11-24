# Sudoku solver

## Hill-climbing algorithm
- Generate random initial state
- Generate successors
- Pick best successor
- If we're at a local min then start over with a new random state
- We're done when we have solved the board

Pseudocode from textbook:
```
function HillClimbing(problem) returns a state that is a local maximum
  current ← MakeNode(problem.initial_state)
  loop do
    neighbor ← a highest-valued successor of current
    if neighbor.value <= current.value then return current.state
current ← neighbor
```

### Problem representation
Flat vector of fixed numbers

### State representation
Flat vector of numbers

### Successor function
All possible states by changing any single number on the board

### Evaluation function
Total number of conflicts on the board

### Misc
At each iteration, print state, score, iteration number

## Genetic algorithm
Who knows

print *(ans.data._M_impl._M_start)@ans.data.size()
