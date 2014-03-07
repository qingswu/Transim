//*********************************************************
//	PNRSplit.hpp - split drive and walk legs
//*********************************************************

#ifndef PNRSPLIT_HPP
#define PNRSPILT_HPP

#include "Execution_Service.hpp"
#include "Select_Service.hpp"
#include "Db_File.hpp"
#include "TypeDefs.hpp"
#include "String.hpp"

#include "..\TcadLib\CaliperMTX.h"
#include "..\TcadLib\MTXERR.h"

#include <vector>
using namespace std;

extern int tc_status;

//---------------------------------------------------------
//	PNRSplit - execution class definition
//---------------------------------------------------------

class PNRSplit : public Execution_Service, public Select_Service
{
public:
	PNRSplit (void);
	virtual ~PNRSplit (void);

	virtual void Execute (void);

protected:
	enum PNRSplit_Keys { 
		DRIVE_TO_TRANSIT_TRIPS = 1, PARKING_NODE_SKIM, PARKING_ZONE_MAP_FILE, NEW_DRIVE_TO_TRANSIT_TRIPS, 
		DRIVE_TO_TRANSIT_TABLE, PARKING_NODE_TABLE, WALK_TO_TRANSIT_TABLE, DRIVE_TO_PARKING_TABLE,
	};
	virtual void Program_Control (void);

private:

	float *pnr_data;
	long *row_id, *col_id, *node_data;

	int num_zones, num_pnr, pnr_id, node_id;
	float **walk_data, **drive_data;
	String pnr_table, node_table, drive_table, walk_table, new_filename;

	Int_Map parking_zone_map, org_map, des_map;

	TC_MATRIX pnr_trip;
	TC_MATRIX node_skim;
	TC_MATRIX new_trip;

	Db_File parking_zone_file;
	void Process (void);

};
#endif
