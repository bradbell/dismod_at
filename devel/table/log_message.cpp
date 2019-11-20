// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
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
	ostream
$$

$section Put a Message in the Log Table$$

$head Syntax$$
$icode%unix_time% = log_message(%db%, %os%, %message_type%, %message%)
%$$
$icode%unix_time% = log_message(
	%db%, %os%, %message_type%, %message%, %table_name%, %row_id%
)%$$

$head db$$
This argument has prototype
$codei%
	sqlite3* %db%
%$$
and is the database connection.

$head os$$
This argument has prototype
$codei%
	std::ostream* %os%
%$$
If it is not null and
$icode message_type$$ is $code error$$ or $code warning$$,
the message is also written to the output stream
with a new line added at the end
(as well as being logged in the database).

$head message_type$$
This argument has prototype
$codei%
	const std::string& %message_type%
%$$
and must be one of the following:
$code command$$,
$code error$$,
$code warning$$,
$code value$$.
This value gets written in the
$cref/message_type/log_table/message_type/$$ column of the log table.

$head message$$
This argument has prototype
$codei%
	const std::string& %message%
%$$
This value gets written in the
$cref/message/log_table/message/$$ column of the log table.

$head table_name$$
This argument has prototype
$codei%
	const std::string& %table_name%
%$$
This value gets written in the
$cref/table_name/log_table/table_name/$$ column of the log table.
If $icode table_name$$ it is not present or empty, null is used.

$head row_id$$
This argument has prototype
$codei%
	const size_t& %row_id%
%$$
This value gets written in the
$cref/row_id/log_table/row_id/$$ column of the log table.
Note that the value $code DISMOD_AT_NULL_SIZE_T$$
gets converted to a $code null$$.
If $icode row_id$$ is not present, $code null$$ is used.
If $icode row_id$$ is present and not $code DISMOD_AT_NULL_SIZE_T$$,
$icode table_name$$ must not be empty.

$head unix_time$$
The return value has prototype
$codei%
	std::time_t %unix_time%
%$$
It is the value written in the log table for
$cref/unix_time/log_table/unix_time/$$.

$head Example$$
Check the $code log$$ table in the database after any
$cref command$$.

$end
-----------------------------------------------------------------------------
*/
# include <cstdlib>
# include <ctime>
# include <cassert>
# include <dismod_at/log_message.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/get_column_max.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/configure.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

std::time_t log_message(
	sqlite3*           db           ,
	std::ostream*      os           ,
	const std::string& message_type ,
	const std::string& message      ,
	const std::string& table_name   ,
	const size_t&      row_id       )
{	static bool recursive = false;
	using std::string;
	using CppAD::to_string;

	// check assumption one table_name and row_id columns of log
	assert( table_name != "" || row_id == DISMOD_AT_NULL_SIZE_T );

	// check message type
	assert(
		message_type == "command" ||
		message_type == "error"   ||
		message_type == "warning" ||
		message_type == "value"
	);
	// get time
	std::time_t unix_time = std::time( DISMOD_AT_NULL_PTR );
	//
	// write errors and warnings to standard error
	bool write2os = os != DISMOD_AT_NULL_PTR;
	write2os &= (message_type == "error" || message_type == "warning" );
	if( write2os )
	{	*os << "dismod_at " << message_type << ": " << message << std::endl;
		if( table_name != "" )
		{	*os << "detected in " << table_name << " table";
			if( row_id != DISMOD_AT_NULL_SIZE_T )
				*os << " in row with " << table_name << "_id = " << row_id;
			*os << std::endl;
		}
	}
	//
	if( ! recursive )
	{	recursive = true;
		//
		string sql_cmd = "create table if not exists log("
			" log_id              integer primary key,"
			" message_type        text,"
			" table_name          text,"
			" row_id              integer,"
			" unix_time           integer,"
			" message             text"
			");";
		dismod_at::exec_sql_cmd(db, sql_cmd);

		// message with single quotes escaped
		string escaped_message = "";
		for(size_t i = 0; i < message.size(); i++)
		{	if( message[i] == '\'' )
				escaped_message += "''";
			else
				escaped_message += message[i];
		}

		// determine next primary key value
		string select_cmd  = "select * from log";
		string column_name = "log_id";
		string max_str     = get_column_max(db, select_cmd, column_name);
		size_t log_id      = 0;
		if( max_str != "" )
			log_id = std::atoi( max_str.c_str() ) + 1;

		// add this message to the log file
		sql_cmd  = "insert into log values ( ";
		sql_cmd += to_string( log_id );
		sql_cmd += " , '";
		sql_cmd += message_type;
		if( table_name  == "" )
			sql_cmd += "' , null,";
		else
			sql_cmd += "' , '" + table_name + "', ";
		if( row_id == DISMOD_AT_NULL_SIZE_T )
			sql_cmd += "null";
		else
			sql_cmd += to_string( row_id );
		sql_cmd += " , ";
		sql_cmd += to_string( unix_time );
		sql_cmd += " , '";
		sql_cmd += escaped_message;
		sql_cmd += "' );";
		dismod_at::exec_sql_cmd(db, sql_cmd);
	}
	recursive = false;
	//
	return unix_time;
}
std::time_t log_message(
	sqlite3*           db           ,
	std::ostream*      os           ,
	const std::string& message_type ,
	const std::string& message      )
{	std::string table_name = "";
	size_t      row_id     = DISMOD_AT_NULL_SIZE_T;
	return log_message(db, os, message_type, message, table_name, row_id);
}

} // END_DISMOD_AT_NAMESPACE
