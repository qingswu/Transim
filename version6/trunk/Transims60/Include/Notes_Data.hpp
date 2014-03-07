//*********************************************************
//	Notes_Data.hpp - base class for notes data field
//*********************************************************

#ifndef NOTES_DATA_HPP
#define NOTES_DATA_HPP

#include "APIDefs.hpp"
#include "Buffer.hpp"

//---------------------------------------------------------
//	Notes_Data class definition
//---------------------------------------------------------

class SYSLIB_API Notes_Data
{
public:
	Notes_Data (void)                { Clear (); }

	string Notes (void)              { return (notes.String ()); }
	void   Notes (char *value)       { notes.Data (value); }
	void   Notes (const char *value) { notes.Data (value); }
	void   Notes (string value)      { if (!value.empty ()) notes.Data (value); else notes.Clear (); }

	void   Clear (void)              { notes.Size (0); }

private:
	Buffer notes;
};

#endif
