//*********************************************************
//	Db_Matrix.cpp - General Matrix DLL Interface
//*********************************************************

#include "Db_Matrix.hpp"

#include <math.h>

//---------------------------------------------------------
//	Db_Matrix constructors
//---------------------------------------------------------

Db_Matrix::Db_Matrix (Access_Type access, string format, Field_Type type) : Db_Header (access, format)
{
	org_field = des_field = period_field = -1;
	num_org = num_des = 0;
	data_flag = matrix_flag = false;
	Type (type);

	File_Type ("Matrix File");
	File_ID ("Matrix");
	Header_Lines (2);
}

Db_Matrix::Db_Matrix (Access_Type access, Format_Type format, Field_Type type) : Db_Header (access, format)
{
	org_field = des_field = period_field = -1;
	num_org = num_des = 0;
	data_flag = matrix_flag = false;
	Type (type);

	File_Type ("Matrix File");
	File_ID ("Matrix");
	Header_Lines (2);
}

//---------------------------------------------------------
//	Db_Open
//---------------------------------------------------------

bool Db_Matrix::Db_Open (string filename) 
{
	Set_Field_Numbers ();

	return (Db_File::Db_Open (filename));
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

bool Db_Matrix::Close (void)
{
	return (Db_Header::Close ());
}

//---------------------------------------------------------
//	Read_Row
//---------------------------------------------------------

bool Db_Matrix::Read_Row (int origin, int period)
{
	int n, j, des, org, time;
	bool stat;

	Int_Itr itr;

	if (!data_flag) {
		if (!Allocate_Data ()) return (false);
	} else {
		Zero_Data ();
	}
	stat = false;

	for (;;) {
		time = Get_Integer (period_field);
		if (time > period) break;

		if (time == period) {
			org = Get_Integer (org_field);
			if (org > origin) return (true);

			if (org == origin) {
				if (Add_Org (org) < 0) break;

				des = Get_Integer (des_field);

				j = Add_Des (des);
				if (j < 0) break;

				for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
					switch (type) {
						default:
						case DB_DOUBLE:
							double_row [n] [j] = Get_Double (*itr);
							break;
						case DB_FLOAT:
							float_row [n] [j] = (float) Get_Double (*itr);
							break;
						case DB_INTEGER:
							int_row [n] [j] = Get_Integer (*itr);
							break;
						case DB_SHORT:
							short_row [n] [j] = (short) Get_Integer (*itr);
							break;
					}
				}
				stat = true;
			}
		}
		if (!Db_Header::Read ()) break;
	}
	return (stat);
}

bool Db_Matrix::Read_Row (void *data, int org, int table, int period)
{
	org = table = period = 0;
	int size = 0;

	switch (type) {
		default:
		case DB_DOUBLE:
			size = (int) sizeof (double);
		case DB_FLOAT:
			size = (int) sizeof (float);
		case DB_INTEGER:
			size = (int) sizeof (int);
		case DB_SHORT:
			size = (int) sizeof (short);
	}
	memset (data, '\0', Num_Des () * size);
	return (false);
}

//---------------------------------------------------------
//	Write_Row
//---------------------------------------------------------

bool Db_Matrix::Write_Row (int origin, int period)
{
	if (!data_flag) return (false);

	int n, j, des, lvalue;
	double dvalue, total, min_total;

	Int_Itr itr;
	Int_Map_Itr des_itr;

	n = 0;
	for (itr = table_fields.begin (); itr != table_fields.end (); itr++) {
		j = Field (*itr)->Decimal ();
		if (j > n) n = j;
	}
	min_total = 5.0 / pow (10.0, (n + 1));

	Put_Field (org_field, origin);
	Put_Field (period_field, period);

	for (des_itr = des_map.begin (); des_itr != des_map.end (); des_itr++) {
		des = des_itr->first;
		j = des_itr->second;

		Put_Field (des_field, des);

		total = 0;
	
		switch (type) {
			default:
			case DB_DOUBLE:
				for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
					total += dvalue = double_row [n] [j];
					Put_Field (*itr, dvalue);
				}
				break;
			case DB_FLOAT:
				for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
					total += dvalue = float_row [n] [j];
					Put_Field (*itr, dvalue);
				}
				break;
			case DB_INTEGER:
				for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
					total += lvalue = int_row [n] [j];
					Put_Field (*itr, lvalue);
				}
				break;
			case DB_SHORT:
				for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
					total += lvalue = short_row [n] [j];
					Put_Field (*itr, lvalue);
				}
				break;
		}
		if (total >= min_total) {
			if (!Db_Header::Write ()) return (false);
		}
	}
	return (true);
}

bool Db_Matrix::Write_Row (void *data, int org, int table, int period)
{
	org = table = period = *((int *) data);
	return (false);
}

//---------------------------------------------------------
//	Read_Matrix
//---------------------------------------------------------

