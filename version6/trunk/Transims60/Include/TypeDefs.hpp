//*********************************************************
//	TypeDefs.hpp - standard type definitions
//*********************************************************

#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#include <set>
#include <list>
#include <vector>
#include <deque>
#include <map>

#ifdef HASH_MAP
#  include <hash_map>
#  define HASH  hash_map
   using namespace stdext;
#else
#  define HASH  map
   using namespace std;
#endif

#include "String.hpp"
#include "Buffer.hpp"
#include "Dtime.hpp"

//---- array of strings ----

typedef vector <String>              Strings;
typedef Strings::iterator            Str_Itr;
typedef Strings::reverse_iterator    Str_RItr;
typedef Strings::pointer             Str_Ptr;

//---- array of string arrays ----

typedef vector <Strings>             Strs_Array;
typedef Strs_Array::iterator         Strs_Itr;
typedef Strs_Array::reverse_iterator Strs_RItr;
typedef Strs_Array::pointer          Strs_Ptr;

//---- an integer sorted array of strings ----

typedef map <int, String>            Str_Map;
typedef pair <int, String>           Str_Map_Data;
typedef Str_Map::iterator            Str_Map_Itr;
typedef Str_Map::reverse_iterator    Str_Map_RItr;
typedef pair <Str_Map_Itr, bool>     Str_Map_Stat;

//---- a string id map ----

typedef map <String, int>            Str_ID;
typedef pair <String, int>           Str_ID_Data;
typedef Str_ID::iterator             Str_ID_Itr;
typedef pair <Str_ID_Itr, bool>      Str_ID_Stat;

//---- array of bool ----

typedef vector <bool>                Bools;
typedef Bools::iterator              Bool_Itr;
typedef Bools::reverse_iterator      Bool_RItr;
typedef Bools::pointer               Bool_Ptr;

//---- array of char ----

typedef vector <char>                Chars;
typedef Chars::iterator              Char_Itr;
typedef Chars::reverse_iterator      Char_RItr;
typedef Chars::pointer               Char_Ptr;

//---- array of doubles ----

typedef vector <double>              Doubles;
typedef Doubles::iterator            Dbl_Itr;
typedef Doubles::reverse_iterator    Dbl_RItr;	
typedef Doubles::pointer             Dbl_Ptr;

//---- array of double arrays ----

typedef vector <Doubles>             Dbls_Array;
typedef Dbls_Array::iterator         Dbls_Itr;
typedef Dbls_Array::reverse_iterator Dbls_RItr;
typedef Dbls_Array::pointer          Dbls_Ptr;

//---- 3 dimensional double array ----

typedef vector <Dbls_Array>          Dbl3_Array;
typedef Dbl3_Array::iterator         Dbl3_Itr;
typedef Dbl3_Array::reverse_iterator Dbl3_RItr;
typedef Dbl3_Array::pointer          Dbl3_Ptr;

//---- 4 dimensional double array ----

typedef vector <Dbl3_Array>          Dbl4_Array;
typedef Dbl4_Array::iterator         Dbl4_Itr;
typedef Dbl4_Array::reverse_iterator Dbl4_RItr;
typedef Dbl4_Array::pointer          Dbl4_Ptr;

//---- array of floats ----

typedef vector <float>               Floats;
typedef Floats::iterator             Flt_Itr;
typedef Floats::reverse_iterator     Flt_RItr;	
typedef Floats::pointer              Flt_Ptr;

//---- array of float arrays ----

typedef vector <Floats>              Flts_Array;
typedef Flts_Array::iterator         Flts_Itr;
typedef Flts_Array::reverse_iterator Flts_RItr;
typedef Flts_Array::pointer          Flts_Ptr;

//---- 3 dimensional float array ----

typedef vector <Flts_Array>          Flt3_Array;
typedef Flt3_Array::iterator         Flt3_Itr;
typedef Flt3_Array::reverse_iterator Flt3_RItr;
typedef Flt3_Array::pointer          Flt3_Ptr;

//---- 4 dimensional float array ----

typedef vector <Flt3_Array>          Flt4_Array;
typedef Flt4_Array::iterator         Flt4_Itr;
typedef Flt4_Array::reverse_iterator Flt4_RItr;
typedef Flt4_Array::pointer          Flt4_Ptr;

//---- array of unsigned ----

typedef vector <unsigned>            Unsigns;
typedef Unsigns::iterator            UInt_Itr;
typedef Unsigns::reverse_iterator    UInt_RItr;
typedef Unsigns::pointer             UInt_Ptr;

//---- array of integers ----

typedef vector <int>                 Integers;
typedef Integers::iterator           Int_Itr;
typedef Integers::reverse_iterator   Int_RItr;
typedef Integers::pointer            Int_Ptr;

//---- array of integer arrays ----

typedef vector <Integers>            Ints_Array;
typedef Ints_Array::iterator         Ints_Itr;
typedef Ints_Array::reverse_iterator Ints_RItr;
typedef Ints_Array::pointer          Ints_Ptr;

//---- 3 dimensional integer array ----

