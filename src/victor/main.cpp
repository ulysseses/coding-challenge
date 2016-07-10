#include "victor/med_deg_stream.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "Usage:" << std::endl;
		std::cout << "rolling_median input_filename output_filename" << std::endl;
		return 1;
	}
	MedDegStream mds(argv[1], argv[2]);
	mds.process();
	return 0;
}
