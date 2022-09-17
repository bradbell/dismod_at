// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_table_column_xam.cpp$$
$spell
	xam
$$

$section C++ get_table_column_xam: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_table_column_xam(void)
{
	bool     ok = true;
	using    std::string;

	string   file_name = "example.db";
	bool     new_file  = true;
	sqlite3* db        = dismod_at::open_connection(file_name, new_file);

	// create three columns one with each type of data
	const char* sql_cmd[] = {
		"create table mytable("
			" mytable_id    integer primary key,"
			" mytable_text  text, "
			" mytable_real  real);",
		"insert into  mytable values(1,    'one',         1.0);"
		"insert into  mytable values(0,    'zero',        0.0);",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);
	string table_name   = "mytable";
	string column_name;
	string column_type;

	// int
	column_name = "mytable_id";
	column_type = dismod_at::get_table_column_type(
		db, table_name, column_name
	);
	CppAD::vector<int> int_result;
	dismod_at::get_table_column(
		db, table_name, column_name, int_result
	);
	ok &= column_type   == "integer";
	ok &= int_result[0] == 0;
	ok &= int_result[1] == 1;

	// text
	column_name = "mytable_text";
	column_type = dismod_at::get_table_column_type(
		db, table_name, column_name
	);
	CppAD::vector<string> text_result;
	dismod_at::get_table_column(
		db, table_name, column_name, text_result
	);
	ok &= column_type    == "text";
	ok &= text_result[0] == "zero";
	ok &= text_result[1] == "one";

	// real
	column_name = "mytable_real";
	CppAD::vector<double> real_result;
	column_type = dismod_at::get_table_column_type(
		db, table_name, column_name
	);
	dismod_at::get_table_column(
		db, table_name, column_name, real_result
	);
	ok &= column_type    == "real";
	ok &= real_result[0] == 0.0;
	ok &= real_result[1] == 1.0;

	// check return value when column does not exist
	column_name = "bad_column_name";
	column_type = dismod_at::get_table_column_type(
		db, table_name, column_name
	);
	ok &= column_type == "";

	// check return value when table does not exist
	table_name = "bad_table_name";
	column_type = dismod_at::get_table_column_type(
		db, table_name, column_name
	);
	ok &= column_type == "";

	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
