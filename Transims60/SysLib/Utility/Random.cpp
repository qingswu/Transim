//********************************************************* 
//	Random.cpp - random number class
//*********************************************************

#include "Random.hpp"

#include <time.h>

//-----------------------------------------------------------
//	Random constructor
//-----------------------------------------------------------

Random::Random (int seed) 
{
	Seed (seed);
}

//---------------------------------------------------------
//	random_seed - set the random number seed
//---------------------------------------------------------

int Random::Seed (int seed)
{
	if (seed < 1) {
		seed = (int) time (0);
	}
	next_random_number = seed;

	return (seed);
}

//---------------------------------------------------------
//	Probability - random probability
//---------------------------------------------------------

double Random::Probability (void)
{
	next_random_number = next_random_number * 1103515245L + 12345L;

	return ((double) ((unsigned int) (next_random_number >> 16) & 0x7fff) / (0x7fff + 1.0));
}

//---------------------------------------------------------
//	Randomize - randomize a vector of integers
//---------------------------------------------------------

void Random::Randomize (Integers &list)
{
	int index, size;
	Integers new_list;
	Int_Itr itr;

	size = (int) list.size ();
	if (size < 2) return;

	new_list.assign (size, -1);

	for (itr = list.begin (); itr != list.end (); itr++) {
		for (;;) {
			index = (int) (size * Probability ());
			if (index < size && new_list [index] == -1) {
				new_list [index] = *itr;
				break;
			}
		}
	}
	list.swap (new_list);
}
