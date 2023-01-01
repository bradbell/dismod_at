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
$begin set_command$$
$spell
   sim
   var
   dismod
   init
   avgint
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section Directly Setting Table Values$$

$head Syntax$$
$codei%dismod_at %database% set option %name% %value%
%$$
$comment The syntax cases below are implementd in the dismod_at main program
$$
$codei%dismod_at %database% set avgint data
%$$
$codei%dismod_at %database% set %table_out% %source%
%$$
$codei%dismod_at %database% set %table_out% %source% %sample_index%
%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head option$$
This documentation is for the case where
$code option$$ follows $code set$$.

$subhead name$$
Is the name of the option we are setting.
It must be a valid
$cref/option_name/option_table/Table Format/option_name/$$.

$subhead value$$
Is the new
$cref/option_value/option_table/Table Format/option_value/$$
for the specified option name.
If $icode value$$ is the empty string, null is used for the corresponding
value in the database; see
$cref/text can be null, not empty/database/Null/Text can be Null, not Empty/$$.

$subhead init$$
The set option commands does not require the $code init$$ to be run first.
In addition, if $icode option_name$$ is not $code parent_node_id$$,
the $cref model_variables$$ do not change and you do not have to
re-run the $cref init_command$$.

$head avgint$$
This documentation is for the case where
$code avgint$$ follows $code set$$.
In this case $code data$$ must follow $code avgint$$
and the $cref avgint_table$$ is set to be equal to the $cref data_table$$.
The only difference is that the column name
$cref/data_id/data_table/data_id/$$
in the data table has column name
$cref/avgint_id/avgint_table/avgint_id/$$
in the avgint table.

$head table_out$$
The $icode table_out$$ cases set the values in $icode table_out$$
using the values specified by $icode source$$.
If this table exists before the command,
the values originally in the table are lost.

$subhead start_var$$
If $icode table_out$$ is $code start_var$$,
the $cref/start_var/start_var_table/$$ table is created.
Note that this table may also be created directly by the user
(with the aid of the $cref var_table$$).

$subhead scale_var$$
If $icode table_out$$ is $code scale_var$$,
the $cref/scale_var/scale_var_table/$$ table is created.
Note that this table may also be created directly by the user
(with the aid of the $cref var_table$$).

$subhead truth_var$$
If $icode table_out$$ is $code truth_var$$,
the $cref/truth_var/truth_var_table/$$ table is created.
Note that this table may also be created directly by the user
(with the aid of the $cref var_table$$).

$head source$$
The set command $icode source$$ must be one of the
possibilities listed below
(and not be the same as $icode table_out$$).
Only the case where $icode source$$ is $icode sample$$
has the extra argument $icode sample_index$$.

$subhead sample$$
If $icode source$$ is $code sample$$,
$icode sample_index$$ must be present.
In this case the $cref model_variables$$ in the sample table,
and corresponding to the specified sample index,
are used for the values in $icode table_out$$.

$subhead prior_mean$$
If $icode source$$ is $code prior_mean$$,
the mean of the priors is used for the values in $icode table_out$$.

$subhead fit_var$$
If $icode source$$ is $code fit_var$$,
the results of the previous fit is used for the values in $icode table_out$$.

$subhead start, scale, truth$$
If $icode source$$ is
$cref/start_var/start_var_table/$$,
$cref/scale_var/scale_var_table/$$, or
$cref/truth_var/truth_var_table/$$,
the contents of this table are copied to $icode table_out$$.

$children%example/get_started/set_command.py
%$$
$head Example$$
The files
$cref set_command.py$$ and $cref set_command.py$$
contains examples and tests using this command.

$end
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
