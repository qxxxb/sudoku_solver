flags = -std=c++11 -g -Wall
shared_cpp = lib.cpp optional.hpp
shared_h = lib.h

all: TestHarness TestStateIter TestEval

TestHarness: TestHarness.cpp $(shared_cpp) $(shared_h)
	g++ $(flags) -o $@ TestHarness.cpp $(shared_cpp)

TestStateIter: tests/TestStateIter.cpp $(shared_cpp) $(shared_h)
	g++ $(flags) -o $@ tests/TestStateIter.cpp $(shared_cpp)

TestEval: tests/TestEval.cpp $(shared_cpp) $(shared_h)
	g++ $(flags) -o $@ tests/TestEval.cpp $(shared_cpp)

clean:
	rm -f TestHarness TestStateIter TestEval
