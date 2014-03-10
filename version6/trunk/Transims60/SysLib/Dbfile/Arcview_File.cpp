//********************************************************* 
//	Arcview_File.cpp - Arcview Shape File interface
//*********************************************************

#include "Arcview_File.hpp"

//-----------------------------------------------------------
//	Arcview_Base constructors - access only
//-----------------------------------------------------------

Arcview_Base::Arcview_Base () : Points (), Projection ()
{
	file = 0;
}

//-----------------------------------------------------------
//	Arc_Setup
//-----------------------------------------------------------

void Arcview_Base::Arc_Setup (Db_Header *_file)
{
	shape_type = VECTOR;
	z_flag = m_flag = false;

	file = _file;
	if (file == 0) {
		exe->Error ("Arc_Setup Pointer is Zero");
	}

	Part_Size (1);
	Num_Points (1);

	shape_file.Initialize (file->File_Access (), BINARY);
	shape_file.File_Type (file->File_Type ());

	memset (&shape_header, '\0', sizeof (shape_header));
	shape_header.file_code = 9994;
	shape_header.file_size = sizeof (shape_header) / sizeof (short);
	shape_header.version = 1000;
	shape_header.shape_type = ARC_TYPE;
	shape_header.box.xmin = shape_header.box.ymin = -NAN;
	shape_header.box.xmax = shape_header.box.ymax = NAN;
	
	if (Z_Flag ()) {
		shape_header.zbox.min = shape_header.mbox.min = -NAN;
		shape_header.zbox.max = shape_header.mbox.max = NAN;
	}
	index_file.Initialize (file->File_Access (), BINARY);
	index_file.File_Type (file->File_Type ());

	memset (&index_header, '\0', sizeof (index_header));
	index_header.file_code = 9994;
	index_header.file_size = sizeof (index_header) / sizeof (short);
	index_header.version = 1000;

	parts [0] = 0;
}

//-----------------------------------------------------------
//	Arc_Open
//-----------------------------------------------------------

