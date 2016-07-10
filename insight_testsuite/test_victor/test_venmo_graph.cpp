#include "victor/venmo_graph.hpp"
#include "gtest/gtest.h"
#include <time.h>
#include <stdio.h>


static time_t create_time(char const* datetime) {
	tm time_obj;
	sscanf(datetime, "%4d-%2d-%2dT%2d:%2d:%2dZ",
		   &time_obj.tm_year, &time_obj.tm_mon, &time_obj.tm_mday,
		   &time_obj.tm_hour, &time_obj.tm_min, &time_obj.tm_sec);
	// tm_year -= 1900
	// tm_mon--
	time_obj.tm_year -= 1900;
	--time_obj.tm_mon;
	return mktime(&time_obj);
}

TEST(VenmoGraphTest, VenmoGraphWorks) {
	VenmoGraph graph;
	double const precision = 0.0001;
	ASSERT_NEAR(graph.extract_median("A", "B", create_time("2016-07-09T16:19:01Z")),
		1.000, precision);
	ASSERT_EQ(graph.num_vertices(), 2);
	ASSERT_EQ(graph.num_edges(), 1);

	ASSERT_NEAR(graph.extract_median("C", "D", create_time("2016-07-09T16:19:00Z")),
		1.000, precision);
	ASSERT_EQ(graph.num_vertices(), 4);
	ASSERT_EQ(graph.num_edges(), 2);

	ASSERT_NEAR(graph.extract_median("B", "E", create_time("2016-07-09T16:19:10Z")),
		1.000, precision);
	ASSERT_EQ(graph.num_vertices(), 5);
	ASSERT_EQ(graph.num_edges(), 3);

	ASSERT_NEAR(graph.extract_median("C", "A", create_time("2016-07-09T16:19:20Z")),
		2.000, precision);
	ASSERT_EQ(graph.num_vertices(), 5);
	ASSERT_EQ(graph.num_edges(), 4);

	ASSERT_NEAR(graph.extract_median("A", "E", create_time("2016-07-09T16:19:19Z")),
		2.000, precision);
	ASSERT_EQ(graph.num_vertices(), 5);
	ASSERT_EQ(graph.num_edges(), 5);

	ASSERT_NEAR(graph.extract_median("D", "E", create_time("2016-07-09T16:20:05Z")),
		1.000, precision);
	ASSERT_EQ(graph.num_vertices(), 5);
	ASSERT_EQ(graph.num_edges(), 4);

	ASSERT_NEAR(graph.extract_median("D", "E", create_time("2016-07-09T16:20:15Z")),
		1.500, precision);
	ASSERT_EQ(graph.num_vertices(), 4);
	ASSERT_EQ(graph.num_edges(), 3);

	ASSERT_NEAR(graph.extract_median("A", "F", create_time("2016-07-09T16:20:21Z")),
		1.000, precision);
	ASSERT_EQ(graph.num_vertices(), 4);
	ASSERT_EQ(graph.num_edges(), 2);

	ASSERT_NEAR(graph.extract_median("G", "A", create_time("2016-07-09T16:20:22Z")),
		1.000, precision);
	ASSERT_EQ(graph.num_vertices(), 5);
	ASSERT_EQ(graph.num_edges(), 3);

	ASSERT_NEAR(graph.extract_median("A", "E", create_time("2016-07-09T16:20:22Z")),
		1.000, precision);
	ASSERT_EQ(graph.num_vertices(), 5);
	ASSERT_EQ(graph.num_edges(), 4);

	ASSERT_NEAR(graph.extract_median("E", "G", create_time("2016-07-09T16:20:22Z")),
		2.000, precision);
	ASSERT_EQ(graph.num_vertices(), 5);
	ASSERT_EQ(graph.num_edges(), 5);

	graph.extract_median("F", "H", create_time("2016-07-09T16:20:22Z"));
	ASSERT_NEAR(graph.extract_median("I", "J", create_time("2016-07-09T16:20:22Z")),
		1.500, precision);
	ASSERT_EQ(graph.num_vertices(), 8);
	ASSERT_EQ(graph.num_edges(), 7);
}
