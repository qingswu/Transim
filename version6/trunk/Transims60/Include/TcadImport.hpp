//*********************************************************
//	TcadImport.hpp - create TransCAD matrix from text file
//*********************************************************

#ifndef PNRSPLIT_HPP
#define PNRSPILT_HPP

#include "Execution_Service.hpp"
#include "Db_Header.hpp"
#include "TypeDefs.hpp"
#include "String.hpp"

#include "..\TcadLib\CaliperMTX.h"
#include "..\TcadLib\MTXERR.h"

#include <vector>
using namespace std;

extern int tc_status;

//---------------------------------------------------------
//	TcadImport - execution class definition
//---------------------------------------------------------

class TcadImport : public Execution_Service
{
public:
	TcadImport (void);
	virtual ~TcadImport (void);

	virtual void Execute (void);

protected:
	enum TcadImport_Keys { 
		HIGHEST_ZONE_NUMBER = 1, INPUT_MATRIX_FILE, INPUT_MATRIX_FORMAT, 
		INPUT_ORIGIN_FIELD, INPUT_DESTINATION_FIELD, INPUT_TABLE_FIELDS,
		NEW_TRANSCAD_MATRIX, NEW_MATRIX_LABEL, NEW_TABLE_NAMES, 
	};
	virtual void Program_Control (void);

private:

	float ***trips;

	int num_zones, tables, org_field, des_field;
	Integers table_fields;
	Strings table_names;
	String new_filename, new_label;

	TC_MATRIX new_trip;
	Db_Header input_file;

	void Process (void);

};
#endif
