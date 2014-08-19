//*********************************************************
//	Group_Report.cpp - Summarize Group Changes
//*********************************************************

#include "NewLandUse.hpp"

//---------------------------------------------------------
//	Group_Report
//---------------------------------------------------------

void NewLandUse::Group_Report (int number)
{
	int field, num_fields, cover, zones, tot_zones;
	Zone_Field_Itr field_itr;
	Int_Set groups;
	Int_Set_Itr group_itr;
	Doubles data, result, tot_data, tot_result;
	double dvalue;

	Show_Message ("Creating a Group Summary Report -- Record");
	Set_Progress ();

	report_code = number;
	number = group_index [number];

	Header_Number (GROUP_REPORT);
	New_Page ();
	
	num_fields = (int) data_names.size ();

	for (field_itr = zone_field_array.begin (); field_itr != zone_field_array.end (); field_itr++) {
		groups.insert (field_itr->group [number]);
	}
	tot_data.assign (num_fields, 0);
	tot_result.assign (num_fields, 0);
	tot_zones = 0;

	for (group_itr = groups.begin (); group_itr != groups.end (); group_itr++) {
		data.assign (num_fields, 0);
		result.assign (num_fields, 0);
		zones = 0;

		Show_Progress ();

		for (field_itr = zone_field_array.begin (); field_itr != zone_field_array.end (); field_itr++) {
			if (field_itr->group [number] != *group_itr) continue;

			zones++;
			for (field=0; field < num_fields; field++) {
				data [field] += field_itr->data [0] [field];

				dvalue = field_itr->data [0] [field] - field_itr->change [0] [field];

				for (cover=1; cover < cover_types; cover++) {
					dvalue += field_itr->change [cover] [field];
				}
				if (dvalue < 0) dvalue = 0;

				result [field] += dvalue;
			}
		}
		Break_Check (4);
		Print (1, String ("%5d  %5d Before") % *group_itr % zones);
		tot_zones += zones;

		for (field=0; field < num_fields; field++) {
			Print (0, String (" %8.0lf") % data [field]);
			tot_data [field] += data [field];
		}
		Print (1, "             After ");

		for (field=0; field < num_fields; field++) {
			Print (0, String (" %8.0lf") % result [field]);
			tot_result [field] += result [field];
		}
		Print (1, "             Change");

		for (field=0; field < num_fields; field++) {
			Print (0, String (" %8.0lf") % (result [field] - data [field]));
		}
		Print (1);
	}
	End_Progress ();

	Break_Check (4);
	Print (1, String ("Total  %5d Before") % tot_zones);

	for (field=0; field < num_fields; field++) {
		Print (0, String (" %8.0lf") % tot_data [field]);
	}
	Print (1, "             After ");

	for (field=0; field < num_fields; field++) {
		Print (0, String (" %8.0lf") % tot_result [field]);
	}
	Print (1, "             Change");

	for (field=0; field < num_fields; field++) {
		Print (0, String (" %8.0lf") % (tot_result [field] - tot_data [field]));
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Group_Header
//---------------------------------------------------------

void NewLandUse::Group_Header (void)
{
	Str_Itr itr;

	Print (1, "Group Summary Report for Group ") << report_code;
	Print (2, "Group  Zones Type  ");
	for (itr = data_names.begin (); itr != data_names.end (); itr++) {
		Print (0, String (" %8.8s") % *itr);
	}
	Print (1);
}

/*********************************************|***********************************************

	Group Summary Report

	Group  Zones Type     Field1   Field2   Field3   Field4   Field5   Field6   Field7

	ddddd  ddddd Before dddddddd dddddddd dddddddd dddddddd dddddddd dddddddd dddddddd
	             After  dddddddd dddddddd dddddddd dddddddd dddddddd dddddddd dddddddd
	             Change dddddddd dddddddd dddddddd dddddddd dddddddd dddddddd dddddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