bool Arcview_Base::Arc_Open (string filename)
{
	bool messages, status;

	if (file == 0) 	exe->Error ("Arcview_Base is not Initialized");

	if (filename.empty ()) {
		filename = file->Filename ();
	}
	if (filename.empty ()) return (false);

	//---- open the dbase file ----

	if (filename.size () > 4 && filename [filename.size () - 4] == '.') {
		filename.erase (filename.size () - 3, 3);
	} else {
		filename += ".";
	}
	filename += "dbf";
	
	messages = exe->Send_Messages ();
	exe->Send_Messages (false);

	status = file->Db_Header::Open (filename);

	exe->Send_Messages (messages);
	if (!status) return (status);

	shape_file.File_Type (file->File_Type ());
	shape_file.File_Access (file->File_Access ());

	index_file.File_Type (file->File_Type ());
	index_file.File_Access (file->File_Access ());

	//---- open the shape file ----
	
	filename.erase (filename.size () - 3, 3);
	filename += "shp";

	if (!shape_file.Open (filename)) {
		return (file->Status (NOT_OPEN));
	}
	if (file->First_Open () && messages) {
		String text, result;
		text = file->File_Type ();

		text.Split_Last (result);

		exe->Print (1, text) << " Format = " << Data_Format (file->Dbase_Format (), file->Model_Format ());
	}
	exe->Send_Messages (false);

	if (file->File_Access () == CREATE) {
		Write_Projection ();

		switch (Shape_Type ()) {
			case DOT:
				shape_header.shape_type = POINT_TYPE;
				break;
			case MDOTS:
				shape_header.shape_type = MULTIPOINT_TYPE;
				break;
			case LINE:
			case VECTOR:
				shape_header.shape_type = ARC_TYPE;
				break;
			case POLYGON:
				shape_header.shape_type = POLYGON_TYPE;
				break;
			default:
				break;
		}
		if (Z_Flag () || M_Flag ()) {
			if (!Z_Flag ()) {
				Z_Flag (true);
			}
			shape_header.shape_type += (M_Flag ()) ? 20 : 10;
		}
		shape_header.file_size = sizeof (shape_header) / sizeof (short);
		shape_header.box.xmin = shape_header.box.ymin = -NAN;
		shape_header.box.xmax = shape_header.box.ymax = NAN;

		if (Z_Flag ()) {
			shape_header.zbox.min = shape_header.mbox.min = -NAN;
			shape_header.zbox.max = shape_header.mbox.max = NAN;
		}
		Reorder_Bits (&shape_header, 7);

		if (!shape_file.Write (&shape_header, sizeof (shape_header), 0L)) {
			return (file->Status (HEADER_WRITE));
		}
	} else {
		if (!shape_file.Read (&shape_header, sizeof (shape_header), 0L)) {
			return (file->Status (HEADER_READ));
		}
	}
	Reorder_Bits (&shape_header, 7);

	switch ((shape_header.shape_type % 10)) {
		case POINT_TYPE:		//---- point ----
			Shape_Type (DOT);
			break;
		case MULTIPOINT_TYPE:	//---- multipoint ----
			Shape_Type (MDOTS);
			break;
		case ARC_TYPE:			//---- arc ----
			Shape_Type (VECTOR);
			break;
		case POLYGON_TYPE:		//---- polygon ----
			Shape_Type (POLYGON);
			break;
		default:
			return (file->Status (HEADER_READ));
	}
	Z_Flag ((shape_header.shape_type / 10) > 0);
	M_Flag ((shape_header.shape_type / 10) == 2);

	extent.xmin = shape_header.box.xmin;
	extent.ymin = shape_header.box.ymin;
	extent.xmax = shape_header.box.xmax;
	extent.ymax = shape_header.box.ymax;

	Convert (&(extent.xmin), &(extent.ymin));
	Convert (&(extent.xmax), &(extent.ymax));

	if (M_Flag ()) {
		_extent.min = shape_header.mbox.min;
		_extent.max = shape_header.mbox.max;
	} else if (Z_Flag ()) {
		_extent.min = shape_header.zbox.min;
		_extent.max = shape_header.zbox.max;
	}
	shape_file.First_Offset (sizeof (shape_header));
	shape_file.Record_Size (sizeof (Arc_Record));
	
	Num_Points (1);
	Part_Size (1);

	//---- open the index file ----
	
	filename.erase (filename.size () - 3, 3);
	filename += "shx";

	if (!index_file.Open (filename)) {
		return (file->Status (NOT_OPEN));
	}

	if (file->File_Access () == CREATE) {
		index_header.file_size = sizeof (index_header) / sizeof (short);

		Reorder_Bits (&index_header, 7);
		
		if (!index_file.Write (&index_header, sizeof (index_header), 0L)) {
			return (file->Status (HEADER_WRITE));
		}

		//---- delete ArcGIS spatial index files ----

		filename.erase (filename.size () - 3, 3);
		filename += "sbn";
		remove (filename.c_str ());

		filename.erase (filename.size () - 3, 3);
		filename += "sbx";
		remove (filename.c_str ());

		//---- remove uDig support files ----
		
		filename.erase (filename.size () - 3, 3);
		filename += "qix";
		remove (filename.c_str ());

		filename.erase (filename.size () - 3, 3);
		filename += "fix";
		remove (filename.c_str ());

	} else {
		if (!index_file.Read (&index_header, sizeof (index_header), 0L)) {
			return (file->Status (HEADER_READ));
		}
	}
	Reorder_Bits (&index_header, 7);

	index_file.First_Offset (sizeof (index_header));
	index_file.Record_Size (sizeof (Arc_Record));

	exe->Send_Messages (messages);

	return (status);
}

//-----------------------------------------------------------
//	Close
//-----------------------------------------------------------

