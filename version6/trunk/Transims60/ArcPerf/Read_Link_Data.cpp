//*********************************************************
//	Read_Link_Data.cpp - read the link data file
//*********************************************************

#include "ArcPerf.hpp"

//---------------------------------------------------------
//	Read_Link_Data
//---------------------------------------------------------

void ArcPerf::Read_Link_Data (void)
{
	int link, num_rec, period, num_period;
	Dtime low, high;
	String buffer;

	Link_Data *link_ptr;
	Int_Map_Itr map_itr;
		
	num_rec = 0;

	if (link_period_flag) {
		Print (1);
		num_period = link_data_file.Num_Periods ();
		
		for (period=0; period < num_period; period++) {

			link_data_file.Period_Range (period, low, high);

			//---- create a new shape file ----

			buffer = String ("%s_%s_%s.shp") % linkname % low.Time_Label (true) % high.Time_Label (true);

			arcview_link_data.First_Open (true);

			if (!arcview_link_data.Open (buffer)) {
				File_Error ("Opening ArcView Link Data File", arcview_link_data.Shape_Filename ());
			}
			arcview_link_data.Write_Header ();

			Show_Message (String ("Writing %s %s -- Record") % arcview_link_data.File_Type () % high.Time_String ());
			Set_Progress ();

			//---- process each link ----

			while (link_data_file.Read ()) {
				Show_Progress ();

				//---- get the link direction ----

				link = link_data_file.Link ();

				map_itr = link_map.find (link);
				if (map_itr == link_map.end ()) continue;

				link_ptr = &link_array [map_itr->second];
				if (link_ptr->Use () == 0) continue;

				//---- copy the data fields ----

				arcview_link_data.Copy_Fields (link_data_file);

				arcview_link_data.Put_Field (3, link_data_file.Data_AB (period));
				arcview_link_data.Put_Field (4, link_data_file.Data_BA (period));				

				Draw_Link (arcview_link_data, link_ptr, 2);
				num_rec++;
			}
			End_Progress ();
			arcview_link_data.Close ();
			link_data_file.Rewind ();
		}

	} else {
	
		Show_Message (String ("Reading %s -- Record") % arcview_link_data.File_Type ());
		Set_Progress ();

		//---- process each link ----

		while (link_data_file.Read ()) {
			Show_Progress ();

			//---- get the link direction ----

			link = link_data_file.Link ();

			map_itr = link_map.find (link);
			if (map_itr == link_map.end ()) continue;

			link_ptr = &link_array [map_itr->second];
			if (link_ptr->Use () == 0) continue;

			//---- copy the data fields ----

			arcview_link_data.Copy_Fields (link_data_file);

			Draw_Link (arcview_link_data, link_ptr, 2);
			num_rec++;
		}
		End_Progress ();
		arcview_link_data.Close ();
	}

	Print (2, "Number of Arcview Link Data Records = ") << num_rec;
}
