// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin error_exit$$
$spell
   sqlite
   const
   std
$$

$section Printing and Logging Fatal Error Messages$$

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
and to standard error.

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

$head assert$$
An assertion is generated before exiting, incase we are running in debug mode.

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
# include <cppad/utility/to_string.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/configure.hpp>

namespace {
   // initial value corresponding to not initialized
   // 2DO: this is not thread safe
   sqlite3* db_previous_ = DISMOD_AT_NULL_PTR;
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void error_exit(
   const std::string& message      ,
   const std::string& table_name   ,
   const size_t&      row_id       )
{
   sqlite3* db = db_previous_;

   // check that if table_name is empty, row_id is null
   assert( table_name != "" || row_id == DISMOD_AT_NULL_SIZE_T );

   // db should never be null
   assert( db != DISMOD_AT_NULL_PTR );

   // write to standard error and log table
   std::string message_type = "error";
   log_message(db, &std::cerr, message_type, message, table_name, row_id);
   //
   // close the database
   sqlite3_close(db);
   //
   // if running in debugger, stop here
   assert(false);
   //
   // now exit program
   std::exit(1);
}
void error_exit(
   const std::string& message      ,
   const std::string& table_name   )
{  size_t      row_id     = DISMOD_AT_NULL_SIZE_T;
   error_exit(message, table_name, row_id);
   return;
}
void error_exit(
   const std::string& message      )
{  std::string table_name = "";
   error_exit(message, table_name);
   return;
}

// Turn error logging on and off using this call
void error_exit(sqlite3* db)
{  db_previous_ = db; }

} // END_DISMOD_AT_NAMESPACE
