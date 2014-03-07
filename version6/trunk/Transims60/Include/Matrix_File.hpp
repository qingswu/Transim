//********************************************************* 
//	Matrix_File.hpp - Matrix File Input/Output
//*********************************************************

#ifndef MATRIX_FILE_HPP
#define MATRIX_FILE_HPP

#include "Db_Matrix.hpp"

//---------------------------------------------------------
//	Matrix_File Class definition
//---------------------------------------------------------

class SYSLIB_API Matrix_File : public Db_Matrix
{
public:
	Matrix_File (Access_Type access, string format, Matrix_Type matrix_type = TRIP_TABLE, Units_Type od = ZONE_OD);
	Matrix_File (string filename, Access_Type access, string format, Matrix_Type matrix_type = TRIP_TABLE, Units_Type od = ZONE_OD);
	Matrix_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Matrix_Type matrix_type = TRIP_TABLE, Units_Type od = ZONE_OD);
	Matrix_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Matrix_Type matrix_type = TRIP_TABLE, Units_Type od = ZONE_OD);

	Matrix_Type  Data_Type (void)             { return (matrix_type); }
	void  Data_Type (Matrix_Type value)       { matrix_type = value; }

	Units_Type OD_Units (void)                { return (od_units); }
	void  OD_Units (Units_Type type)          { od_units = type; }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual void Setup (void);

private:
	Matrix_Type matrix_type;
	Units_Type od_units;
};

#endif
