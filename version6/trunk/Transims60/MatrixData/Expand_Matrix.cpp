//*********************************************************
//	Expand_Matrix.cpp - expand matrix files into memory
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Expand_Matrix
//---------------------------------------------------------

void MatrixData::Expand_Matrix (void)
{
	int i, j, k, in_j, out_i, out_j, n, org, des, period, num_periods, out_period, p1, p2, max_i, max_k;
	double value, base, total, expand;
	bool flag;

	Dtime low, high;
	Matrix_Itr matrix_itr;
	Int_Map *des_map;
	Int_Map_Itr des_itr;
	Int_Set *org_set, *des_set, o_set, d_set;
	Int_Set_Itr o_itr, d_itr;
	Int_Itr int_itr;
	Doubles time_share, zone_share, group_total;

	i = j = k = out_i = out_j = max_i = max_k = 0;
	total = 0.0;

	//---- process the distribution matrix ----

	matrix_itr = matrix_group.begin ();

	Show_Message (String ("Reading %s -- Record") % matrix_itr->matrix->File_Type ());
	Set_Progress ();

	num_periods = matrix_itr->matrix->Num_Periods ();
	if (num_periods == 0) {
		num_periods = 1;
	}
	time_share.assign (num_periods, 0.0);

	if (Zone_Equiv_Flag ()) {
		max_i = (int) zone_equiv.Zone_Map ()->size ();
		max_k = zone_equiv.Max_Group () + 1;
		zone_share.assign (max_i, 0.0);
		group_total.assign (max_k, 0.0);
	}

	//---- process each period ----

	for (period=0; period < num_periods; period++) {

		if (period_flag) {
			matrix_itr->matrix->Period_Range (period, low, high);
			out_period = new_matrix->Period (low, high);
		} else {
			out_period = 0;
		}
		if (out_period < 0) continue;

		//---- process each origin zone ----

		for (org=1; matrix_itr->matrix->Read_Row (org, period); org++) {

			if (matrix_itr->matrix->Org_Index (org) < 0) continue;

			i = org;

			if (matrix_itr->transpose) {
				out_j = i = new_matrix->Add_Des (i);
			} else {
				out_i = i = new_matrix->Add_Org (i);
			}
			if (i < 0) continue;

			if (Zone_Equiv_Flag ()) {
				k = zone_equiv.Zone_Group (org);
				if (k < 0 || k >= max_k) {
					Warning (String ("No Equivalence for Zone %d") % org);
				}
			}

			des_map = matrix_itr->matrix->Des_Map ();
				
			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
				flag = false;
						
				des = des_itr->first;
				in_j = des_itr->second;
						
				j = des;
				if (j < 0) continue;

				if (matrix_itr->transpose) {
					out_i = j = new_matrix->Add_Org (j);
				} else {
					out_j = j = new_matrix->Add_Des (j);
				}
				if (j < 0) continue;

				//---- user processing script ----

				if (script_flag) {
					matrix_itr->record->Origin (org);
					matrix_itr->record->Destination (des);
					matrix_itr->record->Period (period);

					for (n=0; n < matrix_itr->matrix->Tables (); n++) {
						matrix_itr->matrix->Get_Cell_Index (in_j, n, value);
						value *= matrix_itr->factor;
						matrix_itr->record->Table (n, value);
					}
					new_matrix->Reset_Record ();
					new_matrix->Copy_Fields (*matrix_itr->record);

					if (program.Execute (-1, matrix_itr->group) == 0) continue;

					//---- add the matrix record to array ----
			
					for (n=0; n < new_matrix->Tables (); n++) {
						value = new_matrix->Table (n) / factor;

						if (value != 0) {
							new_matrix->Add_Cell_Index (out_period, out_i, out_j, n, value);
							flag = true;

							time_share [out_period] += value;
							total += value;
	
							if (Zone_Equiv_Flag ()) {
								zone_share [i] += value;
								group_total [k] += value;
							}
						}
					}

				} else {
			
					//---- add the matrix record to array ----
			
					for (n=0; n < matrix_itr->matrix->Tables (); n++) {
						if (n >= new_matrix->Tables ()) break;

						matrix_itr->matrix->Get_Cell_Index (in_j, n, value);
						value *= matrix_itr->factor / factor;

						if (value != 0) {
							new_matrix->Add_Cell_Index (out_period, out_i, out_j, n, value);
							flag = true;

							time_share [out_period] += value;
							total += value;
	
							if (Zone_Equiv_Flag ()) {
								zone_share [i] += value;
								group_total [k] += value;
							}
						}
					}
				}
				if (flag) {
					Show_Progress ();
				}
			}
		}
	}
	End_Progress ();

	matrix_itr->matrix->Close ();

	//---- calculate the default distribution factors ----

	if (total <= 0.0) {
		Error ("No Trips in the Distribution Matrix");
	}
	for (period=0; period < num_periods; period++) {
		time_share [period] /= total;
	}
	if (Zone_Equiv_Flag ()) {
		des_map = zone_equiv.Zone_Map ();

		for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
			if (matrix_itr->transpose) {
				i = new_matrix->Add_Des (des_itr->first);
			} else {
				i = new_matrix->Add_Org (des_itr->first);
			}
			if (i < 0) continue;
			k = des_itr->second;

			if (group_total [k] > 0.0) {
				zone_share [i] /= group_total [k];
			} else {
				n = (int) zone_equiv.Group_List (k)->size ();
				zone_share [i] = 1.0 / n;
			}
		}
	}

	//---- process the expansion matrix ----

	for (++matrix_itr; matrix_itr != matrix_group.end (); matrix_itr++) {

		Show_Message (String ("Expanding %s -- Record") % matrix_itr->matrix->File_Type ());
		Set_Progress ();

		num_periods = matrix_itr->matrix->Num_Periods ();
		if (num_periods == 0) {
			num_periods = 1;
		}

		//---- process each period ----

		for (period=0; period < num_periods; period++) {

			if (period_flag) {
				matrix_itr->matrix->Period_Range (period, low, high);
				p1 = new_matrix->Period (low);
				p2 = new_matrix->Period (high - 1);
			} else {
				p1 = p2 = 0;
			}
			if (p1 < 0 || p2 < 0) continue;

			//---- process each origin zone ----

			for (org=1; matrix_itr->matrix->Read_Row (org, period); org++) {

				if (matrix_itr->matrix->Org_Index (org) < 0) continue;

				org_set = (Zone_Equiv_Flag ()) ? zone_equiv.Group_List (org) : 0;
				if (org_set == 0) {
					o_set.clear ();
					o_set.insert (org);
					org_set = &o_set;
				}
				des_map = matrix_itr->matrix->Des_Map ();
				
				for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
						
					des = des_itr->first;
					in_j = des_itr->second;

					des_set = (Zone_Equiv_Flag ()) ? zone_equiv.Group_List (des) : 0;
					if (des_set == 0) {
						d_set.clear ();
						d_set.insert (des);
						des_set = &d_set;
					}

					//---- expand the table data ----

					if (matrix_itr->break_flag) {
			
						for (n=0; n < matrix_itr->matrix->Tables (); n++) {
							matrix_itr->matrix->Get_Cell_Index (in_j, n, value);

							value *= matrix_itr->factor / factor;
							//if (value == 0) continue;

							//---- sub the trips in the distribution matrix ----

							total = 0;
							flag = false;

							for (k=0, int_itr = matrix_itr->fields.begin (); int_itr != matrix_itr->fields.end (); int_itr++, k++) {
								if (*int_itr != n) continue;

								matrix_itr->breaks.Period_Range (k, low, high);
								p1 = (low > 0) ? new_matrix->Period (low) : 0;
								p2 = new_matrix->Period (high - 1);

								if (p1 < 0 || p2 < 0) continue;
								flag = true;

								for (out_period = p1; out_period <= p2; out_period++) {
									for (o_itr = org_set->begin (); o_itr != org_set->end (); o_itr++) {
										if (matrix_itr->transpose) {
											out_j = i = new_matrix->Add_Des (*o_itr);
										} else {
											out_i = i = new_matrix->Add_Org (*o_itr);
										}
										if (i < 0) continue;

										for (d_itr = des_set->begin (); d_itr != des_set->end (); d_itr++) {
											if (matrix_itr->transpose) {
												out_i = j = new_matrix->Add_Org (*d_itr);
											} else {
												out_j = j = new_matrix->Add_Des (*d_itr);
											}
											if (j < 0) continue;

											new_matrix->Get_Cell_Index (out_period, out_i, out_j, 0, base);
											total += base;
										}
									}
								}
							}
							if (!flag) continue;

							//---- distribute to the periods ----
						
							if (total > 0) {
								if (value == 0) value = 0.01;
								expand = value / total;

								for (k=0, int_itr = matrix_itr->fields.begin (); int_itr != matrix_itr->fields.end (); int_itr++, k++) {
									if (*int_itr != n) continue;

									matrix_itr->breaks.Period_Range (k, low, high);
									p1 = (low > 0) ? new_matrix->Period (low) : 0;
									p2 = new_matrix->Period (high - 1);
									if (p1 < 0 || p2 < 0) continue;

									for (out_period = p1; out_period <= p2; out_period++) {
										for (o_itr = org_set->begin (); o_itr != org_set->end (); o_itr++) {
											if (matrix_itr->transpose) {
												out_j = i = new_matrix->Add_Des (*o_itr);
											} else {
												out_i = i = new_matrix->Add_Org (*o_itr);
											}
											if (i < 0) continue;

											for (d_itr = des_set->begin (); d_itr != des_set->end (); d_itr++) {
												if (matrix_itr->transpose) {
													out_i = j = new_matrix->Add_Org (*d_itr);
												} else {
													out_j = j = new_matrix->Add_Des (*d_itr);
												}
												if (j < 0) continue;

												new_matrix->Factor_Cell_Index (out_period, out_i, out_j, 0, expand);
											}
										}
									}
								}

							} else {
								if (value == 0) continue;

								total = 0.0;

								for (k=0, int_itr = matrix_itr->fields.begin (); int_itr != matrix_itr->fields.end (); int_itr++, k++) {
									if (*int_itr != n) continue;
									matrix_itr->breaks.Period_Range (k, low, high);
									p1 = (low > 0) ? new_matrix->Period (low) : 0;
									p2 = new_matrix->Period (high - 1);
									if (p1 < 0 || p2 < 0) continue;

									for (out_period = p1; out_period <= p2; out_period++) {
										total += time_share [out_period];
									}
								}

								for (k=0, int_itr = matrix_itr->fields.begin (); int_itr != matrix_itr->fields.end (); int_itr++, k++) {
									if (*int_itr != n) continue;

									matrix_itr->breaks.Period_Range (k, low, high);
									p1 = (low > 0) ? new_matrix->Period (low) : 0;
									p2 = new_matrix->Period (high - 1);
									if (p1 < 0 || p2 < 0) continue;

									for (out_period = p1; out_period <= p2; out_period++) {
										for (o_itr = org_set->begin (); o_itr != org_set->end (); o_itr++) {
											if (matrix_itr->transpose) {
												out_j = i = new_matrix->Add_Des (*o_itr);
											} else {
												out_i = i = new_matrix->Add_Org (*o_itr);
											}
											if (i < 0) continue;

											for (d_itr = des_set->begin (); d_itr != des_set->end (); d_itr++) {
												if (matrix_itr->transpose) {
													out_i = j = new_matrix->Add_Org (*d_itr);
												} else {
													out_j = j = new_matrix->Add_Des (*d_itr);
												}
												if (j < 0) continue;

												expand = value * time_share [out_period] / total;
												if (Zone_Equiv_Flag ()) {
													expand *= zone_share [out_i] * zone_share [out_j];
												}
												new_matrix->Set_Cell_Index (out_period, out_i, out_j, 0, expand);
											}
										}
									}
								}
							}
						}

					} else {
			
						for (n=0; n < matrix_itr->matrix->Tables (); n++) {
							if (n >= new_matrix->Tables ()) break;

							matrix_itr->matrix->Get_Cell_Index (in_j, n, value);
							value *= matrix_itr->factor / factor;
							//if (value == 0) continue;

							total = 0;
							for (out_period = p1; out_period <= p2; out_period++) {
								for (o_itr = org_set->begin (); o_itr != org_set->end (); o_itr++) {
									if (matrix_itr->transpose) {
										out_j = i = new_matrix->Add_Des (*o_itr);
									} else {
										out_i = i = new_matrix->Add_Org (*o_itr);
									}
									if (i < 0) continue;

									for (d_itr = des_set->begin (); d_itr != des_set->end (); d_itr++) {
										if (matrix_itr->transpose) {
											out_i = j = new_matrix->Add_Org (*d_itr);
										} else {
											out_j = j = new_matrix->Add_Des (*d_itr);
										}
										if (j < 0) continue;

										new_matrix->Get_Cell_Index (out_period, out_i, out_j, n, base);
										total += base;
									}
								}
							}
							if (total > 0) {
								if (value == 0) value = 0.01;
								expand = value / total;

								for (out_period = p1; out_period <= p2; out_period++) {
									for (o_itr = org_set->begin (); o_itr != org_set->end (); o_itr++) {
										if (matrix_itr->transpose) {
											out_j = i = new_matrix->Add_Des (*o_itr);
										} else {
											out_i = i = new_matrix->Add_Org (*o_itr);
										}
										if (i < 0) continue;

										for (d_itr = des_set->begin (); d_itr != des_set->end (); d_itr++) {
											if (matrix_itr->transpose) {
												out_i = j = new_matrix->Add_Org (*d_itr);
											} else {
												out_j = j = new_matrix->Add_Des (*d_itr);
											}
											if (j < 0) continue;

											new_matrix->Factor_Cell_Index (out_period, out_i, out_j, n, expand);
										}
									}
								}

							} else {
								if (value == 0) continue;
								total = 0.0;

								for (out_period = p1; out_period <= p2; out_period++) {
									total += time_share [out_period];
								}
								for (out_period = p1; out_period <= p2; out_period++) {
									for (o_itr = org_set->begin (); o_itr != org_set->end (); o_itr++) {
										if (matrix_itr->transpose) {
											out_j = i = new_matrix->Add_Des (*o_itr);
										} else {
											out_i = i = new_matrix->Add_Org (*o_itr);
										}
										if (i < 0) continue;

										for (d_itr = des_set->begin (); d_itr != des_set->end (); d_itr++) {
											if (matrix_itr->transpose) {
												out_i = j = new_matrix->Add_Org (*d_itr);
											} else {
												out_j = j = new_matrix->Add_Des (*d_itr);
											}
											if (j < 0) continue;

											expand = value * time_share [out_period] / total;
											if (Zone_Equiv_Flag ()) {
												expand *= zone_share [out_i] * zone_share [out_j];
											}
											new_matrix->Set_Cell_Index (out_period, out_i, out_j, n, expand);
										}
									}
								}
							}
						}
					}
					Show_Progress ();
				}
			}
		}
		End_Progress ();

		matrix_itr->matrix->Close ();
	}
}
