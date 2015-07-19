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
$begin get_argument_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_argument_table: Example and Test$$

$code
$verbatim%example/devel/table/get_argument_table_xam.cpp%
	0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/get_argument_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_argument_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
		"create table argument("
			" argument_id     integer primary key,"
			" argument_name   text unique,"
			" argument_value  text"
		")",
		"insert into argument values(0, 'parent_node_id',  '0'   )",
		"insert into argument values(1, 'ode_step_size',   '20.0')",
		"insert into argument values(2, 'tolerance',       '1e-8')",
		"insert into argument values(3, 'max_num_iter',    '100' )",
		"insert into argument values(4, 'print_level',     '0'    )",
		"insert into argument values(5, 'random_seed',     '0'   )",
		"insert into argument values(6, 'number_sample',   '1'   )",
		"insert into argument values(7, 'rate_info',       'chi_positive' )"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the argument table
	vector<dismod_at::argument_struct> argument_table =
		dismod_at::get_argument_table(db);
	ok  &= argument_table.size() == 8;
	//
	ok  &= argument_table[0].argument_name  == "parent_node_id";
	ok  &= argument_table[0].argument_value == "0";
	//
	ok  &= argument_table[1].argument_name  == "ode_step_size";
	ok  &= argument_table[1].argument_value == "20.0";
	//
	ok  &= argument_table[2].argument_name  == "tolerance";
	ok  &= argument_table[2].argument_value == "1e-8";
	//
	ok  &= argument_table[3].argument_name  == "max_num_iter";
	ok  &= argument_table[3].argument_value == "100";
	//
	ok  &= argument_table[4].argument_name  == "print_level";
	ok  &= argument_table[4].argument_value == "0";
	//
	ok  &= argument_table[5].argument_name  == "random_seed";
	ok  &= argument_table[5].argument_value == "0";
	//
	ok  &= argument_table[6].argument_name  == "number_sample";
	ok  &= argument_table[6].argument_value == "1";
	//
	ok  &= argument_table[7].argument_name  == "rate_info";
	ok  &= argument_table[7].argument_value == "chi_positive";
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
