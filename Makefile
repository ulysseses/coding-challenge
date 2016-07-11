CXXFLAGS += -std=c++11 -Wall -Wextra -O3

rolling_median : 
	g++ $(CXXFLAGS) -Isrc src/victor/main.cpp -o $@

test :
	cd insight_testsuite && $(MAKE)

clean :
	rm -f rolling_median