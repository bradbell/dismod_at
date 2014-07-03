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
$begin get_rate_table_xam.cpp$$
$spell
	xam
$$

$section get_rate_table: Example and Test$$

$code
$verbatim%example/cpp/get_rate_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <iostream>
# include <fstream>
# include <cstdio>
# include <sqlite3.h>
# include <dismod_at/get_rate_table.hpp>


namespace {
	void exec_sql_cmd(sqlite3* db, const char* sql_cmd)
	{	char* zErrMsg                              = nullptr;
		int (*callback)(void*, int, char**,char**) = nullptr;
    	void* callback_arg                         = nullptr;
		int rc = sqlite3_exec(db, sql_cmd, callback, callback_arg, &zErrMsg);
		if( rc )
		{	assert(zErrMsg != nullptr );
			std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			exit(1);
		}
	}
}

bool get_rate_table_xam(void)
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
		"create table rate(rate_id integer primary key, rate_name text)",
		"insert into rate values(0, 'omega')",
		"insert into rate values(1, 'chi')",
		"insert into rate values(2, 'rho')",
		"insert into rate values(3, 'iota')"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		exec_sql_cmd(db, sql_cmd[i]);


	// get the rate table
	vector<dismod_at::rate_enum> rate_table = dismod_at::get_rate_table(db);
	ok  &= rate_table[0] == dismod_at::omega_enum;
	ok  &= rate_table[1] == dismod_at::chi_enum;
	ok  &= rate_table[2] == dismod_at::rho_enum;
	ok  &= rate_table[3] == dismod_at::iota_enum;
 
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
