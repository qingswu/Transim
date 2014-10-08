//*********************************************************
//	SubareaNet.hpp - Create a subarea network files
//*********************************************************

#ifndef SUBAREANET_HPP
#define	SUBAREANET_HPP

#include "Data_Service.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Db_Array.hpp"
#include "TypeDefs.hpp"
#include "Shape_Tools.hpp"

class SYSLIB_API SubareaNet : public Data_Service
{
public:

	SubareaNet (void);
	
	virtual void Execute (void);

protected:
	enum SubareaNet_Keys { 
		SUBAREA_BOUNDARY_POLYGON = 1, SUBAREA_COORDINATE_BOX, SUBAREA_NODE_CODE, EXTERNAL_OFFSET_LENGTH
	};

	virtual void Program_Control (void);

	virtual bool Get_Node_Data (Node_File &file, Node_Data &data);
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);
	virtual bool Get_Link_Data (Link_File &file, Link_Data &data, Dir_Data &ab, Dir_Data &ba);
	virtual bool Get_Pocket_Data (Pocket_File &file, Pocket_Data &data);
	virtual bool Get_Lane_Use_Data (Lane_Use_File &file, Lane_Use_Data &data);
	virtual bool Get_Connect_Data (Connect_File &file, Connect_Data &data);
	virtual bool Get_Turn_Pen_Data (Turn_Pen_File &file, Turn_Pen_Data &data);
	virtual bool Get_Parking_Data (Parking_File &file, Parking_Data &data);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);
	virtual bool Get_Access_Data (Access_File &file, Access_Data &data);
	virtual bool Get_Sign_Data (Sign_File &file, Sign_Data &data);
	virtual bool Get_Signal_Data (Signal_File &file, Signal_Data &data);
	virtual bool Get_Timing_Data (Timing_File &file, Timing_Record &data);
	virtual bool Get_Phasing_Data (Phasing_File &file, Phasing_Record &data);
	virtual bool Get_Detector_Data (Detector_File &file, Detector_Data &data);
	virtual bool Get_Stop_Data (Stop_File &file, Stop_Data &data);

private:

	int max_location, max_parking, max_access, max_stop, max_route, max_zone;
	int nnode, nzone, nlink, nshort, nboundary, nshape, npocket, nconnect;
	int nlocation, nparking, naccess, nlane_use, nturn, ntoll;
	int nsign, nsignal, ntiming, nphasing, ndetector, ncoord;
	int nstop, nroute, nschedule, ndriver, external_offset;
	int new_zone, new_location, new_parking, new_access, new_stop, new_route;
	bool transit_flag, zone_flag, box_flag, node_flag;
	int x_min, y_min, x_max, y_max, node_code;

	Arcview_File subarea_boundary;	

	Db_Data_Array node_db, timing_db;
	
	Projection projection;
	Projection_Service proj_service;

	void Write_Node (void);
	void Write_Boundary (void);
	void Write_Route (void);
};
#endif
