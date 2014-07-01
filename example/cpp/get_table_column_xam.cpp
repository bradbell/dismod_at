# include <iostream>
# include <fstream>
# include <cstdio>
# include <sqlite3.h>
# include <dismod_at/get_table_column.hpp>

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

bool get_table_column_xam(void)
{
	bool     ok = true;
	using    std::cerr;
	using    std::cout;
	using    std::endl;
	using    std::string;
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

	// create three columns one with each type of data
	exec_sql_cmd(db, "create table mytable(one text,   two int, three real)");
	exec_sql_cmd(db, "insert into  mytable values('hello',    1,       2.0)");
	exec_sql_cmd(db, "insert into  mytable values('goodbye',  3,       4.0)");
	string table_name   = "mytable";
	string column_name;

	// text 
	column_name = "one";
	CppAD::vector<string> text_result;
	dismod_at::get_table_column(
		db, table_name, column_name, text_result
	);
	ok &= text_result[0] == "hello";
	ok &= text_result[1] == "goodbye";

	// int 
	column_name = "two";
	CppAD::vector<int> int_result;
	dismod_at::get_table_column(
		db, table_name, column_name, int_result
	);
	ok &= int_result[0] == 1;
	ok &= int_result[1] == 3;

	// real 
	column_name = "three";
	CppAD::vector<double> real_result;
	dismod_at::get_table_column(
		db, table_name, column_name, real_result
	);
	ok &= real_result[0] == 2.0;
	ok &= real_result[1] == 4.0;

	// close database and return
	sqlite3_close(db);
	return ok;
}
