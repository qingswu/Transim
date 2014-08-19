//*********************************************************
//	Gap_Data.hpp - Gap Data Structures
//*********************************************************

#ifndef GAP_DATA_HPP
#define GAP_DATA_HPP

#include "Trip_Index.hpp"
#include "Partition_Files.hpp"

#include <map>
using namespace std;

//---- gap data array ----

typedef struct {
	Dtime time;
	int   current;
	int   previous;
} Gap_Data;

typedef Vector <Gap_Data>          Gap_Data_Array;
typedef Gap_Data_Array::iterator   Gap_Data_Itr;

//---- gap sum array;

typedef struct {
	double current;
	double previous;
	double abs_diff;
	double diff_sq;
	double max_gap;
	int count;
} Gap_Sum;

typedef Vector <Gap_Sum>           Gap_Sum_Array;
typedef Gap_Sum_Array::iterator    Gap_Sum_Itr;

//---- trip gap_map ----

typedef map <Trip_Index, Gap_Data>      Trip_Gap_Map;
typedef pair <Trip_Index, Gap_Data>     Trip_Gap_Map_Data;
typedef Trip_Gap_Map::iterator          Trip_Gap_Map_Itr;
typedef pair <Trip_Gap_Map_Itr, bool>   Trip_Gap_Map_Stat;
	
typedef Partition_Data <Trip_Gap_Map>   Trip_Gap_Map_Array;

#endif
