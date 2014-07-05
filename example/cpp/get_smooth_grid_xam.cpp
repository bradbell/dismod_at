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
$verbatim%example/cpp/get_smooth_grid_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <sqlite3.h>
# include <dismod_at/get_smooth_grid.hpp>
# include <dismod_at/get_smooth_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

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
	"create table smooth(smooth_id integer primary key, smooth_name text)",
	"insert into smooth values(0, 'constant')",
	"insert into smooth values(1, 'age_only')",
	"insert into smooth values(2, 'time_only')",
	"insert into smooth values(3, 'age_time')",
	//
	"create table smooth_grid("
		" smooth_grid_id integer primary key,"
		" smooth_id      integer, "
		" age            real,"
		" time           real,"
		" value_like_id  integer,"
		" dage_like_id   integer,"
		" dtime_like_id  integer)",
	//                 smooth_grid_id, smooth_id,   age,   time,   like_ids
	"insert into smooth_grid values(0,         0,  50.0, 2000.0,   1, 2, 3)",
	"insert into smooth_grid values(1,         3,   0.0, 1980.0,   1, 2, 3)",
	"insert into smooth_grid values(2,         3, 100.0, 1980.0,   1, 2, 3)",
	"insert into smooth_grid values(3,         3,   0.0, 2010.0,   1, 2, 3)",
	"insert into smooth_grid values(4,         3, 100.0, 2010.0,   1, 2, 3)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the smooth table
	vector<string> smooth_table = dismod_at::get_smooth_table(db);
	ok  &= smooth_table.size() == 4;
	ok  &= smooth_table[0] == "constant";
	ok  &= smooth_table[1] == "age_only";
	ok  &= smooth_table[2] == "time_only";
	ok  &= smooth_table[3] == "age_time";

	// get the smooth_grid table
	vector<dismod_at::smooth_grid_struct> 
		smooth_grid = dismod_at::get_smooth_grid(db);
	ok  &= smooth_grid.size() == 5;
	//
	ok  &= smooth_grid[0].smooth_id     == 0;
	ok  &= smooth_grid[0].age           == 50.0;
	ok  &= smooth_grid[0].time          == 2000.0;
	ok  &= smooth_grid[0].value_like_id == 1;
	ok  &= smooth_grid[0].dage_like_id  == 2;
	ok  &= smooth_grid[0].dtime_like_id == 3;
	//
	ok  &= smooth_grid[1].smooth_id     == 3;
	ok  &= smooth_grid[1].age           == 0.0;
	ok  &= smooth_grid[1].time          == 1980.0;
	ok  &= smooth_grid[1].value_like_id == 1;
	ok  &= smooth_grid[1].dage_like_id  == 2;
	ok  &= smooth_grid[1].dtime_like_id == 3;
	//
	ok  &= smooth_grid[2].smooth_id     == 3;
	ok  &= smooth_grid[2].age           == 100.0;
	ok  &= smooth_grid[2].time          == 1980.0;
	ok  &= smooth_grid[2].value_like_id == 1;
	ok  &= smooth_grid[2].dage_like_id  == 2;
	ok  &= smooth_grid[2].dtime_like_id == 3;
	//
	ok  &= smooth_grid[3].smooth_id     == 3;
	ok  &= smooth_grid[3].age           == 0.0;
	ok  &= smooth_grid[3].time          == 2010.0;
	ok  &= smooth_grid[3].value_like_id == 1;
	ok  &= smooth_grid[3].dage_like_id  == 2;
	ok  &= smooth_grid[3].dtime_like_id == 3;
	//
	ok  &= smooth_grid[4].smooth_id     == 3;
	ok  &= smooth_grid[4].age           == 100.0;
	ok  &= smooth_grid[4].time          == 2010.0;
	ok  &= smooth_grid[4].value_like_id == 1;
	ok  &= smooth_grid[4].dage_like_id  == 2;
	ok  &= smooth_grid[4].dtime_like_id == 3;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
