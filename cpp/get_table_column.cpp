# include <iostream>
# include <cassert>
# include <sqlite3.h>
# include <dismod_at/get_table_column.hpp>

std::string get_table_column_type(
	sqlite3*           db          ,
	const std::string& table_name  ,
	const std::string& column_name )
{	// check the type for this column
	using std::cerr;
	using std::endl;

	const char *zDataType;
	const char *zCollSeq;
	int NotNull;
	int PrimaryKey;
	int Autoinc;
	int rc = sqlite3_table_column_metadata(
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
		cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		exit(1);
	}
	return std::string(zDataType);
}


namespace {
	typedef int (*callback_type)(void*, int, char**, char**);

	char*  convert(const std::string& not_used, char* v)
	{	return v; }
	int    convert(const int&         not_used, char* v)
	{	return std::atoi(v); }
	double convert(const double&      not_used, char* v)
	{	return std::atof(v); }

	template <class Element>
	int callback(void *result, int argc, char **argv, char **azColName)
	{	typedef CppAD::vector<Element> vector;
		int i;
		assert( argc == 1 );
		assert( result != nullptr );
		vector* vector_result = static_cast<vector*>(result);
		vector_result->push_back( convert(Element(), argv[0] ) );
  		return 0;
  	}
	template int callback<std::string>(void*, int, char**, char**);

	template <class Element>
	void get_column(
		sqlite3*                    db                    , 
		const std::string&          table_name            ,
		const std::string&          column_name           ,
		CppAD::vector<Element>&     vector_result         )
	{	using std::cerr;
		using std::endl;
	
		// check that initial vector is empty
		assert( vector_result.size() == 0 );
	
		// sql command: select column_name from table_name
		std::string cmd = "select ";
		cmd            += column_name;
		cmd            += " from ";
		cmd            += table_name;
	
		// execute sql command
		char* zErrMsg     = nullptr;
		void* NotUsed     = nullptr;
		callback_type fun = callback<Element>;
		void* result      = static_cast<void*>(&vector_result);
		int rc = sqlite3_exec(db, cmd.c_str(), fun, result, &zErrMsg);
		if( rc )
		{	assert(zErrMsg != nullptr );
			cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			exit(1);
		}
		return;
	}
}

void dismod_at::get_table_column(
	sqlite3*                    db                  , 
	const std::string&          table_name          ,
	const std::string&          column_name         ,
	CppAD::vector<std::string>& text_result         )
{
	// check that initial vector is empty
	assert( text_result.size() == 0 );

	// check the type for this column
	std::string col_type = get_table_column_type(db, table_name, column_name);
	assert( col_type == "text" );

	// Use template function for rest
	get_column(db, table_name, column_name, text_result);

	return;
}

void dismod_at::get_table_column(
	sqlite3*                    db                 , 
	const std::string&          table_name         ,
	const std::string&          column_name        ,
	CppAD::vector<int>&         int_result         )
{
	// check that initial vector is empty
	assert( int_result.size() == 0 );

	// check the type for this column
	std::string col_type = get_table_column_type(db, table_name, column_name);
	assert( col_type == "int" );

	// Use template function for rest
	get_column(db, table_name, column_name, int_result);

	return;
}
