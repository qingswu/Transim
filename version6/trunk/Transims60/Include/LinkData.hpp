//*********************************************************
//	LinkData.hpp - Convert Link Data to Link ID
//*********************************************************

#ifndef LINKDATA_HPP
#define LINKDATA_HPP

#include "Data_Service.hpp"
#include "Link_Data_File.hpp"
#include "Best_List.hpp"
#include "Db_Header.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	LinkData - execution class definition
//---------------------------------------------------------

class SYSLIB_API LinkData : public Data_Service
{
public:
	LinkData (void);

	virtual void Execute (void);

protected:
	enum LinkData_Keys { 
		LINK_NODE_LIST_FILE = 1, INPUT_NODE_FILE, INPUT_NODE_FORMAT, NODE_MAP_FILE, COORDINATE_RESOLUTION, 
		DIRECTIONAL_DATA_FILE, DIRECTIONAL_DATA_FORMAT, 
		FROM_NODE_FIELD_NAME, TO_NODE_FIELD_NAME, VOLUME_DATA_FIELD_NAME, SPEED_DATA_FIELD_NAME,
		AB_VOLUME_FIELD_NAME, BA_VOLUME_FIELD_NAME, AB_SPEED_FIELD_NAME, BA_SPEED_FIELD_NAME,
		NEW_LINK_DATA_FILE, NEW_LINK_DATA_FORMAT, 
		NEW_AB_VOLUME_FIELD_NAME, NEW_BA_VOLUME_FIELD_NAME, NEW_AB_SPEED_FIELD_NAME, NEW_BA_SPEED_FIELD_NAME, 
		NEW_DIRECTIONAL_DATA_FILE, NEW_DIRECTIONAL_DATA_FORMAT, NEW_LINK_NODE_FILE, NEW_NODE_MAP_FILE,
	};
	virtual void Program_Control (void);

private:

	bool volume_flag, speed_flag, two_way_flag, data_flag, custom_flag, output_flag;
	bool link_node_flag, input_flag, node_map_flag, new_map_flag, new_link_node_flag; 
	int nequiv, nab, ndir, nlink;
	int from_field, to_field, num_fields;
	double resolution;
	
	Integer_List vol_fields, spd_fields;
	Integer_List vol_ab_fields, vol_ba_fields, spd_ab_fields, spd_ba_fields;
	Integer_List ab_vol_fields, ba_vol_fields, ab_spd_fields, ba_spd_fields;

	Int2_Map data_map;
	Int_Map input_map;
	Ints_Array node_links;

	//---- link node list ----

	typedef struct {
		int      link;
		Integers nodes;
	} Link_Nodes;

	typedef vector <Link_Nodes>        Link_Node_Array;
	typedef Link_Node_Array::iterator  Link_Node_Itr;

	Link_Node_Array link_node_array;

	//---- node path data ----

	typedef struct {
		int   distance;
		int   next_list;
		int   from_node;
		int   dir_index;
	} Node_Path_Data;

	typedef vector <Node_Path_Data>    Node_Path_Array;
	typedef Node_Path_Array::iterator  Node_Path_Itr;

	Node_Path_Array node_path_array;

	//---- link direction data ----

	typedef struct {
		int    link;
		short  num_ab;
		short  num_ba;
		Doubles volume_ab;
		Doubles volume_ba;
		Doubles speed_ab;
		Doubles speed_ba;
	} Data;

	typedef vector <Data>         Data_Array;
	typedef Data_Array::iterator  Data_Itr;

	Data_Array data_array;

	Db_File link_node, new_link_node, node_map_file, new_map_file;
	Db_Header dir_file, new_dir_file, input_node_file;
	Link_Data_File data_file;

	void Read_Node_Map (void);
	void Read_Node_File (void);
	void Read_Link_Nodes (void);
	int  Node_Path (int anode, int bnode);
	void Create_Link_Map (void);
	void Read_Dir_Data (void);
	void Write_Dir_Data (void);
	void Write_Link_Nodes (void);
	void Write_Node_Map (void);
};
#endif
