#include "victor/med_heap_map.hpp"
#include "gtest/gtest.h"
#include <math.h>


TEST(MedHeapMapTest, InsertWorks) {
	MedHeapMap med_heap;

	ASSERT_EQ(med_heap.size_lh(), 0);
	ASSERT_EQ(med_heap.size_gh(), 0);
	ASSERT_EQ(med_heap.size(), 0);

	med_heap.insert("Adam-West");
	ASSERT_EQ(med_heap.size_lh(), 0);
	ASSERT_EQ(med_heap.size_gh(), 1);
	ASSERT_EQ(med_heap.size(), 1);

	med_heap.insert("Professor-Oak");
	ASSERT_EQ(med_heap.size_lh(), 1);
	ASSERT_EQ(med_heap.size_gh(), 1);
	ASSERT_EQ(med_heap.size(), 2);

	med_heap.insert("Christina-Mitchens");
	ASSERT_LT(size_t(abs(med_heap.size_lh() - med_heap.size_gh())), 2) <<
		"Median Heap isn't balanced:\n  med_heap.size_lh() == " <<
		med_heap.size_lh() << "\n  med_heap.size_gh() == " <<
		med_heap.size_gh();
	ASSERT_EQ(med_heap.size(), 3);

	med_heap.insert("Hillary-Clinton");
	ASSERT_LT(size_t(abs(med_heap.size_lh() - med_heap.size_gh())), 2) <<
		"Median Heap isn't balanced:\n  med_heap.size_lh() == " <<
		med_heap.size_lh() << "\n  med_heap.size_gh() == " <<
		med_heap.size_gh();
	ASSERT_EQ(med_heap.size(), 4);

	med_heap.insert("Benjamin-Button");
	ASSERT_LT(size_t(abs(med_heap.size_lh() - med_heap.size_gh())), 2) <<
		"Median Heap isn't balanced:\n  med_heap.size_lh() == " <<
		med_heap.size_lh() << "\n  med_heap.size_gh() == " <<
		med_heap.size_gh();
	ASSERT_EQ(med_heap.size(), 5);

	med_heap.insert("Charlie-bitmyfinger-Unicorn");
	ASSERT_LT(size_t(abs(med_heap.size_lh() - med_heap.size_gh())), 2) <<
		"Median Heap isn't balanced:\n  med_heap.size_lh() == " <<
		med_heap.size_lh() << "\n  med_heap.size_gh() == " <<
		med_heap.size_gh();
	ASSERT_EQ(med_heap.size(), 6);

	med_heap.insert("Hilnold-Trumpton");
	ASSERT_LT(size_t(abs(med_heap.size_lh() - med_heap.size_gh())), 2) <<
		"Median Heap isn't balanced:\n  med_heap.size_lh() == " <<
		med_heap.size_lh() << "\n  med_heap.size_gh() == " <<
		med_heap.size_gh();
	ASSERT_EQ(med_heap.size(), 7);

	med_heap.insert("Shaggy");
	ASSERT_LT(size_t(abs(med_heap.size_lh() - med_heap.size_gh())), 2) <<
		"Median Heap isn't balanced:\n  med_heap.size_lh() == " <<
		med_heap.size_lh() << "\n  med_heap.size_gh() == " <<
		med_heap.size_gh();
	ASSERT_EQ(med_heap.size(), 8);

	EXPECT_EQ(med_heap.degree("Adam-West"), 1);
	EXPECT_EQ(med_heap.degree("Adam-West"), 1);
	EXPECT_EQ(med_heap.degree("Professor-Oak"), 1);
	EXPECT_EQ(med_heap.degree("Christina-Mitchens"), 1);
	EXPECT_EQ(med_heap.degree("Hillary-Clinton"), 1);
	EXPECT_EQ(med_heap.degree("Benjamin-Button"), 1);
	EXPECT_EQ(med_heap.degree("Charlie-bitmyfinger-Unicorn"), 1);
	EXPECT_EQ(med_heap.degree("Hilnold-Trumpton"), 1);
	EXPECT_EQ(med_heap.degree("Shaggy"), 1);
}

