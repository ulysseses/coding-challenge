#ifndef VENMO_GRAPH_HPP_
#define VENMO_GRAPH_HPP_

#include "victor/med_heap_map.hpp"
#include <map>
#include <unordered_map>
#include <time.h>
#include <utility>
#include <string>

class VenmoGraph {
private:
	struct time_comp {
		bool operator()(time_t lhs, time_t rhs) const {
			return difftime(rhs, lhs) > 0.0;
		}
	};

	typedef std::multimap<time_t,
						  std::pair<std::string, std::string>,
						  time_comp>
		Edges;
	typedef std::unordered_map<std::string,
							   std::unordered_map<std::string, time_t>>
	   	Neighbors;

	MedHeapMap _vertices;
	Edges _edges;
	Neighbors _neighbors;
	time_t _latest_time;

	void process_helper(std::string actor, std::string target,
						time_t created_time) {
		// If both actor and target already exist in the graph, just update
		// the edges table.
		// DON'T INCREASE_KEY/INSERT!
		int cmp_val = actor.compare(target);
		bool not_seen_before;
		std::string *name1_ptr;
		std::string *name2_ptr;

		if (cmp_val < 1) {
			not_seen_before = _vertices.process_edge(actor, target);
			name1_ptr = &actor;
			name2_ptr = &target;
		} else if (cmp_val == 0) {
			return;
		} else {
			not_seen_before = _vertices.process_edge(target, actor);
			name1_ptr = &target;
			name2_ptr = &actor;
		}

		if (not_seen_before) {
			_neighbors[*name1_ptr][*name2_ptr] = created_time;
			_edges.emplace(created_time,
				std::make_pair<std::string, std::string>(
				std::move(*name1_ptr), std::move(*name2_ptr)));
		} else {
			time_t old_time = _neighbors[*name1_ptr][*name2_ptr];
			Edges::const_iterator it = _edges.find(old_time);
			for (; it != _edges.cend(); ++it) {
				auto const& p = it->second;
				if (p.first == *name1_ptr && p.second == *name2_ptr) {
					break;
				}
			}
			_edges.erase(it);
			_edges.emplace(created_time,
				std::make_pair<std::string, std::string>(
				std::move(*name1_ptr), std::move(*name2_ptr)));
		}
	}

	void process(std::string actor, std::string target,
				 time_t created_time) {
		if (_latest_time == 0) {
			_latest_time = created_time;
			process_helper(std::move(actor), std::move(target), created_time);
		} else {
			double diff_val = difftime(created_time, _latest_time);
			if (diff_val > -60.0 && diff_val <= 0.0) {
				process_helper(std::move(actor), std::move(target),
							   created_time);
			} else if (diff_val > 0.0) {
				_latest_time = created_time;

				// delete nodes with times that are more than 60 seconds
				// earlier than the latest time
				Edges::const_iterator ub = 
					_edges.upper_bound(created_time - 60);
				for (auto it = _edges.cbegin(); it != ub; ++it) {
					auto const& p = it->second;
					_vertices.decrease_key(p.first);
					_vertices.decrease_key(p.second);
					Neighbors::iterator found = _neighbors.find(p.first);
					(found->second).erase(p.second);
					if ((found->second).empty()) {
						_neighbors.erase(found);
					}
				}
				_edges.erase(_edges.cbegin(), ub);

				process_helper(std::move(actor), std::move(target),
							   created_time);
			}
		}
	}

public:
	double extract_median(std::string actor, std::string target,
						  time_t created_time) {
		process(std::move(actor), std::move(target), created_time);
		return _vertices.median();
	}

	/* Testing & Debugging */
	size_t num_vertices() const {
		return _vertices.size();
	}

	size_t num_edges() const {
		return _edges.size();
	}
};

#endif  // VENMO_GRAPH_HPP_