bool Arcview_Base::Arc_Close (void)
{
	//---- close the shape file ----

	if (shape_file.Is_Open ()) {
		if (shape_file.File_Access () != READ) {
			switch (Shape_Type ()) {
				case DOT:
					shape_header.shape_type = POINT_TYPE;
					break;
				case MDOTS:
					shape_header.shape_type = MULTIPOINT_TYPE;
					break;
				case LINE:
				case VECTOR:
					shape_header.shape_type = ARC_TYPE;
					break;
				case POLYGON:
					shape_header.shape_type = POLYGON_TYPE;
					break;
				default:
					break;
			}
			if (shape_header.box.xmin > shape_header.box.xmax) {
				shape_header.box.xmin = shape_header.box.xmax = 0;
			}
			if (shape_header.box.ymin > shape_header.box.ymax) {
				shape_header.box.ymin = shape_header.box.ymax = 0;
			}
			if (Z_Flag ()) {
				shape_header.shape_type += (M_Flag ()) ? 20 : 10;

				if (M_Flag () || shape_header.zbox.min > shape_header.zbox.max) {
					shape_header.zbox.min = shape_header.zbox.max = 0;
				}
				if (shape_header.mbox.min > shape_header.mbox.max) {
					shape_header.mbox.min = shape_header.mbox.max = 0;
				}
			}
			shape_header.file_size = shape_file.File_Size () / sizeof (short);

			Reorder_Bits (&shape_header, 7);

			if (!shape_file.Write (&shape_header, sizeof (shape_header), 0L)) {
				return (file->Status (HEADER_WRITE));
			}
			Reorder_Bits (&shape_header, 7);
		}
		shape_file.Close ();
	}

	//---- close the index file ----

	if (index_file.Is_Open ()) {
		if (index_file.File_Access () != READ) {
			index_header.shape_type = shape_header.shape_type;
			index_header.box.xmin = shape_header.box.xmin;
			index_header.box.ymin = shape_header.box.ymin;
			index_header.box.xmax = shape_header.box.xmax;
			index_header.box.ymax = shape_header.box.ymax;

			if (Z_Flag ()) {
				index_header.zbox.min = shape_header.zbox.min;
				index_header.zbox.max = shape_header.zbox.max;
				index_header.mbox.min = shape_header.mbox.min;
				index_header.mbox.max = shape_header.mbox.max;
			}
			index_header.file_size = index_file.File_Size () / sizeof (short);
			Reorder_Bits (&index_header, 7);

			if (!index_file.Write (&index_header, sizeof (index_header), 0L)) {
				return (file->Status (HEADER_WRITE));
			}
			Reorder_Bits (&index_header, 7);
		}
		index_file.Close ();
	}

	//---- close the dbase file -----

	return (file->Db_Header::Close ());
}

//----------------------------------------------------------
//	Z_Flag
//----------------------------------------------------------

void Arcview_Base::Z_Flag (bool flag)
{
	if (z_flag != flag) {
		z_flag = flag;
		if (file->File_Access () == CREATE) M_Flag (z_flag);

		if (z_flag) {
			zm.resize (Num_Points ());
		}
	}
}

//----------------------------------------------------------
//	Arc_Read
//----------------------------------------------------------

