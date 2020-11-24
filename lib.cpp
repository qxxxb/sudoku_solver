#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_set>
#include <limits.h>
#include "lib.h"

size_t Index(size_t row, size_t col, size_t n) {
    return (row * n) + col;
}

void PrintBoard(std::vector<int> board, size_t n) {
    for (size_t row = 0; row < n; ++row) {
        for (size_t col = 0; col < n; ++col) {
            std::cout << board[(row * n) + col] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

Problem::Problem(std::string filename) {
    std::ifstream f(filename);
    if (!f.good()) {
        throw std::invalid_argument("Couldn't open file `" + filename + "`");
    }

    std::string line;
    getline(f, line);

    size_t n = std::stoi(line);
    this->n = n;
    this->fixed = std::vector<int>(n * n);
    this->rand_dist = std::uniform_int_distribution<int>(1, n);

    for (size_t row = 0; row < n; ++row) {
        getline(f, line);
        for (size_t col = 0; col < line.length(); ++col) {
            char c = line[col];
            if (c != '*') {
                this->fixed[Index(row, col, n)] = c - '0';
            }
        }
    }
}

State::State(Problem* problem) : problem(problem) {
    data = problem->fixed;
}

void State::Print() {
    PrintBoard(data, problem->n);
}

StateIter::StateIter(State* state) : state(state) {
    i = 0;
    cell_value = 1;
}

bool StateIter::FixParams(size_t& i, size_t& cell_value) {
    size_t n = state->problem->n;
    size_t n_cells = state->data.size();

    if (i >= n_cells) return false;
    while (state->problem->IsFixed(i)) {
        i++;
        if (i >= n_cells) return false;
    }

    // Pick the first tweaked cell value
    if (cell_value == (size_t)state->data[i]) {
        // Unchanged from original value, use next value
        cell_value++;
    }

    if (cell_value > n) {
        cell_value = 1;
        i++;
        return FixParams(i, cell_value);
    }

    return true;
}

tl::optional<State> StateIter::Successor() {
    if (!FixParams(i, cell_value)) return tl::nullopt;
    State ans = *state;
    ans.data[i] = cell_value;
    cell_value++;
    return tl::make_optional(ans);
}

std::random_device rand_dev;
std::mt19937 rand_gen(rand_dev());

State Problem::RandomState() {
    State ans(this);
    for (size_t i = 0; i < fixed.size(); ++i) {
        if (!IsFixed(i)) {
            ans.data[i] = rand_dist(rand_gen);
        }
    }
    return ans;
}

int State::CountConflicts() {
    if (eval) return *eval;

    std::unordered_set<int> h;
    size_t n = problem->n;
    int ans = 0;

    for (size_t row = 0; row < n; ++row) {
        for (size_t col = 0; col < n; ++col) {
            int x = data[Index(row, col, n)];
            if (h.count(x) > 0) ans++;
            h.insert(x);
        }
        h.clear();
    }

    for (size_t col = 0; col < n; ++col) {
        for (size_t row = 0; row < n; ++row) {
            int x = data[Index(row, col, n)];
            if (h.count(x) > 0) ans++;
            h.insert(x);
        }
        h.clear();
    }

    size_t m = (int)sqrt(n);
    for (size_t sq = 0; sq < n; ++sq) {
        size_t row_start = (sq / m) * m;
        size_t col_start = (sq % m) * m;
        for (size_t sq_row = 0; sq_row < m; ++sq_row) {
            for (size_t sq_col = 0; sq_col < m; ++sq_col) {
                int row = row_start + sq_row;
                int col = col_start + sq_col;
                int x = data[Index(row, col, n)];
                if (h.count(x) > 0) ans++;
                h.insert(x);
            }
        }
        h.clear();
    }

    *eval = ans;
    return ans;
}

int State::Eval() {
    return CountConflicts();
}

bool State::IsGoal() {
    return CountConflicts() == 0;
}

State Problem::HillClimber(State state) {
    state = this->RandomState();
    int i = 0;
    while (true) {
        for (int x : state.data) {
            std::cout << x << " ";
        }
        std::cout <<
            std::right << std::setw(3) << state.Eval() << " / "
            << i << std::endl;

        auto iter = StateIter(&state);

        State best_succ(this);
        best_succ.eval = INT_MAX; // TODO: hacky

        while (true) {
            auto succ_opt = iter.Successor();
            if (!succ_opt) break;
            auto succ = *succ_opt;
            if (succ.Eval() < best_succ.Eval()) {
                best_succ = succ;
            }
        }

        if (state.IsGoal()) {
            return state;
        }

        if (best_succ.Eval() < state.Eval()) {
            state = best_succ;
        } else {
            // Local min, restart at a random state
            state = this->RandomState();
        }

        i++;
    }

    return state;
}
