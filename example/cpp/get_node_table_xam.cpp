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
$begin get_node_table_xam.cpp$$
$spell
	xam
$$

$section C++ get_node_table: Example and Test$$
$index example, C++ get_node_table$$
$index get_node_table, C++ example$$

$code
$verbatim%example/cpp/get_node_table_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <iostream>
# include <fstream>
# include <cstdio>
# include <sqlite3.h>
# include <dismod_at/get_node_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>

bool get_node_table_xam(void)
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
		"create table node"
		"(node_id integer primary key, node_name text, parent int)",
		"insert into node values(0, 'world',          -1)",
		"insert into node values(1, 'north_america',  0)",
		"insert into node values(2, 'united_states',  1)",
		"insert into node values(3, 'canada',         1)"
	};
	size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
	for(size_t i = 0; i < n_command; i++)
		dismod_at::exec_sql_cmd(db, sql_cmd[i]);


	// get the node table
	vector<dismod_at::node_struct> node_table = dismod_at::get_node_table(db);
	ok  &= node_table.size() == 4;
	//
	ok  &= node_table[0].name == "world";
	ok  &= node_table[1].name == "north_america";
	ok  &= node_table[2].name == "united_states";
	ok  &= node_table[3].name == "canada";
	//
	ok  &= node_table[0].parent == -1;
	ok  &= node_table[1].parent == 0;
	ok  &= node_table[2].parent == 1;
	ok  &= node_table[3].parent == 1;
 	//
	// close database and return
	sqlite3_close(db);
	return ok;
}
// END C++
