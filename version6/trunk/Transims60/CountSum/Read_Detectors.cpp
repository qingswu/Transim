//*********************************************************
//	Read_Detectors.cpp - read signal detector file
//*********************************************************

#include "CountSum.hpp"

#include "Compass.hpp"

//---------------------------------------------------------
//	Read_Signal_Detector
//---------------------------------------------------------

void CountSum::Read_Signal_Detectors (void)
{
	int id, node, index, dir;
	String name;
	bool flag;

	Str_Itr str_itr;
	Signal_Pt_Itr signal_itr;
	Int_Map_Itr detect_itr;
    Int_Map_Stat detect_stat;
	Dir_Data *dir_ptr;
	List_Data *list;
	Int2_Key dir_offset;

	Compass_Points compass (4);

	//---- read signal detectors----

	Show_Message (String ("Reading %s -- Record") % detector_file.File_Type ());
	Set_Progress ();

	while (detector_file.Read_Record ()) {
		Show_Progress ();

		//---- get the signal number ----

		id = detector_file.Get_Integer (device_field);
		if (id == 0) continue;

		signal_itr = signal_pt_map.find (id);
		if (signal_itr == signal_pt_map.end ()) continue;
		
		id = detector_file.Get_Integer (detector_field);
		if (id == 0) continue;

		if (detector_types.size () > 0) {
			name = detector_file.Get_String (det_type_field).Trim ();

			flag = false;
			for (str_itr = detector_types.begin (); str_itr != detector_types.end (); str_itr++) {
				if (str_itr->Equals (name)) {
					flag = true;
					break;
				}
			}
			if (!flag) continue;
		}
		name = detector_file.Get_String (det_name_field).Trim ();

		if (name.Starts_With ("NB" ) || name.Starts_With ("NB-")) {
			dir = 0;
		} else if (name.Starts_With ("EB ") || name.Starts_With ("EB-")) {
			dir = 1;
		} else if (name.Starts_With ("WB ") || name.Starts_With ("WB-")) {
			dir = 2;
		} else if (name.Starts_With ("SB ") || name.Starts_With ("SB-")) {
			dir = 3;
		} else {
			Warning ("Decector Direction was Not Found for Signal ") << id;
			continue;
		}
		node = signal_itr->second.node;

		list = &node_list [node];
		if (list == 0) continue;

		for (index = list->To_List (); index >= 0; index = dir_ptr->First_Connect_To ()) {
			dir_ptr = &dir_array [index];

			if (dir == compass.Bearing_Point (dir_ptr->Out_Bearing ())) {
				detect_stat = detect_dir_map.insert (Int_Map_Data (id, index));
				if (!detect_stat.second) {
					Warning ("Duplicate Detector Number = ") << id;
				}
				dir_offset.first = index;
				dir_offset.second = 0;

				offset_index_map.insert (Offset_Index_Data (dir_offset, (int) offset_index_map.size ()));
				break;
			}
		}

	}
	End_Progress ();

	detector_file.Close ();

	if (Progress_Count () != (int) detect_dir_map.size ()) {
		Show_Message (1, "Number of Signal Detector Records = ") << detect_dir_map.size ();
	}
	Print (2, "Number of ") << detector_file.File_Type () << " Records = " << detect_dir_map.size ();
}
