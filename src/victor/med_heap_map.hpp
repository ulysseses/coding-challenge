#ifndef MED_HEAP_MAP_HPP_
#define MED_HEAP_MAP_HPP_

#include <vector>
#include <unordered_map>
#include <string>

// ANC, abbreviation of ANCESTOR for binary heaps
#define ANC(i) ((i - 1) >> 1)

// DES1, abbreviation of DESCENDENT1 for binary heaps
#define DES1(i) ((i >> 1) + 1)

// DES2, abbreviation of DESCENDENT2 for binary heaps
#define DES2(i) ((i >> 1) + 2)


class MedHeapMap {
private:
	struct Info {
		uint32_t *node;
		bool in_gh;
	};

	std::vector<uint32_t> _lh, _gh;
	std::unordered_map<std::string, Info> _fmap;
	std::unordered_map<uint32_t*, std::string> _bmap;
	size_t _size;

	void swap_nodes(uint32_t* node1, uint32_t* node2) {
		// XOR trick for swapping integers
		*node1 ^= *node2;
		*node2 ^= *node1;
		*node1 ^= *node2;

		// swap pointers in _fmap & _bmap
		std::string tmp = _bmap[node1];
		_fmap[tmp].node = node2;
		_fmap[_bmap[node2]].node = node1;
		_bmap[node1] = _bmap[node2];
		_bmap[node2] = std::move(tmp);
	}

	uint32_t* up_helper(uint32_t* node, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;

		// move the element in vec up the binary tree until the heap property
		// is satisfied
		size_t i = node - &(vec.front());
		size_t j = ANC(i);
		while (i != 0 && ((in_gh && vec[i] < vec[j]) ||
						  (!in_gh && vec[i] > vec[j]))) {
			swap_nodes(&(vec[i]), &(vec[j]));
			i = j;
			j = ANC(i);
		}
		return &(vec[i]);
	}

	uint32_t* down_helper(uint32_t* node, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;

		// move the element in vec down the binary tree until the heap property
		// is satisfied
		size_t i = node - &(vec.front());
		size_t j = DES1(i);
		size_t k = DES2(i);
		size_t const n = vec.size();
		while (true) {
			if (j >= n) {  // j & k are out of bounds
				return &(vec[i]);
			} else if (k >= n) {  // only k is out of bounds
				if ((in_gh && vec[j] < vec[i]) ||
					  (!in_gh && vec[j] > vec[i])) {
					swap_nodes(&(vec[i]), &(vec[j]));
					return &(vec[j]);
				}
				return &(vec[i]);
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
					swap_nodes(&(vec[i]), &(vec[z]));
				} else {
					return &(vec[i]);
				}

				// continue down the heap
				i = z;
				j = DES1(i);
				k = DES2(i);
			}
		}

		return nullptr;  // never reached
	}

	uint32_t* push_heap(uint32_t degree, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;
		vec.emplace_back(degree);
		return up_helper(&(vec.back()), in_gh);
	}

	void pop_heap(bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;
		erase(&(vec.front()), in_gh);
	}

	void rebalance() {
		ssize_t const size_diff = _lh.size() - _gh.size();
		if (size_diff == 2) {
			uint32_t degree = _lh.front();
			pop_heap(false);
			push_heap(degree, true);
		} else if (size_diff == -2) {
			uint32_t degree = _gh.front();
			pop_heap(true);
			push_heap(degree, false);
		}
	}

	void erase(uint32_t* node, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;
		size_t i = node - &(vec.front());
		size_t j = vec.size() - 1;
		swap_nodes(&(vec[i]), &(vec[j]));
		vec.pop_back();
		--_size;

		down_helper(&(vec[i]), in_gh);

		// re-balance heaps if needed to maintain median heap structure
		rebalance();
	}

	void decrease_key(uint32_t* node, bool in_gh) {
		--(*node);
		if (in_gh) {
			up_helper(node, true);
		} else {
			down_helper(node, false);
		}
	}

	void increase_key(uint32_t* node, bool in_gh) {
		++(*node);
		if (in_gh) {
			down_helper(node, true);
		} else {
			up_helper(node, false);
		}
	}

