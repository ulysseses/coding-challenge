GTEST_DIR = googletest

CPPFLAGS += -isystem $(GTEST_DIR)/include

CXXFLAGS += -g -Wall -Wextra -pthread

TESTS = test_med_heap_map test_venmo_graph test_med_deg_stream

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

all : $(TESTS)

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o

GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^


USER_DIR = test_victor
PROJ_INCL = ../src
GTEST_INCL = googletest/include

test_med_heap_map : gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I$(PROJ_INCL) -I$(GTEST_INCL) \
		$(USER_DIR)/test_med_heap_map.cpp $^ -o $@

test_venmo_graph : gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I$(PROJ_INCL) -I$(GTEST_INCL) \
		$(USER_DIR)/test_venmo_graph.cpp $^ -o $@

test_med_deg_stream : gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I$(PROJ_INCL) -I$(GTEST_INCL) \
		$(USER_DIR)/test_med_deg_stream.cpp $^ -o $@