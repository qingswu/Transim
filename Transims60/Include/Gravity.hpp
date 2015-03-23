//*********************************************************
//	Gravity.hpp - Gravity-based trip distribution model
//*********************************************************

#ifndef GRAVITY_HPP
#define GRAVITY_HPP

#include "Data_Service.hpp"
#include "Db_Matrix.hpp"
#include "TypeDefs.hpp"
#include "Functions.hpp"

//---------------------------------------------------------
//	Gravity - execution class definition
//---------------------------------------------------------

class SYSLIB_API Gravity : public Data_Service
{
public:
	Gravity (void);

	virtual void Execute (void);

protected:
	enum Gravity_Keys { 
		ZONE_PRODUCTION_FIELD = 1, ZONE_ATTRACTION_FIELD, ZONE_PERIOD_FIELDS, 
		SKIM_FILE, SKIM_FORMAT, SKIM_TABLE_NAME, TRIP_TABLE_FILE, TRIP_TABLE_FORMAT, TRIP_TABLE_NAME,
		NEW_TRIP_TABLE_FILE, NEW_TRIP_TABLE_FORMAT, 
		FUNCTION_INPUT_UNITS, FUNCTION_INPUT_FACTOR, FUNCTION_PARAMETERS, 
		CALIBRATE_FUNCTIONS, PARAMETER_CONSTRAINTS,
		BALANCING_METHOD, MAXIMUM_ITERATIONS, MAXIMUM_PERCENT_DIFFERENCE, MAXIMUM_TRIP_DIFFERENCE,
		NEW_INPUT_LENGTH_FILE, NEW_INPUT_LENGTH_FORMAT, NEW_OUTPUT_LENGTH_FILE, NEW_OUTPUT_LENGTH_FORMAT,
		TRIP_LENGTH_INCREMENT, MAXIMUM_TRIP_LENGTH,
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);
	
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);

private:
	enum Gravity_Reports { PRINT_SCRIPT = 1, PRINT_STACK };
	enum Balance_Options { PROD_BAL = 1, ATTR_BAL, AVERAGE_BAL };

	bool in_flag, period_flag, script_flag, trip_flag, in_len_flag, out_len_flag, new_flag, calib_flag;
	int max_iterations, num_periods, prod_field, attr_field, skim_field, trip_field, balance, best_function;
	double max_percent, max_diff, increment, max_value, value_factor, units_factor, total_in;
	Units_Type skim_units, input_units;

	Db_Matrix *trip_file, *new_file, *skim_file;
	Db_Header in_len_file, out_len_file;

	Doubles prod_array, attr_array, in_distb, out_distb, org_tot, des_tot;
	Dbls_Array prod_periods, attr_periods;
	Integers prod_fields, attr_fields;
	Dbl3_Array constraints;

	Function_Array function;

	void Processing (void);
	void Read_Trips (void);
	void Calib_Functions (void);
	void Write_Data (Db_Header &file, Doubles &distb);
};
#endif
