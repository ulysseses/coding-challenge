#include "victor/med_deg_stream.hpp"
#include "gtest/gtest.h"


TEST(MedDegStreamTest, ProcessWorks) {
	char const* in_filename = "";  // fill here
	char const* out_filename = "";  // fill here
	MedDegStream mds(in_filename, out_filename);
	mds.process();
}