public:
	void insert(std::string name) {
		++_size;

		// prepare the heaps if they are empty
		if (_size == 1) {
			_gh.emplace_back(1);
			_fmap[name] = { &(_gh.back()), true };
			_bmap[&(_gh.back())] = std::move(name);
			return;
		} else if (_size == 2) {
			_lh.emplace_back(1);
			_fmap[name] = { &(_lh.back()), false };
			_bmap[&(_lh.back())] = std::move(name);
			return;
		}

		uint32_t* node;
		// insert into either the lesser or greater half
		if (1 > _lh.front()) {
			node = push_heap(1, true);
			_fmap[name] = { node, true };
			_bmap[node] = std::move(name);
		} else {
			node = push_heap(1, false);
			_fmap[name] = { node, false };
			_bmap[node] = std::move(name);
		}

		// re-balance heaps if needed to maintain median heap structure
		rebalance();
	}

	void erase(std::string const& name) {
		Info& info = _fmap[name];
		erase(info.node, info.in_gh);
	}

	void decrease_key(std::string const& name) {
		// Erase a vertex if has degree 1.
		// Otherwise, decrease its key.
		Info const& info = _fmap[name];
		if (*(info.node) == 1) {
			erase(info.node, info.in_gh);
		} else {
			decrease_key(info.node, info.in_gh);
		}
	}

	void increase_key(std::string name) {
		// Assume name exists. Increase its degree.
		Info const& info = _fmap[name];
		increase_key(info.node, info.in_gh);
	}

	bool process_edge(std::string name1, std::string name2) {
		std::unordered_map<std::string, Info>::const_iterator it1 =
			_fmap.find(name1);
		if (it1 != _fmap.cend()) {
			std::unordered_map<std::string, Info>::const_iterator it2 =
				_fmap.find(name2);
				if (it2 != _fmap.cend()) {
					return false;
				} else {
					Info const& info = it1->second;
					increase_key(info.node, info.in_gh);
					insert(std::move(name2));
					return true;
				}
		} else {
			insert(std::move(name1));
			std::unordered_map<std::string, Info>::const_iterator it2 =
				_fmap.find(name2);
			if (it2 != _fmap.cend()) {
				Info const& info = it2->second;
				increase_key(info.node, info.in_gh);
			} else {
				insert(std::move(name2));
			}
			return true;
		}
	}

	double median() const {
		if (empty()) {
			// raise error
		}

		ssize_t const size_diff = _lh.size() - _gh.size();
		// assert(int(abs(size_diff)) < 2);

		// _lh.size() > _gh.size()
		if (size_diff > 0) {
			return _lh.front();
		}
		// _lh.size() == _gh.size()
		else if (size_diff == 0) {
			return (_lh.front() + _gh.front()) / 2.0;
		}
		// _lh.size() < _gh.size()
		else {
			return _gh.front();
		}
	}

	/* Testing & Debugging */
	bool empty() const {
		return _size == 0;
	}

	size_t size() const {
		return _size;
	}

	size_t size_lh() const {
		return _lh.size();
	}

	size_t size_gh() const {
		return _gh.size();
	}

	uint64_t degree(std::string name) const {
		std::unordered_map<std::string, Info>::const_iterator found = 
			_fmap.find(name);
		return *((found->second).node);
	}

	uint64_t in_gh(std::string name) const {
		//return _fmap[name].in_gh;
		std::unordered_map<std::string, Info>::const_iterator found = 
			_fmap.find(name);
		return (found->second).in_gh;
	}

	bool contains(std::string name) const {
		return _fmap.count(name) != 0;
	}

};

#endif  // MED_HEAP_MAP_HPP_
