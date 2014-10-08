//*********************************************************
//	Snapshot_Output.hpp - Output Interface Class
//*********************************************************

#ifndef SNAPSHOT_OUTPUT_HPP
#define SNAPSHOT_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Data_Range.hpp"
#include "Snapshot_File.hpp"
#include "Data_Buffer.hpp"

//---------------------------------------------------------
//	Snapshot_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Snapshot_Output : public Sim_Output_Data
{
public:
	Snapshot_Output (int num);
	~Snapshot_Output (void);

	void Write_Check (void);

	void End_Output (void);
private:

	Snapshot_File *file;
	Data_Range link_range;
	Data_Range subarea_range;
	int x1, y1, x2, y2;		//---- rounded ----
	bool coord_flag;
	bool cell_flag;
	bool size_flag;
	bool compress;
	bool metric_flag;
	unsigned max_size;
	unsigned num_records;


	Data_Buffer data;
	Snapshot_File *temp_file;
};
#endif
