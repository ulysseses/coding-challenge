/**
    Insight Data Engineering Code Challenge
    venmo_graph.hpp
    
    Purpose:
    
    VenmoGraph is the data structure used to store the vertices and edges of the
    Venmo payment graph. It stores the vertices in a median heap map structure
    (defined in src/victor/med_heap_map.hpp). When vertices have degree 0, the
    median heap map takes care of removing it from the graph.
    
    The edges are stored in a multi-map ordered by timestamp. That way, edges
    with a timestamp less than the latest time minus 60 seconds can be
    expired/deleted via a call of:
    
        edges.erase(edges.begin(), edges.upper_bound(latest_time - 60))
    
    This operation is efficient because it takes advantage of the tree structure
    in searching for upper bound time-stamp and traversing the tree up to that
    point, deleting the nodes in the process. The time complexity is O(n), where
    n is the distance between the first and last element to be deleted.
    
    The graph also indirectly stores edges by keeping track of neighbors of each
    vertex. Actually, that isn't technically true, because it stores the
    neighbors of only the lesser of the vertices of an edge, where order is
    defined by name (string) comparison. Doing so saves space.
    
    Storing neighbors is necessary when we need to update an edge with a new
    time-stamp. One can easily linearly search for the old timestamp of the
    edge instead of relying on storing neighbors. This is a bad idea because
    although updating edges with new time-stamps may be a rare occurrence,
    stopping to search the whole multimap blocks the data streaming pipeline.
    VenmoGraph may process elements off of a queue (instead of from a file, as
    is in the Insight Data Engineering Code Challenge), and the queue may reach
    its full capacity while VenmoGraph is still searching its edges. It is
    better to pay the price of maintaining the neighbors container to achieve
    low latency.

    @author Victor Chen
*/
#ifndef VENMO_GRAPH_HPP_
#define VENMO_GRAPH_HPP_

#include "victor/med_heap_map.hpp"
#include <time.h>
#include <map>
#include <unordered_map>
#include <utility>
#include <string>
#include <sstream>

// #include <iostream>
// using std::cout;
// using std::endl;

/**
	VenmoGraph
*/
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

	MedHeapMap _vertices;		// Vertices container
	Edges _edges;				// Edges container
	Neighbors _neighbors;		// Neighbors container
	time_t _latest_time = 0;	// time of an edge with the latest time-stamp
	
	/**
		Sub-routine which:
		
		1. checks if the edge exists already.
		2. updates it with the new timestamp (if it exists),or inserts
		   it edge into the graph (if it doesn't exist yet).
	   
	    @param actor name of the Venmo payment actor.
	    @param target name of the Venmo payment target.
	    @param created_time time of the payment.
	*/
	void process_helper(std::string actor, std::string target,
						time_t created_time) {
		std::string *name1_ptr;
		std::string *name2_ptr;
		
		// edge identifiers are ordered via string comparison
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
				std::make_pair(std::move(*name1_ptr), std::move(*name2_ptr)));
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
				std::make_pair(std::move(*name1_ptr), std::move(*name2_ptr)));
		}
	}

	/**
		Routine which:
		
		A. inserts the edge if it is before and within the time window.
		B. erases edges that fall out of the new time window.
		C. skips the edge if it is before and outside the time window.
	   
	    @param actor name of the Venmo payment actor.
	    @param target name of the Venmo payment target.
	    @param created_time time of the payment.
	*/
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
	/**
		Return the current median, regardless of whether or not the edge is
		inside or outside the time window.
	   
	    @param actor name of the Venmo payment actor.
	    @param target name of the Venmo payment target.
	    @param created_time time of the payment.
	    @return the current median.
	*/
	double extract_median(std::string actor, std::string target,
						  time_t created_time) {
		process(std::move(actor), std::move(target), created_time);
		return _vertices.median();
	}

	/* Testing & Debugging */
	
	/**
		Number of vertices.
		
		@return the number of vertices in the graph.
	*/
	size_t num_vertices() const {
		return _vertices.size();
	}

	/**
		Number of edges.
		
		@return the number of edges in the graph.
	*/
	size_t num_edges() const {
		return _edges.size();
	}
	
	/**
		Dump of the edges and neighbors.
		
		@return string of the dump.
	*/
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
