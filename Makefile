CXXFLAGS += -std=c++11 -Wall -Wextra -O3

rolling_median : 
	g++ $(CXXFLAGS) -Isrc src/victor/main.cpp -o $@

test :
	cd insight_testsuite && $(MAKE)
	cd insight_testsuite && ./test_med_heap_map
	cd insight_testsuite && ./test_venmo_graph
	cd insight_testsuite && ./test_med_deg_stream

clean :
	rm -f rolling_median
	rm -f insight_testsuite/test_med_heap_map
	rm -f insight_testsuite/test_venmo_graph
	rm -f insight_testsuite/test_med_deg_stream
