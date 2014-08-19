//*********************************************************
//	Data_Pack.hpp - mpi data packing/unpacking
//*********************************************************

#ifndef DATA_PACK_HPP
#define DATA_PACK_HPP

#include "Data_Buffer.hpp"

#include <vector>
#include <deque>
using namespace std;

//---------------------------------------------------------
//	Vector
//---------------------------------------------------------

template <typename Type>
class Vector : public vector <Type>
{
public:
	Vector (void) {}

	void Free (void) 
	{
		vector <Type> temp;
		swap (temp);
	}
	bool Pack (Data_Buffer &data, bool type_flag = false)
	{
		size_t num = vector <Type>::size ();
		if (data.Add_Data (&num, sizeof (num))) {
			if (num == 0) return (true);
			if (type_flag) {
				num *= sizeof (Type);
				return (data.Add_Data (&vector <Type>::at (0), num));
			} else {
				typename vector <Type>::iterator itr;
				for (itr=vector <Type>::begin (); itr != vector <Type>::end (); itr++) {
					if (!itr->Pack (data)) return (false);
				}
				return (true);
			}
		}
		return (false);
	}
	bool UnPack (Data_Buffer &data, bool type_flag = false)
	{
		size_t num;
		if (data.Get_Data (&num, sizeof (num))) {
			Type rec;
			if (type_flag) {
				vector <Type>::assign (num, rec);
				num *= sizeof (rec);
				return (data.Get_Data (&vector <Type>::at (0), num));
			} else {
				vector <Type>::reserve (num);
				for (int i=0; i < (int) num; i++) {
					if (!rec.UnPack (data)) return (false);
					vector <Type>::push_back (rec);
				}
				return (true);
			}
		}
		return (false);
	}
};

//---------------------------------------------------------
//	Deque
//---------------------------------------------------------

template <typename Type>
class Deque : public deque <Type>
{
public:
	Deque (void) {}

	void Free (void) 
	{
		deque <Type> temp;
		swap (temp);
	}
	bool Pack (Data_Buffer &data)
	{
		size_t num = deque <Type>::size ();
		if (data.Add_Data (&num, sizeof (num))) {
			if (num == 0) return (true);
			typename deque <Type>::iterator itr;
			for (itr=deque <Type>::begin (); itr != deque <Type>::end (); itr++) {
				if (!itr->Pack (data)) return (false);
			}
			return (true);
		}
		return (false);
	}
	bool UnPack (Data_Buffer &data)
	{
		size_t num;
		if (data.Get_Data (&num, sizeof (num))) {
			Type rec;
			for (int i=0; i < (int) num; i++) {
				if (!rec.UnPack (data)) return (false);
				deque <Type>::push_back (rec);
			}
			return (true);
		}
		return (false);
	}
};

#endif
