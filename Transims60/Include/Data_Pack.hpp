//*********************************************************
//	Data_Pack.hpp - mpi data packing/unpacking
//*********************************************************

#ifndef DATA_PACK_HPP
#define DATA_PACK_HPP

#include "Data_Buffer.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
using namespace std;

//---------------------------------------------------------
//	Pack_File Class definition
//---------------------------------------------------------

class Pack_File
{
public:
	Pack_File (void) {};
	Pack_File (string filename, Access_Type access = READ)     { Open (filename, access); }

	bool Open (string filename, Access_Type access = READ)
	{
		file.clear ();
		if (access == CREATE) {
			file.open (filename.c_str (), ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary);
		} else {
			file.open (filename.c_str (), ios_base::in | ios_base::binary);
		}
		return (file.is_open ());
	}
	bool  Read (void *record, size_t size)
	{ 
		if (!file.is_open () || record == 0 || size == 0) return (false);
		return (!file.read ((char *) record, size).fail ());
	}
	bool  Write (void *record, size_t size)
	{
		if (!file.is_open () || record == 0 || size == 0) return (false);
		return (!file.write ((char *) record, size).fail ());
	}
	void  Close (void)   { file.close (); }
private:
	fstream file;
};

//---------------------------------------------------------
//	Vector
//---------------------------------------------------------

template <typename Type>
class Vector : public vector <Type>
{
public:
	Vector (void) {}

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
				vector <Type>::clear ();
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
	bool Pack (Pack_File &file, bool type_flag = false)
	{
		size_t num = vector <Type>::size ();
		if (file.Write (&num, sizeof (num))) {
			if (num == 0) return (true);
			if (type_flag) {
				num *= sizeof (Type);
				return (file.Write (&vector <Type>::at (0), num));
			} else {
				typename vector <Type>::iterator itr;
				for (itr=vector <Type>::begin (); itr != vector <Type>::end (); itr++) {
					if (!itr->Pack (file)) return (false);
				}
				return (true);
			}
		}
		return (false);
	}
	bool UnPack (Pack_File &file, bool type_flag = false)
	{
		size_t num;
		if (file.Read (&num, sizeof (num))) {
			Type rec;
			if (type_flag) {
				vector <Type>::assign (num, rec);
				num *= sizeof (rec);
				return (file.Read (&vector <Type>::at (0), num));
			} else {
				vector <Type>::clear ();
				vector <Type>::reserve (num);
				for (int i=0; i < (int) num; i++) {
					if (!rec.UnPack (file)) return (false);
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

	bool Pack (Data_Buffer &data, bool type_flag = false)
	{
		size_t num = deque <Type>::size ();
		if (data.Add_Data (&num, sizeof (num))) {
			if (num == 0) return (true);
			typename deque <Type>::iterator itr;
			for (itr=deque <Type>::begin (); itr != deque <Type>::end (); itr++) {
				if (type_flag) {
					if (!data.Add_Data (*itr, sizeof (Type))) return (false);
				} else {
					if (!itr->Pack (data)) return (false);
				}
			}
			return (true);
		}
		return (false);
	}
	bool UnPack (Data_Buffer &data, bool type_flag = false)
	{
		size_t num;
		if (data.Get_Data (&num, sizeof (num))) {
			Type rec;
			deque <Type>::clear ();
			for (int i=0; i < (int) num; i++) {
				if (type_flag) {
					if (!data.Get_Data (&rec, sizeof (rec))) return (false);;
				} else {
					if (!rec.UnPack (data)) return (false);
				}
				deque <Type>::push_back (rec);
			}
			return (true);
		}
		return (false);
	}
	bool Pack (Pack_File &file, bool type_flag = false)
	{
		size_t num = deque <Type>::size ();
		if (file.Write (&num, sizeof (num))) {
			if (num == 0) return (true);
			typename deque <Type>::iterator itr;
			for (itr=deque <Type>::begin (); itr != deque <Type>::end (); itr++) {
				if (type_flag) {
					if (!file.Write (&(*itr), sizeof (Type))) return (false);
				} else {
					if (!itr->Pack (file)) return (false);
				}
			}
			return (true);
		}
		return (false);
	}
	bool UnPack (Pack_File &file, bool type_flag = false)
	{
		size_t num;
		if (file.Read (&num, sizeof (num))) {
			Type rec;
			deque <Type>::clear ();
			for (int i=0; i < (int) num; i++) {
				if (type_flag) {
					if (!file.Read (&rec, sizeof (rec))) return (false);
				} else {
					if (!rec.UnPack (file)) return (false);
				}
				deque <Type>::push_back (rec);
			}
			return (true);
		}
		return (false);
	}
};

#endif
