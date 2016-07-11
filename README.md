# Table of Contents

1. [Description](README.md#description)
2. [Complexity](README.md#complexity)
3. [Environment](README.md#environment)
4. [Installation and Usage](README.md#installation-and-usage)

## Description

This challenge can be seen as a complicated version of computing the median of online/streaming data. An efficient solution would be to use something called a Median Heap structure. Each new data element is pushed into one of two heaps:

1. "lesser-half," a max-heap which contains elements less than or equal to the median.
2. "greater-half," a min-heap which contains elements greater than or equal to the median.

As more data is streamed in, both halves get bigger and bigger, but the median is easily accessible because it is calculated as either:

1. the top of the "lesser-half" max-heap, if the "lesser-half" has more elements than the "greater-half."
2. the top of the "greater-half" min-heap, if the "greater-half" has more elements than the "lesser-half."
3. the average of the top of both heaps, if both heaps have the same number of elements.

The median is invalid when the top of the "lesser-half" is greater than the top of the "greater-half," since the "lesser-half" is no longer a _lesser_ half. An invalid median also occurs when one heap is bigger than the other, by 2 or more elements. This is because the "lesser" or "greater" half no longer contains _half_ of all the elements.

These violations can occur not only when new elements are added to the heap, but also when elements are erased, incremented, or decremented. Such is the situation of this challenege, where Venmo payment edges are unpredictably inserted, expired, or updated with new timestamps.

This solution extends the Median Heap structure with heap deletion, increase-key, decrease-key, and balancing functionalities to deal with the aforementioned problems. It is resilient to malformed data and can scale well with data.


## Complexity

The solution has a worst-case time complexity of `O(N log N)`, where `N` is the total number of elements in the input stream. Average complexity is `O(N log C(N, t))`, where `C(N, t)` is usually smaller than `N`, depending on the size of the time expiration window and frequency of the Venmo transactions. The logarithmic factor is due to the tree structure of the data containers utilized in the solution.

Space complexity is linear in the current number of vertices and edges in the graph (`O(V + E)`).


## Environment

I used the following environment:

* Ubuntu 14.04
* GNU Make 3.81
* g++ 4.8.4 (C++11)
* [JSON for Modern C++](https://github.com/nlohmann/json) (v2.0.1) (already included in `src`)
* [googletest](https://github.com/google/googletest) (Master commit: `d406cb1`) (already included in `insight_testsuite`)
* [cpplint](https://github.com/google/styleguide/tree/gh-pages/cpplint) (already included in `.`)
* t2.medium instance on AWS EC2


## Installation and Usage

1. At the root directory, run `make rolling_median` to build the executable. Optionally run `make test` to run the googletest unit tests.
2. Run `./rolling_median <input filename> <output filename>`. Alternatively, `cd` into `insight_testsuite` and run `./run_tests.sh` to test `rolling_median` on your own test data. Feel free to add your own tests.

Note: `insight_testsuite/test_victor` contains the googletest unit tests. These unit tests don't use text files to validate output against, but they still test the code against sample data the code may see in a real setting. I apologize if this is not in accordance with the _required_ repo directory structure.

Note: Please read the header files located in `src/victor`. They contain documentation on the implementation details and public API of the code used.