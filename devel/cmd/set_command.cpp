// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <dismod_at/set_command.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/get_table_column.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/create_table.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
{xrst_begin set_command}

Directly Setting Table Values
#############################

Syntax
******

   ``dismod_at`` *database* ``set option`` *name* *value*

{xrst_comment The syntax cases below are implementd in the dismod_at main program
}

| ``dismod_at`` *database* ``set avgint data``
| ``dismod_at`` *database* ``set`` *table_out* *source*
| ``dismod_at`` *database* ``set`` *table_out* *source* *sample_index*

database
********
Is an
http://www.sqlite.org/sqlite/ database containing the
``dismod_at`` :ref:`input-name` tables which are not modified.

option
******
This documentation is for the case where
``option`` follows ``set`` .

name
====
Is the name of the option we are setting.
It must be a valid
:ref:`option_table@Table Format@option_name` .

value
=====
Is the new
:ref:`option_table@Table Format@option_value`
for the specified option name.
If *value* is the empty string, null is used for the corresponding
value in the database; see
:ref:`database@Null@Text can be Null, not Empty` .

init
====
The set option commands does not require the ``init`` to be run first.
In addition, if *option_name* is not ``parent_node_id`` ,
the :ref:`model_variables-name` do not change and you do not have to
re-run the :ref:`init_command-name` .

avgint
******
This documentation is for the case where
``avgint`` follows ``set`` .
In this case ``data`` must follow ``avgint``
and the :ref:`avgint_table-name` is set to be equal to the :ref:`data_table-name` .
The only difference is that the column name
:ref:`data_table@data_id`
in the data table has column name
:ref:`avgint_table@avgint_id`
in the avgint table.

table_out
*********
The *table_out* cases set the values in *table_out*
using the values specified by *source* .
If this table exists before the command,
the values originally in the table are lost.

start_var
=========
If *table_out* is ``start_var`` ,
the :ref:`start_var<start_var_table-name>` table is created.
Note that this table may also be created directly by the user
(with the aid of the :ref:`var_table-name` ).

scale_var
=========
If *table_out* is ``scale_var`` ,
the :ref:`scale_var<scale_var_table-name>` table is created.
Note that this table may also be created directly by the user
(with the aid of the :ref:`var_table-name` ).

truth_var
=========
If *table_out* is ``truth_var`` ,
the :ref:`truth_var<truth_var_table-name>` table is created.
Note that this table may also be created directly by the user
(with the aid of the :ref:`var_table-name` ).

source
******
The set command *source* must be one of the
possibilities listed below
(and not be the same as *table_out* ).
Only the case where *source* is *sample*
has the extra argument *sample_index* .

sample
======
If *source* is ``sample`` ,
*sample_index* must be present.
In this case the :ref:`model_variables-name` in the sample table,
and corresponding to the specified sample index,
are used for the values in *table_out* .

prior_mean
==========
If *source* is ``prior_mean`` ,
the mean of the priors is used for the values in *table_out* .

fit_var
=======
If *source* is ``fit_var`` ,
the results of the previous fit is used for the values in *table_out* .

start, scale, truth
===================
If *source* is
:ref:`start_var<start_var_table-name>` ,
:ref:`scale_var<scale_var_table-name>` , or
:ref:`truth_var<truth_var_table-name>` ,
the contents of this table are copied to *table_out* .
{xrst_toc_hidden
   example/get_started/set_command.py
}
Example
*******
The files
:ref:`set_command.py-name` and :ref:`set_command.py-name`
contains examples and tests using this command.

