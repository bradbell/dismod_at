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
$begin get_column_max$$
$spell
	str
	sqlite
	const
	std
$$

$section C++: Get The Maximum Values in a Table Column$$

$head Syntax$$
$icode%max_str% = get_column_max(%db%, %table_name%, %column_name%)%$$

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

$head max_str$$
The return value has prototype
$codei%
	std::string %max_str%
%$$
and is the maximum value in the column.
If the table is empty,
or if all the values in the column are $code null$$,
the return value of $icode max_str$$ is the empty string.

$children%example/devel/table/get_column_max_xam.cpp
%$$
$head Example$$
The file $cref get_column_max_xam.cpp$$ contains an example that uses
this function.

$end
------------------------------------------------------------------------------
*/
# include <iostream>
# include <cassert>
# include <dismod_at/get_table_column.hpp>

namespace {
	size_t count_callback;
	int callback(void *result, int argc, char **argv, char **azColName)
	{	count_callback++;
		assert(count_callback == 1);
		assert( argc == 1 );
		assert( result != nullptr );
		std::string* str_ptr = static_cast<std::string*>(result);
		if( argv[0] == nullptr )
			*str_ptr = "";
		else
			*str_ptr = argv[0];
  		return 0;
  	}
}
namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

std::string get_column_max(
	sqlite3*                    db                    ,
	const std::string&          table_name            ,
	const std::string&          column_name           )
{	using std::cerr;
	using std::endl;

	// sql command: select max(column_name) from table_name
	std::string cmd = "select max(";
	cmd            += column_name;
	cmd            += ") from ";
	cmd            += table_name;

	// execute sql command
	count_callback = 0;
	char* zErrMsg  = nullptr;
	std::string max_str;
	int rc = sqlite3_exec(db, cmd.c_str(), callback, &max_str, &zErrMsg);
	if( rc )
	{	assert(zErrMsg != nullptr );
		cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		exit(1);
	}
	assert( count_callback == 1 );
	return max_str;
}

} // END DISMOD_AT_NAMESPACE
