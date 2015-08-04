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
$begin error_exit$$
$spell
	sqlite
	const
	std
$$

$section Log and Print an Error Message and then Exit$$

$head Syntax$$
$codei%error_exit(%db%, %message%)
%$$
$codei%error_exit(%db%, %message%, %table_name%, %row_id%)%$$

$head db$$
This argument has prototype
$codei%
	sqlite3* %db%
%$$
and is the database connection where the log entry is written.
This connection is closed using
$codei%
	sqlite3_close(%db%)
%$$
before the program exits.

$head message$$
This argument has prototype
$codei%
	const std::string& %message%
%$$
This value gets written in the
$cref/message/log_table/message/$$ column of the log table
and to standard error (and an end of line is printed after it).

$head table_name$$
This argument has prototype
$codei%
	const std::string& %table_name%
%$$
This value gets written in the
$cref/table_name/log_table/table_name/$$ column of the log table.
If $icode table_name$$ it is not present, the empty string is used.
If $icode table_name$$ is not empty, it is also written
(with a label) to standard error.

$head row_id$$
This argument has prototype
$codei%
	const size_t& %row_id%
%$$
This value gets written in the
$cref/row_id/log_table/row_id/$$ column of the log table.
Note that the value $code size_t(DISMOD_AT_NULL_INT)$$
gets converted to a $code null$$.
If $icode row_id$$ is not present, $code null$$ is used.
If $icode row_id$$ is present and not $code size_t(DISMOD_AT_NULL_INT)$$,
$icode table_name$$ must not be empty.
In this case, $icode row_id$$ is also written (with a label) to standard error.

$end
-----------------------------------------------------------------------------
*/
# include <cstdlib>
# include <ctime>
# include <cassert>
# include <iostream>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/get_column_max.hpp>
# include <dismod_at/to_string.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void error_exit(
	sqlite3*           db           ,
	const std::string& message      ,
	const std::string& table_name   ,
	const size_t&      row_id       )
{	using std::string;
	using std::cerr;
	using std::endl;

	// check assumption one table_name and row_id columns of log
	assert( table_name != "" || row_id == size_t( DISMOD_AT_NULL_INT ) );

	// write to log table
	string message_type = "error";
	log_message(db, message_type, message, table_name, row_id);

	// write to standard error
	cerr << "Error: " << message << endl;
	if( table_name != "" )
	{	cerr << "Detected in table " << table_name;
		if( row_id != size_t(DISMOD_AT_NULL_INT) )
			cerr << " in row with " << table_name << "_id = " << row_id;
		cerr << endl;
	}
	sqlite3_close(db);
	std::exit(1);
}
void error_exit(
	sqlite3*           db           ,
	const std::string& message      )
{	std::string table_name = "";
	size_t      row_id     = size_t( DISMOD_AT_NULL_INT );
	error_exit(db, message, table_name, row_id);
	return;
}

} // END_DISMOD_AT_NAMESPACE