bool Arcview_Base::Arc_Read (int number)
{
	int i, npts, type;
	off_t offset;
	
	double *z = 0;
	XY_Point *pt;
	XYZ_Point *p;

	Arc_Record record;
	Arc_Shape  shape;
	Arc_Multi_Point  multi;

	if (!file->Check_File ()) return (false);
	if (!shape_file.Check_File ()) return (false);
	if (!index_file.Check_File ()) return (false);

	if (number <= 0) {
		number = file->Record_Number () + 1;
	}

	//---- skip deleted records ----

	for (;; number++) {
		if (!file->Db_Header::Read_Record (number)) return (false);
		if (*(file->Record_Pointer ()) == ' ' || *(file->Record_Pointer ()) == '\0') break;
	}

	//---- get the shape index ----

	offset = index_file.First_Offset () + (number - 1) * sizeof (record);

	if (!index_file.Read (&record, sizeof (record), offset)) {
		return (false);
	}
	Reorder_Bits (&record, 2);

	offset = record.rec_num * sizeof (short);

	if (!shape_file.Read (&record, sizeof (record), offset)) {
		return (false); 
	}
	Reorder_Bits (&record, 2);

	//---- get the shape type ----

	if (!shape_file.Read (&type, sizeof (int))) {
		return (file->Status (RECORD_SYNTAX));
	}

	switch ((type % 10)) {
		case POINT_TYPE:		//---- point ----

			Num_Points (1);

			Shape_Type (DOT);

			p = Get_Points ();
			pt = Get_XY ();

			if (!shape_file.Read (pt, sizeof (XY_Point))) {
				return (file->Status (RECORD_SYNTAX));
			}
			Convert (pt);

			range.xmin = range.xmax = p->x = pt->x;
			range.ymin = range.ymax = p->y = pt->y;

			if (Z_Flag ()) {
				z = Get_ZM ();

				if (!shape_file.Read (z, sizeof (double))) {
					return (file->Status (RECORD_SYNTAX));
				}
				if (*z <= NAN) *z = 0.0;
				_range.min = _range.max = p->z = *z;
			} else {
				p->z = 0.0;
			}
			break;
				
		case ARC_TYPE:			//---- arc ----
		case POLYGON_TYPE:		//---- polygon ----

			if (!shape_file.Read (&shape, sizeof (shape))) {
				return (file->Status (RECORD_SYNTAX));
			}
			Num_Points (shape.num_pts);
			Part_Size (shape.num_parts);
			if (shape.num_pts == 0) break;

			range.xmin = shape_header.box.xmin;
			range.ymin = shape_header.box.ymin;
			range.xmax = shape_header.box.xmax;
			range.ymax = shape_header.box.ymax;

			Convert (&(range.xmin), &(range.ymin));
			Convert (&(range.xmax), &(range.ymax));

			//---- get the part offsets ----

			if (!shape_file.Read (Get_Parts (), shape.num_parts * sizeof (int))) {
				return (file->Status (RECORD_SYNTAX));
			}

			//---- get the point data ----

			p = Get_Points ();
			pt = Get_XY ();

			if (!shape_file.Read (pt, shape.num_pts * sizeof (XY_Point))) {
				return (file->Status (RECORD_SYNTAX));
			}
			npts = shape.num_pts;

			Shape_Type ((type == ARC_TYPE) ? ((npts == 2) ? LINE : VECTOR) : POLYGON);

			for (i=0; i < npts; i++, pt++, p++) {
				Convert (pt);

				p->x = pt->x;
				p->y = pt->y;
				p->z = 0.0;
			}
			if (Z_Flag ()) {
				if (!shape_file.Read (&_range, sizeof (_range))) {
					return (file->Status (RECORD_SYNTAX));
				}
				z = Get_ZM ();

				if (!shape_file.Read (z, shape.num_pts * sizeof (double))) {
					return (file->Status (RECORD_SYNTAX));
				}
				if (_range.min <= NAN) _range.min = 0;
				if (_range.max <= NAN) _range.max = 0;

				p = Get_Points ();

				for (i=0; i < npts; i++, z++, p++) {
					if (*z <= NAN) *z = 0;
					p->z = *z;
				}
			}
			break;
				
		case MULTIPOINT_TYPE:		//---- multipoint ----

			if (!shape_file.Read (&multi, sizeof (multi))) {
				return (file->Status (RECORD_SYNTAX));
			}
			Num_Points (multi.num_pts);
			if (multi.num_pts == 0) break;

			range.xmin = multi.box.xmin;
			range.ymin = multi.box.ymin;
			range.xmax = multi.box.xmax;
			range.ymax = multi.box.ymax;

			Convert (&(range.xmin), &(range.ymin));
			Convert (&(range.xmax), &(range.ymax));

			p = Get_Points ();
			pt = Get_XY ();

			if (!shape_file.Read (pt, multi.num_pts * sizeof (XY_Point))) {
				return (file->Status (RECORD_SYNTAX));
			}
			npts = multi.num_pts;

			Shape_Type (MDOTS);
			
			for (i=0; i < npts; i++, pt++, p++) {
				Convert (pt);

				p->x = pt->x;
				p->y = pt->y;
				p->z = 0.0;
			}
			if (Z_Flag ()) {
				if (!shape_file.Read (&_range, sizeof (_range))) {
					return (file->Status (RECORD_SYNTAX));
				}
				z = Get_ZM ();

				if (!shape_file.Read (z, multi.num_pts * sizeof (double))) {
					return (file->Status (RECORD_SYNTAX));
				}
				if (_range.min <= NAN) _range.min = 0;
				if (_range.max <= NAN) _range.max = 0;

				p = Get_Points ();

				for (i=0; i < npts; i++, z++, p++) {
					if (*z <= NAN) *z = 0;
					p->z = *z;
				}
			}
			break;

		default:
			Num_Points (0);
			Part_Size (0);
			return (false);
			break;
	}
	return (true);
}

//----------------------------------------------------------
//	Arc_Write
//----------------------------------------------------------

