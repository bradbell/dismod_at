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
$begin get_table_column_xam.cpp$$
$spell
	xam
$$

$section C++ get_table_column_xam: Example and Test$$
$index get_table_column, example$$
$index example, get_table_column$$

$code
$verbatim%example/devel/table/get_table_column_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

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
	void (*exec_sql)(sqlite3*, const string&) = dismod_at::exec_sql_cmd;
	exec_sql(db, "create table mytable(one text,   two int, three real)");
	exec_sql(db, "insert into  mytable values('hello',    1,       2.0)");
	exec_sql(db, "insert into  mytable values('goodbye',  3,       4.0)");
	string table_name   = "mytable";
	string column_name;
	string column_type;

	// text 
	column_name = "one";
	column_type = dismod_at::get_table_column_type(
		db, table_name, column_name
	);
	CppAD::vector<string> text_result;
	dismod_at::get_table_column(
		db, table_name, column_name, text_result
	);
	ok &= column_type    == "text";
	ok &= text_result[0] == "hello";
	ok &= text_result[1] == "goodbye";

	// int 
	column_name = "two";
	column_type = dismod_at::get_table_column_type(
		db, table_name, column_name
	);
	CppAD::vector<int> int_result;
	dismod_at::get_table_column(
		db, table_name, column_name, int_result
	);
	ok &= column_type   == "integer";
	ok &= int_result[0] == 1;
	ok &= int_result[1] == 3;

	// real 
	column_name = "three";
	CppAD::vector<double> real_result;
	column_type = dismod_at::get_table_column_type(
		db, table_name, column_name
	);
	dismod_at::get_table_column(
		db, table_name, column_name, real_result
	);
	ok &= column_type    == "real";
	ok &= real_result[0] == 2.0;
	ok &= real_result[1] == 4.0;

	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
