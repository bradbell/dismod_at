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
$begin get_like_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_like_table: Example and Test$$
$index example, C++ get_like_table$$
$index get_like_table, C++ example$$

$code
$verbatim%example/cpp/get_like_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <iostream>
# include <fstream>
# include <cstdio>
# include <sqlite3.h>
# include <dismod_at/get_like_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>

bool get_like_table_xam(void)
{
	bool   ok = true;
	using  std::cerr;
	using  std::cout;
	using  std::endl;
	using  std::string;
	using  CppAD::vector;	
	const char* file_name = "example.db";

	// delete old version of database
	std::ifstream ifile(file_name);
	if( ifile )
	{	ifile.close();
		std::remove(file_name);
	}

	// open a new database
	sqlite3* db;
	int rc = sqlite3_open(file_name, &db);
	if( rc )
	{	cerr << "Can't create database: " << file_name << endl;
		sqlite3_close(db);
		exit(1);
	}

	// sql commands
	// assume that density_id for uniform density is 0.
	const char* sql_cmd[] = { 
		"create table like(like_id integer primary key,"
			" like_name  text, "
			" density_id integer, "
			" lower        real,"
			" upper        real,"
			" mean         real,"
			" std          real,"
			" eta          real)",
		"insert into like values(0, 'none', 0, null, null, 0.0, null, null)",
		"insert into like values(1, 'rate', 1, 0.0,  1.0,  0.1, 1e-4, 1e-5)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the like table
	vector<dismod_at::like_struct> like_table = dismod_at::get_like_table(db);
	ok  &= like_table.size() == 2;
	//
	ok  &= like_table[0].name       == "none";
	ok  &= like_table[0].density_id == 0;
	ok  &= like_table[0].lower      == std::atof("-inf");
	ok  &= like_table[0].upper      == std::atof("+inf");
	ok  &= like_table[0].mean       == 0.0;
	//
	ok  &= like_table[1].name       == "rate";
	ok  &= like_table[1].density_id == 1;
	ok  &= like_table[1].lower      == 0.0;
	ok  &= like_table[1].upper      == 1.0;
	ok  &= like_table[1].mean       == 0.1;
	ok  &= like_table[1].std        == 1e-4;
	ok  &= like_table[1].eta        == 1e-5;
 	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
