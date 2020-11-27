#include <iostream>
#include <cassert>
#include <map>
#include "../optional.hpp"
#include "../lib.h"

int main() {
    std::map<std::string, int> exps = {
        {"0", 0},
        {"3", 3},
        {"6", 6},
        {"4", 4},
        {"9_0", 0},
        {"9_3", 3},
        {"9_6", 6},
    };

    for (auto p : exps) {
        std::string filename = p.first;
        int exp_eval = p.second;

        Problem problem("tests/eval/" + filename);
        // Returns the same thing since all cells are fixed
        State state = problem.RandomState();

        int eval = state.Eval();
        std::cout << eval << " " << exp_eval << std::endl;
        assert(eval == exp_eval);
    }

    std::cout << "Pass" << std::endl;

    return 0;
}
