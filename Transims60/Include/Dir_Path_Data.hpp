//*********************************************************
//	Dir_Path_Data.hpp - path building data between links
//*********************************************************

#ifndef DIR_PATH_DATA_HPP
#define DIR_PATH_DATA_HPP

#include "TypeDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Dir_Path_Data class definition
//---------------------------------------------------------

class Dir_Path_Data
{
public:
	Dir_Path_Data (void)           { Clear (); }

	int   Distance (void)           { return (distance); }
	int   Next_List (void)          { return (next_list); }
	int   From_Dir (void)           { return (from_dir); }

	void  Distance (int value)      { distance = value; }
	void  Next_List (int value)     { next_list = value; }
	void  From_Dir (int value)      { from_dir = value; }

	void  Clear (void)
	{
		distance = 0; next_list = from_dir = -1;
	}
private:
	int   distance;
	int   next_list;
	int   from_dir;
};

typedef vector <Dir_Path_Data>    Dir_Path_Array;
typedef Dir_Path_Array::iterator  Dir_Path_Itr;

#endif
