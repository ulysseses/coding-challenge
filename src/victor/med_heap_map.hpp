#ifndef MED_HEAP_MAP_HPP_
#define MED_HEAP_MAP_HPP_

#include <vector>
#include <unordered_map>
#include <string>

// #include <iostream>
// using std::cout;
// using std::endl;

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
			_fmap[_bmap[j].g_key].ind = i;
			_fmap[_bmap[i].g_key].ind = j;
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

	void heap_erase(size_t i, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;
		size_t j = vec.size() - 1;
		swap_nodes(i, j, in_gh);
		vec.pop_back();

		down_helper(i, in_gh);
	}

	void map_erase(bool in_gh) {
		// If called, must be called immediately after heap_erase
		if (in_gh) {
			size_t i = _gh.size();
			BInfo& info = _bmap[i];
			_fmap.erase(info.g_key);
			if (info.l_key.empty()) {
				_bmap.erase(i);
			} else {
				info.g_key = "";
			}
		} else {
			size_t i = _lh.size();
			BInfo& info = _bmap[i];
			_fmap.erase(info.g_key);
			if (info.g_key.empty()) {
				_bmap.erase(i);
			} else {
				info.l_key = "";
			}
		}
	}

	void rebalance() {
		// If size differs by 2, transfer the top of one heap
		// into the other heap.
		// Manage the maps accordingly.
		ssize_t const size_diff = _lh.size() - _gh.size();
		if (size_diff == 2) {
			std::string l_key = std::move(_bmap[0].l_key);
			auto& it = _fmap[l_key];
			it.ind = _gh.size();
			it.in_gh = true;
			_bmap[_gh.size()].g_key = std::move(l_key);
			if (_bmap[0].g_key.empty()) {
				_bmap.erase(0);
			}

			uint32_t degree = _lh.front();
			heap_erase(0, false);
			_gh.emplace_back(degree);
			up_helper(_gh.size() - 1, true);
		} else if (size_diff == -2) {
			std::string g_key = std::move(_bmap[0].g_key);
			auto& it = _fmap[g_key];
			it.ind = _lh.size();
			it.in_gh = false;
			_bmap[_lh.size()].l_key = std::move(g_key);
			if (_bmap[0].l_key.empty()) {
				_bmap.erase(0);
			}

			uint32_t degree = _gh.front();
			heap_erase(0, true);
			_lh.emplace_back(degree);
			up_helper(_lh.size() - 1, false);
		}
	}

	void decrease_key(size_t i, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;
		--(vec[i]);
		if (in_gh) {
			up_helper(i, true);
		} else {
			down_helper(i, false);
		}
	}

	void increase_key(size_t i, bool in_gh) {
		std::vector<uint32_t>& vec = in_gh ? _gh : _lh;
		++(vec[i]);
		if (in_gh) {
			down_helper(i, true);
		} else {
			up_helper(i, false);
		}
	}

public:
	void insert(std::string name) {
		++_size;
		// prepare the heaps if they are empty
		if (_size == 1) {
			_gh.emplace_back(1);
			_fmap[name] = { _gh.size() - 1, true };
			_bmap[_gh.size() - 1].g_key = std::move(name);
			return;
		} else if (_size == 2) {
			_lh.emplace_back(1);
			_fmap[name] = { _lh.size() - 1, false };
			_bmap[_lh.size() - 1].l_key = std::move(name);
			return;
		}

		// insert into either the lessor or greater half
		if (1 >= _lh.front()) {
			_gh.emplace_back(1);
			_fmap[name] = { _gh.size() - 1, true };
			_bmap[_gh.size() - 1].g_key = std::move(name);
			up_helper(_gh.size() - 1, true);
		} else {
			_lh.emplace_back(1);
			_fmap[name] = { _lh.size() - 1, false };
			_bmap[_lh.size() - 1].l_key = std::move(name);
			up_helper(_lh.size() - 1, false);
		}

		// re-balance heaps if needed to maintain median heap structure
		rebalance();
	}

	void erase(std::string const& name) {
		--_size;
		FInfo const& info = _fmap[name];
		heap_erase(info.ind, info.in_gh);
		map_erase(info.in_gh);

		// re-balance heaps if needed to maintain median heap structure
		rebalance();
	}

	void increase_key(std::string name) {
		// Assume name exists. Increase its degree.
		FInfo const& info = _fmap[name];
		increase_key(info.ind, info.in_gh);
	}

	bool process_edge(std::string name1, std::string name2) {
		std::unordered_map<std::string, FInfo>::const_iterator it1 =
			_fmap.find(name1);
		if (it1 != _fmap.cend()) {
			std::unordered_map<std::string, FInfo>::const_iterator it2 =
				_fmap.find(name2);
				if (it2 != _fmap.cend()) {
					return false;
				} else {
					FInfo const& info = it1->second;
					increase_key(info.ind, info.in_gh);
					insert(std::move(name2));
					return true;
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
			return true;
		}
	}

	void decrease_key(std::string const& name) {
		// Erase a vertex if has degree 1.
		// Otherwise, decrease its key.
		FInfo const& info = _fmap[name];
		if (info.in_gh) {
			if (_gh[info.ind] == 1) {
				heap_erase(info.ind, info.in_gh);
				map_erase(info.in_gh);
			} else {
				decrease_key(info.ind, info.in_gh);
			}
		} else {
			if (_lh[info.ind] == 1) {
				heap_erase(info.ind, info.in_gh);
				map_erase(info.in_gh);
			} else {
				decrease_key(info.ind, info.in_gh);
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

};

#endif  // MED_HEAP_MAP_HPP_
