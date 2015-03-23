//*********************************************************
//	MatrixData.hpp - Convert files to a new format
//*********************************************************

#ifndef MATRIXDATA_HPP
#define MATRIXDATA_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Db_Header.hpp"
#include "User_Program.hpp"
#include "Db_Matrix.hpp"
#include "TypeDefs.hpp"
#include "Best_List.hpp"

//---------------------------------------------------------
//	MatrixData - execution class definition
//---------------------------------------------------------

class SYSLIB_API MatrixData : public Data_Service, public Select_Service
{
public:
	MatrixData (void);
	virtual ~MatrixData (void);

	virtual void Execute (void);

protected:
	enum MatrixData_Keys { 
		DATA_FILE = 1, DATA_FORMAT, ORIGIN_FIELD, DESTINATION_FIELD, PERIOD_FIELD, 
		LOW_TIME_FIELD, HIGH_TIME_FIELD, PERIOD_NUMBER, DATA_FACTOR_FIELD, TABLE_DATA_FIELD,
		MATRIX_FILE, MATRIX_FORMAT, GROWTH_FACTOR, TRANSPOSE_OD, SELECT_TABLES,
		MATRIX_PERIOD_BREAKS, MATRIX_PERIOD_FIELDS, SQUARE_TABLE_FILE,
		GROWTH_FACTOR_FILE, GROWTH_FACTOR_FORMAT, FACTOR_TABLES, FACTOR_PERIOD_BREAKS, FACTOR_PERIOD_FIELDS,
		NEW_MATRIX_FILE, NEW_MATRIX_FORMAT, NEW_MATRIX_TABLE, NEW_MATRIX_FACTOR, 
		NEW_MATRIX_METHOD, PERCENT_MISSING_DATA, PROBLEM_DISTRIBUTION, 
		MINIMUM_RATIO_VALUE, MAXIMUM_RATIO_VALUE, ZONE_LABEL_FILE,  
		CONVERSION_SCRIPT, CONVERSION_EQUIV_FILE, 
		NEW_MARGIN_TOTAL_FILE, NEW_SQUARE_TABLE_FILE, NEW_START_TIME_FILE, NEW_END_TIME_FILE
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum MatrixData_Reports { ZONE_EQUIV = 1, PRINT_SCRIPT, PRINT_STACK, USER_EQUIV, STATS_REPORT, MARGIN_REPORT };
	enum MatrixData_Methods { ADD_METHOD, RATIO_METHOD, EXPAND_METHOD, SELECT_METHOD };

	String filename;
	int num_records, group, method;

	Db_Matrix *new_matrix, *factor_file, *alt_matrix, *input_matrix;
	bool script_flag, stats_flag, data_flag, matrix_flag, label_flag, margin_flag, square_flag, period_flag, factor_flag, equiv_flag;
	bool start_flag, end_flag, new_flag, input_flag, missing_flag, problem_flag, user_equiv_flag, fac_table_flag;
	double factor, min_ratio, max_ratio, share_missing;
	
	Time_Periods factor_periods;
	Integers factor_fields;
	Random random_missing, random_problem;
	Double_List problem_distribution;
	Str_Map label_map;

	//---- field statistics ----

	typedef struct {
		int count;
		double minimum;
		double maximum;
		double total;
	} Field_Stats;

	typedef vector <Field_Stats>    Stats_Array;
	typedef Stats_Array::iterator   Stats_Itr;

	//---- data groups ----

	typedef struct {
		int  group;
		Db_Header *file;
		int  org_field;
		int  des_field;
		bool period_flag;
		int  period_field;
		bool time_flag;
		int  low_field;
		int  high_field;
		int  factor_field;
		int  period;
		Integers table_fields;
		Stats_Array stats;
	} Data_Group;

	typedef vector <Data_Group>     Data_Array;
	typedef Data_Array::iterator    Data_Itr;

	Data_Array data_group;
	
	//---- matrix groups ----

	typedef struct {
		int  group;
		bool transpose;
		bool break_flag;
		double factor;
		Db_Matrix *matrix;
		Db_Matrix *record;
		Integers table_map;
		Time_Periods breaks;
		Integers fields;
	} Matrix_Group;

	typedef vector <Matrix_Group>     Matrix_Array;
	typedef Matrix_Array::iterator    Matrix_Itr;

	Matrix_Array matrix_group;

	Db_File program_file, label_file, margin_file, square_file, start_file, end_file, input_file;
	Equiv_Data user_equiv;
	User_Program program;
	Db_Base_Array file_array;

	bool Read_Square_Table (void);
	void Read_Data (void);
	void Read_Matrix (void);
	void Expand_Matrix (void);
	void Matrix_Ratios (void);
	void Read_Factors (void);
	void Write_Margin_Totals (void);
	void Write_Square_Table (void);
	void Write_Start_Times (void);
	void Write_End_Times (void);
	
	void Statistics_Report (void);
	void Statistics_Header (void);

	void Margin_Total_Report (void);
	void Margin_Total_Header (void);
};
#endif