typedef vector <Ints_Array>          Int3_Array;
typedef Int3_Array::iterator         Int3_Itr;
typedef Int3_Array::reverse_iterator Int3_RItr;
typedef Int3_Array::pointer          Int3_Ptr;

//---- 4 dimensional integer array ----

typedef vector <Int3_Array>          Int4_Array;
typedef Int4_Array::iterator         Int4_Itr;
typedef Int4_Array::reverse_iterator Int4_RItr;
typedef Int4_Array::pointer          Int4_Ptr;

//---- array of shorts ----

typedef vector <short>               Shorts;
typedef Shorts::iterator             Sht_Itr;
typedef Shorts::reverse_iterator     Sht_RItr;
typedef Shorts::pointer              Sht_Ptr;

//---- array of shorts arrays ----

typedef vector <Shorts>              Shts_Array;
typedef Shts_Array::iterator         Shts_Itr;
typedef Shts_Array::reverse_iterator Shts_RItr;
typedef Shts_Array::pointer          Shts_Ptr;

//---- 3 dimensional short array ----

typedef vector <Shts_Array>          Sht3_Array;
typedef Sht3_Array::iterator         Sht3_Itr;
typedef Sht3_Array::reverse_iterator Sht3_RItr;
typedef Sht3_Array::pointer          Sht3_Ptr;

//---- 4 dimensional short array ----

typedef vector <Sht3_Array>          Sht4_Array;
typedef Sht4_Array::iterator         Sht4_Itr;
typedef Sht4_Array::reverse_iterator Sht4_RItr;
typedef Sht4_Array::pointer          Sht4_Ptr;

//---- integer queue ----

typedef deque <int>                  Int_Queue;
typedef Int_Queue::iterator          Int_Que_Itr;
typedef Int_Queue::reverse_iterator  Int_Que_RItr;
typedef Int_Queue::pointer           Int_Que_Ptr;

//---- array of Int2_Keys ----

typedef pair <int, int>              Int2_Key;
typedef vector <Int2_Key>            Int2_Array;
typedef Int2_Array::iterator         Int2_Itr;
typedef Int2_Array::reverse_iterator Int2_RItr;
typedef Int2_Array::pointer          Int2_Ptr;

//---- array of Int2 arrays ----

typedef vector <Int2_Array>          Int2s_Array;
typedef Int2s_Array::iterator        Int2s_Itr;
typedef Int2s_Array::pointer         Int2s_Ptr;

//---- an integer sorted array of integers ----

typedef map <int, Integers>          Ints_Map;
typedef pair <int, Integers>         Ints_Map_Data;
typedef Ints_Map::iterator           Ints_Map_Itr;
typedef pair <Ints_Map_Itr, bool>    Ints_Map_Stat;

//---- list of integers ----

typedef list <int>                   Int_List;
typedef Int_List::iterator           Int_List_Itr;
typedef Int_List::reverse_iterator   Int_List_RItr;
typedef Int_List::pointer            Int_List_Ptr;

//---- a sorted set of integers ----

typedef set <int, less <int> >       Int_Set;
typedef Int_Set::iterator            Int_Set_Itr;
typedef Int_Set::reverse_iterator    Int_Set_RItr;
typedef pair <Int_Set_Itr, bool>     Int_Set_Stat;

//---- a sorted set of two integers ----

typedef set <Int2_Key>               Int2_Set;
typedef Int2_Set::iterator           Int2_Set_Itr;
typedef Int2_Set::reverse_iterator   Int2_Set_RItr;
typedef pair <Int2_Set_Itr, bool>    Int2_Set_Stat;

//---- an integer sort index ----

typedef map <int, int>               Int_Map;
typedef pair <int, int>              Int_Map_Data;
typedef Int_Map::iterator            Int_Map_Itr;
typedef Int_Map::reverse_iterator    Int_Map_RItr;
typedef pair <Int_Map_Itr, bool>     Int_Map_Stat;

//---- an integer hash index ----

typedef HASH <int, int>              Int_Hash;
typedef pair <int, int>              Int_Hash_Data;
typedef Int_Hash::iterator           Int_Hash_Itr;
typedef Int_Hash::reverse_iterator   Int_Hash_RItr;
typedef pair <Int_Hash_Itr, bool>    Int_Hash_Stat;

//---- two integer sort index ----

typedef map <Int2_Key, int>          Int2_Map;
typedef pair <Int2_Key, int>         Int2_Map_Data;
typedef Int2_Map::iterator           Int2_Map_Itr;
typedef Int2_Map::reverse_iterator   Int2_Map_RItr;
typedef pair <Int2_Map_Itr, bool>    Int2_Map_Stat;

//---- duplicate two integer sort index ----

typedef multimap <Int2_Key, int>     NI2_Map;
typedef pair <Int2_Key, int>         NI2_Map_Data;
typedef NI2_Map::iterator            NI2_Map_Itr;
typedef pair <NI2_Map_Itr, bool>     NI2_Map_Stat;

//---- integer to double ----

typedef map <int, double>            Int_Dbl_Map;
typedef pair <int, double>           Int_Dbl_Map_Data;
typedef Int_Dbl_Map::iterator        Int_Dbl_Map_Itr;
typedef pair <Int_Dbl_Map_Itr, bool> Int_Dbl_Map_Stat;

