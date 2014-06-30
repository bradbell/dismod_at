# include <iostream>
# include <sqlite3.h>


const char* get_table_column_type(
	sqlite3*           db          ,
	const std::string& table_name  ,
	const std::string& column_name )
{	// check the type for this column
	const char *zDataType;
	const char *zCollSeq;
	int NotNull;
	int PrimaryKey;
	int Autoinc;
	rc = sqlite3_table_column_metadata(
		db,
		"main",
		table_name.c_str(),
		column_name.c_str(),
		&zDataType, 
		&zCollSeq, 
		&NotNull, 
		&PrimaryKey, 
		&Autoinc
	);
	if( rc ){
		cerr << "SQL error: " << sqlite3_errmsg(db) << std::end;
		sqlite3_close(db);
		exit(1);
	}
	return std::string(zDataType);
}


namespace {
	int callback_text(void *result, int argc, char **argv, char **azColName)
	{	int i;
		assert( argc == 1 );
		assert( result != null );
		CppAD::vector<std::string>* test_result = result;
		text_result->push_back( argv[0] );
  	}
  	return 0;
}

void get_table_column_text(
	sqlite3*                   db                    , 
	std::string                table_name            ,
	std::string                column_name           ,
	CppAD::vector<std::string> text_result           )
{	// check the type for this column
	std::string col_type = get_table_column_type(db, table_name, table_column);
	assert( col_type == "text" )

	// set for callback to use
	assert( text_result.size() == 0 );
	void* result = &text_result;

	// sql command: select column_name from table_name
	std::string cmd = "select ";
	cmd += column_name;
	cmd += " from ";
	cmd += table_name;

	// execute sql command
	char* zErrMsg = null;
	void* NotUsed = null;
	int rc = sqlite3_exec(db, cmd.c_str, callback_text, result, &zErrMsg);
	if( rc )
	{	assert(zErrMsg != null );
		cerr << "SQL error: " << sqlite3_errmsg(db) << std::end;
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		exit(1)
	}
	return;
}
