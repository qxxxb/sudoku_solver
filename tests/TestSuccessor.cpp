#include <iostream>
#include <cassert>
#include <unordered_set>
#include <vector>
#include "../optional.hpp"
#include "../lib.h"

int main() {
    std::string filenames[] = {
        "sample4",
        "sample4_1",
        "sample4_2",
        "sample4_3",
        "sample9"
    };
    for (auto filename : filenames) {
        Problem problem("tests/" + filename);
        problem.Print();

        size_t n_succs_exp = problem.NBlanks() * (problem.n - 1);

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
