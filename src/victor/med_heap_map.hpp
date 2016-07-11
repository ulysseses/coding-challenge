/**
    Insight Data Engineering Code Challenge
    med_heap_map.hpp
    
    Purpose:
    
    MedHeapMap, short for Median Heap Map, is a data structure that is used to
    compute the median of online data. MedHeapMap exposes a public API, used
    by a VenmoGraph (defined in src/victor/venmo_graph.hpp), for keeping track
    of vertices and their respective degrees.
    
    It consists of 2 heaps (represented as vectors). The "left-half" is a
    max-heap that keeps track of all seen data that lies to the left of the
    current median. The "right-half" is a min-heap that keeps track of all data
    that lies to the right of the current median. The median is easily obtained
    as either the average of the top of both heaps, or the top of the
    larger-sized heap. Thus, the median can be obtained in O(1) time, while the
    heap-map structure allows O(log n) time complexity for insert, delete,
    increase_key, and decrease_key operations (where n is the current number of
    elements stored).
    
    The median heap obeys 2 invariants:
    
    1. The "left-half" and "right-half" differ less than 2 in size.
    2. The top of the "left-half" max-heap is less than or equal to the top of
       the "right-half" min-heap.
       
	When operations such as insert & decrease_key (temporarily) violate the
	invariant, the violation is fixed via heap rotation - the top of one heap is
	popped and pushed into the other heap.
    
    MedHeapMap also consists of a forward and backward index which map between
    the Venmo vertex name and its corresponding location in the Median Heap Map
    data structure, and vice versa. These maps are used to locate nodes in the
    heap map at O(1) time, since these are frequently used operations. Everytime
    the heaps are heapified (i.e. modified to maintain the heap property) or
    balanced, the corresponding locations stored in the indexes are modified
    accordingly.

    @author Victor Chen
*/
#ifndef MED_HEAP_MAP_HPP_
#define MED_HEAP_MAP_HPP_

#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>

// #include <iostream>
// using std::cout;
// using std::endl;


// ANC, abbreviation of ANCESTOR for binary heaps
#define ANC(i) ((i - 1) >> 1)

// DES1, abbreviation of DESCENDENT1 for binary heaps
#define DES1(i) ((i << 1) + 1)

// DES2, abbreviation of DESCENDENT2 for binary heaps
#define DES2(i) ((i << 1) + 2)


/**
	Median Heap Map
*/
class MedHeapMap {
private:
	/**
		Information about which heap and where in the heap and element is
		stored. This is the value stored in the forward map.
	*/
	struct FInfo {
		size_t ind;
		bool in_gh;
	};
	
	/**
		Information about which vertex name a heap element corresponds to.
		This is the value stored in the backwards map.
	*/
	struct BInfo {
		std::string g_key;
		std::string l_key;
	};

	std::vector<uint32_t> _lh;						// less-half max-heap
	std::vector<uint32_t> _gh;						// greater-half min-heap
	std::unordered_map<std::string, FInfo> _fmap;	// name -> heap location
													// forward map
	std::unordered_map<size_t, BInfo> _bmap;		// heap location -> name
													// backward map

	/**
	    Swap elements in a heap, forward map, and backward map.
	
	    @param i
	    @param j
	    @param in_gh whether or not the elements are in _gh.
	*/
	void swap_nodes(size_t i, size_t j, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;

		// swap degrees in the heap
		uint32_t t = vec[i];
		vec[i] = vec[j];
		vec[j] = t;

		// swap corresponding elements in _fmap & _bmap
		if (in_gh) {
			_bmap[i].g_key.swap(_bmap[j].g_key);
			_fmap[_bmap[j].g_key].ind = j;
			_fmap[_bmap[i].g_key].ind = i;
		} else {
			_bmap[i].l_key.swap(_bmap[j].l_key);
			_fmap[_bmap[j].l_key].ind = j;
			_fmap[_bmap[i].l_key].ind = i;
		}
	}
	
	/**
		Float an element up until heap-property is maintained.
		
		@param i index of element in heap.
		@param in_gh whether or not the elements are in _gh.
	*/
	void float_up(size_t i, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;

		// move the element in vec up the binary tree
		size_t j = ANC(i);
		while (i != 0 && ((in_gh && vec[i] < vec[j]) ||
						  (!in_gh && vec[i] > vec[j]))) {
			swap_nodes(i, j, in_gh);
			i = j;
			j = ANC(i);
		}
	}

