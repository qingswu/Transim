//*********************************************************
//	NewLandUse.hpp - redistribute land use data
//*********************************************************

#ifndef NEWLANDUSE_HPP
#define NEWLANDUSE_HPP

#include "Data_Service.hpp"
#include "Db_Matrix.hpp"
#include "TypeDefs.hpp"
#include "Functions.hpp"
#include "Partition_Files.hpp"

//---------------------------------------------------------
//	NewLandUse - execution class definition
//---------------------------------------------------------

class SYSLIB_API NewLandUse : public Data_Service
{
public:
	NewLandUse (void);

	virtual void Execute (void);

protected:
	enum NewLandUse_Keys { 
		ZONE_DATA_FIELD = 1, ZONE_TARGET_FIELD, ZONE_AREA_FIELD, ZONE_GROUP_FIELD, 
		ZONE_TYPE_FIELD, ZONE_COVERAGE_FIELD, ZONE_YEAR_FILE, ZONE_YEAR_FORMAT, 
		TARGET_DENSITY_FILE, TARGET_DENSITY_FORMAT,	PROCESSING_STEP, FUNCTION_PARAMETERS, 
		MAXIMUM_ITERATIONS, MAXIMUM_PERCENT_DIFFERENCE, MAXIMUM_TRIP_DIFFERENCE,
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);
	
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);

private:
	enum NewLandUse_Reports { GROUP_REPORT = 1, ZONE_TARGET };

	Integers data_fields, group_fields, type_fields, cover_fields;
	Integers group_index, type_index, cover_index, year_index;
	Strings data_names;
	int target_field, area_field, year_number, index_field, report_code;
	int num_zones, num_years, num_cover, num_group, num_type, cover_types;
	int max_iterations, best_function;
	double max_percent, max_diff, increment, max_value, value_factor, units_factor, total_in;

	Db_Header target_file;

	Doubles prod_array, attr_array, prod_tot, attr_tot, attr_bal;

	Function_Array function;

	//---- zone file data ----

	typedef struct {
		int        zone;
		int        target;
		double     area;
		Dbls_Array data;
		Integers   group;
		Integers   type;
		Doubles    coverage;
		Dbls_Array max_change;
		Dbls_Array change;
	} Zone_Field_Data;

	typedef vector <Zone_Field_Data>   Zone_Field_Array;
	typedef Zone_Field_Array::iterator Zone_Field_Itr;

	Zone_Field_Array zone_field_array;

	typedef Pointer_Array <Zone_File *> Zone_Year_Array;
	typedef Zone_Year_Array::iterator   Zone_Year_Itr;

	Zone_Year_Array zone_year_array;

	Dbls_Map target_density;
	
	//---- processing steps ----

	typedef struct {
		int   step;
		int   year0;
		int   year1;
		int   cover0;
		int   cover1;
		int   cover_num;
		int   ptype_num;
		int   ptype_logic;
		int   atype_num;
		int   atype_logic;
		int   group_num;
		int   group_logic;
	} Processing_Data;

	typedef vector <Processing_Data>   Processing_Array;
	typedef Processing_Array::iterator Processing_Itr;

	Processing_Array processing_steps;

	void Parse_Step (String text, Processing_Data &step);
	void Read_Files (void);
	void Apply_Density (void);
	void Processing (int field, Processing_Data &step);
	bool Distribute (int field, int step, int group, int cover);
	void Write_Data (void);

	void Group_Report (int number);
	void Group_Header (void);
};
#endif
