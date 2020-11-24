#pragma once
#include <string>
#include <vector>
#include <random>
#include <unordered_set>
#include "optional.hpp"

size_t Index(size_t row, size_t col, size_t n);
void PrintBoard(std::vector<int> board, size_t n);

class Problem;

struct State {
    Problem* problem;
    std::vector<int> data;
    State(Problem* problem);

    bool operator ==(const State& other) const {
        return
            problem == other.problem &&
            data == other.data;
    }

    void Print();

    int CountConflicts();
    int Eval();
    tl::optional<int> eval; // Cached value
    bool IsGoal();
};

// https://stackoverflow.com/a/29855973/6759699
namespace std {
    template<>
    struct hash<std::vector<int>> {
        size_t operator()(const vector<int>& v) const {
            std::hash<int> hasher;
            size_t seed = 0;
            for (int i : v) {
                seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}

// https://stackoverflow.com/a/19195373/6759699
template<class T>
inline void hash_combine(std::size_t& s, const T& v)
{
    std::hash<T> h;
    s^= h(v) + 0x9e3779b9 + (s << 6) + (s>> 2);
}

namespace std {
    template<>
    struct hash<State> {
        size_t operator()(const State& state) const noexcept {
            size_t ans = 0;
            hash_combine(ans, state.problem);
            hash_combine(ans, state.data);
            return ans;
        }
    };
}

class StateIter {
private:
    State* state;
    size_t i;
    size_t cell_value;
    bool FixParams(size_t& i, size_t& cell_value);
public:
    StateIter(State* state);
    tl::optional<State> Successor();
};

class Problem {
private:
    std::uniform_int_distribution<int> rand_dist;
public:
    size_t n;
    std::vector<int> fixed;
    Problem(std::string filename);

    void Print() { PrintBoard(fixed, n); }
    bool IsFixed(size_t i) { return fixed[i] != 0; }

    State RandomState();
    State HillClimber(State state);
};
