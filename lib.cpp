#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_set>
#include <algorithm>
#include <thread>
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

    size_t n = std::stoul(line);
    this->n = n;
    this->fixed = std::vector<int>(n * n);
    this->cell_value_dist = std::uniform_int_distribution<int>(1, n);
    this->n_fixed = 0;

    for (size_t row = 0; row < n; ++row) {
        getline(f, line);
        for (size_t col = 0; col < line.length(); ++col) {
            char c = line[col];
            if (c != '*') {
                this->fixed[Index(row, col, n)] = c - '0';
                this->n_fixed++;
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
    rand_gen.seed(rand_dev());
    State ans(this);
    for (size_t i = 0; i < fixed.size(); ++i) {
        if (!IsFixed(i)) {
            ans.data[i] = cell_value_dist(rand_gen);
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

State Problem::OnePointCrossover(State p1, State p2) {
    rand_gen.seed(rand_dev());
    auto crossover_dist = std::uniform_int_distribution<size_t>(
        0, p1.data.size() - 1
    );
    auto crossover_rand = std::bind(crossover_dist, rand_gen);
    size_t crossover_point = crossover_rand();

    State child = p1;
    for (size_t i = crossover_point; i < p2.data.size(); ++i) {
        child.data[i] = p2.data[i];
    }

    return child;
}

State Problem::NPointCrossover(State p1, State p2) {
    std::vector<size_t> crossover_rand(p1.data.size());
    for (size_t i = 0; i < crossover_rand.size(); ++i) {
        crossover_rand[i] = i;
    }
    std::random_shuffle(crossover_rand.begin(), crossover_rand.end());

    size_t n_crossovers = p1.problem->n;
    std::vector<size_t> crossovers(
        crossover_rand.begin(),
        crossover_rand.begin() + n_crossovers
    );
    std::sort(crossovers.begin(), crossovers.end());

    State child = p1;

    for (size_t i = 1; i < crossovers.size(); i += 2) {
        size_t start = crossovers[i]; // Inclusive
        size_t end; // Exclusive
        if (i == crossovers.size() - 1) {
            // Last interval, crossover until the end
            end = p1.data.size();
        } else {
            end = crossovers[i + 1];
        }

        for (size_t j = start; j < end; ++j) {
            child.data[i] = p2.data[i];
        }
    }

    return child;
}

State Problem::UniformCrossover(State p1, State p2) {
    rand_gen.seed(rand_dev());
    std::uniform_int_distribution<int> uniform_dist(0, 1);
    auto uniform_rand = std::bind(uniform_dist, rand_gen);

    State child = p1;
    for (size_t i = 0; i < child.data.size(); ++i) {
        if (uniform_rand()) {
            child.data[i] = p2.data[i];
        }
    }

    return child;
}

State Problem::Reproduce(State p1, State p2, CrossoverType type) {
    switch (type) {
        case CrossoverType::OnePoint:
            return OnePointCrossover(p1, p2);
        case CrossoverType::NPoint:
            return NPointCrossover(p1, p2);
        case CrossoverType::Uniform:
            return UniformCrossover(p1, p2);
        default:
            throw std::invalid_argument("Invalid crossover type");
    }
}

void Problem::Mutate(State& s) {
    rand_gen.seed(rand_dev());

    auto mutation_dist = std::uniform_int_distribution<size_t>(
        0, s.data.size() - 1
    );
    auto mutation_rand = std::bind(mutation_dist, rand_gen);

    size_t point1 = mutation_rand();
    while (IsFixed(point1)) {
        point1 = mutation_rand();
    }

    size_t point2 = mutation_rand();
    while (IsFixed(point2) || point1 == point2) {
        point2 = mutation_rand();
    }

    std::swap(s.data[point1], s.data[point2]);
}

void Problem::ReproduceChunk(
    std::vector<State>& population,
    std::vector<State>& children,
    std::discrete_distribution<int>& parent_dist,
    size_t start, size_t end,
    double mutate_prob,
    CrossoverType type
) {
    rand_gen.seed(rand_dev());

    std::uniform_real_distribution<double> mutation_dist(0.0, 1.0);
    auto mutation_rand = std::bind(mutation_dist, rand_gen);
    auto parent_rand = std::bind(parent_dist, rand_gen);

    for (size_t i = start; i < end; ++i) {
        State parent1 = population[parent_rand()];
        State parent2 = population[parent_rand()];
        State child = Reproduce(parent1, parent2, type);
        if (mutation_rand() < mutate_prob) {
            Mutate(child);
        }
        children[i] = child;
    }
}

void Problem::EvalGeneticChunk(
    std::vector<State>& population,
    std::vector<int>& parent_probs,
    size_t start, size_t end
) {
    for (size_t i = start; i < end; ++i) {
        parent_probs[i] = EvalGenetic(population[i]);
    }
}

std::tuple<bool, State> Problem::Genetic(
    size_t size,
    double mutate_prob,
    size_t terminate_streak,
    double terminate_epsilon,
    CrossoverType type,
    size_t n_threads
) {
    std::vector<State> population(size);
    std::vector<State> children(size);
    std::vector<int> parent_probs(size);

    for (size_t i = 0; i < size; ++i) {
        population[i] = RandomState();
    }

    State best_state_all;
    best_state_all.eval = INT_MIN;

    int prev_eval;

    size_t streak = 0;
    size_t iter = 0;

    while (true) {
        State best_state;
        best_state.eval = INT_MIN;

        size_t thread_size = size / n_threads;

        {
            std::vector<std::thread> threads;

            for (size_t thread_i = 0; thread_i < n_threads; ++thread_i) {
                size_t start = thread_i * thread_size; // Inclusive
                size_t end; // Exclusive
                if (thread_i == n_threads - 1) {
                    end = size;
                } else {
                    end = (thread_i + 1) * thread_size;
                }

                threads.push_back(
                    std::thread(
                        [
                        this,
                        &population,
                        &parent_probs,
                        start, end
                        ] () mutable {
                            this->EvalGeneticChunk(
                                population,
                                parent_probs,
                                start, end
                            );
                        }
                    )
                );
            }

            for (auto& t : threads) t.join();
        }

        for (size_t i = 0; i < population.size(); ++i) {
            if (EvalGenetic(population[i]) > EvalGenetic(best_state)) {
                best_state = population[i];
            }
        }

        if (abs(prev_eval - EvalGenetic(best_state)) <= terminate_epsilon) {
            streak++;
        } else {
            streak = 0;
        }
        prev_eval = EvalGenetic(best_state);

        if (EvalGenetic(best_state) == GoalEvalGenetic()) {
            return std::tuple<bool, State>(true, best_state);
        }

        for (int x : best_state.data) {
            std::cout << x << " ";
        }

        std::cout <<
            std::right << std::setw(3) <<
            EvalGenetic(best_state) << " / " <<
            GoalEvalGenetic() << " / " <<
            streak << " / " <<
            iter << std::endl;

        if (streak >= terminate_streak) {
            return std::tuple<bool, State>(false, best_state_all);
        }

        if (EvalGenetic(best_state) > EvalGenetic(best_state_all)) {
            best_state_all = best_state;
        }

        std::discrete_distribution<int> parent_dist(
            std::begin(parent_probs), std::end(parent_probs)
        );

        {
            std::vector<std::thread> threads;

            for (size_t thread_i = 0; thread_i < n_threads; ++thread_i) {
                size_t start = thread_i * thread_size; // Inclusive
                size_t end; // Exclusive
                if (thread_i == n_threads - 1) {
                    end = size;
                } else {
                    end = (thread_i + 1) * thread_size;
                }

                threads.push_back(
                    std::thread(
                        [
                            this,
                            &population,
                            &children,
                            &parent_dist,
                            start, end,
                            mutate_prob,
                            type
                        ] () mutable {
                            this->ReproduceChunk(
                                population,
                                children,
                                parent_dist,
                                start, end,
                                mutate_prob,
                                type
                            );
                        }
                    )
                );
            }

            for (auto& t : threads) t.join();
        }

        population = children;
        iter++;
    }
}
