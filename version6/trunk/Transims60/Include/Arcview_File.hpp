//********************************************************* 
//	Arcview_File.hpp - Arcview Shape File interface
//*********************************************************

#ifndef ARCVIEW_FILE_HPP
#define ARCVIEW_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Projection.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Arcview_Base Class definition
//---------------------------------------------------------

class SYSLIB_API Arcview_Base : public Points, public Projection
{
public:
	Arcview_Base ();
	~Arcview_Base ()						{ Arc_Close (); }
	
	bool Arc_Open (string filename = "");
	bool Arc_Close (void);
	
	void Arc_First_Open (bool flag)         { shape_file.First_Open (flag); }

	bool Arc_Read (int number = 0);
	bool Arc_Write (int number = 0);
	
	void Arc_Setup (Db_Header *file);

	Integers parts;

	int * Get_Parts (void)                  { return (&(parts.front ())); }	
	XYZ_Point * Get_Points (void)           { return (&(front ())); }

	bool  Z_Flag (void)                     { return (z_flag); }
	void  Z_Flag (bool flag);

	bool  M_Flag (void)                     { return (m_flag); }
	void  M_Flag (bool flag)                { m_flag = flag; }

	int   Part_Size (void)                  { return ((int) parts.size ()); }
	void  Part_Size (int num)               { parts.resize (num); }

	int   Num_Points (void)                 { return ((int) size ()); }
	void  Num_Points (int num)              { resize (num); pts.resize (num); if (z_flag) zm.resize (num); }

	Shape_Code Shape_Type (void)            { return (shape_type); }
	void  Shape_Type (Shape_Code type)      { shape_type = type; }

	bool Shape_Filename (string path)       { return (shape_file.Filename (path)); }
	string& Shape_Filename (void)           { return (shape_file.Filename ()); }
	
	bool Set_Projection (Projection_Data input, Projection_Data output);

	void Coordinate_Range (double &xmin, double &ymin, double &xmax, double &ymax)
	                                        { xmin = shape_header.box.xmin; ymin = shape_header.box.ymin;
	                                          xmax = shape_header.box.xmax; ymax = shape_header.box.ymax; }

private:
	void  Write_Projection (void);

	enum Arc_Shapes { POINT_TYPE = 1, ARC_TYPE = 3, POLYGON_TYPE = 5, MULTIPOINT_TYPE = 8 };

	//---- XY_Range typedef ----

	typedef  struct {
		double  xmin;
		double  ymin;
		double  xmax;
		double  ymax;
	} XY_Range;

	//---- Z_Range typedef ----

	typedef  struct {
		double  min;
		double  max;
	} Range;

	//---- Arc_Header typedef ----

#pragma pack (push, 4)
	typedef  struct {
		int       file_code;        //---- 9994 ----
		int       unused [5];
		int       file_size;        //---- words ----
		int       version;          //---- 1000 ----
		int       shape_type;       //---- 1, 3, 5, 8, 11, 13, 15, 18, 21, 23, 25, 28 ----
		XY_Range  box;
		Range     zbox;
		Range     mbox;
	} Arc_Header;
#pragma pack (pop)

	//---- Arc_Record typedef ----

	typedef  struct {
		int  rec_num;           //---- 1... ----
		int  rec_size;          //---- words ----
	} Arc_Record;

	//---- Arc_Multi_Point typedef ----

#pragma pack (push, 4)
	typedef  struct {          //---- shape_type = 8, 18, 28 ----
		XY_Range  box;
		int       num_pts;
	} Arc_Multi_Point;
#pragma pack (pop)

	//---- Arc_Shape typedef ----

	typedef  struct {          //---- shape_types = 3, 5, 13, 15, 23, 25 ----
		XY_Range  box;
		int       num_parts;
		int       num_pts;
	} Arc_Shape;

	Shape_Code shape_type;
	
	bool z_flag, m_flag;
	
	Doubles zm;
	XY_Points pts;

	double * Get_ZM (void)                  { return (&(zm.front ())); }
	XY_Point * Get_XY (void)                { return (&(pts.front ())); }

	XY_Range  extent;
	XY_Range  range;
	
	Range   _extent;
	Range   _range;

	Db_File  shape_file;
	Db_File  index_file;
	Db_Header *file;

	Arc_Header  shape_header;
	Arc_Header  index_header;

	void  Reorder_Bits (void *dat, int words);
};

//---------------------------------------------------------
//	Arcview_File Class definition
//---------------------------------------------------------

class SYSLIB_API Arcview_File : public Db_Header, public Arcview_Base
{
public:
	Arcview_File (Access_Type access = READ);
	Arcview_File (string filename, Access_Type access = READ);
	~Arcview_File ()                            { Arc_Close (); }

	virtual bool Open (string filename = "")	{ return (Arc_Open (filename)); }
	virtual bool Close (void)					{ return (Arc_Close ()); }
	
	virtual void First_Open (bool flag)			{ Arc_First_Open (flag); }

	virtual bool Read_Record (int number = 0)	{ return (Arc_Read (number)); }
	virtual bool Write_Record (int number = 0)  { return (Arc_Write (number)); }
};

typedef vector <Arcview_File *>    Shape_File_Array;
typedef Shape_File_Array::iterator Shape_File_Itr;

#endif