bool Arcview_Base::Arc_Write (int number)
{
	bool insert_flag, first;
	int i, type, size;
	off_t offset;

	double *z = 0;
	XY_Point  *pt;
	XYZ_Point *p;

	Arc_Record record;
	Arc_Shape  shape;
	Arc_Multi_Point  multi;

	if (!file->Check_File ()) return (false);
	if (!shape_file.Check_File ()) return (false);
	if (!index_file.Check_File ()) return (false);
	if (Num_Points () == 0) return (false);

	//---- check the space allocation ----

	Num_Points (Num_Points ());

	//---- determine the record type and size ----
	
	switch (Shape_Type ()) {
		
		case DOT:

			record.rec_size = (sizeof (int) + sizeof (XY_Point)) / sizeof (short);

			if (Z_Flag ()) {
				record.rec_size += ((M_Flag ()) ? 1 : 2) * sizeof (double) / sizeof (short);
			}
			break;

		case LINE:
		case VECTOR:
		case POLYGON:

			record.rec_size = (sizeof (int) + sizeof (shape) + 
				Part_Size () * sizeof (int) + 
				Num_Points () * sizeof (XY_Point)) / sizeof (short);

			if (Z_Flag ()) {
				record.rec_size += ((M_Flag ()) ? 1 : 2) * 
					(sizeof (_range) + Num_Points () * sizeof (double)) / sizeof (short);
			}
			break;
				
		case MDOTS:

			record.rec_size = (sizeof (int) + sizeof (multi) + 
				Num_Points () * sizeof (XY_Point)) / sizeof (short);

			if (Z_Flag ()) {
				record.rec_size += ((M_Flag ()) ? 1 : 2) * 
					(sizeof (_range) + Num_Points () * sizeof (double)) / sizeof (short);
			}
			break;
			
		default:		//---- unsupported graphics type ----
			return (true);
	}

	//---- set the index record and shape header ----

	if (number > 0 && number != file->Record_Number () + 1) {
		offset = index_file.First_Offset () + (number - 1) * sizeof (record);

		if (!index_file.Read (&record, sizeof (record), offset)) {
			return (false);
		}
		Reorder_Bits (&record, 2);

		offset = record.rec_num * sizeof (short);
		record.rec_num = number;

		insert_flag = false;

		Reorder_Bits (&record, 1);

		if (!shape_file.Write (&record, sizeof (record), offset)) {
			return (false); 
		}
	} else {
		off_t index_offset = index_file.File_Size ();
		offset = shape_file.File_Size ();

		record.rec_num = offset / sizeof (short);
		
		Reorder_Bits (&record, 2);
	
		if (!index_file.Write (&record, sizeof (record), index_offset)) {
			return (false);
		}
		record.rec_num = (index_offset - index_file.First_Offset ()) / sizeof (record) + 1;
		insert_flag = true;

		Reorder_Bits (&record, 1);

		if (!shape_file.Write (&record, sizeof (record), offset)) {
			return (false); 
		}
	}

	//---- store the shape record ----
	
	switch (Shape_Type ()) {

		case DOT:	//---- point ----

			type = POINT_TYPE;
			if (Z_Flag ()) {
				type += (M_Flag ()) ? 20 : 10;
			}
			shape_header.shape_type = type;
			
			p = Get_Points ();
			pt = Get_XY ();

			pt->x = p->x;
			pt->y = p->y;

			Convert (pt);

			if (pt->x < shape_header.box.xmin) {
				shape_header.box.xmin = pt->x;
			}
			if (pt->y < shape_header.box.ymin) {
				shape_header.box.ymin = pt->y;
			}
			if (pt->x > shape_header.box.xmax) {
				shape_header.box.xmax = pt->x;
			}
			if (pt->y > shape_header.box.ymax) {
				shape_header.box.ymax = pt->y;
			}

			if (Z_Flag ()) {
				z = Get_ZM ();

				*z = p->z;

				if (*z > shape_header.mbox.max) {
					shape_header.mbox.max = *z;
				}
				if (*z < shape_header.mbox.min) {
					shape_header.mbox.min = *z;
				}
				if (!M_Flag ()) {
					if (*z > shape_header.zbox.max) {
						shape_header.zbox.max = *z;
					}
					if (*z < shape_header.zbox.min) {
						shape_header.zbox.min = *z;
					}
				}
			}
			if (insert_flag) {
				if (!shape_file.Write (&type, sizeof (int))) {
					return (false);
				}
				if (!shape_file.Write (pt, sizeof (XY_Point))) {
					return (false);
				}
				if (Z_Flag ()) {
					if (!shape_file.Write (z, sizeof (double))) {
						return (false);
					}
					if (!M_Flag ()) {
						if (!shape_file.Write (z, sizeof (double))) {
							return (false);
						}
					}
				}
			} else {
				offset += sizeof (record);
				if (!shape_file.Write (&type, sizeof (int), offset)) {
					return (false);
				}
				offset += sizeof (int);
				if (!shape_file.Write (pt, sizeof (XY_Point), offset)) {
					return (false);
				}
				if (Z_Flag ()) {
					offset += sizeof (double);
					if (!shape_file.Write (z, sizeof (double), offset)) {
						return (false);
					}
					if (!M_Flag ()) {
						offset += sizeof (double);
						if (!shape_file.Write (z, sizeof (double), offset)) {
							return (false);
						}
					}
				}
			}
			break;

		case LINE:
		case VECTOR:
		case POLYGON:

			if (Shape_Type ()== POLYGON) {
				type = POLYGON_TYPE;
			} else {
				type = ARC_TYPE;
			}
			if (Z_Flag ()) {
				type += (M_Flag ()) ? 20 : 10;
			}
			shape_header.shape_type = type;
				
			shape.box.xmin = 0;
			shape.box.ymin = 0;
			shape.box.xmax = 0;
			shape.box.ymax = 0;
			shape.num_parts = Part_Size ();
			shape.num_pts = Num_Points ();
			first = true;

			p = Get_Points ();
			pt = Get_XY ();

			for (i=0; i < shape.num_pts; i++, pt++, p++) {
				pt->x = p->x;
				pt->y = p->y;

				Convert (pt);

				if (first) {
					shape.box.xmin = pt->x;
					shape.box.ymin = pt->y;
					shape.box.xmax = pt->x;
					shape.box.ymax = pt->y;
					first = false;
				} else {
					if (pt->x < shape.box.xmin) {
						shape.box.xmin = pt->x;
					}
					if (pt->y < shape.box.ymin) {
						shape.box.ymin = pt->y;
					}
					if (pt->x > shape.box.xmax) {
						shape.box.xmax = pt->x;
					}
					if (pt->y > shape.box.ymax) {
						shape.box.ymax = pt->y;
					}
				}
			}
			if (shape.box.xmin < shape_header.box.xmin) {
				shape_header.box.xmin = shape.box.xmin;
			}
			if (shape.box.ymin < shape_header.box.ymin) {
				shape_header.box.ymin = shape.box.ymin;
			}
			if (shape.box.xmax > shape_header.box.xmax) {
				shape_header.box.xmax = shape.box.xmax;
			}
			if (shape.box.ymax > shape_header.box.ymax) {
				shape_header.box.ymax = shape.box.ymax;
			}

			if (Z_Flag ()) {
				_range.min = 0;
				_range.max = 0;
				first = true;

				p = Get_Points ();
				z = Get_ZM ();

				for (i=0; i < Num_Points (); i++, z++, p++) {
					*z = p->z;

					if (first) {
						_range.min = _range.max = *z;
						first = false;
					} else {
						if (*z < _range.min) {
							_range.min = *z;
						}
						if (*z > _range.max) {
							_range.max = *z;
						}
					}
				}
				if (_range.min < shape_header.mbox.min) {
					shape_header.mbox.min = _range.min;
				}
				if (_range.max > shape_header.mbox.max) {
					shape_header.mbox.max = _range.max;
				}
				if (!M_Flag ()) {
					if (_range.min < shape_header.zbox.min) {
						shape_header.zbox.min = _range.min;
					}
					if (_range.max > shape_header.zbox.max) {
						shape_header.zbox.max = _range.max;
					}
				}
			}

			if (insert_flag) {
				if (!shape_file.Write (&type, sizeof (int))) {
					return (false);
				}
				if (!shape_file.Write (&shape, sizeof (shape))) {
					return (false);
				}
				if (!shape_file.Write (Get_Parts (), shape.num_parts * sizeof (int))) {
					return (false);
				}
				if (!shape_file.Write (Get_XY (), shape.num_pts * sizeof (XY_Point))) {
					return (false);
				}
				if (Z_Flag ()) {
					if (!shape_file.Write (&_range, sizeof (_range))) {
						return (false);
					}
					if (!shape_file.Write (Get_ZM (), shape.num_pts * sizeof (double))) {
						return (false);
					}
					if (!M_Flag ()) {
						if (!shape_file.Write (&_range, sizeof (_range))) {
							return (false);
						}
						if (!shape_file.Write (Get_ZM (), shape.num_pts * sizeof (double))) {
							return (false);
						}
					}
				}
			} else {
				offset += sizeof (record);
				if (!shape_file.Write (&type, sizeof (int), offset)) {
					return (false);
				}
				offset += sizeof (int);
				if (!shape_file.Write (&shape, sizeof (shape), offset)) {
					return (false);
				}
				offset += sizeof (shape);
				size = shape.num_parts * sizeof (int);
				if (!shape_file.Write (Get_Parts (), size, offset)) {
					return (false);
				}
				offset += size;
				size = shape.num_pts * sizeof (XY_Point);
				if (!shape_file.Write (Get_XY (), size, offset)) {
					return (false);
				}
				if (Z_Flag ()) {
					offset += size;
					size = sizeof (_range);
					if (!shape_file.Write (&_range, size, offset)) {
						return (false);
					}
					offset += size;
					size = shape.num_pts * sizeof (double);
					if (!shape_file.Write (Get_ZM (), size, offset)) {
						return (false);
					}
					if (!M_Flag ()) {
						offset += size;
						size = sizeof (_range);
						if (!shape_file.Write (&_range, size, offset)) {
							return (false);
						}
						offset += size;
						size = shape.num_pts * sizeof (double);
						if (!shape_file.Write (Get_ZM (), size, offset)) {
							return (false);
						}
					}
				}
			}
			break;
				
		case MDOTS:

			type = MULTIPOINT_TYPE;
			if (Z_Flag ()) {
				type += (M_Flag ()) ? 20 : 10;
			}
			shape_header.shape_type = type;

			multi.box.xmin = 0;
			multi.box.ymin = 0;
			multi.box.xmax = 0;
			multi.box.ymax = 0;
			multi.num_pts = Num_Points ();
			first = true;
			
			p = Get_Points ();
			pt = Get_XY ();
		
			for (i=0; i < multi.num_pts; i++, pt++, p++) {
				pt->x = p->x;
				pt->y = p->y;

				Convert (pt);

				if (first) {
					multi.box.xmin = pt->x;
					multi.box.ymin = pt->y;
					multi.box.xmax = pt->x;
					multi.box.ymax = pt->y;
					first = false;
				} else {
					if (pt->x < multi.box.xmin) {
						multi.box.xmin = pt->x;
					}
					if (pt->y < multi.box.ymin) {
						multi.box.ymin = pt->y;
					}
					if (pt->x > multi.box.xmax) {
						multi.box.xmax = pt->x;
					}
					if (pt->y > multi.box.ymax) {
						multi.box.ymax = pt->y;
					}
				}
			}
			if (multi.box.xmin < shape_header.box.xmin) {
				shape_header.box.xmin = multi.box.xmin;
			}
			if (multi.box.ymin < shape_header.box.ymin) {
				shape_header.box.ymin = multi.box.ymin;
			}
			if (multi.box.xmax > shape_header.box.xmax) {
				shape_header.box.xmax = multi.box.xmax;
			}
			if (multi.box.ymax > shape_header.box.ymax) {
				shape_header.box.ymax = multi.box.ymax;
			}

			if (Z_Flag ()) {
				_range.min = 0;
				_range.max = 0;
				first = true;

				p = Get_Points ();
				z = Get_ZM ();

				for (i=0; i < Num_Points (); i++, z++, p++) {
					*z = p->z;

					if (first) {
						_range.min = _range.max = *z;
						first = false;
					} else {
						if (*z < _range.min) {
							_range.min = *z;
						}
						if (*z > _range.max) {
							_range.max = *z;
						}
					}
				}
				if (_range.min < shape_header.mbox.min) {
					shape_header.mbox.min = _range.min;
				}
				if (_range.max > shape_header.mbox.max) {
					shape_header.mbox.max = _range.max;
				}
				if (!M_Flag ()) {
					if (_range.min < shape_header.zbox.min) {
						shape_header.zbox.min = _range.min;
					}
					if (_range.max > shape_header.zbox.max) {
						shape_header.zbox.max = _range.max;
					}
				}
			}

			if (insert_flag) {
				if (!shape_file.Write (&type, sizeof (int))) {
					return (false);
				}
				if (!shape_file.Write (&multi, sizeof (multi))) {
					return (false);
				}
				if (!shape_file.Write (Get_XY (), multi.num_pts * sizeof (XY_Point))) {
					return (false);
				}
				if (Z_Flag ()) {
					if (!shape_file.Write (&_range, sizeof (_range))) {
						return (false);
					}
					if (!shape_file.Write (Get_ZM (), multi.num_pts * sizeof (double))) {
						return (false);
					}
					if (!M_Flag ()) {
						if (!shape_file.Write (&_range, sizeof (_range))) {
							return (false);
						}
						if (!shape_file.Write (Get_ZM (), multi.num_pts * sizeof (double))) {
							return (false);
						}
					}
				}
			} else {
				offset += sizeof (record);
				if (!shape_file.Write (&type, sizeof (int), offset)) {
					return (false);
				}
				offset += sizeof (int);
				if (!shape_file.Write (&multi, sizeof (multi), offset)) {
					return (false);
				}
				offset += sizeof (multi);
				size = multi.num_pts * sizeof (XY_Point);
				if (!shape_file.Write (Get_XY (), size, offset)) {
					return (false);
				}
				if (Z_Flag ()) {
					offset += size;
					size = sizeof (_range);
					if (!shape_file.Write (&_range, size, offset)) {
						return (false);
					}
					offset += size;
					size = multi.num_pts * sizeof (double);
					if (!shape_file.Write (Get_ZM (), size, offset)) {
						return (false);
					}
					if (!M_Flag ()) {
						offset += size;
						size = sizeof (_range);
						if (!shape_file.Write (&_range, size, offset)) {
							return (false);
						}
						offset += size;
						size = multi.num_pts * sizeof (double);
						if (!shape_file.Write (Get_ZM (), size, offset)) {
							return (false);
						}
					}
				}
			}
			break;
			
		default:		//---- unsupported graphics type ----
			return (true);
	}
	return (file->Db_Header::Write_Record (number));
}

