//*********************************************************
//	Trip_Gap_Data.hpp - Trip Gap Summary Data
//*********************************************************

#ifndef TRIP_GAP_DATA_HPP
#define TRIP_GAP_DATA_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"
#include "Time_Periods.hpp"
#include "Db_File.hpp"
#include "Data_Pack.hpp"
#include "Gap_Data.hpp"

//---------------------------------------------------------
//	Trip_Gap_Data Class definition
//---------------------------------------------------------

class SYSLIB_API Trip_Gap_Data
{
public:
	Trip_Gap_Data (void);

	void Clear (void);

	bool Report_Flags (bool gap_report);
	bool Set_Periods (Time_Periods &periods);

	bool Open_Trip_Gap_File (string filename);
	bool Output_Flag (void)                      { return (output_flag); }
	bool Active_Flag (void)                      { return (active_flag); }
	
	bool Cost_Flag (void)                        { return (cost_flag); }
	void Cost_Flag (bool flag)                   { cost_flag = flag; }

	void Add_Trip_Gap_Data (Dtime tod, double input, double compare);
	void Write_Trip_Gap_File (void);

	void Trip_Gap_Report (int number);
	void Trip_Gap_Header (void);

	void Replicate (Trip_Gap_Data &data);
	void Merge_Data (Trip_Gap_Data &data);

#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data)                { return (gap_array.Pack (data, true)); }
	bool UnPack (Data_Buffer &data)              { return (gap_array.UnPack (data, true)); }
#endif

private:
	void Initialize (void);

	int num_periods, period;
	bool gap_report, output_flag, cost_flag, period_flag, active_flag;

	Gap_Sum_Array gap_sum_array;

	Time_Periods periods;
	Db_File gap_file;
};

#endif
