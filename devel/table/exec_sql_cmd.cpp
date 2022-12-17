// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin exec_sql_cmd$$
$spell
   sql
   dismod
   cmd
   std
   hpp
   sqlite
   sep
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section C++: Execution of an SQL Command$$

$head Syntax$$
$codei%exec_sql_cmd(%db%, %sql_cmd%)
%$$
$icode%result% = exec_sql_cmd(%db%, %sql_cmd%, %sep%)%$$

$head Purpose$$
Executes an SQL command, check for an error,
and optionally return result if no error occurs.

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

$head sep$$
If this argument is present, it has prototype
$codei%
   char %sep%
%$$
and is the character is used to separate columns on the same line of the result
($code '\n'$$ is used to separate lines of the result).
It is assumed that $icode sep$$ and $code '\n'$$ do not occur
in any of the result values.

$head result$$
If the $icode sep$$ argument is present,
the return value has prototype
$codei%
   std::string& result
%$$
It contains the output corresponding to the command.

$head Error$$
If an error occurs, an error message is printed and this routine
aborts; i.e., it does not return if an error occurs.

$end
-----------------------------------------------------------------------------
*/
# include <cstdlib>
# include <cstring>
# include <iostream>
# include <cassert>

# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/error_exit.hpp>


namespace {
   // see https://stackoverflow.com/questions/1805982/use-of-sqlite3-exec
   int exec_sql_cmd_callback(
      void*  callback_arg ,
      int    argc         ,
      char** argv         ,
      char** column       )
   {  std::string* result = reinterpret_cast<std::string*>(callback_arg);
      char sep = (*result)[0];
      //
      for(int i = 0; i < argc; ++i)
      {
         if( i > 0 )
            *result += sep;
         if( argv[i] != DISMOD_AT_NULL_PTR )
         {  assert( std::strchr( argv[i], sep  ) == DISMOD_AT_NULL_PTR );
            assert( std::strchr( argv[i], '\n' ) == DISMOD_AT_NULL_PTR );
            *result += argv[i];
         }
      }
      *result += "\n";
      //
      return 0;
   }
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

std::string exec_sql_cmd(sqlite3* db, const std::string& sql_cmd, char sep)
{  // initialize result as just the separator character
   std::string result;
   result += sep;
   void* callback_arg = reinterpret_cast<void *>(& result);
   //
   char* zErrMsg                              = DISMOD_AT_NULL_PTR;
   int (*callback)(void*, int, char**,char**) = exec_sql_cmd_callback;
   int rc = sqlite3_exec(
      db,
      sql_cmd.c_str(),
      callback,
      callback_arg,
      &zErrMsg
   );
   if( rc )
   {  assert(zErrMsg != DISMOD_AT_NULL_PTR );
      std::string message = "SQL error: ";
      message += sqlite3_errmsg(db);
      message += ". SQL command: " + sql_cmd;
      sqlite3_free(zErrMsg);
      error_exit(message);
   }
   //
   // remove the separator character from the beginning of the result
   assert( result[0] == sep );
   return result.substr(1);
}

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
   {  assert(zErrMsg != DISMOD_AT_NULL_PTR );
      std::string message = "SQL error: ";
      message += sqlite3_errmsg(db);
      message += ". SQL command: " + sql_cmd;
      sqlite3_free(zErrMsg);
      error_exit(message);
   }
}

} // END_DISMOD_AT_NAMESPACE
