//*********************************************************
//	Update_Routes.cpp - update route nodes
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Update_Routes
//---------------------------------------------------------

void RoutePrep::Update_Routes (int node, int from, int to, int new1, int new2)
{
	Route_Node route_node;
	Route_Nodes_Itr route_itr;
	Route_Node_Itr rnode_itr, rnode1_itr;

	//---- update the route nodes ----

	for (route_itr = route_nodes_array.begin (); route_itr != route_nodes_array.end (); route_itr++) {
		for (rnode_itr = route_itr->nodes.begin (); rnode_itr != route_itr->nodes.end (); rnode_itr++) {
			if (rnode_itr->Node () == node) {
				if (to >= 0 && rnode_itr != route_itr->nodes.end ()) {
					rnode1_itr = rnode_itr + 1;

					if (rnode1_itr != route_itr->nodes.end () && rnode1_itr->Node () == to) {
						rnode_itr->Node (new2);

						route_node.Clear ();
						route_node.Node (new1);

						route_itr->nodes.insert (rnode_itr, route_node);
						break;

					} else if (rnode_itr != route_itr->nodes.begin ()) {
						rnode1_itr = rnode_itr - 1;

						if (rnode1_itr->Node () == to) {
							rnode_itr->Node (new1);

							route_node.Clear ();
							route_node.Node (new2);

							route_itr->nodes.insert (rnode_itr, route_node);
							break;
						}
					}
				}
				if (from >= 0) {
					if (rnode_itr != route_itr->nodes.begin ()) {
						rnode1_itr = rnode_itr - 1;

						if (rnode1_itr->Node () == from) {
							rnode_itr->Node (new2);

							route_node.Clear ();
							route_node.Node (new1);

							route_itr->nodes.insert (rnode_itr, route_node);
						} else if (rnode_itr != route_itr->nodes.end ()) {
							rnode1_itr = rnode_itr + 1;

							if (rnode1_itr != route_itr->nodes.end () && rnode1_itr->Node () == from) {
								rnode_itr->Node (new1);

								route_node.Clear ();
								route_node.Node (new2);

								route_itr->nodes.insert (rnode_itr, route_node);
							}
						}
					} else if (rnode_itr != route_itr->nodes.end ()) {
						rnode1_itr = rnode_itr + 1;

						if (rnode1_itr != route_itr->nodes.end () && rnode1_itr->Node () == from) {
							rnode_itr->Node (new1);

							route_node.Clear ();
							route_node.Node (new2);

							route_itr->nodes.insert (rnode_itr, route_node);
						}
					}
				}
				break;
			}
		}
	}
}
