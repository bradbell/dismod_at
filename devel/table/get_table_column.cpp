// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
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

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
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
If there is no column named $icode column_name$$ in the table
named $icode table_name$$, $icode column_type$$ is the empty string.
Otherwise its value is either $code text$$, $code integer$$, or $code real$$
depending on the type of the column in the database.

$head result$$
The input size of this vector must be zero.
Upon return it contains the values in the specified column.
The results are ordered using the $cref/primary key/database/Primary Key/$$
for this table.

$subhead text$$
If the column has type $code text$$, this argument has
prototype
$codei%
   CppAD::vector<std::string>& %result%
%$$
If a text value is $code null$$,
it is returned as the empty string.
It is an error for any of the text values
in the database to be the empty string.

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
# include <dismod_at/exec_sql_cmd.hpp>

namespace {
   using std::string;

   // Type specified by sqlite3_exec
   typedef int (*callback_type)(void*, int, char**, char**);

   // set by get_column, used by convert
   string   table_name_;
   string   column_name_;

   const char* convert(const std::string& not_used, char* v, size_t row_id)
   {  if( v == DISMOD_AT_NULL_PTR )
         return "";
      if( *v == '\0' )
      {  string msg = "The empty string appears in the text column ";
         msg += column_name_;
         dismod_at::error_exit(msg, table_name_, row_id);
      }
      return v;
   }
   int    convert(const int& not_used, char* v, size_t row_id)
   {  if(  v == DISMOD_AT_NULL_PTR )
         return DISMOD_AT_NULL_INT;
      //
      int value = std::atoi(v);
      //
      // no integer values should be the minimum integer
      if( value == DISMOD_AT_NULL_INT )
      {  string msg = "The minimum integer appears in the int column ";
         msg += column_name_;
         dismod_at::error_exit(msg, table_name_, row_id);
      }
      //
      return value;
   }
   double convert(const double& not_used, char* v, size_t row_id)
   {  if( v == DISMOD_AT_NULL_PTR )
         return std::numeric_limits<double>::quiet_NaN();
      double value = std::atof(v);
      if( value != value )
      {  string msg = "The value nan appears in the double column ";
         msg += column_name_;
         dismod_at::error_exit(msg, table_name_, row_id);
      }
      return value;
   }

   template <class Element>
   int callback(void *result, int argc, char **argv, char **azColName)
   {  typedef CppAD::vector<Element> vector;
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
      {  assert(zErrMsg != DISMOD_AT_NULL_PTR );
         std::string message = "SQL error: ";
         message += sqlite3_errmsg(db);
         message += ". SQL command: " + cmd;
         sqlite3_free(zErrMsg);
         dismod_at::error_exit(message);
      }
      return;
   }
}


namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

std::string get_table_column_type(
   sqlite3*           db          ,
   const std::string& table_name  ,
   const std::string& column_name )
{  // check if column exists
   char sep = ',';
   std::string sql_cmd = "PRAGMA table_info(" + table_name + ")";
   std::string sql_ret = exec_sql_cmd(db, sql_cmd, sep);
   std::string column_type = "";
   size_t next_line       = 0;
   while( next_line < sql_ret.size() && column_type == "" )
   {  // start of next column name in this table
      size_t start = sql_ret.find(sep, next_line) + 1;
      size_t stop  = sql_ret.find(sep, start);
      std::string column = sql_ret.substr(start, stop - start);
      if( column == column_name )
      {  start        = stop + 1;
         stop         = sql_ret.find(sep, start);
         column_type  = sql_ret.substr(start, stop - start);
         assert( stop > start );
      }
      //
      next_line = sql_ret.find('\n', next_line) + 1;
   }
   if( column_type == "" )
      return column_type;
   //
   // sqlite seems to use upper case for its types
   for(size_t i = 0; i < column_type.size(); i++)
      column_type[i] = char( std::tolower( column_type[i] ) );
   //
   // sqlite seems to use int for its integer type
   if( column_type == "int" )
      column_type = "integer";
   //
   bool ok = column_type == "integer";
   ok |= column_type == "text";
   ok |= column_type == "real";
   if( ! ok )
   {  string msg = "Column " + column_name + " has type " + column_type
         + "\nwhich is not one of the following: integer, real, or text";
      dismod_at::error_exit(msg, table_name_);
   }

   return column_type;
}


void get_table_column(
   sqlite3*                    db                  ,
   const std::string&          table_name          ,
   const std::string&          column_name         ,
   CppAD::vector<std::string>& text_result         )
{  // for error message
   size_t null_id = DISMOD_AT_NULL_SIZE_T;

   // set globals used by error messages
   table_name_ = table_name;
   column_name_ = column_name;

   // check that initial vector is empty
   assert( text_result.size() == 0 );

   // check the type for this column
   std::string col_type = get_table_column_type(db, table_name, column_name);
   if( col_type != "text" )
   {  std::string msg = "get_table_column for column = " + column_name;
      msg += " in table " + table_name + ".\n";
      if( col_type == "" )
         msg += "Could not find table or column in table.";
      else
         msg += "Expected type to be text not " + col_type;
      error_exit(msg, table_name, null_id);
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
{  // for error message
   size_t null_id = DISMOD_AT_NULL_SIZE_T;

   // set globals used by error messages
   table_name_ = table_name;
   column_name_ = column_name;

   // check that initial vector is empty
   assert( int_result.size() == 0 );

   // check the type for this column
   std::string col_type = get_table_column_type(db, table_name, column_name);
   if( col_type != "integer" )
   {  std::string msg = "get_table_column for column = " + column_name;
      msg += " in table " + table_name + ".\n";
      if( col_type == "" )
         msg += "Could not find table or column in table.";
      else
         msg += "Expected type to be integer not " + col_type;
      error_exit(msg, table_name, null_id);
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
{  // for error message
   size_t null_id = DISMOD_AT_NULL_SIZE_T;

   // set globals used by error messages
   table_name_ = table_name;
   column_name_ = column_name;

   // check that initial vector is empty
   assert( double_result.size() == 0 );

   // check the type for this column
   std::string col_type = get_table_column_type(db, table_name, column_name);
   if( col_type != "real" )
   {  std::string msg = "get_table_column for column = " + column_name;
      msg += " in table " + table_name + ".\n";
      if( col_type == "" )
         msg += "Could not find table or column in table.";
      else
         msg += "Expected type to be real not " + col_type;
      error_exit(msg, table_name, null_id);
   }

   // Use template function for rest
   get_column(db, table_name, column_name, double_result);

   return;
}

} // END DISMOD_AT_NAMESPACE
