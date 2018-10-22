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
$begin get_sample_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_sample_table: Example and Test$$

$srcfile%example/devel/table/get_sample_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_sample_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_sample_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
	"create table sample("
		" sample_id    integer primary key,"
		" sample_index   integer,"
		" var_id         integer,"
		" var_value      real"
	")",
	"insert into sample values(0, 0, 0, 0e-3)",
	"insert into sample values(1, 0, 1, 1e-3)",
	"insert into sample values(2, 0, 2, 2e-3)",
	"insert into sample values(3, 1, 0, 0e-3)",
	"insert into sample values(4, 1, 1, 1e-3)",
	"insert into sample values(5, 1, 2, 2e-3)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the sample table
	vector<dismod_at::sample_struct> sample_table =
		dismod_at::get_sample_table(db);
	ok  &= sample_table.size() == 6;
	//
	ok  &= sample_table[0].sample_index == 0;
	ok  &= sample_table[1].sample_index == 0;
	ok  &= sample_table[2].sample_index == 0;
	ok  &= sample_table[3].sample_index == 1;
	ok  &= sample_table[4].sample_index == 1;
	ok  &= sample_table[5].sample_index == 1;
	//
	ok  &= sample_table[0].var_id       == 0;
	ok  &= sample_table[1].var_id       == 1;
	ok  &= sample_table[2].var_id       == 2;
	ok  &= sample_table[3].var_id       == 0;
	ok  &= sample_table[4].var_id       == 1;
	ok  &= sample_table[5].var_id       == 2;
	//
	ok  &= sample_table[0].var_value    == 0e-3;
	ok  &= sample_table[1].var_value    == 1e-3;
	ok  &= sample_table[2].var_value    == 2e-3;
	ok  &= sample_table[3].var_value    == 0e-3;
	ok  &= sample_table[4].var_value    == 1e-3;
	ok  &= sample_table[5].var_value    == 2e-3;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
