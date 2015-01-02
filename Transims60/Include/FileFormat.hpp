//*********************************************************
//	FileFormat.hpp - Convert files to a new format
//*********************************************************

#ifndef FILEFORMAT_HPP
#define FILEFORMAT_HPP

#include "Execution_Service.hpp"
#include "Db_Header.hpp"
#include "User_Program.hpp"
#include "Db_Array.hpp"
#include "Db_Matrix.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	FileFormat - execution class definition
//---------------------------------------------------------

class SYSLIB_API FileFormat : public Execution_Service
{
public:
	FileFormat (void);

	virtual void Execute (void);

protected:
	enum FileFormat_Keys { 
		DATA_FILE = 1, DATA_FORMAT, NEW_DATA_FILE, NEW_DATA_FORMAT, 
		DATA_SELECT_FIELD, COPY_EXISTING_FIELDS, 
		NEW_FILE_HEADER, NEW_DATA_FIELD, SORT_BY_FIELDS,
		DATA_FIELD_MAP, DATA_INDEX_FIELD, NEW_COMBINE_FIELDS_FILE, NEW_COMBINE_FIELDS_FORMAT, 
		MATRIX_FILE, MATRIX_FORMAT, NEW_MATRIX_FILE, NEW_MATRIX_FORMAT,
		SELECT_TABLES, CONVERSION_SCRIPT,
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum FileFormat_Reports { PRINT_SCRIPT = 1, PRINT_STACK, STATS_REPORT };

	String filename;
	int num_records;

	bool script_flag, data_flag, matrix_flag, stats_flag, combine_flag, index_flag;
	Db_Header combine_file;

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
		int group;
		bool sort_flag;
		bool select_flag;
		bool int_flag;
		int  sel_field;
		int  index_field;
		Integers  sel_values;
		Strings sel_strings;
		Db_Header *file;
		Db_Header *new_file;
		Db_Base *sort_string;
		Db_Data_Array *dbase;
		Str_ID sort_id;
		Stats_Array input;
		Stats_Array output;
		Integers in_fields;
		Integers out_fields;
		Integers field_types;
	} Data_Group;

	typedef vector <Data_Group>     Data_Array;
	typedef Data_Array::iterator    Data_Itr;

	Data_Array data_group;

	//---- matrix groups ----

	typedef struct {
		int group;
		Db_Matrix *matrix;
		Db_Matrix *new_matrix;
		Integers table_map;
	} Matrix_Group;

	typedef vector <Matrix_Group>     Matrix_Array;
	typedef Matrix_Array::iterator    Matrix_Itr;

	Matrix_Array matrix_group;

	Db_File program_file;
	User_Program program;
	Db_Base_Array file_array;
	
	void Combine_Index (void);
	void Combine_Fields (void);
	void Format_Data (void);
	void Format_Matrix (void);
	
	void Statistics_Report (void);
	void Statistics_Header (void);
};
#endif