	/**
		Sink an element down until heap-property is maintained.
		
		@param i index of element in heap.
		@param in_gh whether or not the elements are in _gh.
	*/
	void sink_down(size_t i, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;

		// move the element in vec down the binary tree
		size_t j = DES1(i);
		size_t k = DES2(i);
		size_t const n = vec.size();
		while (true) {
			if (j >= n) {  // j & k are out of bounds
				return;
			} else if (k >= n) {  // only k is out of bounds
				if ((in_gh && vec[j] < vec[i]) ||
					  (!in_gh && vec[j] > vec[i])) {
					swap_nodes(i, j, in_gh);
					return;
				}
				return;
			} else {  // both j & k are not out of bounds
				size_t z;
				if ((in_gh && vec[j] < vec[k]) ||
					  (!in_gh && vec[j] > vec[k])) {
					z = j;
				} else {
					z = k;
				}
				if ((in_gh && vec[z] < vec[i]) ||
					  (!in_gh && vec[z] > vec[i])) {
					swap_nodes(i, z, in_gh);
				} else {
					return;
				}

				// continue down the heap
				i = z;
				j = DES1(i);
				k = DES2(i);
			}
		}

		return;  // never reached
	}

	/**
		Pop an element off of one heap and push it into another heap.
		
		@param into_gh whether or not to rotate into _gh.
	*/
	void rotate(bool into_gh) {
		if (into_gh) {
			uint32_t degree = _lh.front();
			swap_nodes(0, _lh.size() - 1, false);
			_lh.pop_back();
			_gh.emplace_back(degree);

			std::unordered_map<size_t, BInfo>::iterator it =
				_bmap.find(_lh.size());
			std::string name = std::move((it->second).l_key);
			FInfo& info = _fmap[name];
			info.ind = _gh.size() - 1;
			info.in_gh = true;
			_bmap[_gh.size() - 1].g_key = std::move(name);
			if ((it->second).g_key.empty()) {
				_bmap.erase(it);
			}

			sink_down(0, false);
			float_up(_gh.size() - 1, true);
		} else {
			uint32_t degree = _gh.front();
			swap_nodes(0, _gh.size() - 1, true);
			_gh.pop_back();
			_lh.emplace_back(degree);

			std::unordered_map<size_t, BInfo>::iterator it =
				_bmap.find(_gh.size());
			std::string name = std::move((it->second).g_key);
			FInfo& info = _fmap[name];
			info.ind = _lh.size() - 1;
			info.in_gh = false;
			_bmap[_lh.size() - 1].l_key = std::move(name);
			if ((it->second).l_key.empty()) {
				_bmap.erase(it);
			}

			sink_down(0, true);
			float_up(_lh.size() - 1, false);
		}
	}


	/**
		Erase an element from the heap. Make sure to rotate to fix
		median heap invariance.
		
		@param i index of element in heap.
		@param in_gh whether or not the elements are in _gh.
	*/
	void erase(size_t i, bool in_gh) {
		if (in_gh) {
			swap_nodes(i, _gh.size() - 1, true);
			_gh.pop_back();
			std::unordered_map<size_t, BInfo>::iterator it =
				_bmap.find(_gh.size());
			std::string name = std::move((it->second).g_key);
			_fmap.erase(std::move(name));
			if ((it->second).l_key.empty()) {
				_bmap.erase(it);
			}
			sink_down(i, true);
			if (_lh.size() == _gh.size() + 2) {
				rotate(true);
			}
		} else {
			swap_nodes(i, _lh.size() - 1, false);
			_lh.pop_back();
			std::unordered_map<size_t, BInfo>::iterator it =
				_bmap.find(_lh.size());
			std::string name = std::move((it->second).l_key);
			_fmap.erase(std::move(name));
			if ((it->second).g_key.empty()) {
				_bmap.erase(it);
			}
			sink_down(i, false);
			if (_gh.size() == _lh.size() + 2) {
				rotate(false);
			}
		}
	}

	/**
		Increment the value of an element in the heap. Make sure to sink/float
		and rotate to maintain invariance.
		
		@param i index of element in heap.
		@param in_gh whether or not the elements are in _gh.
	*/
	void increase_key(size_t i, bool in_gh) {
		if (in_gh) {
			++(_gh[i]);
			sink_down(i, true);
		} else {
			++(_lh[i]);
			float_up(i, false);
			if (_lh.front() > _gh.front()) {
				ssize_t const size_diff = ssize_t(_lh.size()) -
										  ssize_t(_gh.size());
				if (size_diff == 0) {
					rotate(true);
					rotate(false);
				} else if (size_diff == 1) {
					rotate(true);
				} else {  // size_diff == -1
					rotate(false);
					rotate(true);
				}
			}
		}
	}

