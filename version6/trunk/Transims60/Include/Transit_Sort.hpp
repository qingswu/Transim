//*********************************************************
//	Transit_Sort.hpp - sort impedance for transit path building
//*********************************************************

#ifndef TRANSIT_SORT_HPP
#define TRANSIT_SORT_HPP

#include "APIDefs.hpp"
#include "Bin_Heap.hpp"

#include <deque>
using namespace std;

//---------------------------------------------------------
//	Transit_Path_Index class definition
//---------------------------------------------------------

class SYSLIB_API Transit_Path_Index
{
public:
	Transit_Path_Index (void)       { Clear (); }

	int      Index (void)           { return (index); }
	int      Type (void)            { return (type); }
	int      Path (void)            { return (path); }
	
	void     Index (int value)      { index = value; }
	void     Type (int value)       { type = (char) value; }
	void     Path (int value)       { path = (char) value; }

	void     Clear (void) 
	{
		Index (-1); Type (0); Path (0);
	}

private:
	int  index;
	char type;
	char path;
};

typedef deque <Transit_Path_Index>               Transit_Queue;

typedef Bin_Heap <Transit_Path_Index> Transit_Sort;

SYSLIB_API bool operator != (Transit_Path_Index left, Transit_Path_Index right);

#endif
