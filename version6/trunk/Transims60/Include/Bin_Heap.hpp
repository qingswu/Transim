//*********************************************************
//	Bin_Heap.hpp - binary heap -- ordered queue
//*********************************************************

#ifndef BIN_HEAP_HPP
#define BIN_HEAP_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Binary Heap Class definition
//---------------------------------------------------------

template <typename Type>
class SYSLIB_API Bin_Heap
{
	typedef vector <Type> Type_Array;

public:
	//---------------------------------------------------------
	//	Bin_Heap constructor
	//---------------------------------------------------------

	Bin_Heap (int size = 0)
	{
		Initialize (size);
	}

	//---------------------------------------------------------
	//	Initialize
	//---------------------------------------------------------

	void Initialize (int size)
	{
		Reset ();

		if (size > 0) {
			Type _id;
			size++;
			list.assign (size, 0);
			id.assign (size, _id);
			value.assign (size, 0);
		}
	}

	//---------------------------------------------------------
	//	Reset
	//---------------------------------------------------------

	void Reset (void)
	{
		id_index = list_size = last_index = 0;
	}

	//---------------------------------------------------------
	//	Insert
	//---------------------------------------------------------

	bool Insert (Type _id, unsigned _value)
	{
		if ((list_size + 1) >= Max_Size ()) return (false);

		int index, index2, list1, list2;

		if (last_index == 0) {
			if ((id_index + 1) >= (int) id.size ()) return (false);
			index = ++id_index;
		} else {
			index = last_index;
			last_index = 0;
		}
		list [++list_size] = index;
		id [index] = _id;
		value [index] = _value;

		for (index = list_size; index != 1; index = index2) {
			list1 = list [index];
			index2 = index / 2;
			list2 = list [index2];

			if (value [list1] > value [list2]) break;

			list [index2] = list1;
			list [index] = list2;
		}
		return (true);
	}

	//---------------------------------------------------------
	//	Update
	//---------------------------------------------------------

	bool Update (Type _id, unsigned _value)
	{
		int index, index2, list1, list2;

		for (index = 1; index <= list_size; index++) {
			list1 = list [index];
			if (id [list1] != _id) continue;

			value [list1] = _value;

			for (; index != 1; index = index2) {
				list1 = list [index];
				index2 = index / 2;
				list2 = list [index2];

				if (value [list1] > value [list2]) break;

				list [index2] = list1;
				list [index] = list2;
			}
			return (true);
		}
		return (false);
	}

	//---------------------------------------------------------
	//	Remove
	//---------------------------------------------------------

	bool Remove (Type &return_id)
	{
		if (list_size == 0 || (int) list.size () < 2) return (false);

		int i, j, j2, j3;

		last_index = list [1];
		return_id = id [last_index];

		list [1] = list [list_size--];

		for (i = 1; ; ) {
			j = i;
			j2 = j * 2;
			j3 = j2 + 1;

			if (j3 <= list_size) {
				if (value [list [j]] >= value [list [j2]]) {
					i = j2;
				}
				if (value [list [i]] >= value [list [j3]]) {
					i = j3;
				}
			} else if (j2 <= list_size) {
				if (value [list [j]] >= value [list [j2]]) {
					i = j2;
				}
			}
			if (i == j) break;

			j2 = list [j];
			list [j] = list [i];
			list [i] = j2;
		}
		return (true);
	}

	//---------------------------------------------------------
	//	Max_Size
	//---------------------------------------------------------

	int  Max_Size (void)
	{
		return ((int) list.size ());
	}

	//---------------------------------------------------------
	//	List_Size
	//---------------------------------------------------------

	int  List_Size (void)
	{
		return (list_size);
	}

private:
	int  id_index, list_size, last_index;

	Integers list;
	Type_Array id;
	Unsigns value;
};

typedef Bin_Heap <int> Int_Heap;

#endif
