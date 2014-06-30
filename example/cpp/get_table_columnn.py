#! /bin/env python
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
import sys
import os
import shutil
# ------------------------------------------------------------------------
parent_dir = os.getcwd()
if os.path.isfile('junk') :
	os.remove('junk')
if os.path.isdir('junk' ) :
	shutil.rmtree('junk')
os.mkdir('junk')
os.chdir('junk')
# ------------------------------------------------------------------------
# create a database and a table 
fp  = open('example.sql', 'w')
sql = '''
create table mytable(one text, two int);
insert into mytable values('hello',   1);
insert into mytable values('goodbye', 2);
select * from mytable;
'''
fp.write(sql);
fp.close();
os.system('sqlite3 example.db < example.sql')
# ------------------------------------------------------------------------
# C++ program tha
fp  = open('example.cpp', 'w')
cpp = r'''
# include <iostream>
# include <sqlite3.h>
# include <dismod_at/get_table_column.hpp>

int main(int argc, char **argv){
	sqlite3* db;
	int      rc;
	bool     ok = true;
	using    std::cerr;
	using    std::cout;
	using    std::endl;
	using    std::string;

	rc = sqlite3_open("example.db", &db);
	if( rc ){
		  cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
		  sqlite3_close(db);
		  exit(1);
	}
	string table_name   = "mytable";
	string table_column = "one";
	CppAD::vector<string> text_result;
	dismod_at::get_table_column_text(
		db, table_name, table_column, text_result
	);
	sqlite3_close(db);
	//
	ok &= text_result[0] == "hello";
	ok &= text_result[1] == "goodbye";
	if( ! ok ) 
	{	cout << "get_table_column: Error" << endl;
		return 1;
	}
	cout << "get_table_column: OK" << endl;
	return 0;
}
'''
fp.write(cpp)
fp.close()
cmd  = 'g++ example.cpp ../cpp/get_table_column.cpp'
cmd += ' -I/home/bradbell/prefix/cppad/include'
cmd += ' -I' + parent_dir
cmd += ' -std=c++11 -lsqlite3 -o example'
print cmd
os.system(cmd)
os.system('./example')
