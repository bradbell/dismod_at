// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_data_subset_xam.cpp$$
$spell
	xam
	nslist
$$

$section C++ get_data_subset: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>

bool get_data_subset_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// sql commands
	const char* sql_cmd[] = {
		"create table data_subset("
		"data_subset_id     integer primary key,"
		"data_id            integer,"
		"hold_out           integer,"
		"density_id         integer,"
		"eta                   real,"
		"nu                    real)",
		"insert into data_subset values(0, 1, 0, 1, 2, 3)",
		"insert into data_subset values(1, 2, 0, 2, 3, 4)",
		"insert into data_subset values(2, 3, 1, 3, 4, 5)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the data_subset table
	vector<dismod_at::data_subset_struct> data_subset_table =
		dismod_at::get_data_subset(db);

	// check the table
	size_t n_subset = data_subset_table.size();
	ok  &= n_subset == 3;
	ok  &= data_subset_table[0].data_id  == 1;
	ok  &= data_subset_table[0].hold_out == 0;
	ok  &= data_subset_table[0].density_id == 1;
	ok  &= data_subset_table[0].eta == 2;
	ok  &= data_subset_table[0].nu  == 3;
	//
	ok  &= data_subset_table[1].data_id  == 2;
	ok  &= data_subset_table[1].hold_out == 0;
	ok  &= data_subset_table[1].density_id == 2;
	ok  &= data_subset_table[1].eta == 3;
	ok  &= data_subset_table[1].nu  == 4;
	//
	ok  &= data_subset_table[2].data_id  == 3;
	ok  &= data_subset_table[2].hold_out == 1;
	ok  &= data_subset_table[2].density_id == 3;
	ok  &= data_subset_table[2].eta == 4;
	ok  &= data_subset_table[2].nu == 5;

	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
