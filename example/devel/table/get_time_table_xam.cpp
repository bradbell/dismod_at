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
$begin get_time_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_time_table: Example and Test$$
$index example, C++ get_time_table$$
$index get_time_table, C++ example$$

$code
$verbatim%example/devel/table/get_time_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/get_time_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_time_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;	

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = { 
		"create table time(time_id integer primary key, time real)",
		"insert into time values(0, 1980.0)"   ,
		"insert into time values(1, 1990.0)"   ,
		"insert into time values(2, 2000.0)"   ,
		"insert into time values(3, 2010.0)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the time table
	vector<double> time_table = dismod_at::get_time_table(db);
	ok  &= time_table.size() == 4;
	ok  &= time_table[0] == 1980.0;
	ok  &= time_table[1] == 1990.0;
	ok  &= time_table[2] == 2000.0;
	ok  &= time_table[3] == 2010.0;
 
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