//---- two integer to double ----

typedef map <Int2_Key, double>       I2_Dbl_Map;
typedef pair <Int2_Key, double>      I2_Dbl_Map_Data;
typedef I2_Dbl_Map::iterator         I2_Dbl_Map_Itr;
typedef pair <I2_Dbl_Map_Itr, bool>  I2_Dbl_Map_Stat;

//---- two integer to Integers ----

typedef map <Int2_Key, Integers>     I2_Ints_Map;
typedef pair <Int2_Key, Integers>    I2_Ints_Map_Data;
typedef I2_Ints_Map::iterator        I2_Ints_Map_Itr;
typedef pair <I2_Ints_Map_Itr, bool> I2_Ints_Map_Stat;

//---- duplicate index sort ----

typedef multimap <int, int>          NInt_Map;
typedef pair <int, int>              NInt_Map_Data;
typedef NInt_Map::iterator           NInt_Map_Itr;
typedef NInt_Map::reverse_iterator   NInt_Map_RItr;

//---- array of buffers ----

typedef vector <Buffer>              Buffers;
typedef Buffers::iterator            Buf_Itr;
typedef Buffers::reverse_iterator    Buf_RItr;
typedef Buffers::pointer             Buf_Ptr;

//---- an integer sorted array of buffers ----

typedef map <int, Buffer>            Buf_Map;
typedef pair <int, Buffer>           Buf_Map_Data;
typedef Buf_Map::iterator            Buf_Map_Itr;
typedef Buf_Map::reverse_iterator    Buf_Map_RItr;
typedef pair <Buf_Map_Itr, bool>     Buf_Map_Stat;

//---- array of Dtimes ----

typedef vector <Dtime>               Dtimes;
typedef Dtimes::iterator             Dtime_Itr;
typedef Dtimes::reverse_iterator     Dtime_RItr;
typedef Dtimes::pointer              Dtime_Ptr;


//----	XY_Point -----

struct XY_Point {
	double  x;
	double  y;

	XY_Point (void) : x(0), y(0) {}
	XY_Point (double x, double y) : x(x), y(y) {}
};

struct XYZ_Point {
	double  x;
	double  y;
	double  z;

	XYZ_Point (void) : x(0), y(0), z(0) {}
	XYZ_Point (double x, double y, double z) : x(x), y(y), z(z) {}
};

struct XYZ {
	int  x;
	int  y;
	int  z;

	XYZ (void) : x(0), y(0), z(0) {}
	XYZ (int x, int y, int z) : x(x), y(y), z(z) {}
};

//---- array of XY points ----

typedef vector <XY_Point>            XY_Points;
typedef XY_Points::iterator          XY_Itr;
typedef XY_Points::reverse_iterator  XY_RItr;
typedef XY_Points::pointer           XY_Ptr;

//---- array of XYZ points ----

typedef vector <XYZ_Point>           Points;
typedef Points::iterator             Points_Itr;
typedef Points::reverse_iterator     Points_RItr;
typedef Points::pointer              Points_Ptr;

//---- array of XYZ points ----

typedef vector <XYZ>                 XYZ_Array;
typedef XYZ_Array::iterator          XYZ_Itr;
typedef XYZ_Array::reverse_iterator  XYZ_RItr;
typedef XYZ_Array::pointer           XYZ_Ptr;

//---- an integer sorted array of XY_Points ----

typedef map <int, XY_Point>          XY_Map;
typedef pair <int, XY_Point>         XY_Map_Data;
typedef XY_Map::iterator             XY_Map_Itr;
typedef XY_Map::reverse_iterator     XY_Map_RItr;
typedef pair <XY_Map_Itr, bool>      XY_Map_Stat;

//---- an integer sorted array of XYZ_Points ----

typedef map <int, XYZ_Point>         Point_Map;
typedef pair <int, XYZ_Point>        Point_Map_Data;
typedef Point_Map::iterator          Point_Map_Itr;
typedef Point_Map::reverse_iterator  Point_Map_RItr;
typedef pair <Point_Map_Itr, bool>   Point_Map_Stat;

//---- an integer sorted array of XYZ ----

typedef map <int, XYZ>               XYZ_Map;
typedef pair <int, XYZ>              XYZ_Map_Data;
typedef XYZ_Map::iterator            XYZ_Map_Itr;
typedef XYZ_Map::reverse_iterator    XYZ_Map_RItr;
typedef pair <XYZ_Map_Itr, bool>     XYZ_Map_Stat;

//---- an integer sorted array of Point arrays ----

typedef map <int, Points>            Points_Map;
typedef pair <int, Points>           Points_Map_Data;
typedef Points_Map::iterator         Points_Map_Itr;
typedef Points_Map::reverse_iterator Points_Map_RItr;
typedef pair <Points_Map_Itr, bool>  Points_Map_Stat;

//---- an array of Points data ----

typedef vector <Points>              Points_Array;
typedef Points_Array::iterator       Points_Array_Itr;
typedef Points_Array::pointer        Points_Array_Ptr;

#endif
