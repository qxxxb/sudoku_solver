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
- Create a population (set of states)
- Initialize with random states
- Create a new population (same size as parent population)
- Pick two random states from the parent population
  - Reproduce to create a child
  - Mutate child
  - Add child to new population
- Replace parent population with new population

### Reproduce
- Cross-over
  - Pick a random point
  - Flip

### Mutate
- Pick a random cell
- Mutate it
