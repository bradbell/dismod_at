// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin exec_sql_cmd$$
$spell
	sql
	dismod
	cmd
	std
	hpp
	sqlite
$$

$section C++: Execution of an SQL Command$$

$index sql, C++ execute$$
$index execute, sql command$$
$index C++, execute sql$$

$head Syntax$$
$codei%exec_sql_cmd(%db%, %sql_cmd%)%$$

$head Purpose$$
Executes an SQL command that does not return any information
(and check for an error).

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$

$head sql_cmd$$
This argument has prototype
$codei%
	std::string& sql_cmd
%$$
It contains the command that is executed.

$head Error$$
If an error occurs, and error message is printing and this routine
aborts; i.e., it does not return if an error occurs.

$end
-----------------------------------------------------------------------------
*/
# include <cstdlib>
# include <iostream>
# include <cassert>

# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void exec_sql_cmd(sqlite3* db, const std::string& sql_cmd)
{
	char* zErrMsg                              = DISMOD_AT_NULL_PTR;
	int (*callback)(void*, int, char**,char**) = DISMOD_AT_NULL_PTR;
	void* callback_arg                         = DISMOD_AT_NULL_PTR;
	int rc = sqlite3_exec(
		db,
		sql_cmd.c_str(),
		callback,
		callback_arg,
		&zErrMsg
	);
	if( rc )
	{	assert(zErrMsg != DISMOD_AT_NULL_PTR );
		std::string message = "SQL error: ";
		message += sqlite3_errmsg(db);
		message += ". SQL command: " + sql_cmd;
		sqlite3_free(zErrMsg);
		error_exit(message);
	}
}

} // END_DISMOD_AT_NAMESPACE
