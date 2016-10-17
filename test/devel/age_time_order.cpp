// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
Check cases where age and time grids are not in order.
*/
# include <dismod_at/smooth_info.hpp>
# include <dismod_at/weight_info.hpp>
# include <dismod_at/get_weight_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/get_age_table.hpp>
# include <dismod_at/get_time_table.hpp>
# include <dismod_at/null_int.hpp>

namespace { // BEGIN EMPTY_NAMESPACE

// put bool on its own line so that test_one.sh works correctly
bool
test_smooth_info(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
	"create table age(age_id integer primary key, age real)",
	"insert into age values(0,  00.0)",
	"insert into age values(1,  25.0)",
	"insert into age values(2, 100.0)",
	"insert into age values(3,  50.0)",
	"insert into age values(4,  75.0)",
	//
	"create table time(time_id integer primary key, time real)",
	"insert into time values(0,  2010.0)",
	"insert into time values(1,  1990.0)",
	//
	"create table smooth("
		" smooth_id      integer primary key,"
		" smooth_name    text,   "
		" n_age          integer,"
		" n_time         integer,"
		" mulstd_value_prior_id   integer,"
		" mulstd_dage_prior_id    integer,"
		" mulstd_dtime_prior_id   integer)",
	"insert into smooth values(0, 'bilinear', 3, 2, 1, 1, 1)",
	//
	"create table smooth_grid("
		" smooth_grid_id integer primary key,"
		" smooth_id      integer,"
		" age_id         integer,"
		" time_id        integer,"
		" value_prior_id integer,"
		" dage_prior_id  integer,"
		" dtime_prior_id integer)",
	"insert into smooth_grid values(0,  0,  0,  0,  1,    2, null)",
	"insert into smooth_grid values(2,  0,  2,  0,  3, null, null)",
	"insert into smooth_grid values(1,  0,  3,  0,  2,    3, null)",
	"insert into smooth_grid values(3,  0,  0,  1,  5,    6,    7)",
	"insert into smooth_grid values(5,  0,  2,  1,  1, null,    3)",
	"insert into smooth_grid values(4,  0,  3,  1,  7,    8,    9)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get age table
	vector<double> age_table = dismod_at::get_age_table(db);

	// get time table
	vector<double> time_table = dismod_at::get_time_table(db);

	// get the smooth table
	vector<dismod_at::smooth_struct>
		smooth_table = dismod_at::get_smooth_table(db);

	// get the smooth_grid table
	vector<dismod_at::smooth_grid_struct>
		smooth_grid_table = dismod_at::get_smooth_grid(db);

	// extract the smoothing information
	size_t smooth_id = 0;
	dismod_at::smooth_info  s_info(
		age_table, time_table, smooth_id, smooth_table, smooth_grid_table
	);

	ok  &= s_info.age_size()  == 3;
	ok  &= s_info.time_size() == 2;
	//
	ok  &= age_table[ s_info.age_id(0) ]   == 0.0;
	ok  &= age_table[ s_info.age_id(1) ]   == 50.0;
	ok  &= age_table[ s_info.age_id(2) ]   == 100.0;
	//
	ok  &= time_table[ s_info.time_id(0)]  == 1990.0;
	ok  &= time_table[ s_info.time_id(1)]  == 2010.0;
	//
	ok  &= s_info.value_prior_id(0, 1) ==  1;
	ok  &= s_info.value_prior_id(1, 1) ==  2;
	ok  &= s_info.value_prior_id(2, 1) ==  3;
	ok  &= s_info.value_prior_id(0, 0) ==  5;
	ok  &= s_info.value_prior_id(1, 0) ==  7;
	ok  &= s_info.value_prior_id(2, 0) ==  1;
	//
	ok  &= s_info.dage_prior_id(0, 1)  ==  2;
	ok  &= s_info.dage_prior_id(1, 1)  ==  3;
	ok  &= s_info.dage_prior_id(2, 1)  == DISMOD_AT_NULL_SIZE_T;
	ok  &= s_info.dage_prior_id(0, 0)  ==  6;
	ok  &= s_info.dage_prior_id(1, 0)  ==  8;
	ok  &= s_info.dage_prior_id(2, 0)  == DISMOD_AT_NULL_SIZE_T;
	//
	ok  &= s_info.dtime_prior_id(0, 1) == DISMOD_AT_NULL_SIZE_T;
	ok  &= s_info.dtime_prior_id(1, 1) == DISMOD_AT_NULL_SIZE_T;
	ok  &= s_info.dtime_prior_id(2, 1) == DISMOD_AT_NULL_SIZE_T;
	ok  &= s_info.dtime_prior_id(0, 0) ==  7;
	ok  &= s_info.dtime_prior_id(1, 0) ==  9;
	ok  &= s_info.dtime_prior_id(2, 0) ==  3;
	//
	ok  &= s_info.mulstd_value()      == 1;
	ok  &= s_info.mulstd_dage()       == 1;
	ok  &= s_info.mulstd_dtime()      == 1;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// put bool on its own line so that test_one.sh works correctly
bool
test_weight_info(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
	"create table age(age_id integer primary key, age real)",
	"insert into age values(0,  00.0)",
	"insert into age values(1,  25.0)",
	"insert into age values(2, 100.0)",
	"insert into age values(3,  50.0)",
	"insert into age values(4,  75.0)",
	//
	"create table time(time_id integer primary key, time real)",
	"insert into time values(0,  2010.0)",
	"insert into time values(1,  1990.0)",
	//
	"create table weight("
		" weight_id      integer primary key,"
		" weight_name    text,   "
		" n_age          integer,"
		" n_time         integer)",
	"insert into weight values(0, 'bilinear', 3, 2)",
	//
	"create table weight_grid("
		" weight_grid_id integer primary key,"
		" weight_id      integer,"
		" age_id         integer,"
		" time_id        integer,"
		" weight         real)",
	"insert into weight_grid values(0,  0,  0,  0, 1.0)", // (0,   2010)
	"insert into weight_grid values(2,  0,  2,  0, 2.0)", // (100, 2010)
	"insert into weight_grid values(1,  0,  3,  0, 3.0)", // (50,  2010)
	"insert into weight_grid values(3,  0,  0,  1, 4.0)", // (0,   1990)
	"insert into weight_grid values(5,  0,  2,  1, 5.0)", // (100, 1990)
	"insert into weight_grid values(4,  0,  3,  1, 6.0)", // (50,  1990)
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get age table
	vector<double> age_table = dismod_at::get_age_table(db);

	// get time table
	vector<double> time_table = dismod_at::get_time_table(db);

	// get the weight table
	vector<dismod_at::weight_struct>
		weight_table = dismod_at::get_weight_table(db);

	// get the weight_grid table
	vector<dismod_at::weight_grid_struct>
		weight_grid_table = dismod_at::get_weight_grid(db);

	// extract the weighting information
	size_t weight_id = 0;
	dismod_at::weight_info  w_info(
		age_table, time_table, weight_id, weight_table, weight_grid_table
	);

	ok  &= w_info.age_size()  == 3;
	ok  &= w_info.time_size() == 2;
	//
	ok  &= age_table[ w_info.age_id(0) ]   == 0.0;
	ok  &= age_table[ w_info.age_id(1) ]   == 50.0;
	ok  &= age_table[ w_info.age_id(2) ]   == 100.0;
	//
	ok  &= time_table[ w_info.time_id(0)]  == 1990.0;
	ok  &= time_table[ w_info.time_id(1)]  == 2010.0;
	//
	ok  &= w_info.weight(0, 0) == 4.0;
	ok  &= w_info.weight(1, 0) == 6.0;
	ok  &= w_info.weight(2, 0) == 5.0;
	//
	ok  &= w_info.weight(0, 1) == 1.0;
	ok  &= w_info.weight(1, 1) == 3.0;
	ok  &= w_info.weight(2, 1) == 2.0;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}

} // END EMPTY_NAMESPACE

bool age_time_order(void)
{	bool ok = true;
	ok &= test_smooth_info();
	ok &= test_weight_info();
	return ok;
}