{xrst_end set_command}
*/
// ----------------------------------------------------------------------------
void set_option_command(
   sqlite3*                                        db           ,
   const CppAD::vector<option_struct>&             option_table ,
   const std::string&                              name         ,
   const std::string&                              value        )
{  using std::string;
   using CppAD::vector;

   // option_table contains all the valid option names with default values
   // for the names that are not in the option table, check name is valid
   bool found = false;
   for(size_t i = 0; i < option_table.size(); ++i)
      found |= option_table[i].option_name == name;
   if( ! found )
   {  string msg  = "set option name value: ";
      msg += "name = " + name + " is not a valid option name\n";
      error_exit(msg);
   }
   string table_name  = "option";
   string column_name = "option_name";
   CppAD::vector<string> option_name;
   get_table_column(db, table_name, column_name, option_name);
   found = false;
   for(size_t i = 0; i < option_name.size(); i++)
      found |= option_name[i] == name;
   string sql_cmd;
   if( found )
   {  sql_cmd  = "update option set option_value =";
      if( value == "" )
         sql_cmd += " null ";
      else
         sql_cmd += " '" + value + "' ";
      sql_cmd += "where option_name = '" + name + "'";
      exec_sql_cmd(db, sql_cmd);
   }
   else
   {  string option_id = CppAD::to_string( option_name.size() );
      sql_cmd  = "insert into option ";
      sql_cmd += "values ('" + option_id + "','";
      if( value == "" )
         sql_cmd += name + "', null )";
      else
         sql_cmd += name + "','" + value + "')";
      exec_sql_cmd(db, sql_cmd);
   }
   //
   return;
}
// ----------------------------------------------------------------------------
void set_avgint_command(sqlite3* db )
{  using std::string;
   using CppAD::vector;
   //
   // remove old avgint
   string sql_cmd = "drop table if exists avgint";
   exec_sql_cmd(db, sql_cmd);
   //
   // information for data table
   sql_cmd  = "pragma table_info(data)";
   char sep = ',';
   string data_info = exec_sql_cmd(db, sql_cmd, sep);
   //
   // Create empty avgint talble with same columns as data except that
   // primary key is avgint_id instead of data_id
   size_t start = 0;
   char   eol   = '\n';
   size_t line  = 0;
   sql_cmd      = "create table avgint (\n";
   while( start < data_info.size() )
   {  // cid
      size_t stop = data_info.find(sep, start);
      string sub  = data_info.substr(start, stop - start);
      assert( size_t( std::atoi( sub.c_str() ) ) == line  );
      // name
      start    = stop + 1;
      stop     = data_info.find(sep, start);
      sub      = data_info.substr(start, stop - start);
      if( line == 0 )
         sql_cmd += "avgint_id ";
      else
         sql_cmd += ",\n" + sub + " ";
      // name
      start = stop + 1;
      stop  = data_info.find(sep, start);
      sub   = data_info.substr(start, stop - start);
      if( line == 0 )
         sql_cmd += "integer primary key";
      else
         sql_cmd += sub;
      // notnull
      start = stop + 1;
      stop  = data_info.find(sep, start);
      sub   = data_info.substr(start, stop - start);
      assert( sub == "0" );
      // default
      start = stop + 1;
      stop  = data_info.find(sep, start);
      sub   = data_info.substr(start, stop - start);
      assert( sub == "" );
      // primary
      start = stop + 1;
      stop  = data_info.find(eol, start);
      sub   = data_info.substr(start, stop - start);
      assert( line != 0 || sub == "1" );
      assert( line == 0 || sub == "0" );
      //
      start = stop + 1;
      ++line;
   }
   sql_cmd += "\n)";
   exec_sql_cmd(db, sql_cmd);
   //
   // copy values from data table to avgint table
   sql_cmd = "insert into avgint select * from data";
   exec_sql_cmd(db, sql_cmd);
   //
   return;
}
// ----------------------------------------------------------------------------
void set_command(
   const std::string&                     table_out    ,
   const std::string&                     source       ,
   const std::string&                     sample_index ,
   sqlite3*                               db           ,
   const CppAD::vector<double>&           prior_mean   )
{  using std::string;
   using CppAD::to_string;
   using CppAD::vector;
   //
   // string used for error messaging
   string msg;
   //
   if( table_out != "start_var"
   &&  table_out != "scale_var"
   &&  table_out != "truth_var" )
   {  msg  = "dismod_at set command table_out = " + table_out;
      msg += "\nis not one of the following: ";
      msg += "start_var, scale_var, truth_var";
      error_exit(msg);
   }
   if( source != "sample"
   &&  source != "prior_mean"
   &&  source != "fit_var"
   &&  source != "start_var"
   &&  source != "scale_var"
   &&  source != "truth_var" )
   {  msg  = "dismod_at set command source = ";
      msg += source + " is not one of the following: ";
      msg += "sample, prior_mean, fit_var, start_var, scale_var, truth_var";
      error_exit(msg);
   }
   if( source == "sample" && sample_index == "" )
   {  msg  = "dismod_at set command source = sample";
      msg += " sample_index is missing";
      error_exit(msg);
   }
   if( source == table_out )
   {  msg  = "dismod_at set command table_out and source are the same";
      error_exit(msg);
   }
   //
   string sql_cmd = "drop table if exists ";
   sql_cmd       += table_out;
   exec_sql_cmd(db, sql_cmd);
   //
   // create table_out
   string table_name_out = table_out;
   size_t n_var          = prior_mean.size();
   vector<string> col_name(1), col_type(1), row_value(n_var);
   vector<bool>   col_unique(1);
   col_name[0]       = table_out + "_value";
   col_type[0]       = "real";
   col_unique[0]     = false;
   //
   if( source == "sample" )
   {  // index
      size_t index = size_t ( std::atoi( sample_index.c_str() ) );
      //
      // var_value
      vector<double> var_value;
      string table_name_in  = "sample";
      string column_name    = "var_value";
      get_table_column(
         db, table_name_in, column_name, var_value
      );
      // n_sample
      if( var_value.size() % n_var != 0 )
      {  msg  = "sample table size not a multiple of number of variables";
         error_exit(msg);
      }
      size_t n_sample = var_value.size() / n_var;
      if( n_sample <= index )
      {  msg  = "dismod_at set command: sample_index >= number of samples";
         error_exit(msg);
      }
      //
      // put in row_value
      for(size_t var_id = 0; var_id < n_var; var_id++)
      {  size_t sample_id = index * n_var + var_id;
            row_value[var_id] = to_string(var_value[sample_id]);
      }
   }
   else if( source == "prior_mean" )
   {  for(size_t var_id = 0; var_id < n_var; var_id++)
         row_value[var_id] = CppAD::to_string( prior_mean[var_id] );
   }
   else
   {  vector<double> var_value;
      string table_name_in  = source;
      string column_name    = source + "_value";
      get_table_column(
         db, table_name_in, column_name, var_value
      );
      //
      // put it in row_value
      for(size_t var_id = 0; var_id < n_var; var_id++)
         row_value[var_id] = to_string( var_value[var_id] );
   }
   create_table(
      db, table_name_out, col_name, col_type, col_unique, row_value
   );
   return;
}

} // END_DISMOD_AT_NAMESPACE
