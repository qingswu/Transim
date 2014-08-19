//*********************************************************
//	Memory_Pool.hpp - reusable memory allocated records
//*********************************************************

#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include "Data_Pack.hpp"

//---------------------------------------------------------
//	Memory_Record class definition
//---------------------------------------------------------

class Memory_Record
{
public:
	Memory_Record (void)              { Clear (); }

	int  Next_Record (void)           { return (next_rec); }

	void Next_Record (int value)      { next_rec = value; }
	
	void Clear (void)                 { next_rec = -1; }

	bool Pack (Data_Buffer &data)     { return (data.Add_Data (this, sizeof (*this))); }
	bool UnPack (Data_Buffer &data)   { return (data.Get_Data (this, sizeof (*this))); }

private:
	int   next_rec;
};

//---------------------------------------------------------
//	Memory_Pool class definition
//---------------------------------------------------------

template <typename Type>
class Memory_Pool : public Vector <Type>
{
public:
	Memory_Pool (void)           { Clear (); }

	Type Get_Record (int index)
	{
		return (at (index));
	}
	int  Put_Record (Type &rec, int from_index = -1)
	{
		int rec_id;
		rec.Next_Record (-1);	//---- end of list ----

		if (free_count == 0) {
			rec_id = (int) size ();
			push_back (rec);
		} else {
			rec_id = first_free;
			at (rec_id) = rec;
			if (--free_count > 0) {
				typename Vector <Type>::iterator itr;
				for (itr = begin () + first_free; itr != end (); itr++, first_free++) {
					if (itr->Next_Record () == -2) break;	//---- free record ----
				}
			}
		}
		if (from_index >= 0) {
			Type *rec_ptr = Record_Pointer (from_index);
			rec_ptr->Next_Record (rec_id);
		}
		return (rec_id);
	}
	int  Free_Record (Type *rec_ptr)
	{
		int next_rec = rec_ptr->Next_Record ();
		rec_ptr->Next_Record (-2);		//---- mark as free ----
		return (next_rec);
	}
	int  Free_Record (int index)
	{
		return (Free_Record (Record_Pointer (index)));
	}
	void Update_Record (Type &rec, int index)
	{
		Type *rec_ptr = Record_Pointer (index);
		rec.Next_Record (rec_ptr->Next_Record ());
		*rec_ptr = rec;
	}
	Type * Record_Pointer (int index)
	{
		return (&at (index));
	}
	bool Check_Free (void)
	{
		first_free = -1;
		free_count = 0;

		int rec = 0;
		typename Vector <Type>::iterator itr;
		for (itr = begin (); itr != end (); itr++, rec++) {
			if (itr->Next_Record () == -2) {		//---- free record ----
				if (free_count++ == 0) {
					first_free = rec;
				}
			}
		}
		return (free_count > 0);
	}
	void Clear (void)
	{
		first_free = -1; free_count = 0; Free ();
	}
	bool Pack (Data_Buffer &data)
	{
		if (data.Add_Data (&first_free, sizeof (first_free))) {
			if (data.Add_Data (&free_count, sizeof (free_count))) {
				return (Vector <Type>::Pack (data));
			}
		}
		return (false);
	}
	bool UnPack (Data_Buffer &data)
	{
		if (data.Get_Data (&first_free, sizeof (first_free))) {
			if (data.Get_Data (&free_count, sizeof (free_count))) {
				return (Vector <Type>::UnPack (data));
			}
		}
		return (false);
	}
private:
	int first_free;
	int free_count;
};

#endif
