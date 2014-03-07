//*********************************************************
//	TcadMatrix.hpp - TransCAD Matrix DLL Interface
//*********************************************************

#ifndef TCADMATRIX_HPP
#define TCADMATRIX_HPP

#include "APIDefs.hpp"
#include "Db_Matrix.hpp"

#include "..\TcadLib\CaliperMTX.h"

extern int tc_status;

//---------------------------------------------------------
//	TransCAD_Matrix Class definition
//---------------------------------------------------------

class SYSLIB_API TransCAD_Matrix : public Db_Matrix
{
public:
	TransCAD_Matrix (Access_Type access = READ);

	virtual bool Db_Open (string filename);
	virtual bool Is_Open (void)        { return (fh != 0); }
	virtual bool Close (void);
	
	virtual bool Read_Row (int org = 0, int period = 0);
	virtual bool Write_Row (int org = 0, int period = 0);

	virtual bool Read_Row (void *data, int org, int table, int period = 0);
	virtual bool Write_Row (void *data, int org, int table, int period = 0);
	
	virtual bool Read_Matrix (void);
	virtual bool Write_Matrix (void);

	virtual bool Rewind (void)         { return (true); }

	//virtual Db_Matrix * Replicate_Matrix (String filename, Strings tables);

private:
	virtual bool Load_DLL (void);
	
	virtual bool Read_Header (bool stat);

	TC_MATRIX fh;

	static bool DLL_loaded;
};
#endif