	/**
		Decrement the value of an element in the heap. Make sure to sink/float
		and rotate to maintain invariance.
		
		@param i index of element in heap.
		@param in_gh whether or not the elements are in _gh.
	*/
	void decrease_key(size_t i, bool in_gh) {
		if (in_gh) {
			--(_gh[i]);
			float_up(i, true);
			if (_lh.front() > _gh.front()) {
				ssize_t const size_diff = ssize_t(_lh.size()) -
										  ssize_t(_gh.size());
				if (size_diff == 0) {
					rotate(false);
					rotate(true);
				} else if (size_diff == -1) {
					rotate(false);
				} else {  // size_diff == 1
					rotate(true);
					rotate(false);
				}
			}
		} else {
			--(_lh[i]);
			sink_down(i, false);
		}
	}

public:
	/**
		Insert an element into the heap. Make sure to sink/float
		and rotate to maintain invariance.
		
		@param name name of the element to be inserted.
	*/
	void insert(std::string name) {
		// prepare the heaps if they are empty
		if (empty()) {
			_gh.emplace_back(1);
			_fmap[name] = { _gh.size() - 1, true };
			_bmap[_gh.size() - 1].g_key = std::move(name);
			return;
		} else if (size() == 1) {
			_lh.emplace_back(1);
			_fmap[name] = { _lh.size() - 1, false };
			_bmap[_lh.size() - 1].l_key = std::move(name);
			return;
		}

		// insert into either the lessor or greater half
		if (1 < _lh.front()) {
			_lh.emplace_back(1);
			_fmap[name] = { _lh.size() - 1, false };
			_bmap[_lh.size() - 1].l_key = std::move(name);
			float_up(_lh.size() - 1, false);
			if (_lh.size() == _gh.size() + 2) {
				rotate(true);
			}
		} else {
			_gh.emplace_back(1);
			_fmap[name] = { _gh.size() - 1, true };
			_bmap[_gh.size() - 1].g_key = std::move(name);
			float_up(_gh.size() - 1, true);
			if (_gh.size() == _lh.size() + 2) {
				rotate(false);
			}
		}
	}

	/**
		Erase an element from the heap. Make sure to sink/float
		and rotate to maintain invariance.
		
		@param name name of the element to be erased.
	*/
	void erase(std::string const& name) {
		FInfo const& info = _fmap[name];
		erase(info.ind, info.in_gh);
	}

	/**
		Increment the value of an element in the heap. Make sure to sink/float
		and rotate to maintain invariance.
		
		@param name name of the element to be incremented.
	*/
	void increase_key(std::string name) {
		// Assume name exists. Increase its degree.
		FInfo const& info = _fmap[name];
		increase_key(info.ind, info.in_gh);
	}

	/**
		Decrement the value of an element in the heap. Any vertices with degree
		0 are erased. Make sure to sink/float and rotate to maintain invariance.
		
		@param name name of the element to be decremented.
		@return true if the element was erased, false otherwise.
	*/
	bool decrease_key(std::string const& name) {
		// Erase a vertex if has degree 1. Return false.
		// Otherwise, decrease its key. Return true.
		FInfo const& info = _fmap[name];
		if (info.in_gh) {
			if (_gh[info.ind] == 1) {
				erase(info.ind, info.in_gh);
				return false;
			} else {
				decrease_key(info.ind, info.in_gh);
				return true;
			}
		} else {
			if (_lh[info.ind] == 1) {
				erase(info.ind, info.in_gh);
				return false;
			} else {
				decrease_key(info.ind, info.in_gh);
				return true;
			}
		}
	}
	
	/**
		API used by a VenmoGraph object. VenmoGraph inserts/modifies vertices in
		pairs (i.e. edges). This method will insert/modify both vertices into
		the graph.
		
		@param name1 name of the 1st element to be inserted/incremented.
		@param name2 name of the 2nd element to be inserted/incremented.
	*/
	void process_edge(std::string name1, std::string name2) {
		std::unordered_map<std::string, FInfo>::const_iterator it1 =
			_fmap.find(name1);
		if (it1 != _fmap.cend()) {
			FInfo const& info1 = it1->second;
			increase_key(info1.ind, info1.in_gh);
			std::unordered_map<std::string, FInfo>::const_iterator it2 =
				_fmap.find(name2);
			if (it2 != _fmap.cend()) {
				FInfo const& info2 = it2->second;
				increase_key(info2.ind, info2.in_gh);
			} else {
				insert(std::move(name2));
			}
		} else {
			insert(std::move(name1));
			std::unordered_map<std::string, FInfo>::const_iterator it2 =
				_fmap.find(name2);
			if (it2 != _fmap.cend()) {
				FInfo const& info = it2->second;
				increase_key(info.ind, info.in_gh);
			} else {
				insert(std::move(name2));
			}
		}
	}

