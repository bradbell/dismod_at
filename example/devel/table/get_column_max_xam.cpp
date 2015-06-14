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
$begin get_column_max_xam.cpp$$
$spell
	xam
$$

$section C++ get_column_max: Example and Test$$

$code
$verbatim%example/devel/table/get_column_max_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/get_column_max.hpp>

bool get_column_max_xam(void)
{
	bool   ok = true;
	using std::string;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// create the fit table
	const char* sql_cmd[] = {
	"create table fit("
		"fit_id              integer primary key,"
		" parent_node_id     integer,"
		" ode_step_size      real,"
		" tolerance          real,"
		" max_num_iter       integer"
	")",
	"insert into fit values(0, 4, 0.5,  1e-8, 500)",
	"insert into fit values(1, 5, 0.25, 1e-8, 400)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// check fit_id
	string table_name  = "fit";
	string column_name = "fit_id";
	string max_str     = dismod_at::get_column_max(
		db, table_name, column_name
	);
	ok              &= std::atoi(max_str.c_str()) == 1;

	// check parent_node_id
	column_name = "parent_node_id";
	max_str     = dismod_at::get_column_max(db, table_name, column_name);
	ok          &= std::atoi(max_str.c_str()) == 5;

	// check max_num_iter
	column_name = "max_num_iter";
	max_str     = dismod_at::get_column_max(db, table_name, column_name);
	ok          &= std::atoi(max_str.c_str()) == 500;

	// check ode_step_size
	column_name = "ode_step_size";
	max_str     = dismod_at::get_column_max(db, table_name, column_name);
	ok          &= std::atof(max_str.c_str()) == 0.5;

	// check tolerance
	column_name = "tolerance";
	max_str     = dismod_at::get_column_max(db, table_name, column_name);
	ok          &= std::atof(max_str.c_str()) == 1e-8;

	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
