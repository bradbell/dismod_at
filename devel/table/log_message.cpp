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
$begin log_message$$
$spell
	sqlite
	const
	std
	bool
$$

$section Put a Message in the Log Table$$

$head Syntax$$
$codei%log_message(%db%, %message_type%, %message%)
%$$

$head db$$
This argument has prototype
$codei%
	sqlite3* %db%
%$$
and is the database connection.

$head message_type$$
This argument has prototype
$codei%
	const std::string& %message_type%
%$$
and must be one of the following:
$code comamnd$$, $code error$$.
This value gets written in the
$cref/message_type/log_table/message_type/$$ column of the log table.

$head message$$
This argument has prototype
$codei%
	const std::string& %message%
%$$
This value gets written in the
$cref/message/log_table/message/$$ column of the log table.

$head Example$$
Check the $code log$$ table in the database after any
$cref command$$.

$end
-----------------------------------------------------------------------------
*/
# include <ctime>
# include <cassert>
# include <dismod_at/log_message.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/get_column_max.hpp>
# include <dismod_at/to_string.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void log_message(
	sqlite3*           db           ,
	const std::string& message_type ,
	const std::string& message      )
{	using std::string;

	// check message type
	assert( message_type == "command" || message_type == "error" );

	string sql_cmd = "create table if not exists log("
		" log_id              integer primary key,"
		" message_type        text,"
		" year                integer,"
		" month               integer,"
		" day                 integer,"
		" hour                integer,"
		" minute              integer,"
		" second              integer,"
		" message             text"
		");";
	dismod_at::exec_sql_cmd(db, sql_cmd);

	// determine next primary key value
	string select_cmd  = "select * from log";
	string column_name = "log_id";
	string max_str     = get_column_max(db, select_cmd, column_name);
	size_t log_id      = 0;
	if( max_str != "" )
		log_id = std::atoi( max_str.c_str() ) + 1;

	// get time
	time_t rawtime;
	struct std::tm* tm_ptr;
	std::time( &rawtime );
	tm_ptr = std::gmtime( &rawtime );

	// add this message to the log file
	sql_cmd  = "insert into log values ( ";
	sql_cmd += to_string( log_id );
	sql_cmd += " , '";
	sql_cmd += message_type;
	sql_cmd += "' , ";
	sql_cmd += to_string( tm_ptr->tm_year + 1900 );
	sql_cmd += " , ";
	sql_cmd += to_string( tm_ptr->tm_mon + 1 );
	sql_cmd += " , ";
	sql_cmd += to_string( tm_ptr->tm_mday );
	sql_cmd += " , ";
	sql_cmd += to_string( tm_ptr->tm_hour );
	sql_cmd += " , ";
	sql_cmd += to_string( tm_ptr->tm_min );
	sql_cmd += " , ";
	sql_cmd += to_string( tm_ptr->tm_sec );
	sql_cmd += " , '";
	sql_cmd += message;
	sql_cmd += "' );";
	dismod_at::exec_sql_cmd(db, sql_cmd);

	return;
}

} // END_DISMOD_AT_NAMESPACE
