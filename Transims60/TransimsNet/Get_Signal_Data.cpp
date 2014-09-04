//*********************************************************
//	Get_Signal_Data.cpp - read the signal file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Signal_Data
//---------------------------------------------------------

bool TransimsNet::Get_Signal_Data (Signal_File &file, Signal_Data &signal_rec)
{
	if (!file.Nested () && (delete_node_flag || update_node_flag)) {
		int node, num;
		String svalue;
		Strings nodes;
		Str_Itr str_itr;

		svalue = file.Nodes ();
		if (svalue.empty ()) return (false);

		num = svalue.Parse (nodes, "|+/");
		if (num == 0) return (false);

		for (str_itr = nodes.begin (); str_itr != nodes.end (); str_itr++) {
			node = str_itr->Integer ();
			if ((delete_node_flag && delete_node_range.In_Range (node)) ||
				(update_node_flag && update_node_range.In_Range (node))) {
				xsignal++;
				return (false);
			}
		}
	}
	return (Data_Service::Get_Signal_Data (file, signal_rec));
}
