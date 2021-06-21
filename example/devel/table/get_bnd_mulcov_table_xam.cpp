// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_bnd_mulcov_table_xam.cpp$$
$spell
	xam
	bnd_mulcov
$$

$section C++ get_bnd_mulcov_table: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cmath>
# include <dismod_at/get_bnd_mulcov_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_bnd_mulcov_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
		"create table bnd_mulcov("
		"bnd_mulcov_id     integer primary key,"
		"max_upper         real)",
		"insert into bnd_mulcov values(0,   null)",
		"insert into bnd_mulcov values(1, +100.0)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the bnd_mulcov table
	vector<dismod_at::bnd_mulcov_struct> bnd_mulcov_table =
		dismod_at::get_bnd_mulcov_table(db);

	// check the table
	size_t n_bnd_mulcov = bnd_mulcov_table.size();
	ok  &= n_bnd_mulcov == 2;
	ok  &= std::isnan( bnd_mulcov_table[0].max_upper );
	ok  &= bnd_mulcov_table[1].max_upper  == + 100.0;

	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
