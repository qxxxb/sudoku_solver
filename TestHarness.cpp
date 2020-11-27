#include <iostream>
#include <string>
#include <tuple>
#include "lib.h"
#include "optional.hpp"

int main(int argc, char *argv[]) {
#ifdef GENETIC
    if (argc != 8) {
        throw std::invalid_argument("Invalid number of arguments");
    }

    size_t population_size = std::stoul(argv[2]);
    double mutate_prob = std::stod(argv[3]);
    size_t terminate_streak = std::stoul(argv[4]);
    size_t terminate_epsilon = std::stoul(argv[5]);
    auto type = (Problem::CrossoverType)std::stoi(argv[6]);
    size_t n_threads = std::stoul(argv[7]);
#else
    if (argc != 2) {
        throw std::invalid_argument("Invalid number of arguments");
    }
#endif

    std::string filename = argv[1];
    Problem problem(filename);
    problem.Print();
    std::cout << std::endl;

#ifdef GENETIC
    bool is_goal;
    State best_state;
    std::tie(is_goal, best_state) = problem.Genetic(
        population_size,
        mutate_prob,
        terminate_streak,
        terminate_epsilon,
        type,
        n_threads
    );
    std::cout << std::endl;

    if (is_goal) {
        std::cout << "Found goal" << std::endl;
    } else {
        std::cout << "Couldn't find goal" << std::endl;
    }

    std::cout <<
        "Best state: " <<
        problem.EvalGenetic(best_state) << " / " <<
        problem.GoalEvalGenetic() << std::endl;

    best_state.Print();
#else
    State ans = problem.HillClimber(problem.RandomState());
    std::cout << std::endl;
    ans.Print();
#endif

    return 0;
}
