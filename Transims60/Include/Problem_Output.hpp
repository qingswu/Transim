//*********************************************************
//	Problem_Output.hpp - Output Interface Class
//*********************************************************

#ifndef PROBLEM_OUTPUT_HPP
#define PROBLEM_OUTPUT_HPP

#include "Sim_Output_Data.hpp"
#include "Problem_Data.hpp"
#include "Problem_File.hpp"

//---------------------------------------------------------
//	Problemt_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Problem_Output : public Sim_Output_Data
{
public:
	Problem_Output (void);
	~Problem_Output (void);

	void Output_Problem (Problem_Data &problem);

private:

	Problem_File *file;
};
#endif
