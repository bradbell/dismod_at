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
$begin get_covariate_table_xam.cpp$$
$spell
	covariate
	xam
$$

$section C++ get_covariate_table: Example and Test$$
$index example, C++ get_covariate_table$$
$index get_covariate_table, C++ example$$

$code
$verbatim%example/devel/table/get_covariate_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_covariate_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
	"create table covariate"
	"(covariate_id integer primary key, covariate_name text, reference real)",
	"insert into covariate values(0, 'sex',      0.0)",
	"insert into covariate values(1, 'income',   2000.0)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the covariate table
	vector<dismod_at::covariate_struct>
		covariate_table = dismod_at::get_covariate_table(db);
	ok  &= covariate_table.size() == 2;
	//
	ok  &= covariate_table[0].covariate_name  == "sex";
	ok  &= covariate_table[0].reference       == 0.0;
 	//
	ok  &= covariate_table[1].covariate_name  == "income";
	ok  &= covariate_table[1].reference       == 2000.0;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
