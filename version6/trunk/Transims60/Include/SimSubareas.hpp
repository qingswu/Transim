//*********************************************************
//	SimSubareas.hpp - generate geographic subareas
//*********************************************************

#ifndef RANDOMSELECT_HPP
#define RANDOMSELECT_HPP

#include "Data_Service.hpp"
#include "Arcview_File.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	SimSubareas - execution class definition
//---------------------------------------------------------

class SYSLIB_API SimSubareas : public Data_Service
{
public:
	SimSubareas (void);

	virtual void Execute (void);

protected:
	enum SimSubareas_Keys { 
		NUMBER_OF_SUBAREAS = 1, CENTER_NODE_NUMBER, SUBAREA_BOUNDARY_FILE
	};
	virtual void Program_Control (void);

private:
	int num_subareas, center, subarea_field;
	bool boundary_flag;

	Integers subarea_count;
	Arcview_File boundary_file;

	void Boundary_Nodes (void);
	void Distribute_Nodes (void);
};
#endif
