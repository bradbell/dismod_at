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
$begin get_multiplier_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_multiplier_table: Example and Test$$
$index example, C++ get_multiplier_table$$
$index get_multiplier_table, C++ example$$

$code
$verbatim%example/devel/table/get_multiplier_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/dismod_at.hpp>

bool get_multiplier_table_xam(void)
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
		"create table multiplier("
			" multiplier_id       integer primary key,"
			" multiplier_type    text,"
			" rate_id            integer,"
			" integrand_id       integer,"
			" covariate_id       integer,"
			" smooth_id          integer)",
		"insert into multiplier values(0, 'mean', -1,  2, 1, 2)",
		"insert into multiplier values(1, 'rate',  1, -1, 2, 2)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the multiplier table
	vector<dismod_at::multiplier_struct> 
		multiplier_table = dismod_at::get_multiplier_table(db);
	ok  &= multiplier_table.size() == 2;
	//
	ok  &= multiplier_table[0].multiplier_type  == "mean";
	ok  &= multiplier_table[1].multiplier_type  == "rate";
	//
	ok  &= multiplier_table[0].rate_id == -1;
	ok  &= multiplier_table[1].rate_id == 1;
	//
	ok  &= multiplier_table[0].integrand_id == 2;
	ok  &= multiplier_table[1].integrand_id == -1;
	//
	ok  &= multiplier_table[0].covariate_id == 1;
	ok  &= multiplier_table[1].covariate_id == 2;
	//
	ok  &= multiplier_table[0].smooth_id == 2;
	ok  &= multiplier_table[1].smooth_id == 2;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
