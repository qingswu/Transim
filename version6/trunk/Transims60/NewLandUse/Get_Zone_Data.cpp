//*********************************************************
//	Get_Zone_Data.cpp - Read the Zone File
//*********************************************************

#include "NewLandUse.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool NewLandUse::Get_Zone_Data (Zone_File &file, Zone_Data &data)
{
	int lvalue;
	double dvalue;

	Zone_Field_Data field_data, *field_ptr;
	Doubles fields;
	Int_Itr fld_itr;
	Str_Itr str_itr;

	if (Data_Service::Get_Zone_Data (file, data)) {
		if (year_number == 0) {
			zone_field_array.push_back (field_data);
			field_ptr = &zone_field_array.back ();

			field_ptr->zone = data.Zone ();
			field_ptr->target = file.Get_Integer (target_field);
			field_ptr->area = file.Get_Double (area_field);

			//---- data fields ----

			field_ptr->data.push_back (fields);

			for (fld_itr = data_fields.begin (); fld_itr != data_fields.end (); fld_itr++) {
				dvalue = file.Get_Double (*fld_itr);
				field_ptr->data [year_number].push_back (dvalue);
			}

			//---- group fields ----

			for (fld_itr = group_fields.begin (); fld_itr != group_fields.end (); fld_itr++) {
				lvalue = file.Get_Integer (*fld_itr);
				field_ptr->group.push_back (lvalue);
			}

			//---- type fields ----

			for (fld_itr = type_fields.begin (); fld_itr != type_fields.end (); fld_itr++) {
				lvalue = file.Get_Integer (*fld_itr);
				field_ptr->type.push_back (lvalue);
			}

			//---- coverage fields ----

			for (fld_itr = cover_fields.begin (); fld_itr != cover_fields.end (); fld_itr++) {
				dvalue = file.Get_Double (*fld_itr);
				field_ptr->coverage.push_back (dvalue);
			}

		} else {
			Int_Map_Itr map_itr = zone_map.find (data.Zone ());
			if (map_itr == zone_map.end ()) {
				Warning (String ("Zone %d in %s is Not in the Zone File") % data.Zone () % file.File_Type ());
				return (false);
			}
			field_ptr = &zone_field_array [map_itr->second];

			//---- data fields ----

			field_ptr->data.push_back (fields);

			for (str_itr = data_names.begin (); str_itr != data_names.end (); str_itr++) {
				dvalue = file.Get_Double (*str_itr);
				field_ptr->data [year_number].push_back (dvalue);
			}
		}
		return (true);
	}
	return (false);
}

