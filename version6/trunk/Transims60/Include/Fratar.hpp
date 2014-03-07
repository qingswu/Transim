//*********************************************************
//	Fratar.hpp - Factor a tables based on new marginals
//*********************************************************

#ifndef FRATAR_HPP
#define FRATAR_HPP

#include "Execution_Service.hpp"
#include "Db_Header.hpp"
#include "User_Program.hpp"
#include "Db_Array.hpp"
#include "Matrix_File.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Fratar - execution class definition
//---------------------------------------------------------

class SYSLIB_API Fratar : public Execution_Service
{
public:
	Fratar (void);

	virtual void Execute (void);

protected:
	enum Fratar_Keys { 
		TRIP_TABLE_FILE = 1, TRIP_TABLE_FORMAT, NEW_TRIP_TABLE_FILE, NEW_TRIP_TABLE_FORMAT, 
		BASE_MARGIN_FILE, BASE_MARGIN_FORMAT, TRIP_MARGIN_FILE, TRIP_MARGIN_FORMAT, 
		MARGIN_TYPE_FIELD, MARGIN_TRIP_FIELD, MARGIN_TYPE_MAP_FILE, MARGIN_TYPE_MAP_FORMAT, 
		MAXIMUM_ITERATIONS, PERCENT_TRIP_DIFFERENCE, MINIMUM_TRIP_DIFFERENCE, MAXIMUM_TRIP_DIFFERENCE,
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum Fratar_Reports { PRINT_SCRIPT = 1, PRINT_STACK };

	bool in_flag, period_flag, script_flag;
	int max_iterations;
	double percent_diff, min_diff, max_diff;

	Matrix_File in_file, new_file;
	Dbl3_Array odt_array;

	Db_File program_file;
	User_Program program;

	//---- margin groups ----

	typedef struct {
		int group;
		int type_field;
		int trip_field;
		Db_Header *base;
		Db_Header *file;
		Db_Header *map_file;
		Doubles target;
		Doubles total;
		Int_Map type_map;
		bool base_flag;
		bool map_flag;
	} Margin_Group;

	typedef vector <Margin_Group>     Margin_Array;
	typedef Margin_Array::iterator    Margin_Itr;

	Margin_Array margin_group;

	void Read_Margins (void);
	void Read_Table (void);
	void Processing (void);
	void Write_Table (void);
};
#endif
