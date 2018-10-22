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
$begin get_age_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_age_table: Example and Test$$

$srcfile%example/devel/table/get_age_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_age_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_age_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
		"create table age(age_id integer primary key, age real)",
		"insert into age values(0, 0.0)"    ,
		"insert into age values(1, 20.0)"   ,
		"insert into age values(2, 40.0)"   ,
		"insert into age values(3, 60.0)"   ,
		"insert into age values(4, 80.0)"   ,
		"insert into age values(5,100.0)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the age table
	vector<double> age_table = dismod_at::get_age_table(db);
	ok  &= age_table.size() == 6;
	ok  &= age_table[0] == 0.0;
	ok  &= age_table[1] == 20.0;
	ok  &= age_table[2] == 40.0;
	ok  &= age_table[3] == 60.0;
	ok  &= age_table[4] == 80.0;
	ok  &= age_table[5] == 100.0;

	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
