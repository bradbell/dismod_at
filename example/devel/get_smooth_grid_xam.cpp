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
$begin get_smooth_grid_xam.cpp$$
$spell
	xam
$$

$section C++ get_smooth_grid: Example and Test$$
$index example, C++ get_smooth_grid$$
$index get_smooth_grid, C++ example$$

$code
$verbatim%example/devel/get_smooth_grid_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/dismod_at.hpp>

bool get_smooth_grid_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;	

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = { 
	"create table smooth("
		"smooth_id   integer primary key,"
		"smooth_name text,"
		"n_age       int,"
		"n_time      int)",
	"insert into smooth values(0, 'constant',  1, 1)",
	"insert into smooth values(1, 'age_only',  2, 1)",
	"insert into smooth values(2, 'time_only', 1, 2)",
	"insert into smooth values(3, 'age_time',  2, 2)",
	//
	"create table smooth_grid("
		" smooth_grid_id integer primary key,"
		" smooth_id      integer, "
		" age_id         integer,"
		" time_id        integer,"
		" value_like_id  integer,"
		" dage_like_id   integer,"
		" dtime_like_id  integer)",
	//                 smooth_grid_id, smooth_id,age_id,time_id,   like_ids
	"insert into smooth_grid values(0,         0,     1,      1,   1, 2, 3)",
	"insert into smooth_grid values(1,         3,     0,      0,   1, 2, 3)",
	"insert into smooth_grid values(2,         3,     2,      0,   1, 2, 3)",
	"insert into smooth_grid values(3,         3,     0,      2,   1, 2, 3)",
	"insert into smooth_grid values(4,         3,     2,      2,   1, 2, 3)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the smooth table
	vector<dismod_at::smooth_struct> 
		smooth_table = dismod_at::get_smooth_table(db);
	ok  &= smooth_table.size() == 4;
	ok  &= smooth_table[0].smooth_name == "constant";
	ok  &= smooth_table[0].n_age       == 1;
	ok  &= smooth_table[0].n_time      == 1;
	//
	ok  &= smooth_table[1].smooth_name == "age_only";
	ok  &= smooth_table[1].n_age       == 2;
	ok  &= smooth_table[1].n_time      == 1;
	//
	ok  &= smooth_table[2].smooth_name == "time_only";
	ok  &= smooth_table[2].n_age       == 1;
	ok  &= smooth_table[2].n_time      == 2;
	//
	ok  &= smooth_table[3].smooth_name == "age_time";
	ok  &= smooth_table[3].n_age       == 2;
	ok  &= smooth_table[3].n_time      == 2;
	//
	// get the smooth_grid table
	vector<dismod_at::smooth_grid_struct> 
		smooth_grid = dismod_at::get_smooth_grid(db);
	ok  &= smooth_grid.size() == 5;
	//
	ok  &= smooth_grid[0].smooth_id     == 0;
	ok  &= smooth_grid[0].age_id        == 1;
	ok  &= smooth_grid[0].time_id       == 1;
	ok  &= smooth_grid[0].value_like_id == 1;
	ok  &= smooth_grid[0].dage_like_id  == 2;
	ok  &= smooth_grid[0].dtime_like_id == 3;
	//
	ok  &= smooth_grid[1].smooth_id     == 3;
	ok  &= smooth_grid[1].age_id        == 0;
	ok  &= smooth_grid[1].time_id       == 0;
	ok  &= smooth_grid[1].value_like_id == 1;
	ok  &= smooth_grid[1].dage_like_id  == 2;
	ok  &= smooth_grid[1].dtime_like_id == 3;
	//
	ok  &= smooth_grid[2].smooth_id     == 3;
	ok  &= smooth_grid[2].age_id        == 2;
	ok  &= smooth_grid[2].time_id       == 0;
	ok  &= smooth_grid[2].value_like_id == 1;
	ok  &= smooth_grid[2].dage_like_id  == 2;
	ok  &= smooth_grid[2].dtime_like_id == 3;
	//
	ok  &= smooth_grid[3].smooth_id     == 3;
	ok  &= smooth_grid[3].age_id        == 0;
	ok  &= smooth_grid[3].time_id       == 2;
	ok  &= smooth_grid[3].value_like_id == 1;
	ok  &= smooth_grid[3].dage_like_id  == 2;
	ok  &= smooth_grid[3].dtime_like_id == 3;
	//
	ok  &= smooth_grid[4].smooth_id     == 3;
	ok  &= smooth_grid[4].age_id        == 2;
	ok  &= smooth_grid[4].time_id       == 2;
	ok  &= smooth_grid[4].value_like_id == 1;
	ok  &= smooth_grid[4].dage_like_id  == 2;
	ok  &= smooth_grid[4].dtime_like_id == 3;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
