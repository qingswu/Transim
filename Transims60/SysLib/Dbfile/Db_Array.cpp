//********************************************************* 
//	Db_Array.cpp - database arrays
//*********************************************************

#include "Db_Array.hpp"

//-----------------------------------------------------------
//	Read_Record
//-----------------------------------------------------------

bool Db_Data_Array::Read_Record (int number)
{
	if (number > 0) {
		index = number - 1;
	} else {
		index++;
	}
	if (index < 0 || index >= (int) size ()) {
		return (false);
	} else {
		return (Record (at (index))); 
	}
}

//-----------------------------------------------------------
//	Write_Record
//-----------------------------------------------------------

bool Db_Data_Array::Write_Record (int number)
{
	if (!Record ().OK ()) return (false);

	if (number > 0) {
		index = number - 1;
	} else {
		index++;
	}
	if (index < 0 || index > (int) size ()) return (false);

	if (index == (int) size ()) {
		push_back (Record ());
	} else {
		Buffer &buffer = at (index);
		buffer = Record ();
	}
	return (true); 
}

//-----------------------------------------------------------
//	Add_Record
//-----------------------------------------------------------

bool Db_Data_Array::Add_Record (void)
{
	if (!Record ().OK ()) return (false);
	push_back (Record ());
	return (true);
}

//-----------------------------------------------------------
//	Read_Shared_Record
//-----------------------------------------------------------

bool Db_Data_Array::Read_Shared_Record (Db_Data_Array *dat, int number)
{
	if (dat == 0) return (false);
	if (number < 1 || number >= (int) dat->size ()) return (false);

	return (Record (dat->at (number - 1))); 
}

//-----------------------------------------------------------
//	Write_Shared_Record
//-----------------------------------------------------------

bool Db_Data_Array::Write_Shared_Record (Db_Data_Array *dat, int number)
{
	if (dat == 0) return (false);
	if (number < 1 || number >= (int) size ()) return (false);

	Buffer &buffer = dat->at (number - 1);
	buffer = Record ();
	return (true); 
}

//-----------------------------------------------------------
//	Read_Record
//-----------------------------------------------------------

bool Db_Sort_Array::Read_Record (int index)
{
	Buf_Map_Itr itr = find (index);

	if (itr == end ()) {
		return (false);
	} else {
		return (Record (itr->second));
	}
}

//-----------------------------------------------------------
//	Write_Record
//-----------------------------------------------------------

bool Db_Sort_Array::Write_Record (int index)
{
	if (!Record ().OK ()) return (false);

	Buf_Map_Itr itr = find (index);

	if (itr == end ()) {
		Buf_Map_Stat map_stat = insert (Buf_Map_Data (index, Record ()));
		return (map_stat.second);
	} else {
		itr->second = Record ();
		return (true);
	}
}

//-----------------------------------------------------------
//	Read_Shared_Record
//-----------------------------------------------------------

bool Db_Sort_Array::Read_Shared_Record (Db_Sort_Array *dat, int index)
{
	if (dat == 0) return (false);

	Buf_Map_Itr itr = dat->find (index);
	if (itr == dat->end ()) return (false);

	return (Record (itr->second));
}

//-----------------------------------------------------------
//	Write_Shared_Record
//-----------------------------------------------------------

bool Db_Sort_Array::Write_Shared_Record (Db_Sort_Array *dat, int index)
{
	if (dat == 0) return (false);

	Buf_Map_Itr itr = dat->find (index);

	if (itr == dat->end ()) {
		Buf_Map_Stat map_stat = dat->insert (Buf_Map_Data (index, Record ()));
		return (map_stat.second);
	} else {
		itr->second = Record ();
		return (true);
	}
}
