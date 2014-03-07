//*********************************************************
//	Read_Data.cpp - Read the Zone Data File into Memory
//*********************************************************

#include "GridData.hpp"

//---------------------------------------------------------
//	Read_Data
//---------------------------------------------------------

void GridData::Read_Data (void)
{
	int zone, field, count;

	Db_Sort_Array *data;
	Db_Header *file;
	Data_Itr data_itr;

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
		file = data_itr->file;
		data = data_itr->data_db;
		field = data_itr->zone_field;

		//---- read the data file ----

		Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		Set_Progress ();

		count = 0;

		while (file->Read ()) {
			Show_Progress ();

			//---- get the join field id ----

			zone = file->Get_Integer (field);
			if (zone == 0) continue;

			//---- copy the data fields ----

			data->Copy_Fields (*file);

			//---- save the database record ----

			if (!data->Write_Record (zone)) {
				Error ("Writing Data File Database");
			}
			count++;
		}
		End_Progress ();

		Print (2, String ("Number of Zone Data #%d Records = %d") % data_itr->group % count);
		
		file->Close ();
	}
}