bool Db_Matrix::Read_Matrix (void)
{
	int n, org, des, period, num_periods;
	Int_Itr itr;

	if (!data_flag || !matrix_flag) {
		if (!Allocate_Data (true)) return (false);
	}
	exe->Show_Message (String ("Reading %s -- Record") % File_Type ());
	exe->Set_Progress ();

	num_periods = Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	while (Db_Header::Read ()) {
		exe->Show_Progress ();

		org = Add_Org (Get_Integer (org_field));
		if (org < 0) continue;

		period = Get_Integer (period_field);
		if (period < 0 || period >= num_periods) continue;

		des = Add_Des (Get_Integer (des_field));
		if (des < 0) continue;

		for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
			switch (type) {
				default:
				case DB_DOUBLE:
					double_matrix [period] [org] [n] [des] = Get_Double (*itr);
					break;
				case DB_FLOAT:
					float_matrix [period] [org] [n] [des] = (float) Get_Double (*itr);
					break;
				case DB_INTEGER:
					int_matrix [period] [org] [n] [des] = Get_Integer (*itr);
					break;
				case DB_SHORT:
					short_matrix [period] [org] [n] [des] = (short) Get_Integer (*itr);
					break;
			}
		}
	}
	exe->End_Progress ();
	return (true);
}

//---------------------------------------------------------
//	Write_Matrix
//---------------------------------------------------------

bool Db_Matrix::Write_Matrix (void)
{
	int i, j, n, org, des, period, num_periods, lvalue, num_out;
	double dvalue, total, min_total;

	Int_Itr itr;
	Int_Map_Itr org_itr, des_itr;
	
	if (!data_flag || !matrix_flag) return (false);

	exe->Show_Message (String ("Writing %s -- Record") % File_Type ());
	exe->Set_Progress ();

	num_periods = Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	num_out = n = 0;

	for (itr = table_fields.begin (); itr != table_fields.end (); itr++) {
		j = Field (*itr)->Decimal ();
		if (j > n) n = j;
	}
	min_total = 5.0 / pow (10.0, (n + 1));

	//---- process each time period ----

	for (period=0; period < num_periods; period++) {

		Put_Field (period_field, period);

		//---- process each origin zone ----

		for (org_itr = org_map.begin (); org_itr != org_map.end (); org_itr++) {
			org = org_itr->first;
			i = org_itr->second;

			Put_Field (org_field, org);

			//---- process each destination ----

			for (des_itr = des_map.begin (); des_itr != des_map.end (); des_itr++) {
				des = des_itr->first;
				j = des_itr->second;

				Put_Field (des_field, des);

				total = 0;

				for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
					switch (type) {
						default:
						case DB_DOUBLE:
							total += dvalue = double_matrix [period] [i] [n] [j];
							Put_Field (*itr, dvalue);
							break;
						case DB_FLOAT:
							total += dvalue = float_matrix [period] [i] [n] [j];
							Put_Field (*itr, dvalue);
							break;
						case DB_INTEGER:
							total += lvalue = int_matrix [period] [i] [n] [j];
							Put_Field (*itr, lvalue);
							break;
						case DB_SHORT:
							total += lvalue = short_matrix [period] [i] [n] [j];
							Put_Field (*itr, lvalue);
							break;
					}
				}
				if (total >= min_total) {
					exe->Show_Progress ();

					if (!Db_Header::Write ()) return (false);
					num_out++;
				}
			}
		}
	}
	exe->End_Progress ();
	exe->Print (1, String ("Number of %s Records = %d") % File_Type () % num_out);
	return (true);
}

//---------------------------------------------------------
//	Set_Fields
//---------------------------------------------------------

bool Db_Matrix::Set_Fields (int org, int des, int period)
{
	if (!data_flag) return (false);

	Put_Field (org_field, org);
	Put_Field (des_field, des);
	Put_Field (period_field, period);

	des = Des_Index (des);
	if (des < 0) return (false);

	int n;
	Int_Itr itr;

	switch (type) {
		default:
		case DB_DOUBLE:
			for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
				Put_Field (*itr, double_row [n] [des]);
			}
			break;
		case DB_FLOAT:
			for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
				Put_Field (*itr, (double) float_row [n] [des]);
			}
			break;
		case DB_INTEGER:
			for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
				Put_Field (*itr, int_row [n] [des]);
			}
			break;
		case DB_SHORT:
			for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
				Put_Field (*itr, (int) short_row [n] [des]);
			}
			break;
	}
	return (true);
}

//---------------------------------------------------------
//	Get_Fields
//---------------------------------------------------------

