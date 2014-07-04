// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Density Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_density_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_density_table: Example and Test$$
$index example, C++ get_density_table$$
$index get_density_table, C++ example$$

$code
$verbatim%example/cpp/get_density_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <iostream>
# include <fstream>
# include <cstdio>
# include <sqlite3.h>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>

bool get_density_table_xam(void)
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
	const char* sql_cmd[] = { 
		"create table density(density_id integer primary key, density_name text)",
		"insert into density values(0, 'log_laplace')",
		"insert into density values(1, 'log_gaussian')",
		"insert into density values(2, 'laplace')",
		"insert into density values(3, 'gaussian')"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the density table
	vector<dismod_at::density_enum> density_table = dismod_at::get_density_table(db);
	ok  &= density_table.size() == 4;
	ok  &= density_table[0] == dismod_at::log_laplace_enum;
	ok  &= density_table[1] == dismod_at::log_gaussian_enum;
	ok  &= density_table[2] == dismod_at::laplace_enum;
	ok  &= density_table[3] == dismod_at::gaussian_enum;
 
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
