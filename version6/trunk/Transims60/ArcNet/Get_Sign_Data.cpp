//*********************************************************
//	Get_Sign_Data.cpp - additional sign processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Sign_Data
//---------------------------------------------------------

bool ArcNet::Get_Sign_Data (Sign_File &file, Sign_Data &sign_rec)
{
	if (Data_Service::Get_Sign_Data (file, sign_rec)) {
		if (arcview_sign.Is_Open ()) {
			arcview_sign.Copy_Fields (file);
			
			Dir_Data *dir_ptr = &dir_array [sign_rec.Dir_Index ()];

			Link_Data *link_ptr = &link_array [dir_ptr->Link ()];

			int offset = link_ptr->Length () - Round (sign_setback);

			if (dir_ptr->Dir () == 0) {
				offset -= link_ptr->Boffset ();
				offset = MAX (offset, link_ptr->Aoffset ());
			} else {
				offset -= link_ptr->Aoffset ();
				offset = MAX (offset, link_ptr->Boffset ());
			}

			Point_Offset (dir_ptr->Link (), dir_ptr->Dir (), offset, sign_side, arcview_sign);

			if (!arcview_sign.Write_Record ()) {
				Error (String ("Writing %s") % arcview_sign.File_Type ());
			}
		}
	}
	return (false);
}
