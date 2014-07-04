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
$begin get_integrand_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_integrand_table: Example and Test$$
$index example, C++ get_integrand_table$$
$index get_integrand_table, C++ example$$

$code
$verbatim%example/cpp/get_integrand_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <iostream>
# include <fstream>
# include <cstdio>
# include <sqlite3.h>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>

bool get_integrand_table_xam(void)
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
		"create table integrand"
		"(integrand_id integer primary key, integrand_name text)",
		"insert into integrand values(0, 'mtall')",
		"insert into integrand values(1, 'prevalence')",
		"insert into integrand values(2, 'remission')",
		"insert into integrand values(3, 'incidence')",
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the integrand table
	vector<dismod_at::integrand_enum> integrand_table = 
		dismod_at::get_integrand_table(db);
	ok  &= integrand_table.size() == 4;
	ok  &= integrand_table[0] == dismod_at::mtall_enum;
	ok  &= integrand_table[1] == dismod_at::prevalence_enum;
	ok  &= integrand_table[2] == dismod_at::remission_enum;
	ok  &= integrand_table[3] == dismod_at::incidence_enum;
 
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
