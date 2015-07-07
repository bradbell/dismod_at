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
$begin get_simulate_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_simulate_table: Example and Test$$

$code
$verbatim%example/devel/table/get_simulate_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/get_simulate_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_simulate_table_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
	"create table simulate("
		" simulate_id   integer primary key,"
		" sample_id     integer,"
		" data_id       integer,"
		" meas_value    real"
	")",
	"insert into simulate values(0, 0, 0, 0e-3)",
	"insert into simulate values(1, 0, 1, 1e-3)",
	"insert into simulate values(2, 0, 2, 2e-3)",
	"insert into simulate values(3, 1, 0, 0e-3)",
	"insert into simulate values(4, 1, 1, 1e-3)",
	"insert into simulate values(5, 1, 2, 2e-3)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the simulate table
	vector<dismod_at::simulate_struct> simulate_table =
		dismod_at::get_simulate_table(db);
	ok  &= simulate_table.size() == 6;
	//
	ok  &= simulate_table[0].sample_id    == 0;
	ok  &= simulate_table[1].sample_id    == 0;
	ok  &= simulate_table[2].sample_id    == 0;
	ok  &= simulate_table[3].sample_id    == 1;
	ok  &= simulate_table[4].sample_id    == 1;
	ok  &= simulate_table[5].sample_id    == 1;
	//
	ok  &= simulate_table[0].data_id      == 0;
	ok  &= simulate_table[1].data_id      == 1;
	ok  &= simulate_table[2].data_id      == 2;
	ok  &= simulate_table[3].data_id      == 0;
	ok  &= simulate_table[4].data_id      == 1;
	ok  &= simulate_table[5].data_id      == 2;
	//
	ok  &= simulate_table[0].meas_value   == 0e-3;
	ok  &= simulate_table[1].meas_value   == 1e-3;
	ok  &= simulate_table[2].meas_value   == 2e-3;
	ok  &= simulate_table[3].meas_value   == 0e-3;
	ok  &= simulate_table[4].meas_value   == 1e-3;
	ok  &= simulate_table[5].meas_value   == 2e-3;
	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
