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
$begin variable_name_xam.cpp$$
$spell
	var
$$

$section C++ pack_info::variable_name: Example and Test$$
$index example, C++ pack_info$$
$index pack_info, C++ example$$

$code
$srcfile%example/devel/utility/variable_name_xam.cpp
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
		covariate_table, \
		integrand_table, \
		mulcov_table, \
		node_table, \
		smooth_table, \
		time_table, \
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
	"insert into age values(0,   0)",
	"insert into age values(1,  50)",
	"insert into age values(2, 100)",
	//
	"create table time(time_id integer primary key, time real)",
	"insert into time values(0, 1990)",
	"insert into time values(1, 2000)",
	"insert into time values(2, 2010)",
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
		" mulstd_value_prior_id   integer,"
		" mulstd_dage_prior_id    integer,"
		" mulstd_dtime_prior_id   integer)",
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
	"insert into smooth_grid values(0,  0,  0,  0,  1,    1,  1)",
	"insert into smooth_grid values(1,  0,  2,  0,  1, null,  1)",
	"insert into smooth_grid values(2,  0,  0,  2,  1,    1, null)",
	"insert into smooth_grid values(3,  0,  2,  2,  1, null, null)",
	"insert into smooth_grid values(4,  1,  1,  1,  1, null, null)",
	//
	"create table covariate("
		" covariate_id   integer primary key,"
		" covariate_name text,"
		" reference      real,"
		" max_difference real)",
	"insert into covariate values(0, 'sex', '0.0', '0.6')",
	//
	"create table integrand("
		" integrand_id   integer primary key,"
		" integrand_name text,"
		" eta            real)",
	"insert into integrand values(0, 'prevalence', 1e-6)",
	"insert into integrand values(1, 'Sincidence', 1e-6)",
	//
	"create table mulcov("
		" mulcov_id      integer primary key,"
		" mulcov_type    text,"
		" rate_id        integer,"
		" integrand_id   integer,"
		" covariate_id   integer,"
		" smooth_id      integer)",
	"insert into mulcov values(0,  'meas_value',  -1,  0, 0, 0)",
	"insert into mulcov values(1,  'meas_std',   -1,  1, 0, 1)",
	"insert into mulcov values(2,  'rate_value',   1, -1, 0, 0)",
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
	// covariate_table
	vector<dismod_at::covariate_struct>
		covariate_table = dismod_at::get_covariate_table(db);
	// integrand table
	vector<dismod_at::integrand_struct>
		integrand_table = dismod_at::get_integrand_table(db);
	// mulcov table
	vector<dismod_at::mulcov_struct>
		mulcov_table = dismod_at::get_mulcov_table(db);
	//
	// s_info_vec
	vector<dismod_at::smooth_info> s_info_vec(2);
	size_t smooth_id = 0;
	s_info_vec[smooth_id] = dismod_at::smooth_info(
		db, age_table, time_table, smooth_id, smooth_table, smooth_grid_table
	);
	smooth_id = 1;
	s_info_vec[smooth_id] = dismod_at::smooth_info(
		db, age_table, time_table, smooth_id, smooth_table, smooth_grid_table
	);
	// empty data table
	vector<dismod_at::data_struct>     data_table;
	// child_object
	size_t parent_node_id = 0;
	size_t n_child        = 2;
	dismod_at::child_info child_object(
		parent_node_id, node_table, data_table
	);
	// pack_object
	size_t n_integrand = integrand_table.size();
	dismod_at::pack_info pack_object(
		db, n_integrand, n_child,
		smooth_table, mulcov_table, rate_table
	);
	// ------------------------------------------------------------------
	// check mulstd
	string name;
	smooth_id = 0;
	size_t offset    = pack_object.mulstd_offset(smooth_id, 0);
	name   = VARIABLE_NAME(offset);
	ok    &= name == "value_mulstd(bilinear)";
	offset = pack_object.mulstd_offset(smooth_id, 1);
	name   = VARIABLE_NAME(offset);
	ok    &= name == "dage_mulstd(bilinear)";
	offset = pack_object.mulstd_offset(smooth_id, 2);
	name   = VARIABLE_NAME(offset);
	ok    &= name == "dtime_mulstd(bilinear)";
	smooth_id = 1;
	offset = pack_object.mulstd_offset(smooth_id, 0);
	name   = VARIABLE_NAME(offset);
	offset = pack_object.mulstd_offset(smooth_id, 1);
	ok    &= name == "value_mulstd(constant)";
	name   = VARIABLE_NAME(offset);
	offset = pack_object.mulstd_offset(smooth_id, 2);
	ok    &= name == "dage_mulstd(constant)";
	name   = VARIABLE_NAME(offset);
	ok    &= name == "dtime_mulstd(constant)";
	//
	// check pini
	size_t n_var;
	dismod_at::pack_info::subvec_info info;
	for(size_t child_id = 0; child_id <= n_child; child_id++)
	{	info    = pack_object.rate_info(dismod_at::pini_enum, child_id);
		n_var  = info.n_var;
		offset = info.offset;
		ok    &= n_var == 1;
		name   = VARIABLE_NAME(offset);
		switch( child_id )
		{	case 0:
			ok &= name == "pini(north_america;50;2000)"; // first child
			break;
			case 1:
			ok &= name == "pini(south_america;50;2000)"; // second child
			break;
			case 2:
			ok &= name == "pini(world;50;2000)";         // parent
			break;
		}
	}
	//
	// check omega
	for(size_t child_id = 0; child_id <= n_child; child_id++)
	{	info    = pack_object.rate_info(dismod_at::omega_enum, child_id);
		n_var  = info.n_var;
		offset = info.offset;
		name   = VARIABLE_NAME(offset);
		switch( child_id )
		{	case 0:
			ok &= n_var == 1;
			ok &= name == "omega(north_america;50;2000)"; // first child
			break;
			case 1:
			ok &= n_var == 1;
			ok &= name == "omega(south_america;50;2000)"; // second child
			break;
			case 2:
			ok &= n_var == 4;
			ok &= name == "omega(world;0;1990)";        // parent (1 of 4)
			break;
		}
	}
	//
	// mulcov_meas_value
	size_t integrand_id = 0;
	size_t n_cov = pack_object.mulcov_meas_value_n_cov(integrand_id);
	assert( n_cov == 1 );
	info   = pack_object.mulcov_meas_value_info(integrand_id, 0);
	n_var  = info.n_var;
	offset = info.offset;
	ok    &= n_var == 4;
	name   = VARIABLE_NAME(offset + 0);
	ok    &= name == "mean_mulcov(sex;prevalence;0;1990)";
	name   = VARIABLE_NAME(offset + 1);
	ok    &= name == "mean_mulcov(sex;prevalence;100;1990)";
	name   = VARIABLE_NAME(offset + 2);
	ok    &= name == "mean_mulcov(sex;prevalence;0;2010)";
	name   = VARIABLE_NAME(offset + 3);
	ok    &= name == "mean_mulcov(sex;prevalence;100;2010)";
	//
	// mulcov_meas_std
	integrand_id = 1;
	n_cov        = pack_object.mulcov_meas_std_n_cov(integrand_id);
	assert( n_cov == 1 );
	info   = pack_object.mulcov_meas_std_info(integrand_id, 0);
	n_var  = info.n_var;
	offset = info.offset;
	ok    &= n_var == 1;
	name   = VARIABLE_NAME(offset + 0);
	ok    &= name == "std_mulcov(sex;Sincidence;50;2000)";
	//
	// mulcov_rate_value
	size_t rate_id = 1;
	n_cov = pack_object.mulcov_rate_value_n_cov(rate_id);
	assert( n_cov == 1 );
	info   = pack_object.mulcov_rate_value_info(rate_id, 0);
	n_var  = info.n_var;
	offset = info.offset;
	ok    &= n_var == 4;
	name   = VARIABLE_NAME(offset + 0);
	ok    &= name == "mean_mulcov(sex;iota;0;1990)";
	name   = VARIABLE_NAME(offset + 1);
	ok    &= name == "mean_mulcov(sex;iota;100;1990)";
	name   = VARIABLE_NAME(offset + 2);
	ok    &= name == "mean_mulcov(sex;iota;0;2010)";
	name   = VARIABLE_NAME(offset + 3);
	ok    &= name == "mean_mulcov(sex;iota;100;2010)";

	return ok;
}
// END C++