//----------------------------------------------------------
//	Reorder_Bits
//----------------------------------------------------------

void Arcview_Base::Reorder_Bits (void *dat, int words)
{
	int i;
	char *data, ch;

	data = (char *) dat;

	for (i=0; i < words; i++, data+=4) {
		ch = *data;
		*data = *(data+3);
		*(data+3) = ch;

		ch = *(data+1);
		*(data+1) = *(data+2);
		*(data+2) = ch;
	}
}

//---------------------------------------------------------
//	Set_Projection
//---------------------------------------------------------

bool Arcview_Base::Set_Projection (Projection_Data input, Projection_Data output)
{
	if (!Projection::Set_Projection (input, output)) return (false);

	if (shape_file.File_Access () == CREATE) {
		Write_Projection ();
	}
	return (true);
}

//---------------------------------------------------------
//	Write_Projection
//---------------------------------------------------------

void Arcview_Base::Write_Projection (void)
{
	if (shape_file.Is_Open ()) {
		string& projection = Get_Projection_String ();

		if (!projection.empty ()) {
			FILE *fp;

			string filename = shape_file.Filename ();

			filename.erase (filename.size () - 3, 3);
			filename += "prj";

			fp = f_open (filename, "wb");

			if (fp) {
				fputs (projection.c_str (), fp);
				fclose (fp);
			}
		}
	}
}

//-----------------------------------------------------------
//	Arcview_File constructors - access only
//-----------------------------------------------------------

Arcview_File::Arcview_File (Access_Type access) : Db_Header (access, DBASE), Arcview_Base ()
{
	Arc_Setup (this);
}

//-----------------------------------------------------------
//	Arcview_File constructors - with filename
//-----------------------------------------------------------

Arcview_File::Arcview_File (string filename, Access_Type access) : Db_Header (access, DBASE), Arcview_Base ()
{
	Arc_Setup (this);
	Arc_Open (filename);
}

