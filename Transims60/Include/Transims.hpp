//*********************************************************
//	Transims.hpp - execute TRANSIMS program libraries
//*********************************************************

#ifndef TRANSIMS_HPP
#define TRANSIMS_HPP

#include "Execution_Service.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Transims - execution class definition
//---------------------------------------------------------

class Transims : public Execution_Service
{
public:
	Transims (void);

	virtual void Execute (void);

protected:
	enum Transims_Keys { 
		PROGRAM_NAME = 1, COMMAND_LINE, 
	};
	virtual void Program_Control (void);

private:
	Strings program_names, command_lines;
};
#endif
