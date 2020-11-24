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
./TestHarness tests/sample
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
