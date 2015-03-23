//*********************************************************
//	Draw_Service - draw a transit stop service file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Draw_Service
//---------------------------------------------------------

void ArcNet::Draw_Stop_Service (void)
{
	int i, j, runs, num_user;
	int stop_field, x_field, y_field, routes_field, num_service;

	Line_Itr line_itr;
	XYZ_Point point;
	Point_Map_Itr stop_pt_itr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Stop_Itr st_itr;
	Ints_Array data_array;
	Dbls_Array user_array;
	Integers *service_ptr, service_data;
	Doubles *user_ptr, user_data;

	stop_field = arcview_service.Required_Field ("STOP", "STOP_ID", "ID");
	x_field = arcview_service.Required_Field (X_FIELD_NAMES);
	y_field = arcview_service.Required_Field (Y_FIELD_NAMES);
	routes_field = arcview_service.Optional_Field ("ROUTES");

	num_service = transit_time_periods.Num_Ranges () + 1;
	num_user = (int) user_fields.size ();

	service_data.assign (num_service, 0);
	data_array.assign (stop_array.size (), service_data);

	if (num_user > 0) {
		user_data.assign (num_user, 0);
		user_array.assign (stop_array.size (), user_data);
	}

	arcview_service.push_back (point);

	Show_Message ("Draw Stop Service -- Record");
	Set_Progress ();

	//---- process each transit route ----

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		Show_Progress ();

		if (num_user > 0) {
			line_db.Read_Record (line_itr->Route ());
		}
		for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {
			service_ptr = &data_array [stop_itr->Stop ()];

			service_ptr->at (0) += 1;

			for (i=0; i < transit_time_periods.Num_Ranges (); i++) {
				runs = 0;

				for (run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++) {
					if (transit_time_periods.In_Index (run_itr->Schedule ()) == i) {
						runs++;
					}
				}
				service_ptr->at (i+1) += runs;
			}
			if (num_user > 0) {
				user_ptr = &user_array [stop_itr->Stop ()];

				for (i=0; i < num_user; i++) {
					user_ptr->at (i) += line_db.Get_Double (user_fields [i]);
				}
			}
		}
	}

	//---- draw the stops ----

	for (i=0, st_itr = stop_array.begin (); st_itr != stop_array.end (); st_itr++, i++) {
		stop_pt_itr = stop_pt.find (st_itr->Stop ());
		service_ptr = &data_array [i];

		arcview_service.Put_Field (stop_field, st_itr->Stop ());
		arcview_service.Put_Field (x_field, stop_pt_itr->second.x);
		arcview_service.Put_Field (y_field, stop_pt_itr->second.y);
		arcview_service.Put_Field (routes_field, service_ptr->at (0));

		for (j=0; j < transit_time_periods.Num_Ranges (); j++) {
			arcview_service.Put_Field (service_field + j, service_ptr->at (j+1));
		}
		if (num_user > 0) {
			user_ptr = &user_array [i];

			for (j=0; j < num_user; j++) {
				arcview_service.Put_Field (user_field + j, user_ptr->at (j));
			}
		}
		arcview_service [0] = stop_pt_itr->second;

		if (!arcview_service.Write_Record ()) {
			Error (String ("Writing %s") % arcview_service.File_Type ());
		}
	}
	End_Progress ();

	arcview_service.Close ();
}
