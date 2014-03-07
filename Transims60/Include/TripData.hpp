//*********************************************************
//	TripData.hpp - Trip Table Data Processing
//*********************************************************

#ifndef TRIPDATA_HPP
#define TRIPDATA_HPP

#include "Data_Service.hpp"
#include "Matrix_Data.hpp"
#include "User_Program.hpp"
#include "Db_Array.hpp"
#include "Db_Header.hpp"
#include "Data_Range.hpp"
#include "TypeDefs.hpp"

#include <map>
#include <vector>
using namespace std;

//---------------------------------------------------------
//	TripData - execution class definition
//---------------------------------------------------------

class SYSLIB_API TripData : public Data_Service
{
public:

	TripData (void);

	virtual void Execute (void);

protected:
	enum TripData_Keys { 
		TRIP_FILE = 1, TRIP_FORMAT, NEW_TRIP_FILE, NEW_TRIP_FORMAT, 
		COPY_EXISTING_FIELDS, CLEAR_ALL_FIELDS, ZONE_FILE_HEADER, NEW_ZONE_FIELD, 
		CONVERSION_SCRIPT, DATA_FILE, DATA_FORMAT, DATA_JOIN_FIELD, ZONE_JOIN_FIELD 
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);
	
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);

private:
	enum TripData_Reports { PRINT_SCRIPT = 1, PRINT_STACK };

	int num_data_files;
	bool copy_flag, script_flag, data_flag;
	
	Db_Base_Array data_rec;
	Zone_File *input_file, *output_file;

	Matrix_File trip_file, new_trip_file;
	
	//---- data groups ----

	typedef struct {
		int group;
		int join_field;
		int zone_field;
		Db_Header *file;
		Db_Sort_Array *data_db;
	} Data_Group;

	typedef vector <Data_Group>          Data_Array;
	typedef Data_Array::iterator         Data_Itr;

	Data_Array data_group;

	Db_File program_file;
	User_Program program;

	void Set_Files (void);
	void Read_Data (void);
	void Write_Trip (void);
};
#endif


