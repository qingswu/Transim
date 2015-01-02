//*********************************************************
//	PathSkim.hpp - Build and Skim Network Paths
//*********************************************************

#ifndef PATHSKIM_HPP
#define PATHSKIM_HPP

#include "APIDefs.hpp"
#include "Router_Service.hpp"
#include "Time_Periods.hpp"
#include "Data_Range.hpp"
#include "Skim_File.hpp"

//---------------------------------------------------------
//	PathSkim - execution class definition
//---------------------------------------------------------

class SYSLIB_API PathSkim : public Router_Service
{
public:
	PathSkim (void);

	virtual void Execute (void);

protected:
	enum PathSkim_Keys { 
		ROUTE_FROM_SPECIFIED_LOCATIONS = 1, ROUTE_TO_SPECIFIED_LOCATIONS, 
		ROUTE_AT_SPECIFIED_TIMES, ROUTE_BY_TIME_INCREMENT, ROUTE_WITH_TIME_CONSTRAINT, 
		ROUTE_WITH_SPECIFIED_MODE, ROUTE_WITH_USE_RESTRICTION,
		ROUTE_USING_VEHICLE_TYPE, ROUTE_USING_TRAVELER_TYPE,
		ROUTE_FROM_SPECIFIED_ZONES, ROUTE_TO_SPECIFIED_ZONES, ORIGIN_LOCATIONS_PER_ZONE,
		DESTINATION_LOCATIONS_PER_ZONE, LOCATION_SELECTION_METHOD, ORIGIN_ZONE_FILE, 
		DESTINATION_ZONE_FILE, ORIGIN_LOCATION_FILE, DESTINATION_LOCATION_FILE, 
		NEW_ORIGIN_LOCATION_FILE, NEW_DESTINATION_LOCATION_FILE, SKIM_TRANSIT_LOAD_FACTOR,
		NEW_ACCESSIBILITY_FILE, NEW_ACCESSIBILITY_FORMAT, 
		ORIGIN_WEIGHT_FIELD, DESTINATION_WEIGHT_FIELD, MAXIMUM_TRAVEL_TIME,
	};
	virtual void Program_Control (void);
	virtual bool Save_Plans (Plan_Ptr_Array *array_ptr, int part=0);
	virtual bool Save_Skims (One_To_Many *skim_ptr);
	virtual bool Skim_Check (Plan_Ptr ptr);
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);

private:
	enum PathSkim_Reports { ZONE_EQUIV = 1 };
	
	bool mode_flag, select_org, select_des, select_time, forward_flag, user_loc_flag;
	bool sel_org_zone, sel_des_zone, zone_loc_flag, org_loc_flag, des_loc_flag;
	bool plan_flag, problem_flag, skim_flag, zone_skim_flag, district_flag, zone_flag;
	bool accessibility_flag;

	double load_factor;
	int new_mode, use_type, veh_type, traveler_type;
	int nprocess, constraint, total_records, num_org, num_des, method, cells_out;
	int org_wt_fld, des_wt_fld;
	Dtime max_travel_time;

	Skim_File *skim_file;
	Plan_File *plan_file;
	Problem_File *problem_file;
	Db_File user_org_file, user_des_file;
	Db_File org_loc_file, des_loc_file;
	Db_Header accessibility_file;
	Zone_File *zone_file;

	Time_Periods route_periods;
	Data_Range org_range, des_range, purpose_range;
	Data_Range org_zone_range, des_zone_range;

	Random random_loc;
	Integers warn_flag;
	Integers org_loc, des_loc;
	Ints_Map org_zone_loc, des_zone_loc;
	Int_Set org_out, des_out, per_out;
	Doubles org_wt, des_wt, des_wt_total;

	void Zone_Locations (void);
	void Build_Paths (void);
	void Skim_Paths (void);
	void Zone_Skims (void);
	void Flow_Skims (void);
	bool Location_List (int zone, int num_sel, Integers *locations, Integers &selected);
	void Output_Skims (int period);
	void User_Locations (void);
	void New_Zone_Locations (void);

	struct Path_Locations {
		int from_zone;
		Integers from_loc;
		Integers to_zone;
		Integers to_loc;
		Path_Locations (void) : from_zone(0) {}
	};
	typedef vector <Path_Locations>  Path_Loc_Array;
	typedef Path_Loc_Array::iterator Path_Loc_Itr;
};
#endif
