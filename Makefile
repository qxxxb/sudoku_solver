flags = -std=c++11 -g -Wall -pthread
shared_cpp = lib.cpp optional.hpp
shared_h = lib.h

all: TestHarness TestHarnessGenetic TestSuccessor TestEval

TestHarness: TestHarness.cpp $(shared_cpp) $(shared_h)
	g++ $(flags) -o $@ TestHarness.cpp $(shared_cpp)

TestHarnessGenetic: TestHarness.cpp $(shared_cpp) $(shared_h)
	g++ $(flags) -DGENETIC -o $@ TestHarness.cpp $(shared_cpp)

TestSuccessor: tests/TestSuccessor.cpp $(shared_cpp) $(shared_h)
	g++ $(flags) -o $@ tests/TestSuccessor.cpp $(shared_cpp)

TestEval: tests/TestEval.cpp $(shared_cpp) $(shared_h)
	g++ $(flags) -o $@ tests/TestEval.cpp $(shared_cpp)

clean:
	rm -f TestHarness TestHarnessGenetic TestSuccessor TestEval
