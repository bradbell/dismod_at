// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_started.cpp$$

$section C++ Getting Started$$ 

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/dismod_at.hpp>

int main(int argc, const char* argv[])
{
	assert( argc == 2 );
	std::string file_name = argv[1];
	bool new_file         = false;
	sqlite3* db           = dismod_at::open_connection(file_name, new_file);
	//
	dismod_at::db_input_struct db_input;
	dismod_at::get_db_input(db, db_input);
	//
	std::cout << "get_started.cpp: OK" << std::endl;
	return 0;
}
