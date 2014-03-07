//*********************************************************
//	TDF_Matrix.hpp - travel demand forecasting matrix
//*********************************************************

#ifndef TDF_MATRIX_HPP
#define TDF_MATRIX_HPP

#include "Db_Matrix.hpp"

#ifdef TPPLIB
#include "TppMatrix.hpp"
#endif
#ifdef TCADLIB
#include "TcadMatrix.hpp"
#endif

//---------------------------------------------------------
//	TDF_Matrix
//---------------------------------------------------------

Db_Matrix * TDF_Matrix (Format_Type format = TRANSIMS)
{
	if (format == TPPLUS || format == TRANPLAN) {
#ifdef TPPLIB
		return (new TPPlus_Matrix ());
#else
		exe->Error ("Cube Matrix Processing is Disabled");
#endif
	} else if (format == TRANSCAD) {
#ifdef TCADLIB
		return (new TransCAD_Matrix ());
#else
		exe->Error ("TransCAD Matrix Processing is Disabled");
#endif
	} 
	return (new Db_Matrix ());
}

#endif
