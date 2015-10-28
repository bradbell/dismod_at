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
$begin get_option_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_option_table: Example and Test$$

$code
$verbatim%example/devel/table/get_option_table_xam.cpp%
	0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/get_option_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_option_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
	"create table option("
		" option_id       integer primary key,"
		" option_name     text unique,"
		" option_value    text"
	")",
	"insert into option values(0, 'parent_node_id',  '0'            )",
	"insert into option values(1, 'number_sample',   '1'            )",
	"insert into option values(2, 'ode_step_size',   '20.0'         )",
	"insert into option values(3, 'random_seed',     '0'            )",

	"insert into option values(4, 'quasi_fixed',           'true'         )",
	"insert into option values(5, 'derivative_test_fixed', 'first-order'  )",
	"insert into option values(6, 'max_num_iter_fixed',    '100'          )",
	"insert into option values(7, 'print_level_fixed',     '0'            )",
	"insert into option values(8, 'tolerance_fixed',       '1e-8'         )",

	"insert into option values(9, 'derivative_test_random', 'second-order' )",
	"insert into option values(10,'max_num_iter_random',    '100'          )",
	"insert into option values(11,'print_level_random',     '0'            )",
	"insert into option values(12,'tolerance_random',       '1e-8'         )"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the option table
	vector<dismod_at::option_struct> option_table =
		dismod_at::get_option_table(db);
	ok  &= option_table.size() == 13;
	//
	ok  &= option_table[0].option_name    == "parent_node_id";
	ok  &= option_table[0].option_value   == "0";
	//
	ok  &= option_table[1].option_name    == "number_sample";
	ok  &= option_table[1].option_value   == "1";
	//
	ok  &= option_table[2].option_name    == "ode_step_size";
	ok  &= option_table[2].option_value   == "20.0";
	//
	ok  &= option_table[3].option_name    == "random_seed";
	ok  &= option_table[3].option_value   == "0";
	//
	//
	ok  &= option_table[4].option_name    == "quasi_fixed";
	ok  &= option_table[4].option_value   == "true";
	//
	ok  &= option_table[5].option_name    == "derivative_test_fixed";
	ok  &= option_table[5].option_value   == "first-order";
	//
	ok  &= option_table[6].option_name    == "max_num_iter_fixed";
	ok  &= option_table[6].option_value   == "100";
	//
	ok  &= option_table[7].option_name    == "print_level_fixed";
	ok  &= option_table[7].option_value   == "0";
	//
	ok  &= option_table[8].option_name    == "tolerance_fixed";
	ok  &= option_table[8].option_value   == "1e-8";
	//
	ok  &= option_table[9].option_name   == "derivative_test_random";
	ok  &= option_table[9].option_value  == "second-order";
	//
	ok  &= option_table[10].option_name    == "max_num_iter_random";
	ok  &= option_table[10].option_value   == "100";
	//
	ok  &= option_table[11].option_name    == "print_level_random";
	ok  &= option_table[11].option_value   == "0";
	//
	ok  &= option_table[12].option_name    == "tolerance_random";
	ok  &= option_table[12].option_value   == "1e-8";
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
