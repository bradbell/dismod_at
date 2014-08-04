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
$begin get_run_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_run_table: Example and Test$$
$index example, C++ get_run_table$$
$index get_run_table, C++ example$$

$code
$verbatim%example/devel/table/get_run_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/include/get_run_table.hpp>
# include <dismod_at/include/exec_sql_cmd.hpp>
# include <dismod_at/include/open_connection.hpp>

bool get_run_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;	

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = { 
	"create table run"
	"(run_id                 integer primary key,"
		" parent_node_id     integer,"
		" pini_smooth_id     integer,"
		" ode_step_size      real,"
		" n_sample           integer)",
	"insert into run values(0, 4, 1, 0.5,  500)",
	"insert into run values(1, 5, 1, 0.25, 500)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the run table
	vector<dismod_at::run_struct> 
		run_table = dismod_at::get_run_table(db);
	ok  &= run_table.size() == 2;
	//
	ok  &= run_table[0].parent_node_id     == 4;
	ok  &= run_table[0].pini_smooth_id     == 1;
	ok  &= run_table[0].ode_step_size      == 0.5;
	ok  &= run_table[0].n_sample           == 500;
	//
	ok  &= run_table[1].parent_node_id     == 5;
	ok  &= run_table[1].pini_smooth_id     == 1;
	ok  &= run_table[1].ode_step_size      == 0.25;
	ok  &= run_table[1].n_sample           == 500;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
