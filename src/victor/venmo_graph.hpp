#ifndef VENMO_GRAPH_HPP_
#define VENMO_GRAPH_HPP_

#include "victor/med_heap_map.hpp"
#include <map>
#include <unordered_map>
#include <time.h>
#include <utility>
#include <string>
#include <sstream>

// #include <iostream>
// using std::cout;
// using std::endl;

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
	time_t _latest_time = 0;

	void process_helper(std::string actor, std::string target,
						time_t created_time) {
		std::string *name1_ptr;
		std::string *name2_ptr;
		
		// Edge identifiers are ordered via string comparison
		int cmp_val = actor.compare(target);

		if (cmp_val < 1) {
			name1_ptr = &actor;
			name2_ptr = &target;
		} else if (cmp_val == 0) {
			return;
		} else {
			name1_ptr = &target;
			name2_ptr = &actor;
		}
		
		// check _neighbors for the edge
		bool not_seen_before;
		Neighbors::iterator const it = _neighbors.find(*name1_ptr);
		std::unordered_map<std::string, time_t>::iterator it2;
		if (it != _neighbors.end()) {
			it2 = (it->second).find(*name2_ptr);
			if (it2 != (it->second).end()) {
				not_seen_before = false;
			} else {
				not_seen_before = true;
			}
		} else {
			not_seen_before = true;
		}

		if (not_seen_before) {
			// New edge encountered -> just insert into _vertices
			// and update _edegs & _neighbors.
			_vertices.process_edge(actor, target);
			_neighbors[*name1_ptr][*name2_ptr] = created_time;
			_edges.emplace(created_time,
				std::make_pair<std::string, std::string>(
				std::move(*name1_ptr), std::move(*name2_ptr)));
		} else {
			// Old edge encountered -> don't insert, just update its time.
			time_t& time_ref = it2->second;
			time_t old_time = time_ref;
			time_ref = created_time;
			Edges::const_iterator it3 = _edges.find(old_time);
			for (; it3 != _edges.cend(); ++it3) {
				auto const& p = it3->second;
				if (p.first == *name1_ptr && p.second == *name2_ptr) {
					break;
				}
			}
			_edges.erase(it3);
			_edges.emplace(created_time,
				std::make_pair<std::string, std::string>(
				std::move(*name1_ptr), std::move(*name2_ptr)));
		}
	}

	void process(std::string actor, std::string target,
				 time_t created_time) {
		if (_latest_time == 0) {
			// First edge of the graph. Insert it.
			_latest_time = created_time;
			process_helper(std::move(actor), std::move(target), created_time);
		} else {
			double diff_val = difftime(created_time, _latest_time);
			if (diff_val > -60.0 && diff_val <= 0.0) {
				// Edge is before & within the latest time. Deal with it.
				process_helper(std::move(actor), std::move(target),
							   created_time);
			} else if (diff_val > 0.0) {
				// Edge is the latest time. Erase all edges more than 60 seconds
				// old. Then, deal with this new edge.
				_latest_time = created_time;
				
				Edges::const_iterator ub =
					_edges.upper_bound(created_time - 60);
				for (auto it = _edges.cbegin(); it != ub; ++it) {
					auto const& p = it->second;
					bool b1 = _vertices.decrease_key(p.first);
					_vertices.decrease_key(p.second);
					if (b1) {
						_neighbors.erase(p.first);
					} else {
						_neighbors[p.first].erase(p.second);
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
	
	std::string dump() {
		std::stringstream ss;
		
		ss << "----- Edges -----\n";
		for (auto const& p : _edges) {
			time_t created_time = p.first;
			auto const& p2 = p.second;
			std::string const& name1 = p2.first;
			std::string const& name2 = p2.second;
			ss << created_time << ": " << name1 << ' ' << name2 << '\n';
		}
		ss << '\n';
		
		ss << "----- Neighbors -----\n";
		for (auto const& p : _neighbors) {
			std::string const& name = p.first;
			ss << name << ":\n";
			for (auto const& p2 : p.second) {
				std::string const& neighbor = p2.first;
				time_t created_time = p2.second;
				ss << "  " << neighbor << " at " << created_time << '\n';
			}
		}
		ss << '\n';
		
		return ss.str();
	}
};

#endif  // VENMO_GRAPH_HPP_
