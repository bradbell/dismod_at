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
$begin get_weight_grid_xam.cpp$$
$spell
	xam
$$

$section C++ get_weight_grid: Example and Test$$
$index example, C++ get_weight_grid$$
$index get_weight_grid, C++ example$$

$code
$verbatim%example/cpp/get_weight_grid_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <sqlite3.h>
# include <dismod_at/get_weight_grid.hpp>
# include <dismod_at/get_weight_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_weight_grid_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;	

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = { 
	"create table weight(weight_id integer primary key, weight_name text)",
	"insert into weight values(0, 'constant')",
	"insert into weight values(1, 'bilinear')",
	//
	"create table weight_grid("
		" weight_grid_id integer primary key,"
		" weight_id      integer, "
		" age            real,"
		" time           real,"
		" weight         real)",
	//                 weight_grid_id, weight_id,   age,   time, weight
	"insert into weight_grid values(0,         0,  50.0, 2000.0,    1.0)",
	"insert into weight_grid values(1,         1,   0.0, 1980.0,    0.5)",
	"insert into weight_grid values(2,         1, 100.0, 1980.0,    1.0)",
	"insert into weight_grid values(3,         1,   0.0, 2010.0,    1.0)",
	"insert into weight_grid values(4,         1, 100.0, 2010.0,    1.5)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the weight table
	vector<string> weight_table = dismod_at::get_weight_table(db);
	ok  &= weight_table.size() == 2;
	ok  &= weight_table[0] == "constant";
	ok  &= weight_table[1] == "bilinear";

	// get the weight_grid table
	vector<dismod_at::weight_grid_struct> 
		weight_grid = dismod_at::get_weight_grid(db);
	ok  &= weight_grid.size() == 5;
	//
	ok  &= weight_grid[0].weight_id == 0;
	ok  &= weight_grid[0].age       == 50.0;
	ok  &= weight_grid[0].time      == 2000.0;
	ok  &= weight_grid[0].weight    == 1.0; 
	//
	ok  &= weight_grid[1].weight_id == 1;
	ok  &= weight_grid[1].age       == 0.0;
	ok  &= weight_grid[1].time      == 1980.0;
	ok  &= weight_grid[1].weight    == 0.5; 
	//
	ok  &= weight_grid[2].weight_id == 1;
	ok  &= weight_grid[2].age       == 100.0;
	ok  &= weight_grid[2].time      == 1980.0;
	ok  &= weight_grid[2].weight    == 1.0; 
	//
	ok  &= weight_grid[3].weight_id == 1;
	ok  &= weight_grid[3].age       == 0.0;
	ok  &= weight_grid[3].time      == 2010.0;
	ok  &= weight_grid[3].weight    == 1.0; 
	//
	ok  &= weight_grid[4].weight_id == 1;
	ok  &= weight_grid[4].age       == 100.0;
	ok  &= weight_grid[4].time      == 2010.0;
	ok  &= weight_grid[4].weight    == 1.5; 
 	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