	/**
		Current median.
		
		@return the current median.
	*/
	double median() const {
		ssize_t const size_diff = ssize_t(_lh.size()) -
								  ssize_t(_gh.size());

		if (size_diff > 0) {  // _lh.size() > _gh.size()
			return _lh.front();
		}
		else if (size_diff == 0) {  // _lh.size() == _gh.size()
			return (_lh.front() + _gh.front()) / 2.0;
		}
		else {  // _lh.size() < _gh.size()
			return _gh.front();
		}
	}

	/**
		Check if the median heap map is empty.
		
		@return whether the heap map is empty or not.
	*/
	bool empty() const {
		return size() == 0;
	}

	/**
		Median heap map size.
		
		@return the number of elements in the median heap map.
	*/
	size_t size() const {
		return _lh.size() + _gh.size();
	}

	/**
		Less-half heap size.
		
		@return the number of elements in the less-half heap.
	*/
	size_t size_lh() const {
		return _lh.size();
	}

	/**
		Greater-half heap size.
		
		@return the number of elements in the greater-half heap.
	*/
	size_t size_gh() const {
		return _gh.size();
	}

	/* Testing & Debugging */

	/**
		Degree of an element.
		
		@param name name of the element.
		@return the degree of the element.
	*/
	uint64_t degree(std::string name) const {
		std::unordered_map<std::string, FInfo>::const_iterator found =
			_fmap.find(name);
		if ((found->second).in_gh) {
			return _gh[(found->second).ind];
		} else {
			return _lh[(found->second).ind];
		}
	}

	/**
		Which half the element belongs to.
		
		@param name name of the element.
		@return the half the element belongs to.
	*/
	bool in_gh(std::string name) const {
		std::unordered_map<std::string, FInfo>::const_iterator found =
			_fmap.find(name);
		return (found->second).in_gh;
	}

	/**
		Whether or not the median heap map contains an element.
		
		@param name name of the element.
		@return whether or not element is contained.
	*/
	bool contains(std::string name) const {
		return _fmap.count(name) != 0;
	}

	/**
		Dump of what are inside the less-half and greater-half heaps.
		
		@return string of the dump.
	*/
	std::string dump() const {
		std::stringstream ss;
		
		// dump lh
		ss << "----- _lh -----\n";
		for (size_t i = 0; i < _lh.size(); ++i) {
			std::unordered_map<size_t, BInfo>::const_iterator const it =
				_bmap.find(i);
			std::string const name = (it->second).l_key;
			ss << i << ": " << name << ", " << _lh[i] << '\n';
		}
		ss << '\n';
		
		// dump gh
		ss << "----- _gh -----\n";
		for (size_t i = 0; i < _gh.size(); ++i) {
			std::unordered_map<size_t, BInfo>::const_iterator const it =
				_bmap.find(i);
			std::string const name = (it->second).g_key;
			ss << i << ": " << name << ", " << _gh[i] << '\n';
		}
		ss << '\n';
		
		return ss.str();
	}

	/**
		Dump of what are inside the heaps as well as the maps.
		
		@return string of the dump.
	*/
	std::string dump2() const {
		std::stringstream ss;

		// dump _lh
		ss << "----- _lh -----\n";
		for (uint32_t x : _lh) {
			ss << x << ' ';
		}
		ss << "\n\n";

		// dump _gh
		ss << "----- _gh -----\n";
		for (uint32_t x : _gh) {
			ss << x << ' ';
		}
		ss << "\n\n";

		// dump _fmap
		ss << "----- _fmap -----\n";
		for (auto const& p : _fmap) {
			ss << p.first << " : (";
			ss << p.second.ind << ", ";
			ss << (p.second.in_gh ? "true" : "false") << ")\n";
		}
		ss << '\n';

		// dump _bmap
		ss << "----- _bmap -----\n";
		for (auto const& p : _bmap) {
			ss << p.first << '\n';
			if (!p.second.g_key.empty()) {
				ss << "  g: " << p.second.g_key << '\n';
			}
			if (!p.second.l_key.empty()) {
				ss << "  l: " << p.second.l_key << '\n';
			}
		}

		return ss.str();
	}
};

#endif  // MED_HEAP_MAP_HPP_
