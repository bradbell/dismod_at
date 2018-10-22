// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_nslist_pair_xam.cpp$$
$spell
	nslist
	xam
$$

$section C++ get_nslist_pair_table: Example and Test$$

$srcfile%example/devel/table/get_nslist_pair_xam.cpp%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_nslist_pair.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_nslist_pair_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
		"create table nslist_pair("
			"nslist_pair_id       integer primary key,"
			"nslist_id            integer,"
			"node_id              integer,"
			"smooth_id            integer)",
		"insert into nslist_pair values(0, 0, 0, 1)",
		"insert into nslist_pair values(1, 0, 1, 2)",
		"insert into nslist_pair values(2, 0, 2, 3)",
		"insert into nslist_pair values(3, 1, 0, 4)",
		"insert into nslist_pair values(4, 1, 1, 5)",
		"insert into nslist_pair values(5, 1, 2, 6)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the rate table
	vector<dismod_at::nslist_pair_struct> nslist_pair_table =
			dismod_at::get_nslist_pair(db);
	size_t n_nslist_pair = nslist_pair_table.size();
	ok  &= n_nslist_pair == 6;
	//
	for(size_t id = 0; id < n_nslist_pair; id++)
	{
		ok &= size_t(nslist_pair_table[id].nslist_id)    == (id / 3);
		ok &= size_t(nslist_pair_table[id].node_id)    == (id % 3);
		ok &= size_t(nslist_pair_table[id].smooth_id)  == (id + 1);
	}

	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
