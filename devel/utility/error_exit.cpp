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
$begin error_exit$$
$spell
	sqlite
	const
	std
$$

$section Log and Print an Error Message and then Exit$$

$head Syntax$$
$codei%error_exit(%db%)
%$$
$codei%error_exit(%message%)
%$$
$codei%error_exit(%message%, %table_name%)
%$$
$codei%error_exit(%message%, %table_name%, %row_id%)%$$

$head db$$
This argument has prototype
$codei%
	sqlite3* %db%
%$$

$head message$$
This argument has prototype
$codei%
	const std::string& %message%
%$$
This value gets written to
standard error (and an end of line is printed after it).

$subhead null$$
If the previous $icode db$$ was a the null pointer
(or there was no previous $icode db$$), an assert is generated.
It is not the null pointer,
the value $icode message$$ is also written to the
$cref/message/log_table/message/$$ column of the log table
(as well as to standard error).

$head table_name$$
This argument has prototype
$codei%
	const std::string& %table_name%
%$$
If $icode db$$ is not the null pointer, this value gets written in the
$cref/table_name/log_table/table_name/$$ column of the log table.
If $icode table_name$$ it is not present, the empty string is used.
If $icode table_name$$ is not empty, it is also written
(with a label) to standard error.

$head row_id$$
This argument has prototype
$codei%
	const size_t& %row_id%
%$$
If $icode db$$ is not the null pointer, this value gets written in the
$cref/row_id/log_table/row_id/$$ column of the log table.
Note that the value $code DISMOD_AT_NULL_SIZE_T$$
gets converted to a $code null$$.
If $icode row_id$$ is not present, $code null$$ is used.
If $icode row_id$$ is present and not $code DISMOD_AT_NULL_SIZE_T$$,
$icode table_name$$ must not be empty.
In this case, $icode row_id$$ is also written (with a label) to standard error.

$end
-----------------------------------------------------------------------------
*/
# include <cstdlib>
# include <ctime>
# include <cassert>
# include <iostream>
# include <iostream>
# include <sqlite3.h>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/get_column_max.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/configure.hpp>

namespace {
	sqlite3* db_previous_ = DISMOD_AT_NULL_PTR;
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void error_exit(
	const std::string& message      ,
	const std::string& table_name   ,
	const size_t&      row_id       )
{	using std::string;
	using std::cerr;
	using std::endl;
	sqlite3* db = db_previous_;

	// check assumption one table_name and row_id columns of log
	assert( table_name != "" || row_id == DISMOD_AT_NULL_SIZE_T );

	// write to log table
	string message_type = "error";
	if( db != DISMOD_AT_NULL_PTR )
	{	log_message(db, message_type, message, table_name, row_id);
		sqlite3_close(db);
	}

	// write to standard error
	cerr << "Error: " << message << endl;
	if( table_name != "" )
	{	cerr << "Detected in table " << table_name;
		if( row_id != DISMOD_AT_NULL_SIZE_T )
			cerr << " in row with " << table_name << "_id = " << row_id;
		cerr << endl;
	}
	std::exit(1);
}
void error_exit(
	const std::string& message      ,
	const std::string& table_name   )
{	size_t      row_id     = DISMOD_AT_NULL_SIZE_T;
	error_exit(message, table_name, row_id);
	return;
}
void error_exit(
	const std::string& message      )
{	std::string table_name = "";
	error_exit(message, table_name);
	return;
}

void error_exit(sqlite3* db)
{	db_previous_ = db; }

} // END_DISMOD_AT_NAMESPACE
