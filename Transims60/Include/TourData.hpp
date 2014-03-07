//*********************************************************
//	TourData.hpp - Tour Data Processing
//*********************************************************

#ifndef TOURDATA_HPP
#define TOURDATA_HPP

#include "Data_Service.hpp"
#include "Db_Header.hpp"
#include "TypeDefs.hpp"
#include "Data_Range.hpp"
#include "Db_Matrix.hpp"

#include <map>
#include <vector>
using namespace std;

typedef struct {
	int origin;
	int destination;
	int type;
} Stats_Index;

bool operator < (Stats_Index left, Stats_Index right);

//---------------------------------------------------------
//	TourData - execution class definition
//---------------------------------------------------------

class SYSLIB_API TourData : public Data_Service
{
public:

	TourData (void);

	virtual void Execute (void);

protected:
	enum TourData_Keys { 
		ZONE_GROUP_FIELD = 1, TOUR_FILE, TOUR_FORMAT, COMPARE_TOUR_FILE, COMPARE_TOUR_FORMAT, 
		NEW_TOUR_FILE, NEW_TOUR_FORMAT, TOUR_PURPOSE_FIELD, TOUR_TYPE_FIELD, TOUR_INCOME_FIELD, 
		TOUR_ORIGIN_FIELD, TOUR_DESTINATION_FIELD, TOUR_POINT_FIELD, 
		TARGET_TOUR_MATRIX, TARGET_TOUR_FORMAT, BASE_TOUR_MATRIX, BASE_TOUR_FORMAT,
		NEW_RESULT_MATRIX, NEW_RESULT_FORMAT,
		TABLE_PURPOSE_RANGE, TABLE_TYPE_RANGE, TABLE_INCOME_RANGE, 
		TABLE_TARGET_FIELD, TABLE_BASE_FIELD, SPECIAL_ATTRACTIONS,
		MIN_ABSOLUTE_DIFFERENCE, MIN_PERCENT_DIFFERENCE, DISTANCE_FACTOR, ATTRACTION_FACTOR,
		NEW_ATTRACTION_FILE, NEW_ATTRACTION_FORMAT
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);
	
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);

private:
	enum TourData_Reports { PRINT_SCRIPT = 1, PRINT_STACK, ATTRACTIONS, GROUP_STATS };

	int num_data_files;
	bool result_flag, stats_flag, attr_flag, attr_file_flag, compare_flag, group_flag, income_flag;
	double min_difference, min_percent_diff, distance_factor, attraction_factor;
	
	Db_Base_Array data_rec;
	Db_Matrix *target_matrix, *base_matrix, *result_matrix;

	//---- district mapping ----

	Zone_File *zone_file, attr_file;
	int group_field, max_zone, max_group, num_group;

	Int_Map group_map;
	Integers zone_group;
	Ints_Array group_zones;
	Dbls_Array od_distance;

	Db_Header tour_file, compare_file, new_file;
	int purpose_field, type_field, org_field, des_field, point_field, income_field;

	Random random_select, random_point;

	//---- table groups ----

	typedef struct {
		int group;
		int target;
		int base;
		int special;
		Data_Range purpose;
		Data_Range type;
		Data_Range income;
		Integers   zone_wt;
		Ints_Array zone_pts;
		Dbls_Array tours;
		Integers   attr;
	} Table_Group;

	typedef vector <Table_Group>          Table_Array;
	typedef Table_Array::iterator         Table_Itr;

	Table_Array table_groups;

	typedef struct {
		int input;
		int target;
		int moved;
		double in_len;
		double out_len;
		Int_Set new_des;
	} Group_Stats;

	typedef map <Stats_Index, Group_Stats>   Stats_Map;
	typedef pair <Stats_Index, Group_Stats>  Stats_Map_Data;
	typedef Stats_Map::iterator              Stats_Map_Itr;
	typedef pair <Stats_Map_Itr, bool>       Stats_Map_Stat;

	Stats_Map group_stats;

	void Zone_Groups (void);
	void Normalize_Origins (void);
	void Zone_Distance (void);
	void Read_Tour (void);
	void Write_Tour (void);
	bool Process (int org, int &des, int &point, Table_Itr table_itr);
	
	void Attraction_Report (void);
	void Attraction_Header (void);

	void Group_Stats_Report (void);
	void Group_Stats_Header (void);

	void Write_Attractions (void);
};
#endif