TEST(MedHeapMapTest, IncreaseKeyWorks) {
	MedHeapMap med_heap;
	size_t old_size;

	med_heap.insert("Adam-West");
	med_heap.insert("Professor-Oak");

	old_size = med_heap.size();
	med_heap.increase_key("Adam-West");
	ASSERT_LT(size_t(abs(med_heap.size_lh() - med_heap.size_gh())), 2) <<
		"Median Heap isn't balanced:\n  med_heap.size_lh() == " <<
		med_heap.size_lh() << "\n  med_heap.size_gh() == " <<
		med_heap.size_gh();
	ASSERT_EQ(old_size, med_heap.size()) << "increase_key() modified size";
	med_heap.insert("Christina-Mitchens");

	old_size = med_heap.size();
	med_heap.increase_key("Christina-Mitchens");
	ASSERT_LT(size_t(abs(med_heap.size_lh() - med_heap.size_gh())), 2) <<
		"Median Heap isn't balanced:\n  med_heap.size_lh() == " <<
		med_heap.size_lh() << "\n  med_heap.size_gh() == " <<
		med_heap.size_gh();
	ASSERT_EQ(old_size, med_heap.size()) << "increase_key() modified size";
	med_heap.insert("Hillary-Clinton");

	old_size = med_heap.size();
	med_heap.increase_key("Adam-West");
	ASSERT_LT(size_t(abs(med_heap.size_lh() - med_heap.size_gh())), 2) <<
		"Median Heap isn't balanced:\n  med_heap.size_lh() == " <<
		med_heap.size_lh() << "\n  med_heap.size_gh() == " <<
		med_heap.size_gh();
	ASSERT_EQ(old_size, med_heap.size()) << "increase_key() modified size";
	med_heap.increase_key("Hillary-Clinton");

	EXPECT_EQ(med_heap.degree("Adam-West"), 3);
	EXPECT_EQ(med_heap.degree("Professor-Oak"), 1);
	EXPECT_EQ(med_heap.degree("Christina-Mitchens"), 2);
	EXPECT_EQ(med_heap.degree("Hillary-Clinton"), 2);
}

TEST(MedHeapMapTest, EraseWorks) {
	MedHeapMap med_heap;
	med_heap.insert("Adam-West");
	med_heap.insert("Professor-Oak");
	med_heap.insert("Christina-Mitchens");
	med_heap.insert("Hillary-Clinton");
	med_heap.insert("Benjamin-Button");
	med_heap.insert("Charlie-bitmyfinger-Unicorn");
	med_heap.insert("Hilnold-Trumpton");
	med_heap.insert("Shaggy");

	med_heap.erase("Benjamin-Button");
	ASSERT_EQ(med_heap.size(), 7) <<
		"med_heap tried to delete 1 node from 8, expecting 7, "
		"but there remained " <<
		med_heap.size() << " nodes.";
	ASSERT_FALSE(med_heap.contains("Benjamin-Button")) <<
		"Benjamin-Button was erased, but it still shows up in "
		"med_heap.";

	med_heap.erase("Shaggy");
	ASSERT_EQ(med_heap.size(), 6) <<
		"med_heap tried to delete 1 node from 7, expecting 6, "
		"but there remained " <<
		med_heap.size() << " nodes.";

	med_heap.erase("Professor-Oak");
	ASSERT_EQ(med_heap.size(), 5) <<
		"med_heap tried to delete 1 node from 6, expecting 5, "
		"but there remained " <<
		med_heap.size() << " nodes.";

	med_heap.erase("Adam-West");
	ASSERT_EQ(med_heap.size(), 4) <<
		"med_heap tried to delete 1 node from 5, expecting 4, "
		"but there remained " <<
		med_heap.size() << " nodes.";

	med_heap.erase("Hilnold-Trumpton");
	ASSERT_EQ(med_heap.size(), 3) <<
		"med_heap tried to delete 1 node from 4, expecting 3, "
		"but there remained " <<
		med_heap.size() << " nodes.";

	med_heap.erase("Charlie-bitmyfinger-Unicorn");
	ASSERT_EQ(med_heap.size(), 2) <<
		"med_heap tried to delete 1 node from 3, expecting 2, "
		"but there remained " <<
		med_heap.size() << " nodes.";

	med_heap.erase("Christina-Mitchens");
	ASSERT_EQ(med_heap.size(), 1) <<
		"med_heap tried to delete 1 node from 2, expecting 1, "
		"but there remained " <<
		med_heap.size() << " nodes.";

	med_heap.erase("Hillary-Clinton");
	ASSERT_EQ(med_heap.size(), 0) <<
		"med_heap tried to delete 1 node from 1, expecting 0, "
		"but there remained " <<
		med_heap.size() << " nodes.";
}

