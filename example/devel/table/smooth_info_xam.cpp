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
$begin smooth_info_xam.cpp$$
$spell
	xam
$$

$section C++ smooth_info: Example and Test$$
$index example, C++ smooth_info$$
$index smooth_info, C++ example$$

$code
$verbatim%example/devel/table/smooth_info_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/include/smooth_info.hpp>
# include <dismod_at/include/exec_sql_cmd.hpp>
# include <dismod_at/include/open_connection.hpp>

bool smooth_info_xam(void)
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
		" smooth_id      integer primary key,"
		" smooth_name    text,   "
		" n_age          integer,"
		" n_time         integer,"
		" mulstd_value   integer,"
		" mulstd_dage    integer,"
		" mulstd_dtime   integer)",
	"insert into smooth values(0, 'bilinear', 3, 2, 1, 1, 1)",

	"create table smooth_grid("
		" smooth_grid_id integer primary key,"
		" smooth_id      integer,"
		" age_id         integer,"
		" time_id        integer,"
		" value_like_id  integer,"
		" dage_like_id   integer,"
		" dtime_like_id  integer)",
	"insert into smooth_grid values(0,  0,  0,  1,  1,  2, -1)",
	"insert into smooth_grid values(1,  0,  2,  1,  2,  3, -1)",
	"insert into smooth_grid values(2,  0,  4,  1,  3, -1, -1)",
	"insert into smooth_grid values(3,  0,  0,  0,  5,  6,  7)",
	"insert into smooth_grid values(4,  0,  2,  0,  7,  8,  9)",
	"insert into smooth_grid values(5,  0,  4,  0,  1, -1,  3)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the smooth table
	vector<dismod_at::smooth_struct> 
		smooth_table = dismod_at::get_smooth_table(db);

	// get the smooth_grid table
	vector<dismod_at::smooth_grid_struct> 
		smooth_grid_table = dismod_at::get_smooth_grid(db);

	// extract the smoothing information
	size_t smooth_id = 0;
	dismod_at::smooth_info  s_info(smooth_id, smooth_table, smooth_grid_table);

	ok  &= s_info.age_size()  == 3;
	ok  &= s_info.time_size() == 2;
	//
	ok  &= s_info.age_id(0)   == 0;
	ok  &= s_info.age_id(1)   == 2;
	ok  &= s_info.age_id(2)   == 4;
	//
	ok  &= s_info.time_id(0)  == 0;
	ok  &= s_info.time_id(1)  == 1;
	//
	ok  &= s_info.value_like_id(0, 1) ==  1;
	ok  &= s_info.value_like_id(1, 1) ==  2;
	ok  &= s_info.value_like_id(2, 1) ==  3;
	ok  &= s_info.value_like_id(0, 0) ==  5;
	ok  &= s_info.value_like_id(1, 0) ==  7;
	ok  &= s_info.value_like_id(2, 0) ==  1;
	//
	ok  &= s_info.dage_like_id(0, 1)  ==  2;
	ok  &= s_info.dage_like_id(1, 1)  ==  3;
	ok  &= s_info.dage_like_id(2, 1)  == size_t(-1);
	ok  &= s_info.dage_like_id(0, 0)  ==  6;
	ok  &= s_info.dage_like_id(1, 0)  ==  8;
	ok  &= s_info.dage_like_id(2, 0)  == size_t(-1);
	//
	ok  &= s_info.dtime_like_id(0, 1) == size_t(-1);
	ok  &= s_info.dtime_like_id(1, 1) == size_t(-1);
	ok  &= s_info.dtime_like_id(2, 1) == size_t(-1);
	ok  &= s_info.dtime_like_id(0, 0) ==  7;
	ok  &= s_info.dtime_like_id(1, 0) ==  9;
	ok  &= s_info.dtime_like_id(2, 0) ==  3;
	//
	ok  &= s_info.mulstd_value()      == 1;
	ok  &= s_info.mulstd_dage()       == 1;
	ok  &= s_info.mulstd_dtime()      == 1;
 	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
