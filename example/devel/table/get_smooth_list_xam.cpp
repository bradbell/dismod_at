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
$begin get_smooth_list_xam.cpp$$
$spell
	xam
$$

$section C++ get_smooth_list_table: Example and Test$$

$code
$srcfile%example/devel/table/get_smooth_list_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/get_smooth_list.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_smooth_list_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
		"create table smooth_list("
			"smooth_list_id       integer primary key,"
			"smooth_list_index    integer,"
			"node_id              integer,"
			"smooth_id            integer)",
		"insert into smooth_list values(0, 0, 0, 1)",
		"insert into smooth_list values(1, 0, 1, 2)",
		"insert into smooth_list values(2, 0, 2, 3)",
		"insert into smooth_list values(3, 1, 0, 4)",
		"insert into smooth_list values(4, 1, 1, 5)",
		"insert into smooth_list values(5, 1, 2, 6)",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);

	// get the rate table
	vector<dismod_at::smooth_list_struct> smooth_list_table =
			dismod_at::get_smooth_list(db);
	size_t n_smooth_list = smooth_list_table.size();
	ok  &= n_smooth_list == 6;
	//
	for(size_t id = 0; id < n_smooth_list; id++)
	{
		ok &= size_t(smooth_list_table[id].smooth_list_index)    == (id / 3);
		ok &= size_t(smooth_list_table[id].node_id)    == (id % 3);
		ok &= size_t(smooth_list_table[id].smooth_id)  == (id + 1);
	}

	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
