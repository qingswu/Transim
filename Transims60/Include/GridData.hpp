//*********************************************************
//	GridData.hpp - Grid Data Processing
//*********************************************************

#ifndef GRIDDATA_HPP
#define GRIDDATA_HPP

#include "Data_Service.hpp"
#include "User_Program.hpp"
#include "Db_Array.hpp"
#include "Db_Header.hpp"
#include "Arcview_File.hpp"
#include "Data_Range.hpp"
#include "Projection_Service.hpp"
#include "TypeDefs.hpp"

#include <map>
#include <vector>
using namespace std;

//---------------------------------------------------------
//	GridData - execution class definition
//---------------------------------------------------------

class SYSLIB_API GridData : public Data_Service
{
public:

	GridData (void);

	virtual void Execute (void);

protected:
	enum GridData_Keys { 
		ARC_GRID_FILE = 1, GRID_DATA_FILE, GRID_DATA_FORMAT, NEW_ARC_GRID_FILE, 
		COPY_EXISTING_FIELDS, GRID_ZONE_FIELDS, NEW_GRID_FIELD, CONVERSION_SCRIPT, 
		
		ZONE_BOUNDARY_FILE, BOUNDARY_ZONE_FIELD, GRID_SIZE,
		ZONE_DATA_FILE, ZONE_DATA_FORMAT, ZONE_ID_FIELD,
		
		BOUNDARY_POLYGON_FILE, 

		POINT_DATA_FILE, POINT_DATA_FORMAT, POINT_ID_FIELD, POINT_X_FIELD, POINT_Y_FIELD, 
		POINT_DATA_FIELD, MAX_POINT_DISTANCE,

		NEW_SUMMARY_FILE, NEW_SUMMARY_FORMAT, SUMMARY_SELECT_FIELD, SUMMARY_SELECT_VALUES,
		SUMMARY_DATA_FIELD, SUMMARY_DATA_VALUE, SUMMARY_DATA_FILTER,
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum GridData_Reports { PRINT_SCRIPT = 1, PRINT_STACK };

	int zone_field, num_zone_fields, data_offset;
	bool script_flag, grid_flag, point_flag, zone_flag, polygon_flag, summary_flag;
	double grid_size, max_distance;

	Integers out_fields, in_zone_fld, out_zone_fld;
	Db_Header point_file, summary_file;
	Db_Data_Array point_data;

	Arcview_File in_file, out_file, boundary_file;
	Db_Base_Array data_rec, data_files;
	Points_Map boundary;
		
	Projection_Service projection;	

	Db_File program_file;
	User_Program program;

	//---- zone data groups ----

	typedef struct {
		int group;
		int zone_field;
		Db_Header *file;
		Db_Sort_Array *data_db;
	} Data_Group;

	typedef vector <Data_Group>          Data_Array;
	typedef Data_Array::iterator         Data_Itr;

	Data_Array data_group;

	//---- polygon groups ----

	typedef struct {
		int group;
		Arcview_File *file;
		Db_Sort_Array *data_db;
		Points_Map polygon;
	} Polygon_Group;

	typedef vector <Polygon_Group>     Polygon_Array;
	typedef Polygon_Array::iterator    Polygon_Itr;

	Polygon_Array polygons;

	Db_Sort_Array zone_grids;

	//---- summary groups ----

	typedef struct {
		int        group;
		int        select_field;
		bool       all_flag;
		String     field_name;
		Data_Range select_values;
		Integers   grid_counts;
		Dbls_Array data_array;
	} Summary_Group;

	typedef vector <Summary_Group>      Summary_Array;
	typedef Summary_Array::iterator     Summary_Itr;

	Summary_Array summary_array;

	//---- data field array ----

	typedef struct {
		int    data_field;
		int    data_value;
		int    filter_field;
		int    filter_test;
		double filter_value;
		int    filter2_field;
		int    filter2_test;
		double filter2_value;
	} Data_Field;

	typedef vector <Data_Field>         Data_Field_Array;
	typedef Data_Field_Array::iterator  Data_Field_Itr;

	Data_Field_Array data_field_array;

	void Set_Files (void);
	void Read_Data (void);
	void Read_Polygons (void);
	void Read_Boundary (void);
	void Read_Points (void);
	void Zone_Grids (void);
	void Build_Grid (void);
	void Write_Grid (void);
	void Point_Grid (double x0, double y0, double x1, double y1, double x2, double y2);
	void Sum_Grid (void);
	void Write_Summary (void);
};
#endif


