// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin variable_name_xam.cpp$$
$spell
	var
$$

$section C++ pack_info::variable_name: Example and Test$$
$index example, C++ pack_info$$
$index pack_info, C++ example$$

$code
$verbatim%example/devel/utility/variable_name_xam.cpp
%0%// BEGIN C++%// END C++%1%$$
$$
$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/get_age_table.hpp>
# include <dismod_at/get_time_table.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_smooth_table.hpp>
# include <dismod_at/get_smooth_grid.hpp>
# include <dismod_at/smooth_info.hpp>
# include <dismod_at/child_info.hpp>

# define VARIABLE_NAME(index) \
	pack_object.variable_name( \
		index, \
		parent_node_id, \
		age_table, \
		time_table, \
		node_table, \
		smooth_table, \
		s_info_vec, \
		child_object )

bool variable_name_xam(void)
{	bool ok = true;
	using CppAD::vector;
	using std::string;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
	"create table age(age_id integer primary key, age real)",
	"insert into age values(0,   0.0)",
	"insert into age values(1,  50.0)",
	"insert into age values(2, 100.0)",
	//
	"create table time(time_id integer primary key, time real)",
	"insert into time values(0, 1900.0)",
	"insert into time values(1, 2000.0)",
	"insert into time values(2, 2100.0)",
	//
	"create table rate("
		" rate_id           integer primary key,"
		" rate_name         text,"
		" parent_smooth_id  integer,"
		" child_smooth_id   integer)",
	//
	"insert into rate values(0, 'pini',  1, 1)",
	"insert into rate values(1, 'iota',  0, 1)",
	"insert into rate values(2, 'rho',   0, 1)",
	"insert into rate values(3, 'chi',   0, 1)",
	"insert into rate values(4, 'omega', 0, 1)",
	//
	"create table node("
		" node_id        integer primary key,"
		" node_name      text,   "
		" parent         integer)",
	"insert into node values(0, 'world', -1)",
	"insert into node values(1, 'north_america', 0)",
	"insert into node values(2, 'south_america', 0)",
	//
	"create table smooth("
		" smooth_id      integer primary key,"
		" smooth_name    text,   "
		" n_age          integer,"
		" n_time         integer,"
		" mulstd_value   integer,"
		" mulstd_dage    integer,"
		" mulstd_dtime   integer)",
	"insert into smooth values(0, 'bilinear', 2, 2, 1, 1, 1)",
	"insert into smooth values(1, 'constant', 1, 1, 1, 1, 1)",
	//
	"create table smooth_grid("
		" smooth_grid_id integer primary key,"
		" smooth_id      integer,"
		" age_id         integer,"
		" time_id        integer,"
		" value_prior_id integer,"
		" dage_prior_id  integer,"
		" dtime_prior_id integer)",
	"insert into smooth_grid values(0,  0,  0,  0,  1,  1,  1)",
	"insert into smooth_grid values(1,  0,  2,  0,  1, -1,  1)",
	"insert into smooth_grid values(2,  0,  0,  2,  1,  1, -1)",
	"insert into smooth_grid values(3,  0,  2,  2,  1, -1, -1)",
	"insert into smooth_grid values(4,  1,  1,  1,  1, -1, -1)",
	//
	"create table mulcov("
		" mulcov_id      integer primary key,"
		" mulcov_type    text,"
		" rate_id        integer,"
		" integrand_id   integer,"
		" covariate_id   integer,"
		" smooth_id      integer)",
	"insert into mulcov values(0,  'rate_mean',  0,  -1, 0,  0)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);
	// age table
	vector<double> age_table = dismod_at::get_age_table(db);
	// time table
	vector<double> time_table = dismod_at::get_time_table(db);
	// rate table
	vector<dismod_at::rate_struct>
		rate_table = dismod_at::get_rate_table(db);
	// node table
	vector<dismod_at::node_struct>
		node_table = dismod_at::get_node_table(db);
	// smooth table
	vector<dismod_at::smooth_struct>
		smooth_table = dismod_at::get_smooth_table(db);
	// smooth_grid table
	vector<dismod_at::smooth_grid_struct>
		smooth_grid_table = dismod_at::get_smooth_grid(db);
	// mulcov table
	vector<dismod_at::mulcov_struct>
		mulcov_table = dismod_at::get_mulcov_table(db);
	//
	// s_info_vec
	vector<dismod_at::smooth_info> s_info_vec(2);
	s_info_vec[0] = dismod_at::smooth_info(0, smooth_table, smooth_grid_table);
	s_info_vec[1] = dismod_at::smooth_info(1, smooth_table, smooth_grid_table);
	// empty data table
	vector<dismod_at::data_struct> data_table(0);
	// child_object
	size_t parent_node_id = 0;
	size_t n_child        = 2;
	dismod_at::child_info child_object(parent_node_id, node_table, data_table);
	// pack_object
	size_t n_integrand = 1;
	dismod_at::pack_info pack_object(
		n_integrand, n_child,
		smooth_table, mulcov_table, rate_table
	);
	// ------------------------------------------------------------------
	// check mulstd
	size_t smooth_id = 0;
	size_t offset    = pack_object.mulstd_offset(smooth_id);
	string name      = VARIABLE_NAME(offset + 0);
	ok              &= name == "mulstd/bilinear/value";

	return ok;
}
// END C++
