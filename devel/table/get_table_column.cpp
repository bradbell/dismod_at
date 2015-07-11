// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_table_column$$
$spell
	sqlite
	const
	std
	CppAD
	dismod
	hpp
$$

$section C++: Get The Type and Values in a Table Column$$

$head Syntax$$
$icode%column_type% = get_table_column_type(%db%, %table_name%, %column_name%)
%$$
$codei%get_table_column(%db%, %table_name%, %column_name%, %result%)
%$$

$head db$$
This argument has prototype
$codei%
	sqlite3* %db%
%$$
and is the database we are getting information from.

$head table_name$$
This argument has prototype
$codei%
	const std::string& %table_name%
%$$
and is the name of the table we are getting information from.

$head column_name$$
This argument has prototype
$codei%
	const std::string& %column_name%
%$$
and is the name of the column we are getting information from.

$head column_type$$
This return value has prototype
$codei%
	std::string %column_type%
%$$
Its value is either $code text$$, $code integer$$, or $code real$$
depending on the type of the column in the database.

$head result$$
The input size of this vector must be zero.
Upon return it contains the values in the specified column.
The results are ordered using the $cref/primary key/input/Primary Key/$$
for this table.

$subhead text$$
If the column has type $code text$$, this argument has
prototype
$codei%
	CppAD::vector<std::string>& %result%
%$$
It is an error for any of the text values
in the database to be $code null$$.

$subhead integer$$
If the column has type $code integer$$, this argument has
prototype
$codei%
	CppAD::vector<int>& %result%
%$$
If an integer value is $code null$$,
it is returned as the $code int$$ value
$codei%
	std::limits<int>::min()
%$$
It is an error for any of the integer values in the database to
have this value.

$subhead real$$
If the column has type $code real$$, this argument has
prototype
$codei%
	CppAD::vector<double>& %result%
%$$
If a real value is $code null$$, it is returned as the $code double$$
value $code nan$$.
Note that it is not possible for a database value to be $code nan$$.

$children%example/devel/table/get_table_column_xam.cpp
%$$
$head Example$$
The file $cref get_table_column_xam.cpp$$ contains an example that uses
this function.

$end
------------------------------------------------------------------------------
*/
# include <limits>
# include <string>
# include <iostream>
# include <cassert>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/table_error_exit.hpp>

namespace {
	using std::string;

	// Type specified by sqlite3_exec
	typedef int (*callback_type)(void*, int, char**, char**);

	// Name of table column that we are currently working with
	// set by get_column, used by convert
	string table_name_;
	string column_name_;

	char*  convert(const std::string& not_used, char* v, size_t row_id)
	{	if( v == DISMOD_AT_NULLPTR )
		{	string msg = "The null value appears in the text column ";
			msg += column_name_;
			dismod_at::table_error_exit(table_name_, row_id, msg);
		}
		return v;
	}
	int    convert(const int& not_used, char* v, size_t row_id)
	{	if(	v == DISMOD_AT_NULLPTR )
			return std::numeric_limits<int>::min();
		//
		int value = std::atoi(v);
		//
		// no integer values should be the minimum integer
		if( value == std::numeric_limits<int>::min() )
		{	string msg = "The minimum integer appears in the int column ";
			msg += column_name_;
			dismod_at::table_error_exit(table_name_, row_id, msg);
		}
		//
		return value;
	}
	double convert(const double& not_used, char* v, size_t row_id)
	{	if( v == DISMOD_AT_NULLPTR )
			return std::numeric_limits<double>::quiet_NaN();
		return std::atof(v);
	}

	template <class Element>
	int callback(void *result, int argc, char **argv, char **azColName)
	{	typedef CppAD::vector<Element> vector;
		assert( argc == 1 );
		assert( result != DISMOD_AT_NULLPTR );
		vector* vector_result = static_cast<vector*>(result);
		size_t row_id = vector_result->size();
		vector_result->push_back( convert(Element(), argv[0], row_id ) );
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

		// name of the primary key for this table
		std::string primary_key = table_name + "_id";

		// sql command: select column_name from table_name
		std::string cmd = "select ";
		cmd            += column_name;
		cmd            += " from ";
		cmd            += table_name;
		cmd            += " order by ";
		cmd            += primary_key;

		// execute sql command
		char* zErrMsg     = DISMOD_AT_NULLPTR;
		callback_type fun = callback<Element>;
		void* result      = static_cast<void*>(&vector_result);
		int rc = sqlite3_exec(db, cmd.c_str(), fun, result, &zErrMsg);
		if( rc )
		{	assert(zErrMsg != DISMOD_AT_NULLPTR );
			cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			exit(1);
		}
		return;
	}
}


namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

std::string get_table_column_type(
	sqlite3*           db          ,
	const std::string& table_name  ,
	const std::string& column_name )
{	// check the type for this column
	using std::cerr;
	using std::endl;

	// set globals used by error messages
	table_name_ = table_name;
	column_name_ = column_name;

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
	std::string rvalue(zDataType);

	// sqlite seems to use int for its integer type
	if( rvalue == "int" )
		rvalue = "integer";
	assert( rvalue == "integer" || rvalue == "text" || rvalue == "real" );
	return rvalue;
}


void get_table_column(
	sqlite3*                    db                  ,
	const std::string&          table_name          ,
	const std::string&          column_name         ,
	CppAD::vector<std::string>& text_result         )
{

	// set globals used by error messages
	table_name_ = table_name;
	column_name_ = column_name;

	// check that initial vector is empty
	assert( text_result.size() == 0 );

	// check the type for this column
	std::string col_type = get_table_column_type(db, table_name, column_name);
	if( col_type != "text" )
	{	std::cerr << "get_table_column: for table = " << table_name
		<< ", column = " << column_name << std::endl
		<< "expect type to be text, not " << col_type << std::endl;
		exit(1);
	}

	// Use template function for rest
	get_column(db, table_name, column_name, text_result);

	return;
}

void get_table_column(
	sqlite3*                    db                 ,
	const std::string&          table_name         ,
	const std::string&          column_name        ,
	CppAD::vector<int>&         int_result         )
{

	// set globals used by error messages
	table_name_ = table_name;
	column_name_ = column_name;

	// check that initial vector is empty
	assert( int_result.size() == 0 );

	// check the type for this column
	std::string col_type = get_table_column_type(db, table_name, column_name);
	if( col_type != "integer" )
	{	std::cerr << "get_table_column: for table = " << table_name
		<< ", column = " << column_name << std::endl
		<< "expect type to be integer, not " << col_type << std::endl;
		exit(1);
	}

	// Use template function for rest
	get_column(db, table_name, column_name, int_result);

	return;
}

void get_table_column(
	sqlite3*                    db                 ,
	const std::string&          table_name         ,
	const std::string&          column_name        ,
	CppAD::vector<double>&      double_result      )
{
	// set globals used by error messages
	table_name_ = table_name;
	column_name_ = column_name;

	// check that initial vector is empty
	assert( double_result.size() == 0 );

	// check the type for this column
	std::string col_type = get_table_column_type(db, table_name, column_name);
	if( col_type != "real" )
	{	std::cerr << "get_table_column: for table = " << table_name
		<< ", column = " << column_name << std::endl
		<< "expect type to be real, not " << col_type << std::endl;
		exit(1);
	}

	// Use template function for rest
	get_column(db, table_name, column_name, double_result);

	return;
}

} // END DISMOD_AT_NAMESPACE
