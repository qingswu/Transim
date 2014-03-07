//*********************************************************
//	Read_Approach.cpp - read the approach link file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Read_Approach_Links
//---------------------------------------------------------

void TransimsNet::Read_Approach_Links (void)
{
	int link, index;
	Link_Data *link_ptr;
	Int_Map_Itr map_itr;
	Approach_Link_Map_Stat map_stat;
	Approach_Link_Data approach_rec, *approach_ptr;

	//---- read approach link file ----

	Show_Message (String ("Reading %s -- Record") % approach_file.File_Type ());
	Set_Progress ();

	while (approach_file.Read_Record ()) {
		Show_Progress ();

		//---- get the link number ----

		link = approach_file.Link ();
		if (link == 0) continue;

		map_itr = link_map.find (link);
		if (map_itr == link_map.end ()) {
			Warning (String ("Approach Link %d was Not Found") % link);
			continue;
		}
		link_ptr = &link_array [map_itr->second];

		approach_rec.Link (map_itr->second);
		approach_rec.Dir (approach_file.Dir ());

		if (approach_rec.Dir () == 0) {
			index = link_ptr->AB_Dir ();
		} else {
			index = link_ptr->BA_Dir ();
		}
		if (index < 0) {
			Warning (String ("Approach Link %d Direction %s was Not Found") % link % ((approach_rec.Dir () == 0) ? "AB" : "BA"));
			continue;
		}
		map_stat = approach_map.insert (Approach_Link_Map_Data (index, approach_rec));
		if (!map_stat.second) {
			Warning ("Duplicate Approach Link Record for Link = ") << link;
			continue;
		}
		approach_ptr = &map_stat.first->second;

		approach_ptr->Control (approach_file.Control ());
		approach_ptr->Group (approach_file.Group ());
		approach_ptr->LM_Length (approach_file.LM_Length ());
		approach_ptr->Left_Merge (approach_file.Left_Merge ());
		approach_ptr->LT_Length (approach_file.LT_Length ());
		approach_ptr->Left (approach_file.Left ());
		approach_ptr->Left_Thru (approach_file.Left_Thru ());
		approach_ptr->Thru (approach_file.Thru ());
		approach_ptr->Right_Thru (approach_file.Right_Thru ());
		approach_ptr->Right (approach_file.Right ());
		approach_ptr->RT_Length (approach_file.RT_Length ());
		approach_ptr->Right_Merge (approach_file.Right_Merge ());
		approach_ptr->RM_Length (approach_file.RM_Length ());
	}
	End_Progress ();

	approach_file.Close ();

	Print (2, "Number of ") << approach_file.File_Type () << " Records = " << approach_map.size ();
}
