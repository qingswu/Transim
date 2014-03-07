//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "Reschedule.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Reschedule::Program_Control (void)
{
	int i, j, num, stop, field, num2, line;
	String key;
	Strings text;
	Line_Filter filter_data, *filter_ptr;
	File_Data data, *data_ptr;

	//---- open network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- run schedule file ----

	num = Highest_Control_Group (RUN_SCHEDULE_FILE, 0);

	for (i=1; i <= num; i++) {

		//---- open the run schedule file ----

		key = Get_Control_String (RUN_SCHEDULE_FILE, i);

		if (key.empty ()) continue;
		Print (1);

		file_group.push_back (data);

		data_ptr = &file_group.back ();

		data_ptr->run_file = new Db_Header ();
		data_ptr->run_file->File_Type (String ("Run Schedule File #%d") % i);
		run_flag = true;

		//---- get the file format ----

		if (Check_Control_Key (RUN_SCHEDULE_FORMAT, i)) {
			data_ptr->run_file->Dbase_Format (Get_Control_String (RUN_SCHEDULE_FORMAT, i));
		}
		data_ptr->run_file->Open (Project_Filename (key));

		//---- filter field ----

		key = Get_Control_Text (RUN_FILTER_FIELD, i);

		if (!key.empty ()) {
			data_ptr->filter = data_ptr->run_file->Field_Number (key);
			if (data_ptr->filter < 0) {
				Error (String ("Run Filter Field %s was Not Found") % key);
			}
			Print (0, ", Number = ") << (data_ptr->filter + 1);
		}

		//---- run type field ----

		key = Get_Control_Text (RUN_TYPE_FIELD, i);

		if (!key.empty ()) {
			data_ptr->type = data_ptr->run_file->Field_Number (key);
			if (data_ptr->type < 0) {
				Error (String ("Run Type Field %s was Not Found") % key);
			}
			Print (0, ", Number = ") << (data_ptr->type + 1);
		}

		//---- run line filters ----

		num2 = Highest_Control_Group (RUN_SCHEDULE_LINE, i, 0);

		for (j=1; j <= num2; j++) {

			key = Get_Control_String (RUN_SCHEDULE_LINE, i, j);

			if (key.empty ()) continue;
			Print (1);

			line = Get_Control_Integer (RUN_SCHEDULE_LINE, i, j);

			data_ptr->lines.push_back (filter_data);

			filter_ptr = &data_ptr->lines.back ();

			filter_ptr->line = line;

			//---- run filter field ----

			if (data_ptr->filter >= 0) {
				key = Get_Control_Text (RUN_FILTER_RANGE, i, j);

				if (key.Equals ("ALL") || key.empty ()) {
					filter_ptr->low = 0;
					filter_ptr->high = MAX_INTEGER;
				} else {
					key.Range (filter_ptr->low, filter_ptr->high);
				}
			}

			key = Get_Control_String (RUN_STOP_FIELD_FILE, i, j);

			if (!key.empty ()) {
				Db_File file;

				file.File_Type (String ("Run Stop Field File #%d #%d") % i %j);

				file.Open (Project_Filename (key));

				while (file.Read ()) {
					key = file.Record_String ();
					if (key.empty ()) continue;

					key.Parse (text, "=\t");
					if (text.size () < 2) continue;

					stop = text [0].Integer ();
				
					field = data_ptr->run_file->Field_Number (text [1]);

					if (field < 0) {
						Error (String ("Stop Field %s was Not Found") % text [1]);
					}
					filter_ptr->stop_field.insert (Int_Map_Data (stop, field));
				}
				file.Close ();
			}
		}
	}

	//---- read report types ----

	List_Reports ();

	match_dump = Report_Flag (MATCH_DUMP);
} 
