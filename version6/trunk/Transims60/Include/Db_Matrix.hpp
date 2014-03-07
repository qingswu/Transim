//*********************************************************
//	Db_Matrix.hpp - General Matrix DLL Interface
//*********************************************************

#ifndef DB_MATRIX_HPP
#define DB_MATRIX_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Partition_Files.hpp"
#include "Time_Periods.hpp"
#include "TypeDefs.hpp"
#include "Buffer.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Db_Matrix Class definition
//---------------------------------------------------------

class SYSLIB_API Db_Matrix : public Db_Header, public Time_Periods
{
public:
	Db_Matrix (Access_Type access, string format, Field_Type type = DB_DOUBLE);
	Db_Matrix (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Field_Type type = DB_DOUBLE);
	virtual ~Db_Matrix (void)              { Close (); }

	int    Origin (void)                      { return (Get_Integer (org_field)); }
	int    Destination (void)                 { return (Get_Integer (des_field)); }
	int    Period (void)                      { return (Get_Integer (period_field)); }
	int    Period (Dtime time)                { return (Time_Periods::Period (time)); }
	int    Period (Dtime low, Dtime high)     { return (Time_Periods::Period (low, high)); }
	double Table (int num)                    { return (Get_Double (table_fields [num])); }

	void   Origin (int value)                 { Put_Field (org_field, value); }
	void   Destination (int value)            { Put_Field (des_field, value); }
	void   Period (int value)                 { Put_Field (period_field, value); }
	void   Table (int num, double value)      { Put_Field (table_fields [num], value); }

	virtual bool Db_Open (string filename);
	virtual bool Close (void);
	
	virtual bool Read_Row (int org = 0, int period = 0);
	virtual bool Write_Row (int org = 0, int period = 0);

	virtual bool Read_Row (void *data, int org, int table = 0, int period = 0);
	virtual bool Write_Row (void *data, int org, int table = 0, int period = 0);	

	virtual bool Read_Record (int org = 0)    { return (Read_Row (org)); }
	virtual bool Write_Record (int org = 0)   { return (Write_Row (org)); }

	virtual bool Read_Matrix (void);
	virtual bool Write_Matrix (void);

	bool Set_Fields (int org = 0, int des = 0, int period = 0);
	bool Get_Fields (void);

	bool Set_Shared_Fields (Db_Matrix *file, int org = 0, int des = 0, int period = 0);
	bool Get_Shared_Fields (Db_Matrix *file);

	void Tables (int value)                { table_fields.assign (value, -1); }
	int  Tables (void)                     { return ((int) table_fields.size ()); }

	void Type (Field_Type value)           { Clear_Data (); type = value; }
	Field_Type Type (void)                 { return (type); }

	void Num_Org (int value)               { Clear_Data (); num_org = value; }
	int  Num_Org (void)                    { return (num_org); }

	void Num_Des (int value)               { Clear_Data (); num_des = value; }
	int  Num_Des (void)                    { return (num_des); }

	int  Num_Zones (void)                  { return (MAX (num_org, num_des)); }

	int  Add_Org (int org);
	int  Add_Des (int des);

	int  Org_Index (int org);
	int  Des_Index (int des);

	Int_Map * Org_Map (void)               { return (&org_map); }
	Int_Map * Des_Map (void)               { return (&des_map); }

	bool Copy_OD_Map (Db_Matrix *file);
	bool Copy_Matrix_Data (Db_Matrix *file);

	bool Data_Flag (void)                  { return (data_flag); }
	bool Matrix_Flag (void)                { return (matrix_flag); }

	bool Allocate_Data (bool matrix_flag = false);
	void Clear_Data (void);
	
	void * Matrix_Ptr (int period, int org, int tab = 0);
	void * Row_Ptr (int tab = 0);
	
	bool Add_Cell (int des, int tab, int value);
	bool Add_Cell (int des, int tab, double value);
	bool Add_Cell (int period, int org, int des, int tab, int value);
	bool Add_Cell (int period, int org, int des, int tab, double value);

	bool Add_Cell_Index (int des, int tab, int value);
	bool Add_Cell_Index (int des, int tab, double value);
	bool Add_Cell_Index (int period, int org, int des, int tab, int value);
	bool Add_Cell_Index (int period, int org, int des, int tab, double value);

	bool Set_Cell (int des, int tab, int value);
	bool Set_Cell (int des, int tab, double value);
	bool Set_Cell (int period, int org, int des, int tab, double value);
	bool Set_Cell (int period, int org, int des, int tab, int value);
	
	bool Set_Cell_Index (int des, int tab, int value);
	bool Set_Cell_Index (int des, int tab, double value);
	bool Set_Cell_Index (int period, int org, int des, int tab, int value);
	bool Set_Cell_Index (int period, int org, int des, int tab, double value);

	bool Get_Cell (int des, int tab, int &value);
	bool Get_Cell (int des, int tab, double &value);	
	bool Get_Cell (int period, int org, int des, int tab, int &value);
	bool Get_Cell (int period, int org, int des, int tab, double &value);

	bool Get_Cell_Index (int des, int tab, int &value);
	bool Get_Cell_Index (int des, int tab, double &value);
	bool Get_Cell_Index (int period, int org, int des, int tab, int &value);
	bool Get_Cell_Index (int period, int org, int des, int tab, double &value);

	bool Factor_Cell (int des, int tab, double factor);
	bool Factor_Cell (int period, int org, int des, int tab, double factor);

	bool Factor_Cell_Index (int des, int tab, double factor);
	bool Factor_Cell_Index (int period, int org, int des, int tab, double factor);

	bool Factor_Row (int tab, double factor);
	bool Factor_Row (int period, int org, int tab, double factor);
	bool Factor_Row_Index (int period, int org, int tab, double factor);
	
	double Row_Sum (int tab);
	double Row_Sum (int period, int org, int tab);
	double Row_Sum_Index (int period, int org, int tab);

	void Zero_Data (void);
	
	int Num_Records (void)                 { return (Num_Org () * Tables () * Num_Periods ()); }
	int Estimate_Records (void)            { return (Num_Records ()); }
	
	virtual bool Write_Header (string user_lines = "");

	int Table_Number (string name);
	int Table_Field_Number (int num)       { return (table_fields [num]); }
	Db_Field * Table_Field (int num)       { return (Field (table_fields [num])); }
	bool Replicate_Tables (Db_Matrix *file);

	virtual bool Set_Field_Numbers (void);

private:
	virtual bool Load_DLL (void)           { return (true); }

	virtual bool Read_Header (bool stat);

	Dbls_Array double_row;
	Flts_Array float_row;
	Ints_Array int_row;
	Shts_Array short_row;
	
	Dbl4_Array double_matrix;
	Flt4_Array float_matrix;
	Int4_Array int_matrix;
	Sht4_Array short_matrix;

	Field_Type type;
	int org_field, des_field, period_field, num_org, num_des;
	Integers table_fields;
	Int_Map org_map, des_map;
	bool data_flag, matrix_flag;
	Buffer rec_buffer;
};

typedef Db_Matrix *                 Db_Mat_Ptr;
typedef Pointer_Array <Db_Mat_Ptr>  Db_Matrices;
typedef Db_Matrices::iterator       Db_Mat_Itr;
#endif