TEST(MedHeapMapTest, DecreaseKeyWorks) {
	MedHeapMap med_heap;
	med_heap.insert("Adam-West");
	med_heap.insert("Professor-Oak");

	med_heap.insert("Christina-Mitchens");
	med_heap.insert("Hillary-Clinton");

	med_heap.increase_key("Adam-West");
	med_heap.insert("Benjamin-Button");

	med_heap.increase_key("Professor-Oak");
	med_heap.increase_key("Benjamin-Button");

	med_heap.increase_key("Professor-Oak");
	med_heap.insert("Charlie-bitmyfinger-Unicorn");

	med_heap.insert("Hilnold-Trumpton");
	med_heap.insert("Shaggy");

	med_heap.increase_key("Benjamin-Button");
	med_heap.increase_key("Hilnold-Trumpton");

	med_heap.decrease_key("Christina-Mitchens");
	med_heap.decrease_key("Hillary-Clinton");
	ASSERT_FALSE(med_heap.contains("Christina-Mitchens")) <<
		"Erased edge between Christina-Mitchens and Hillary-Clinton, "
		"and this was the only edge Christina-Mitchens was connected to "
		"but was still not erased."
	ASSERT_FALSE(med_heap.contains("Hillary-Clinton")) <<
		"Erased edge between Christina-Mitchens and Hillary-Clinton, "
		"and this was the only edge Hillary-Clinton was connected to "
		"but was still not erased."

	med_heap.decrease_key("Benjamin-Button");
	med_heap.decrease_key("Hilnold-Trumpton");
	ASSERT_TRUE(med_heap.contains("Benjamin-Button")) <<
		"Erased edge between Benjamin-Button and Hilnold-Trumpton, "
		"but Benjamin-Button still had other edges and "
		"ended up being erased.";
	ASSERT_TRUE(med_heap.contains("Hilnold-Trumpton")) <<
		"Erased edge between Benjamin-Button and Hilnold-Trumpton, "
		"but Hilnold-Trumpton still had other edges and "
		"ended up being erased.";

	med_heap.decrease_key("Hilnold-Trumpton");
	med_heap.decrease_key("Shaggy");
	ASSERT_FALSE(med_heap.contains("Hilnold-Trumpton")) <<
		"Erased edge between Hilnold-Trumpton and Shaggy, "
		"and this was the only edge Hilnold-Trumpton was connected to "
		"but was still not erased."
	ASSERT_FALSE(med_heap.contains("Shaggy")) <<
		"Erased edge between Hilnold-Trumpton and Shaggy, "
		"and this was the only edge Shaggy was connected to "
		"but was still not erased."

	med_heap.decrease_key("Adam-West");
	med_heap.decrease_key("Benjamin-Button");
	ASSERT_EQ(med_heap.degree("Professor-Oak"), 2) <<
		"Deletion of edge between Adam-West and Benjamin-Button shouldn't "
		"affect degree of Professor-Oak. It should have degree 3 "
		"but instead has degree " << med_heap.degree("Professor-Oak") << '.';

	med_heap.decrease_key("Charlie-bitmyfinger-Unicorn");
	ASSERT_FALSE(med_heap.contains("Charlie-bitmyfinger-Unicorn")) <<
		"A node with degree 1 was not deleted when its key was decreased.";
}

TEST(MedHeapMapTest, MedianWorks) {
	MedHeapMap med_heap;
	med_heap.insert("A");
	med_heap.insert("B");
	ASSERT_EQ(int(med_heap.median()), 1);

	med_heap.insert("C");
	med_heap.insert("D");
	ASSERT_EQ(int(med_heap.median()), 1);

	med_heap.increase_key("B");
	med_heap.insert("E");
	ASSERT_EQ(int(med_heap.median()), 1);

	med_heap.increase_key("A");
	med_heap.increase_key("C");
	ASSERT_EQ(int(med_heap.median()), 2);

	med_heap.increase_key("A");
	med_heap.increase_key("E");
	ASSERT_EQ(int(med_heap.median()), 2);

	med_heap.decrease_key("A");
	med_heap.decrease_key("B");
	ASSERT_EQ(int(med_heap.median()), 2);	

	med_heap.decrease_key("A");
	med_heap.decrease_key("E");
	ASSERT_EQ(int(med_heap.median()), 1);

	med_heap.decrease_key("A");
	med_heap.decrease_key("C");
	ASSERT_EQ(int(med_heap.median()), 1);
}
