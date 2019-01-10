/***
 * Projekt IFJ2018
 * @name 		Main file
 * @authors: 	Romana Džubárová,		xdzuba00
 *				Šárka Ščavnícká,		xscavn01
 *				Daniel Miloslav Očenáš,	xocena06
 *				Michal Vanka,			xvanka00
 */

#include "synt_a.h"

int main (int argc, char **argv)
{

	int err_code;
	if (argc == 2)
    {
		err_code = synt_a (argv[1]);
	}

	return err_code;
}

