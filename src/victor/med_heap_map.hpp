#ifndef MED_HEAP_MAP_HPP_
#define MED_HEAP_MAP_HPP_

#include <vector>
#include <unordered_map>
#include <string>

// #include <iostream>
// using std::cout;
// using std::endl;
#include <sstream>

// ANC, abbreviation of ANCESTOR for binary heaps
#define ANC(i) ((i - 1) >> 1)

// DES1, abbreviation of DESCENDENT1 for binary heaps
#define DES1(i) ((i >> 1) + 1)

// DES2, abbreviation of DESCENDENT2 for binary heaps
#define DES2(i) ((i >> 1) + 2)


class MedHeapMap {
private:
	struct FInfo {
		size_t ind;
		bool in_gh;
	};

	struct BInfo {
		std::string g_key;
		std::string l_key;
	};

	std::vector<uint32_t> _lh, _gh;
	std::unordered_map<std::string, FInfo> _fmap;
	std::unordered_map<size_t, BInfo> _bmap;
	size_t _size = 0;

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

	void up_helper(size_t i, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;

		// move the element in vec up the binary tree until the heap property
		// is satisfied
		size_t j = ANC(i);
		while (i != 0 && ((in_gh && vec[i] < vec[j]) ||
						  (!in_gh && vec[i] > vec[j]))) {
			swap_nodes(i, j, in_gh);
			i = j;
			j = ANC(i);
		}
	}

	void down_helper(size_t i, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;

		// move the element in vec down the binary tree until the heap property
		// is satisfied
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
			down_helper(i, true);
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
			down_helper(i, false);
		}

		// re-balance heaps if needed to maintain median heap structure
		rebalance();
	}

	void rotate(bool into_gh) {
		// Pop the top of one heap and push it into the other heap.
		// Manage the maps accordingly.
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

			down_helper(0, false);
			up_helper(_gh.size() - 1, true);
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

			down_helper(0, true);
			up_helper(_lh.size() - 1, false);
		}
	}

	void rebalance() {
		// If size differs by 2, transfer the top of one heap
		// into the other heap.
		ssize_t const size_diff = _lh.size() - _gh.size();
		if (size_diff >= 2) {
			rotate(true);
		} else if (size_diff <= -2) {
			rotate(false);
		}
	}

	void increase_key(size_t i, bool in_gh) {
		// If key in gh, increment it and down_helper.
		// If key in lh, increment it and up_helper.
		if (in_gh) {
			++(_gh[i]);
			down_helper(i, true);
		} else {
			++(_lh[i]);
			up_helper(i, false);
			// If lh.top() > gh.top(), rotate lh into gh.
			// Re-balance if necessary.
			if (_lh.front() > _gh.front()) {
				rotate(true);
				rebalance();
			}
		}
	}

	void decrease_key(size_t i, bool in_gh) {
		// If key in lh, decrement it and down_helper.
		// If key in gh, decrement it and up_helper.
		if (in_gh) {
			--(_gh[i]);
			up_helper(i, true);
			// If gh.top() < lh.top(), rotate gh into lh.
			// Re-balance if necessary.
			if (_gh.front() < _lh.front()) {
				rotate(false);
				rebalance();
			}
		} else {
			--(_lh[i]);
			down_helper(i, false);
		}
	}

public:
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
		// Always insert into the greater half.
		// Let rebalance() handle the shuffling from gh to lh.
		_gh.emplace_back(1);
		_fmap[name] = { _gh.size() - 1, true };
		_bmap[_gh.size() - 1].g_key = std::move(name);
		up_helper(_gh.size() - 1, true);

		// re-balance heaps if needed to maintain median heap structure
		rebalance();
	}

	void erase(std::string const& name) {
		FInfo const& info = _fmap[name];
		erase(info.ind, info.in_gh);
	}

	void increase_key(std::string name) {
		// Assume name exists. Increase its degree.
		FInfo const& info = _fmap[name];
		increase_key(info.ind, info.in_gh);
	}

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
		return size() == 0;
	}

	size_t size() const {
		return _lh.size() + _gh.size();
	}

	size_t size_lh() const {
		return _lh.size();
	}

	size_t size_gh() const {
		return _gh.size();
	}

	uint64_t degree(std::string name) const {
		std::unordered_map<std::string, FInfo>::const_iterator found = 
			_fmap.find(name);
		if ((found->second).in_gh) {
			return _gh[(found->second).ind];
		} else {
			return _lh[(found->second).ind];
		}
	}

	bool in_gh(std::string name) const {
		std::unordered_map<std::string, FInfo>::const_iterator found = 
			_fmap.find(name);
		return (found->second).in_gh;
	}

	bool contains(std::string name) const {
		return _fmap.count(name) != 0;
	}

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
