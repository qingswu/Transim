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

Db_Matrix * TDF_Matrix (Access_Type access, Format_Type file_format, Format_Type model_format = TRANSIMS)
{
	if (model_format == TPPLUS || model_format == TRANPLAN) {
#ifdef TPPLIB
		return (new TPPlus_Matrix (access));
#else
		exe->Error ("Cube Matrix Processing is Disabled");
#endif
	} else if (model_format == TRANSCAD) {
#ifdef TCADLIB
		return (new TransCAD_Matrix (access));
#else
		exe->Error ("TransCAD Matrix Processing is Disabled");
#endif
	} 
	return (new Db_Matrix (access, file_format));
}

Db_Matrix * TDF_Matrix (Access_Type access, string format)
{
	Format_Type model_format, file_format;
	Db_Matrix::Data_Format (format, file_format, model_format);

	return (TDF_Matrix (access, file_format, model_format));
}
#endif
