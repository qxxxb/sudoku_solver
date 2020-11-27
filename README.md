# Sudoku solver

## Hill-climbing algorithm

- Solves 4x4 very quickly
- Can't solve 9x9 within a feasible time

### Building

```
make -j4
```

### Usage

```
./TestHarness tests/sample4
```

Output format is `<state> <eval> / <iter>`

### Testing

#### TestSuccessor
```
./TestSuccessor
```
- Test `StateIter::Successor()`
- Verifies that it generates all successors without duplicates
- Testing strategy:
  - Calculate the expected number of successors: `n_blanks * (n - 1)`
  - Use an `unordered_set` to ensure that there are no duplicates
  - Check that the size of the set is equal to the expected number of
    successors

#### TestEval
```
./TestEval
```
- Test `State::Eval()`
- Verify that the number of conflicts are being counted properly
- Testing strategy:
  - Manually count conflicts for different boards and compare with `Eval()`

---

## Genetic algorithm
- Solves 4x4 slower than hill-climbing algorithm
- Managed to solve 9x9 twice within 6 minutes, then never again

### Usage
```
./TestHarnessGenetic <file> <population_size> <mutate_prob> <terminate_streak> <terminate_epsilon> <crossover_type> <n_threads>
```

Arguments:
- `mutate_prob`
  - Probability of mutating any given child
- `terminate_streak`
  - If `terminate_streak` number of iterations have been within
    `terminate_epsilon` of the one before it, then stop
- `crossover_type`
  - `0`: 1-point crossover
  - `1`: N-point crossover, where N is the number of rows on the board.
  - `2`: Uniform crossover

For each population, the best state is selected and is printed like so:
`<state> <eval> / <goal_eval> / <streak> / <iter>`

If the algorithm fails, try running it again or tweaking the parameters.

#### 4-Sudoku

1-point crossover only works well with relatively high mutation rate.
```
./TestHarnessGenetic tests/sample4 200 0.1 200 0 0 1
```

N-point srossover only works well with a high mutation rate, but still takes
longer than 1-point and uniform crossover.
```
./TestHarnessGenetic tests/sample4 200 0.25 200 0 1 1
```

Uniform crossover only works well with relatively high mutation rate.
```
./TestHarnessGenetic tests/sample4 200 0.1 200 0 2 1
```

#### 9-Sudoku

Note: this probably will not find the solution, but may find something close.
```
./TestHarnessGenetic tests/sample9 1024 0.01 256 0 0 4
```
