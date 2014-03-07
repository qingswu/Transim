//*********************************************************
//	Equiv_Data.cpp - base class for Equivalence Files
//*********************************************************

#include "Equiv_Data.hpp"

//---------------------------------------------------------
//	Equiv_Data constructor
//---------------------------------------------------------

Equiv_Data::Equiv_Data (string _type) : Db_File ()
{
	time_flag = false;

	Type (_type);

	if (!_type.empty ()) {
		File_Type (Type () + " Equivalence File");
	} else {
		File_Type ("Equivalence File");
	}
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Equiv_Data::Read (bool report_flag)
{
	int num, count, group;
	double low, high;
	String result, text;
	bool first;

	Equiv_Group equiv_group, *group_ptr;
	Group_Map_Itr group_itr;
	Group_Map_Stat group_stat;
	Int_Set_Stat list_stat;

	exe->Send_Messages (report_flag);
	report_flag = exe->Send_Messages ();

	//---- read the equiv file ----

	if (report_flag) {
		exe->New_Page ();
		exe->Print (1, Type ()) << " Equivalence";
		exe->Print (1);
	}
	first = false;
	count = 0;
	group_map.clear ();

	while (Db_File::Read ()) {

		text = Record_String ();
		if (text.empty ()) continue;

		if (!text.Split (result)) continue;

		group = result.Integer ();
		if (group == 0) continue;

		//---- check for a new group ----

		group_stat = group_map.insert (Group_Map_Data (group, equiv_group));

		group_ptr = &(group_stat.first->second);
		first = group_stat.second;

		//---- check for a label record ----
		
		if (!text.Split (result)) continue;

		num = result.Integer ();

		if (num == 0) {
			group_ptr->label = text;
		}

		//---- create a default label ----

		if (group_ptr->label.empty ()) {
			group_ptr->label ("%s Group %d") % Type () % group;
		}

		if (report_flag && first) {
			exe->Print (1, String ("[%-25.25s] %3d = ") % group_ptr->label % group);
			count = 0;
		}
		if (num == 0) continue;

		//---- process the equiv ranges ----

		if (time_flag) {
			if (!group_ptr->period.Add_Ranges (text)) return (false);

			if (report_flag) {
				exe->Print (0, text);
			}
		} else {
			Strings ranges;
			Str_Itr range_itr;

			//---- unpack the range string ----	

			text.Parse (ranges);

			for (range_itr = ranges.begin (); range_itr != ranges.end (); range_itr++) {
				if (!range_itr->Range (low, high)) continue;

				if (low == 0.0 && high == 0.0) continue;

				if (low > high) {
					if (exe->Send_Messages ()) {
						exe->Write (1, String ("%s Range %.0lf..%.0lf is Illogical") % Type () % low % high); 
					}
					return (false);
				}
				if (report_flag) {
					if (count != 0) {
						if (count == 8) {
							exe->Print (1, String ("%34c") % BLANK);
							count = 0;
						} else {
							exe->Print (0, ", ");
						}
					}
					count++;

					if (low != high) {
						exe->Print (0, String ("%.0lf..%.0lf") % low % high);
					} else {
						exe->Print (0, String ("%.0lf") % low);
					}
				}
				for (num = (int) low; num <= high; num++) {
					list_stat = group_ptr->list.insert (num);
					if (!list_stat.second) {
						if (*list_stat.first == num) {
							exe->Warning (String ("Duplicate Equivalence Value %d in Group %d") % num % group);
						} else {
							goto mem_error;
						}
					}
				}
			}
		}
	}
	return (true);

mem_error:
	if (exe->Send_Messages ()) {
		text (Type ()) + " Group";
		exe->Mem_Error (text);
	}
	return (false);
}

//---------------------------------------------------------
//	Get_Group
//---------------------------------------------------------

int Equiv_Data::Get_Group (int id)
{
	Group_Map_Itr group_itr;

	for (group_itr = group_map.begin (); group_itr != group_map.end (); group_itr++) {
		if (time_flag) {
			if (group_itr->second.period.Period ((Dtime) id) >= 0) {
				return (group_itr->first);
			}
		} else {
			Int_Set_Itr itr;
			for (itr = group_itr->second.list.begin (); itr != group_itr->second.list.end (); itr++) {
				if (*itr == id) {
					return (group_itr->first);
				}
			}
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Group_Label
//---------------------------------------------------------

void Equiv_Data::Group_Label (int group, string label)
{
	Group_Map_Itr group_itr = group_map.find (group);
	if (group_itr != group_map.end ()) {
		group_itr->second.label = label;
	}
}

//---------------------------------------------------------
//	Group_Label
//---------------------------------------------------------

string Equiv_Data::Group_Label (int group)
{
	Group_Map_Itr group_itr = group_map.find (group);

	return ((group_itr == group_map.end ()) ? "" : group_itr->second.label);
}

//---------------------------------------------------------
//	Group_List
//---------------------------------------------------------

Int_Set * Equiv_Data::Group_List (int group)
{
	if (time_flag) return (0);

	Group_Map_Itr group_itr = group_map.find (group);

	return ((group_itr == group_map.end ()) ? 0 : &(group_itr->second.list));
}

//---------------------------------------------------------
//	Group_Period
//---------------------------------------------------------

Time_Periods * Equiv_Data::Group_Period (int group)
{
	if (!time_flag) return (0);

	Group_Map_Itr group_itr = group_map.find (group);

	return ((group_itr == group_map.end ()) ? 0 : &(group_itr->second.period));
}

//---------------------------------------------------------
//	Num_Groups
//---------------------------------------------------------

int Equiv_Data::Num_Groups (void)
{
	int size = (int) group_map.size ();

	if (size == 0) {
		int group;
		String text;
		Equiv_Group equiv_group;

		while (Db_File::Read ()) {
			text = Record_String ();
			if (text.empty ()) continue;
			group = text.Integer ();
			if (group == 0) continue;

			group_map.insert (Group_Map_Data (group, equiv_group));
		}
		Rewind ();
		return ((int) group_map.size ());
	} else {
		return (size);
	}
}

//---------------------------------------------------------
//	Max_Group
//---------------------------------------------------------

int Equiv_Data::Max_Group (void)
{
	Num_Groups ();

	Group_Map_Itr group_itr = group_map.end ();
	group_itr--;

	return (group_itr->first);
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Zone_Equiv::Read (bool report_flag)
{
	if (!Equiv_Data::Read (report_flag)) return (false);

	int z, num_zone, group;
	Int_Set *list;
	Int_Set_Itr list_itr;
	Int_Map_Stat map_stat;

	Group_Map_Itr group_itr;

	num_zone = 0;

	for (group_itr = group_map.begin (); group_itr != group_map.end (); group_itr++) {

		group = group_itr->first;
		list = &(group_itr->second.list);
		if (list->empty ()) continue;

		for (list_itr = list->begin (); list_itr != list->end (); list_itr++) {
			map_stat = zone_map.insert (Int_Map_Data (*list_itr, group));

			if (!map_stat.second) {
				if (exe->Send_Messages ()) {
					exe->Warning (String ("Zone %d was Reassigned from Zone Group %d to %d") % *list_itr % map_stat.first->second % group);
				}
				num_warning++;
			} else {
				if (*list_itr > num_zone) num_zone = *list_itr;
			}
		}
	}

	//---- scan for missing zones ----

	num_missing = 0;

	for (z=1; z <= num_zone; z++) {
		if (zone_map.find (z) == zone_map.end ()) num_missing++;
	}
	return (true);
}

//---------------------------------------------------------
//	Zone_Group
//---------------------------------------------------------

int Zone_Equiv::Zone_Group (int zone)
{
	Int_Map_Itr zone_itr = zone_map.find (zone);

	return ((zone_itr == zone_map.end ()) ? 0 : zone_itr->second);
}

//---------------------------------------------------------
//	Period
//---------------------------------------------------------

int Time_Equiv::Period (int time)
{
	Group_Map_Itr itr;

	for (itr = group_map.begin (); itr != group_map.end (); itr++) {
		if (itr->second.period.In_Range (time)) {
			return (itr->first);
		}
	}
	return (0);
}

