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
The results are ordered using the $cref/primary key/table/Primary Key/$$
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
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>

namespace {
	using std::string;

	// Type specified by sqlite3_exec
	typedef int (*callback_type)(void*, int, char**, char**);

	// set by get_column, used by convert
	sqlite3* db_;
	string   table_name_;
	string   column_name_;

	char*  convert(const std::string& not_used, char* v, size_t row_id)
	{	if( v == DISMOD_AT_NULL_PTR )
		{	string msg = "The null value appears in the text column ";
			msg += column_name_;
			dismod_at::error_exit(db_, msg, table_name_, row_id);
		}
		return v;
	}
	int    convert(const int& not_used, char* v, size_t row_id)
	{	if(	v == DISMOD_AT_NULL_PTR )
			return DISMOD_AT_NULL_INT;
		//
		int value = std::atoi(v);
		//
		// no integer values should be the minimum integer
		if( value == DISMOD_AT_NULL_INT )
		{	string msg = "The minimum integer appears in the int column ";
			msg += column_name_;
			dismod_at::error_exit(db_, msg, table_name_, row_id);
		}
		//
		return value;
	}
	double convert(const double& not_used, char* v, size_t row_id)
	{	if( v == DISMOD_AT_NULL_PTR )
			return std::numeric_limits<double>::quiet_NaN();
		double value = std::atof(v);
		if( value != value )
		{	string msg = "The value nan appears in the double column ";
			msg += column_name_;
			dismod_at::error_exit(db_, msg, table_name_, row_id);
		}
		return value;
	}

	template <class Element>
	int callback(void *result, int argc, char **argv, char **azColName)
	{	typedef CppAD::vector<Element> vector;
		assert( argc == 1 );
		assert( result != DISMOD_AT_NULL_PTR );
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
	{
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
		char* zErrMsg     = DISMOD_AT_NULL_PTR;
		callback_type fun = callback<Element>;
		void* result      = static_cast<void*>(&vector_result);
		int rc = sqlite3_exec(db, cmd.c_str(), fun, result, &zErrMsg);
		if( rc )
		{	assert(zErrMsg != DISMOD_AT_NULL_PTR );
			std::string message = "SQL error: ";
			message += sqlite3_errmsg(db);
			message += ". SQL command: " + cmd;
			sqlite3_free(zErrMsg);
			dismod_at::error_exit(db, message);
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

	// set globals used by error messages
	db_         = db;
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
	if( rc )
	{	std::string message = "SQL error: ";
		message += sqlite3_errmsg(db);
		error_exit(db, message);
	}
	std::string ctype(zDataType);
	//
	// sqlite seems to use upper case for its types
	for(size_t i = 0; i < ctype.size(); i++)
		ctype[i] = std::tolower( ctype[i] );
	//
	// sqlite seems to use int for its integer type
	if( ctype == "int" )
		ctype = "integer";
	//
	bool ok = ctype == "integer" || ctype == "text" || ctype == "real";
	if( ! ok )
	{	string msg = "Column " + column_name + " has type " + ctype
			+ "\nwhich is not one of the following: integer, real, or text";
		dismod_at::error_exit(db_, msg, table_name_);
	}

	return ctype;
}


void get_table_column(
	sqlite3*                    db                  ,
	const std::string&          table_name          ,
	const std::string&          column_name         ,
	CppAD::vector<std::string>& text_result         )
{	// for error message
	size_t null_id = size_t( DISMOD_AT_NULL_INT );

	// set globals used by error messages
	table_name_ = table_name;
	column_name_ = column_name;

	// check that initial vector is empty
	assert( text_result.size() == 0 );

	// check the type for this column
	std::string col_type = get_table_column_type(db, table_name, column_name);
	if( col_type != "text" )
	{	std::string message = "get_table_column for column = " + column_name;
		message += ". Expected type to be text not " + col_type;
		error_exit(db, message, table_name, null_id);
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
{	// for error message
	size_t null_id = size_t( DISMOD_AT_NULL_INT );

	// set globals used by error messages
	table_name_ = table_name;
	column_name_ = column_name;

	// check that initial vector is empty
	assert( int_result.size() == 0 );

	// check the type for this column
	std::string col_type = get_table_column_type(db, table_name, column_name);
	if( col_type != "integer" )
	{	std::string message = "get_table_column for column = " + column_name;
		message += ". Expected type to be integer not " + col_type;
		error_exit(db, message, table_name, null_id);
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
{	// for error message
	size_t null_id = size_t( DISMOD_AT_NULL_INT );

	// set globals used by error messages
	table_name_ = table_name;
	column_name_ = column_name;

	// check that initial vector is empty
	assert( double_result.size() == 0 );

	// check the type for this column
	std::string col_type = get_table_column_type(db, table_name, column_name);
	if( col_type != "real" )
	{	std::string message = "get_table_column for column = " + column_name;
		message += ". Expected type to be real not " + col_type;
		error_exit(db, message, table_name, null_id);
	}

	// Use template function for rest
	get_column(db, table_name, column_name, double_result);

	return;
}

} // END DISMOD_AT_NAMESPACE
