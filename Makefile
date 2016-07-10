CXXFLAGS += -std=c++11 -Wall -Wextra

rolling_median : 
	g++ $(CXXFLAGS) -Isrc src/victor/main.cpp -o $@

test :
	cd insight_testsuite && $(MAKE)