#ifndef MED_DEG_STREAM_HPP_
#define MED_DEG_STREAM_HPP_

#include "victor/venmo_graph.hpp"
#include "json/json.hpp"
#include <stdio.h>
#include <fstream>


using json = nlohmann::json;  // for convenience


class MedDegStream {
private:
	VenmoGraph _graph;
	std::ifstream _ifs;
	std::ofstream _ofs;

public:
	MedDegStream(char const* in_filename, char const* out_filename) {
		_ifs.open(in_filename, std::ifstream::in);
		_ofs.open(out_filename, std::ofstream::out);
		_ofs.precision(2);
	}

	void process() {
		json j;
		json::const_iterator it_actor;
		json::const_iterator it_target;
		json::const_iterator it_created_time;
		json::const_iterator end;
		tm dt;
		while (_ifs >> j) {
			// skip a line if it has any malformed or missing
			// field
			end = j.cend();
			it_actor = j.find("actor");
			std::string actor;
			if (it_actor == end) {
				continue;
			} else {
				actor = *it_actor;
				if (actor.empty()) {
					continue;
				}
			}

			it_target = j.find("target");
			std::string target;
			if (it_target == end) {
				continue;
			} else {
				target = *it_target;
				if (target.empty()) {
					continue;
				}
			}

			it_created_time = j.find("created_time");
			std::string time_str;
			if (it_created_time == end) {
				continue;
			} else {
				time_str = *it_created_time;
				if (time_str.empty()) {
					continue;
				}
			}

			int num_matched = sscanf(time_str.c_str(), "%4d-%2d-%2dT%2d:%2d:%2dZ",
				&dt.tm_year, &dt.tm_mon, &dt.tm_mday, &dt.tm_hour,
				&dt.tm_min, &dt.tm_sec);

			if (num_matched != 6) {
				continue;
			}

			// tm_year -= 1900
			// tm_mon--
			dt.tm_year -= 1900;
			--dt.tm_mon;
			time_t created_time = mktime(&dt);

			double current_median = _graph.extract_median(std::move(actor),
				std::move(target),
				created_time);
			_ofs << current_median << '\n';
		}
		_ofs.flush();
	}

};


#endif  // MED_DEG_STREAM_HPP_