bool Db_Matrix::Get_Fields (void)
{
	if (!data_flag) return (false);

	int des = Des_Index (Get_Integer (des_field));

	int n;
	Int_Itr itr;

	for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
		switch (type) {
			default:
			case DB_DOUBLE:
				double_row [n] [des] = Get_Double (*itr);
				break;
			case DB_FLOAT:
				float_row [n] [des] = (float) Get_Double (*itr);
				break;
			case DB_INTEGER:
				int_row [n] [des] = Get_Integer (*itr);
				break;
			case DB_SHORT:
				short_row [n] [des] = (short) Get_Integer (*itr);
				break;
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Set_Shared_Fields
//---------------------------------------------------------

bool Db_Matrix::Set_Shared_Fields (Db_Matrix *file, int org, int des, int period)
{
	if (file == 0 || file->Data_Flag () == 0) return (false);

	Put_Field (org_field, org);
	Put_Field (des_field, des);
	Put_Field (period_field, period);
	
	des = file->Des_Index (des);

	int n, lvalue;
	double dvalue;
	Int_Itr itr;

	if (type == DB_INTEGER || type == DB_SHORT) {
		for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
			file->Get_Cell_Index (des, n, lvalue);
			Put_Field (*itr, lvalue);
		}
	} else {
		for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
			file->Get_Cell_Index (des, n, dvalue);
			Put_Field (*itr, dvalue);
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Get_Shared_Fields
//---------------------------------------------------------

bool Db_Matrix::Get_Shared_Fields (Db_Matrix *file)
{
	if (file == 0 || !file->Data_Flag ()) return (false);

	int des = file->Des_Index (Get_Integer (des_field));

	int n;
	Int_Itr itr;
	bool flag = (type == DB_INTEGER || type == DB_SHORT);

	for (n=0, itr = table_fields.begin (); itr != table_fields.end (); itr++, n++) {
		if (flag) {
			file->Set_Cell_Index (des, n, Get_Integer (*itr));
		} else {
			file->Set_Cell_Index (des, n, Get_Double (*itr));
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Add_Org
//-----------------------------------------------------------

int Db_Matrix::Add_Org (int org)
{
	if (org <= 0)  return (-1);

	Int_Map_Itr itr = org_map.find (org);
	if (itr != org_map.end ()) {
		return (itr->second);
	}
	if (!data_flag || (int) org_map.size () < num_org) {
		return (org_map.insert (Int_Map_Data (org, (int) org_map.size ())).first->second);
	} else {
		return (-1);	
	}
}

//-----------------------------------------------------------
//	Add_Des
//-----------------------------------------------------------

int Db_Matrix::Add_Des (int des)
{
	if (des <= 0)  return (-1);

	Int_Map_Itr itr = des_map.find (des);
	if (itr != des_map.end ()) {
		return (itr->second);
	}
	if (!data_flag || (int) des_map.size () < num_des) {
		return (des_map.insert (Int_Map_Data (des, (int) des_map.size ())).first->second);
	} else {
		return (-1);
	}
}

//-----------------------------------------------------------
//	Org_Index
//-----------------------------------------------------------

int Db_Matrix::Org_Index (int org)
{
	Int_Map_Itr itr = org_map.find (org);
	if (itr == org_map.end ()) {
		if (org_map.size () == 0) {
			return (-2);
		}
		itr--;
		if (org > itr->first) {
			return (-2);
		} else {
			return (-1);
		}
	} else {
		return (itr->second);
	}
}

//-----------------------------------------------------------
//	Des_Index
//-----------------------------------------------------------

int Db_Matrix::Des_Index (int des)
{
	Int_Map_Itr itr = des_map.find (des);
	if (itr == des_map.end ()) {
		if (des_map.size () == 0) {
			return (-2);
		}
		itr--;
		if (des > itr->first) {
			return (-2);
		} else {
			return (-1);
		}
	} else {
		return (itr->second);
	}
}

//-----------------------------------------------------------
//	Copy_OD_Map
//-----------------------------------------------------------

bool Db_Matrix::Copy_OD_Map (Db_Matrix *file)
{
	Int_Map_Itr itr;
	Int_Map *map;

	map = file->Org_Map ();
	if ((int) map->size () == 0) return (false);

	org_map.clear ();
	Num_Org (file->Num_Org ());
	org_map.insert (map->begin (), map->end ());

	map = file->Des_Map ();
	if ((int) map->size () == 0) return (false);

	des_map.clear ();
	Num_Des (file->Num_Des ());
	des_map.insert (map->begin (), map->end ());
	return (true);
}

//-----------------------------------------------------------
//	Copy_Matrix_Data
//-----------------------------------------------------------

bool Db_Matrix::Copy_Matrix_Data (Db_Matrix *file)
{
	int period, num_periods, table, num_tables;
	double value;
	Int_Map_Itr org_itr, des_itr;
	Int_Map *org_map, *des_map;

	if (!matrix_flag) Allocate_Data (true);

	num_periods = file->Num_Periods ();
	if (num_periods < 1) num_periods = 1;

	num_tables = file->Tables ();

	org_map = file->Org_Map ();
	des_map = file->Des_Map ();

	for (period=0; period < num_periods; period++) {
		for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
				for (table=0; table < num_tables; table++) {
					file->Get_Cell_Index (period, org_itr->second, des_itr->second, table, value);
					if (!Set_Cell (period, org_itr->first, des_itr->first, table, value)) {
						return (false);
					}
				}
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Matrix_Ptr
//---------------------------------------------------------

void * Db_Matrix::Matrix_Ptr (int period, int org, int tab)
{
	if (!matrix_flag) return (false);

	switch (type) {
		default:
		case DB_DOUBLE:
			return ((void *) &double_matrix [period] [org] [tab] [0]);
		case DB_FLOAT:
			return ((void *) &float_matrix [period] [org] [tab] [0]);
		case DB_INTEGER:
			return ((void *) &int_matrix [period] [org] [tab] [0]);
		case DB_SHORT:
			return ((void *) &short_matrix [period] [org] [tab] [0]);
	}
	return (0);
}

//---------------------------------------------------------
//	Row_Ptr
//---------------------------------------------------------

void * Db_Matrix::Row_Ptr (int tab)
{
	if (!data_flag) return (false);

	switch (type) {
		default:
		case DB_DOUBLE:
			return ((void *) &double_row [tab] [0]);
		case DB_FLOAT:
			return ((void *) &float_row [tab] [0]);
		case DB_INTEGER:
			return ((void *) &int_row [tab] [0]);
		case DB_SHORT:
			return ((void *) &short_row [tab] [0]);
	}
	return (0);
}

//---------------------------------------------------------
//	Add_Cell
//---------------------------------------------------------

bool Db_Matrix::Add_Cell (int des, int tab, int value)
{
	return (Add_Cell_Index (Add_Des (des), tab, value));
}

bool Db_Matrix::Add_Cell (int des, int tab, double value)
{
	return (Add_Cell_Index (Add_Des (des), tab, value));
}

bool Db_Matrix::Add_Cell (int period, int org, int des, int tab, int value)
{
	return (Add_Cell_Index (period, Add_Org (org), Add_Des (des), tab, value));
}

bool Db_Matrix::Add_Cell (int period, int org, int des, int tab, double value)
{
	return (Add_Cell_Index (period, Add_Org (org), Add_Des (des), tab, value));
}

//---------------------------------------------------------
//	Add_Cell_Index
//---------------------------------------------------------

bool Db_Matrix::Add_Cell_Index (int des, int tab, int value)
{
	if (!data_flag) return (false);

	switch (type) {
		default:
		case DB_DOUBLE:
			double_row [tab] [des] += (double) value;
			break;
		case DB_FLOAT:
			float_row [tab] [des] += (float) value;
			break;
		case DB_INTEGER:
			int_row [tab] [des] += value;
			break;
		case DB_SHORT:
			short_row [tab] [des] += (short) value;
			break;
	}
	return (true);
}

bool Db_Matrix::Add_Cell_Index (int des, int tab, double value)
{
	if (!data_flag) return (false);

	switch (type) {
		default:
		case DB_DOUBLE:
			double_row [tab] [des] += value;
			break;
		case DB_FLOAT:
			float_row [tab] [des] += (float) value;
			break;
		case DB_INTEGER:
			int_row [tab] [des] += (int) value;
			break;
		case DB_SHORT:
			short_row [tab] [des] += (short) value;
			break;
	}
	return (true);
}

bool Db_Matrix::Add_Cell_Index (int period, int org, int des, int tab, int value)
{
	if (!data_flag) return (false);

	if (!matrix_flag) {
		return (Add_Cell_Index (des, tab, value));
	}
	switch (type) {
		default:
		case DB_DOUBLE:
			double_matrix [period] [org] [tab] [des] += (double) value;
			break;
		case DB_FLOAT:
			float_matrix [period] [org] [tab] [des] += (float) value;
			break;
		case DB_INTEGER:
			int_matrix [period] [org] [tab] [des] += value;
			break;
		case DB_SHORT:
			short_matrix [period] [org] [tab] [des] += (short) value;
			break;
	}
	return (true);
}

bool Db_Matrix::Add_Cell_Index (int period, int org, int des, int tab, double value)
{
	if (!data_flag) return (false);
	
	if (!matrix_flag) {
		return (Add_Cell_Index (des, tab, value));
	}
	switch (type) {
		default:
		case DB_DOUBLE:
			double_matrix [period] [org] [tab] [des] += value;
			break;
		case DB_FLOAT:
			float_matrix [period] [org] [tab] [des] += (float) value;
			break;
		case DB_INTEGER:
			int_matrix [period] [org] [tab] [des] += (int) value;
			break;
		case DB_SHORT:
			short_matrix [period] [org] [tab] [des] += (short) value;
			break;
	}
	return (true);
}

//---------------------------------------------------------
//	Set_Cell
//---------------------------------------------------------

bool Db_Matrix::Set_Cell (int des, int tab, int value)
{
	return (Set_Cell_Index (Add_Des (des), tab, value));
}

bool Db_Matrix::Set_Cell (int des, int tab, double value)
{
	return (Set_Cell_Index (Add_Des (des), tab, value));
}

bool Db_Matrix::Set_Cell (int period, int org, int des, int tab, int value)
{
	return (Set_Cell_Index (period, Add_Org (org), Add_Des (des), tab, value));
}

bool Db_Matrix::Set_Cell (int period, int org, int des, int tab, double value)
{
	return (Set_Cell_Index (period, Add_Org (org), Add_Des (des), tab, value));
}

//---------------------------------------------------------
//	Set_Cell_Index
//---------------------------------------------------------

bool Db_Matrix::Set_Cell_Index (int des, int tab, int value)
{
	if (!data_flag) return (false);

	switch (type) {
		default:
		case DB_DOUBLE:
			double_row [tab] [des] = (double) value;
			break;
		case DB_FLOAT:
			float_row [tab] [des] = (float) value;
			break;
		case DB_INTEGER:
			int_row [tab] [des] = value;
			break;
		case DB_SHORT:
			short_row [tab] [des] = (short) value;
			break;
	}
	return (true);
}

bool Db_Matrix::Set_Cell_Index (int des, int tab, double value)
{
	if (!data_flag) return (false);

	switch (type) {
		default:
		case DB_DOUBLE:
			double_row [tab] [des] = value;
			break;
		case DB_FLOAT:
			float_row [tab] [des] = (float) value;
			break;
		case DB_INTEGER:
			int_row [tab] [des] = (int) value;
			break;
		case DB_SHORT:
			short_row [tab] [des] = (short) value;
			break;
	}
	return (true);
}

bool Db_Matrix::Set_Cell_Index (int period, int org, int des, int tab, int value)
{
	if (!data_flag) return (false);

	if (!matrix_flag) {
		return (Set_Cell_Index (des, tab, value));
	}
	switch (type) {
		default:
		case DB_DOUBLE:
			double_matrix [period] [org] [tab] [des] = (double) value;
			break;
		case DB_FLOAT:
			float_matrix [period] [org] [tab] [des] = (float) value;
			break;
		case DB_INTEGER:
			int_matrix [period] [org] [tab] [des] = value;
			break;
		case DB_SHORT:
			short_matrix [period] [org] [tab] [des] = (short) value;
			break;
	}
	return (true);
}

bool Db_Matrix::Set_Cell_Index (int period, int org, int des, int tab, double value)
{
	if (!data_flag) return (false);

	if (!matrix_flag) {
		return (Set_Cell_Index (des, tab, value));
	}
	switch (type) {
		default:
		case DB_DOUBLE:
			double_matrix [period] [org] [tab] [des] = value;
			break;
		case DB_FLOAT:
			float_matrix [period] [org] [tab] [des] = (float) value;
			break;
		case DB_INTEGER:
			int_matrix [period] [org] [tab] [des] = (int) value;
			break;
		case DB_SHORT:
			short_matrix [period] [org] [tab] [des] = (short) value;
			break;
	}
	return (true);
}

//---------------------------------------------------------
//	Get_Cell
//---------------------------------------------------------

bool Db_Matrix::Get_Cell (int des, int tab, int &value)
{
	return (Get_Cell_Index (Des_Index (des), tab, value));
}

bool Db_Matrix::Get_Cell (int des, int tab, double &value)
{
	return (Get_Cell_Index (Des_Index (des), tab, value));
}

bool Db_Matrix::Get_Cell (int period, int org, int des, int tab, int &value)
{
	return (Get_Cell_Index (period, Org_Index (org), Des_Index (des), tab, value));
}

bool Db_Matrix::Get_Cell (int period, int org, int des, int tab, double &value)
{
	return (Get_Cell_Index (period, Org_Index (org), Des_Index (des), tab, value));
}

//---------------------------------------------------------
//	Get_Cell_Index
//---------------------------------------------------------

bool Db_Matrix::Get_Cell_Index (int des, int tab, int &value)
{
	if (!data_flag) return (false);

	switch (type) {
		default:
		case DB_DOUBLE:
			value = (int) double_row [tab] [des];
			break;
		case DB_FLOAT:
			value = (int) float_row [tab] [des];
			break;
		case DB_INTEGER:
			value = int_row [tab] [des];
			break;
		case DB_SHORT:
			value = (int) short_row [tab] [des];
			break;
	}
	return (true);
}

bool Db_Matrix::Get_Cell_Index (int des, int tab, double &value)
{
	if (!data_flag) return (false);

	switch (type) {
		default:
		case DB_DOUBLE:
			value = double_row [tab] [des];
			break;
		case DB_FLOAT:
			value = (double) float_row [tab] [des];
			break;
		case DB_INTEGER:
			value = (double) int_row [tab] [des];
			break;
		case DB_SHORT:
			value = (double) short_row [tab] [des];
			break;
	}
	return (true);
}

bool Db_Matrix::Get_Cell_Index (int period, int org, int des, int tab, int &value)
{
	if (!data_flag) return (false);

	if (!matrix_flag) {
		if (!Get_Cell_Index (des, tab, value)) return (false);
	} else {
		switch (type) {
			default:
			case DB_DOUBLE:
				value = (int) double_matrix [period] [org] [tab] [des];
				break;
			case DB_FLOAT:
				value = (int) float_matrix [period] [org] [tab] [des];
				break;
			case DB_INTEGER:
				value = int_matrix [period] [org] [tab] [des];
				break;
			case DB_SHORT:
				value = (int) short_matrix [period] [org] [tab] [des];
				break;
		}
	}
	return (true);
}

bool Db_Matrix::Get_Cell_Index (int period, int org, int des, int tab, double &value)
{
	if (!data_flag) return (false);
	
	if (!matrix_flag) {
		if (!Get_Cell_Index (des, tab, value)) return (false);
	} else {
		switch (type) {
			default:
			case DB_DOUBLE:
				value = double_matrix [period] [org] [tab] [des];
				break;
			case DB_FLOAT:
				value = (double) float_matrix [period] [org] [tab] [des];
				break;
			case DB_INTEGER:
				value = (double) int_matrix [period] [org] [tab] [des];
				break;
			case DB_SHORT:
				value = (double) short_matrix [period] [org] [tab] [des];
				break;
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Factor_Cell
//---------------------------------------------------------

bool Db_Matrix::Factor_Cell (int des, int tab, double factor)
{
	return (Factor_Cell_Index (Des_Index (des), tab, factor));
}

bool Db_Matrix::Factor_Cell (int period, int org, int des, int tab, double factor)
{
	return (Factor_Cell_Index (period, Org_Index (org), Des_Index (des), tab, factor));
}

//---------------------------------------------------------
//	Factor_Cell_Index
//---------------------------------------------------------

bool Db_Matrix::Factor_Cell_Index (int des, int tab, double factor)
{
	if (!data_flag || des < 0) return (false);

	switch (type) {
		default:
		case DB_DOUBLE:
			double_row [tab] [des] *= factor;
			break;
		case DB_FLOAT:
			float_row [tab] [des] *= (float) factor;
			break;
		case DB_INTEGER:
			int_row [tab] [des] = DTOI (int_row [tab] [des] * factor);
			break;
		case DB_SHORT:
			short_row [tab] [des] = (short) DTOI (short_row [tab] [des] * factor);
			break;
	}
	return (true);
}

bool Db_Matrix::Factor_Cell_Index (int period, int org, int des, int tab, double factor)
{
	if (!data_flag || org < 0 || des < 0) return (false);

	if (!matrix_flag) {
		return (Factor_Cell_Index (des, tab, factor));
	}
	switch (type) {
		default:
		case DB_DOUBLE:
			double_matrix [period] [org] [tab] [des] *= factor;
			break;
		case DB_FLOAT:
			float_matrix [period] [org] [tab] [des] *= (float) factor;
			break;
		case DB_INTEGER:
			int_matrix [period] [org] [tab] [des] = DTOI (int_matrix [period] [org] [tab] [des] * factor);
			break;
		case DB_SHORT:
			short_matrix [period] [org] [tab] [des] = (short) DTOI (short_matrix [period] [org] [tab] [des] * factor);
			break;
	}
	return (true);
}

//---------------------------------------------------------
//	Factor_Row
//---------------------------------------------------------

bool Db_Matrix::Factor_Row (int tab, double factor)
{
	if (!data_flag) return (false);

	switch (type) {
		default:
		case DB_DOUBLE:
			{
				Doubles *ptr = &double_row [tab];
				for (Dbl_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					*itr *= factor;
				}				
			}
			break;
		case DB_FLOAT:
			{
				float fac = (float) factor;
				Floats *ptr = &float_row [tab];
				for (Flt_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					*itr *= fac;
				}
			}
			break;
		case DB_INTEGER:
			{
				Integers *ptr = &int_row [tab];
				for (Int_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					*itr = DTOI (*itr * factor);
				}
			}
			break;
		case DB_SHORT:
			{
				Shorts *ptr = &short_row [tab];
				for (Sht_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					*itr = (short) DTOI (*itr * factor);
				}
			}
			break;
	}
	return (true);
}

bool Db_Matrix::Factor_Row (int period, int org, int tab, double factor)
{
	return (Factor_Row_Index (period, Org_Index (org), tab, factor));
}

//---------------------------------------------------------
//	Factor_Row_Index
//---------------------------------------------------------

bool Db_Matrix::Factor_Row_Index (int period, int org, int tab, double factor)
{
	if (!data_flag || org < 0) return (false);
	if (!matrix_flag) {
		return (Factor_Row (tab, factor));
	}
	switch (type) {
		default:
		case DB_DOUBLE:
			{
				Doubles *ptr = &double_matrix [period] [org] [tab];
				for (Dbl_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					*itr *= factor;
				}				
			}
			break;
		case DB_FLOAT:
			{
				float fac = (float) factor;
				Floats *ptr = &float_matrix [period] [org] [tab];
				for (Flt_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					*itr *= fac;
				}
			}
			break;
		case DB_INTEGER:
			{
				Integers *ptr = &int_matrix [period] [org] [tab];
				for (Int_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					*itr = DTOI (*itr * factor);
				}
			}
			break;
		case DB_SHORT:
			{
				Shorts *ptr = &short_matrix [period] [org] [tab];
				for (Sht_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					*itr = (short) DTOI (*itr * factor);
				}
			}
			break;
	}
	return (true);
}

//---------------------------------------------------------
//	Row_Sum
//---------------------------------------------------------

double Db_Matrix::Row_Sum (int tab)
{
	if (!data_flag) return (false);

	double total = 0.0;

	switch (type) {
		default:
		case DB_DOUBLE:
			{
				Doubles *ptr = &double_row [tab];
				for (Dbl_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					total += *itr;
				}				
			}
			break;
		case DB_FLOAT:
			{
				Floats *ptr = &float_row [tab];
				for (Flt_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					total += *itr;
				}
			}
			break;
		case DB_INTEGER:
			{
				Integers *ptr = &int_row [tab];
				for (Int_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					total += *itr;
				}
			}
			break;
		case DB_SHORT:
			{
				Shorts *ptr = &short_row [tab];
				for (Sht_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					total += *itr;
				}
			}
			break;
	}
	return (total);
}

double Db_Matrix::Row_Sum (int period, int org, int tab)
{
	return (Row_Sum_Index (period, Org_Index (org), tab));
}

//---------------------------------------------------------
//	Row_Sum_Index
//---------------------------------------------------------

double Db_Matrix::Row_Sum_Index (int period, int org, int tab)
{
	if (!data_flag || org < 0) return (false);
	if (!matrix_flag) {
		return (Row_Sum (tab));
	}
	double total = 0.0;

	switch (type) {
		default:
		case DB_DOUBLE:
			{
				Doubles *ptr = &double_matrix [period] [org] [tab];
				for (Dbl_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					total += *itr;
				}				
			}
			break;
		case DB_FLOAT:
			{
				Floats *ptr = &float_matrix [period] [org] [tab];
				for (Flt_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					total += *itr;
				}
			}
			break;
		case DB_INTEGER:
			{
				Integers *ptr = &int_matrix [period] [org] [tab];
				for (Int_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					total += *itr;
				}
			}
			break;
		case DB_SHORT:
			{
				Shorts *ptr = &short_matrix [period] [org] [tab];
				for (Sht_Itr itr = ptr->begin (); itr != ptr->end (); itr++) {
					total += *itr;
				}
			}
			break;
	}
	return (total);
}

//---------------------------------------------------------
//	Allocate_Data
//---------------------------------------------------------

bool Db_Matrix::Allocate_Data (bool flag)
{
	if (data_flag) Clear_Data ();
	matrix_flag = flag;

	if ((int) org_map.size () > num_org) num_org = (int) org_map.size ();
	if ((int) des_map.size () > num_des) num_des = (int) des_map.size ();

	int periods = Num_Periods ();
	if (periods == 0) periods = 1;
	
	switch (type) {
		default:
		case DB_DOUBLE:
			{
				Doubles doubles;
				doubles.assign (num_des, 0);
				double_row.assign (Tables (), doubles);

				if (matrix_flag) {
					Dbl3_Array dbl3_array;
					dbl3_array.assign (num_org, double_row);
					double_matrix.assign (periods, dbl3_array);
				}
			}
			break;
		case DB_FLOAT:
			{
				Floats floats;
				floats.assign (num_des, 0);
				float_row.assign (Tables (), floats);

				if (matrix_flag) {
					Flt3_Array flt3_array;
					flt3_array.assign (num_org, float_row);
					float_matrix.assign (periods, flt3_array);
				}
			}
			break;
		case DB_INTEGER:
			{
				Integers integers;
				integers.assign (num_des, 0);
				int_row.assign (Tables (), integers);

				if (matrix_flag) {
					Int3_Array int3_array;
					int3_array.assign (num_org, int_row);
					int_matrix.assign (periods, int3_array);
				}
			}
			break;
		case DB_SHORT:
			{
				Shorts shorts;
				shorts.assign (num_des, 0);
				short_row.assign (Tables (), shorts);

				if (matrix_flag) {
					Sht3_Array sht3_array;
					sht3_array.assign (num_org, short_row);
					short_matrix.assign (periods, sht3_array);
				}
			}
			break;
	}
	data_flag = true;
	return (true);
}

//---------------------------------------------------------
//	Clear_Data
//---------------------------------------------------------

void Db_Matrix::Clear_Data (void)
{
	if (!data_flag) return;

	switch (type) {
		default:
		case DB_DOUBLE:
			double_row.clear ();
			if (matrix_flag) double_matrix.clear ();
			break;
		case DB_FLOAT:
			float_row.clear ();
			if (matrix_flag) float_matrix.clear ();
			break;
		case DB_INTEGER:
			int_row.clear ();
			if (matrix_flag) int_matrix.clear ();
			break;
		case DB_SHORT:
			short_row.clear ();
			if (matrix_flag) short_matrix.clear ();
			break;
	}
	data_flag = matrix_flag = false;
}

//---------------------------------------------------------
//	Zero_Data
//---------------------------------------------------------

void Db_Matrix::Zero_Data (void)
{
	if (!data_flag) return;

	switch (type) {
		default:
		case DB_DOUBLE:
			{
				Doubles doubles;
				doubles.assign (num_des, 0);
				double_row.assign (Tables (), doubles);
			}
			break;
		case DB_FLOAT:
			{
				Floats floats;
				floats.assign (num_des, 0);
				float_row.assign (Tables (), floats);
			}
			break;
		case DB_INTEGER:
			{
				Integers integers;
				integers.assign (num_des, 0);
				int_row.assign (Tables (), integers);
			}
			break;
		case DB_SHORT:
			{
				Shorts shorts;
				shorts.assign (num_des, 0);
				short_row.assign (Tables (), shorts);
			}
			break;
	}
}

//-----------------------------------------------------------
//	Table_Number
//-----------------------------------------------------------

int Db_Matrix::Table_Number (string name)
{
	int num;
	Int_Itr int_itr;
	Field_Ptr fld_ptr;

	for (num=0, int_itr = table_fields.begin (); int_itr != table_fields.end (); int_itr++, num++) {
		fld_ptr = Field (*int_itr);
		if (fld_ptr->Name ().Equals (name)) {
			return (num);
		}
	}
	return (-1);
}

//-----------------------------------------------------------
//	Replicate_Tables
//-----------------------------------------------------------

bool Db_Matrix::Replicate_Tables (Db_Matrix *file)
{
	double size;
	Field_Ptr fld_ptr;

	Tables (file->Tables ());

	for (int i=0; i < file->Tables (); i++) {
		fld_ptr = file->Table_Field (i);

		if (fld_ptr->Type () == DB_DOUBLE && fld_ptr->Size () == sizeof (float)) {
			Type (DB_FLOAT);
		} else if (fld_ptr->Type () == DB_INTEGER && fld_ptr->Size () == sizeof (short)) {
			Type (DB_SHORT);
		} else {
			Type (fld_ptr->Type ());
		}
		size = fld_ptr->Size () + (fld_ptr->Decimal () / 10.0);
		Add_Field (fld_ptr->Name (), fld_ptr->Type (), size, fld_ptr->Units ());
	}
	return (Tables () > 0);
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Db_Matrix::Set_Field_Numbers (void)
{
	Field_Ptr fld_ptr;

	org_field = Optional_Field (ORIGIN_FIELD_NAMES);
	des_field = Optional_Field (DESTINATION_FIELD_NAMES);
	period_field = Optional_Field (PERIOD_FIELD_NAMES);
	table_fields.clear ();

	for (int i=0; i < Num_Fields (); i++) {
		if (i == org_field || i == des_field || i == period_field) continue;
		table_fields.push_back (i);

		fld_ptr = Field (i);
		if (fld_ptr->Type () == DB_DOUBLE && fld_ptr->Size () == sizeof (float)) {
			Type (DB_FLOAT);
		} else if (fld_ptr->Type () == DB_INTEGER && fld_ptr->Size () == sizeof (short)) {
			Type (DB_SHORT);
		} else {
			Type (fld_ptr->Type ());
		}
	}
	return (Tables () > 0 && org_field >= 0 && des_field >= 0);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool Db_Matrix::Read_Header (bool stat)
{
	String meta = Header_Record ();
	if (!meta.empty ()) {
		Strings parts;
		Str_Itr itr;

		meta.Parse (parts, "=;");

		for (itr = parts.begin (); itr != parts.end (); itr++) {
			if (itr->Equals ("NUM_ORG")) {
				num_org = (++itr)->Integer ();
			} else if (itr->Equals ("NUM_DES")) {
				num_des = (++itr)->Integer ();
			} else if (itr->Equals ("PERIODS")) {
				clear ();
				Merge_Flag (false);
				if (++itr == parts.end () || itr->empty ()) break;
				if (!Add_Ranges (*itr)) return (false);
			} else if (itr->Equals ("MERGE")) {
				clear ();
				Merge_Flag (true);
				if (++itr == parts.end () || itr->empty ()) break;
				if (!Add_Ranges (*itr)) return (false);
			}
		}
	}
	return (Db_Header::Read_Header (stat));
}

//-----------------------------------------------------------
//	Write_Header
//-----------------------------------------------------------

bool Db_Matrix::Write_Header (string user_lines) 
{
	user_lines = String ("NUM_ORG=%d; NUM_DES=%d; ") % num_org % num_des;
	if (Merge_Flag ()) {
		user_lines += "MERGE=";
	} else {
		user_lines += "PERIODS=";
	}
	if (Num_Periods () > 0) {
		user_lines += Range_String ();
	}
	return (Db_Header::Write_Header (user_lines));
}
