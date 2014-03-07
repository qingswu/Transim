//*********************************************************
//	Function.hpp - array of functions
//*********************************************************

#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"

#include <string>
#include <map>
using namespace std;

typedef struct {
	Function_Type type;
	double a;
	double b;
	double c;
	double d;
} Function;

//---------------------------------------------------------
//	Function_Array Class definition
//---------------------------------------------------------

class SYSLIB_API Function_Array : public Static_Service
{
public:
	Function_Array (void) : Static_Service ()  { }

	int  Num_Functions (void)                  { return ((int) functions.size ()); }

	bool Add_Function (int number, string &parameters, bool print_flag = true);

	bool Add_Function (int number, Function &function, bool print_flag = false);

	bool Update_Function (int number, Function &function);

	double Apply_Function (int number, double value);

	Function Get_Function (int number);

	void Print_Function (int number);

private:

	typedef map <int, Function>        Functions;
	typedef pair <int, Function>       Function_Data;
	typedef Functions::iterator        Function_Itr;
	typedef pair <Function_Itr, bool>  Function_Stat;

	Functions functions;
};

#endif
