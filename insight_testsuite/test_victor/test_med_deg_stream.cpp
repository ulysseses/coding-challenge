#include "victor/med_deg_stream.hpp"
#include "gtest/gtest.h"


namespace victor {

TEST(MedDegStreamTest, ProcessWorks) {
	char const* in_filename = "../venmo_input/venmo-trans.txt";  // fill here
	char const* out_filename = "../venmo_output/victor_out.txt";  // fill here
	MedDegStream mds(in_filename, out_filename);
	mds.process();
}

}  // namespace victor