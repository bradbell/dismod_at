// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_mulcov_table_xam.cpp$$
$spell
	mulcov
	xam
$$

$section C++ get_mulcov_table: Example and Test$$
$index example, C++ get_mulcov_table$$
$index get_mulcov_table, C++ example$$

$code
$verbatim%example/devel/table/get_mulcov_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/get_mulcov_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_mulcov_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	// assume that density_id for uniform density is 0.
	const char* sql_cmd[] = {
		"create table mulcov("
			" mulcov_id          integer primary key,"
			" mulcov_type        text,"
			" rate_id            integer,"
			" integrand_id       integer,"
			" covariate_id       integer,"
			" smooth_id          integer)",
		"insert into mulcov values(0, 'meas_mean', -1,  2, 1, 2)",
		"insert into mulcov values(1, 'rate_mean',  1, -1, 2, 2)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the mulcov table
	vector<dismod_at::mulcov_struct>
		mulcov_table = dismod_at::get_mulcov_table(db);
	ok  &= mulcov_table.size() == 2;
	//
	ok  &= mulcov_table[0].mulcov_type  == dismod_at::meas_mean_enum;
	ok  &= mulcov_table[1].mulcov_type  == dismod_at::rate_mean_enum;
	//
	ok  &= mulcov_table[0].rate_id == -1;
	ok  &= mulcov_table[1].rate_id == 1;
	//
	ok  &= mulcov_table[0].integrand_id == 2;
	ok  &= mulcov_table[1].integrand_id == -1;
	//
	ok  &= mulcov_table[0].covariate_id == 1;
	ok  &= mulcov_table[1].covariate_id == 2;
	//
	ok  &= mulcov_table[0].smooth_id == 2;
	ok  &= mulcov_table[1].smooth_id == 2;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
