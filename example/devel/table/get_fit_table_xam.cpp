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
$begin get_fit_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_fit_table: Example and Test$$
$index example, C++ get_fit_table$$
$index get_fit_table, C++ example$$

$code
$verbatim%example/devel/table/get_fit_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/get_fit_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_fit_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
	"create table fit("
		"fit_id              integer primary key,"
		" parent_node_id     integer,"
		" n_age_ode          integer,"
		" n_time_ode         integer,"
		" ode_step_size      real,"
		" tolerance          real,"
		" max_num_iter       integer"
	")",
	"insert into fit values(0, 4, 3, 5, 0.5,  1e-8, 500)",
	"insert into fit values(1, 5, 3, 5, 0.25, 1e-8, 500)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the fit table
	vector<dismod_at::fit_struct>
		fit_table = dismod_at::get_fit_table(db);
	ok  &= fit_table.size() == 2;
	//
	ok  &= fit_table[0].parent_node_id     == 4;
	ok  &= fit_table[0].n_age_ode          == 3;
	ok  &= fit_table[0].n_time_ode         == 5;
	ok  &= fit_table[0].ode_step_size      == 0.5;
	ok  &= fit_table[0].tolerance          == 1e-8;
	ok  &= fit_table[0].max_num_iter       == 500;
	//
	ok  &= fit_table[1].parent_node_id     == 5;
	ok  &= fit_table[1].n_age_ode          == 3;
	ok  &= fit_table[1].n_time_ode         == 5;
	ok  &= fit_table[1].ode_step_size      == 0.25;
	ok  &= fit_table[1].tolerance          == 1e-8;
	ok  &= fit_table[1].max_num_iter       == 500;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
