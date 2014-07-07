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
$codei%# include <dismod_at/exec_sql_cmd.hpp>
%$$
$codei%dismod_at::exec_sql_cmd(%db%, %sql_cmd%)%$$

$head Purpose$$
Executes an SQL command and check for an error.

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

# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void exec_sql_cmd(sqlite3* db, const std::string& sql_cmd)
{	using std::cerr;
	using std::endl;
	char* zErrMsg                              = nullptr;
	int (*callback)(void*, int, char**,char**) = nullptr;
	void* callback_arg                         = nullptr;
	int rc = sqlite3_exec(
		db, 
		sql_cmd.c_str(), 
		callback, 
		callback_arg, 
		&zErrMsg
	);
	if( rc )
	{	assert(zErrMsg != nullptr );
		cerr << "SQL command: " << sql_cmd << endl;
		cerr << "SQL error:   " << sqlite3_errmsg(db) << endl;
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		exit(1);
	}
}

} // END_DISMOD_AT_NAMESPACE
