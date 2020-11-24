#include <iostream>
#include "lib.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw std::invalid_argument("Invalid number of arguments");
    }

    std::string filename = argv[1];
    Problem problem(filename);
    problem.Print();

    State ans = problem.HillClimber(problem.RandomState());
    std::cout << std::endl;
    ans.Print();

    return 0;
}
