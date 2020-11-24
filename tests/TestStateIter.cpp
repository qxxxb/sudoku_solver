#include <iostream>
#include <cassert>
#include <unordered_set>
#include <vector>
#include "../optional.hpp"
#include "../lib.h"

int main() {
    // Test `StateIter::Successor()`
    // Verifies that it generates all successors without duplicates.
    // Strategy:
    // - Calculate the expected number of successors (n_blanks * 3)
    // - Use an `unordered_set` to ensure that there are no duplicates
    // - Check that the size of the set is equal to the expected number of
    //   successors

    std::string filenames[] = {"sample", "sample9"};
    for (auto filename : filenames) {
        Problem problem("tests/" + filename);
        problem.Print();

        size_t n_blanks = 0;
        for (size_t i = 0; i < problem.fixed.size(); ++i) {
            if (!problem.IsFixed(i)) n_blanks++;
        }
        size_t n_succs_exp = n_blanks * (problem.n - 1);

        const size_t n_trials = 100;
        for (size_t trial = 0; trial < n_trials; ++trial) {
            State state = problem.RandomState();
            auto iter = StateIter(&state);
            std::unordered_set<State> succs;
            while (true) {
                auto succ_opt = iter.Successor();
                if (!succ_opt) break;
                auto succ = *succ_opt;
                assert(succs.count(succ) == 0);
                succs.insert(succ);
            }

            assert(succs.size() == n_succs_exp);
            std::cout << ".";
        }

        std::cout << std::endl;
        std::cout <<
            "Verified successor function for " << n_trials <<
            " trials" << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
