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
$verbatim%example/devel/table/get_weight_grid_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/dismod_at.hpp>

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
	"create table weight("
		"weight_id   integer primary key,"
		"weight_name text,"
		"n_age       int,"
		"n_time      int)",
	"insert into weight values(0, 'constant',  1,  1)",
	"insert into weight values(1, 'bilinear',  2,  2)",
	//
	"create table weight_grid("
		" weight_grid_id integer primary key,"
		" weight_id      integer, "
		" age_id         integer,"
		" time_id        integer,"
		" weight         real)",
	//                 weight_grid_id, weight_id,age_id,time_id, weight
	"insert into weight_grid values(0,         0,     1,      1,    1.0)",
	"insert into weight_grid values(1,         1,     0,      0,    0.5)",
	"insert into weight_grid values(2,         1,     2,      0,    1.0)",
	"insert into weight_grid values(3,         1,     0,      2,    1.0)",
	"insert into weight_grid values(4,         1,     2,      2,    1.5)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the weight table
	vector<dismod_at::weight_struct> 
		weight_table = dismod_at::get_weight_table(db);
	ok  &= weight_table.size() == 2;
	//
	ok  &= weight_table[0].weight_name == "constant";
	ok  &= weight_table[0].n_age  == 1;
	ok  &= weight_table[0].n_time == 1;
	//
	ok  &= weight_table[1].weight_name == "bilinear";
	ok  &= weight_table[1].n_age  == 2;
	ok  &= weight_table[1].n_time == 2;
	//
	// get the weight_grid table
	vector<dismod_at::weight_grid_struct> 
		weight_grid_table = dismod_at::get_weight_grid(db);
	ok  &= weight_grid_table.size() == 5;
	//
	ok  &= weight_grid_table[0].weight_id == 0;
	ok  &= weight_grid_table[0].age_id    == 1;
	ok  &= weight_grid_table[0].time_id   == 1;
	ok  &= weight_grid_table[0].weight    == 1.0; 
	//
	ok  &= weight_grid_table[1].weight_id == 1;
	ok  &= weight_grid_table[1].age_id    == 0;
	ok  &= weight_grid_table[1].time_id   == 0;
	ok  &= weight_grid_table[1].weight    == 0.5; 
	//
	ok  &= weight_grid_table[2].weight_id == 1;
	ok  &= weight_grid_table[2].age_id    == 2;
	ok  &= weight_grid_table[2].time_id   == 0;
	ok  &= weight_grid_table[2].weight    == 1.0; 
	//
	ok  &= weight_grid_table[3].weight_id == 1;
	ok  &= weight_grid_table[3].age_id    == 0;
	ok  &= weight_grid_table[3].time_id   == 2;
	ok  &= weight_grid_table[3].weight    == 1.0; 
	//
	ok  &= weight_grid_table[4].weight_id == 1;
	ok  &= weight_grid_table[4].age_id    == 2;
	ok  &= weight_grid_table[4].time_id   == 2;
	ok  &= weight_grid_table[4].weight    == 1.5; 
 	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
