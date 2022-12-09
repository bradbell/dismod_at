// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <cppad/mixed/exception.hpp>
# include <dismod_at/predict_command.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_sample_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/censor_var_limit.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-------------------------------------------------------------------------------
{xrst_begin predict_command}
{xrst_spell
   withing
}

The Predict Command
###################

Syntax
******
``dismod_at`` *database* ``predict`` *source*

database
********
Is an
http://www.sqlite.org/sqlite/ database containing the
``dismod_at`` :ref:`input-name` tables which are not modified.

source
******
This argument specifies where model variable values to use
for the predictions. The possible values are listed below:

sample
======
If *source* is ``sample`` ,
the values in the :ref:`sample_table-name` are used for the predictions.
In this case there are
:ref:`simulate_command@number_simulate` sets
of model variables that predictions are computed for.
If the samples were simulated using the
:ref:`sample_command@asymptotic` method,
they may not be withing the lower and upper limits for the
corresponding variables.
The variables are censored to be within their limits before
the predictions are computed.

fit_var
=======
If *source* is ``fit_var`` ,
the values in the :ref:`fit_var_table-name` are used for the predictions.
In this case there is only one set of model variables that the
predictions are computed for and
:ref:`predict_table@sample_index` is always zero.

truth_var
=========
If *source* is ``truth_var`` ,
the values in the :ref:`truth_var_table-name` are used for the predictions.
In this case there is only one set of model variables that the
predictions are computed for and
:ref:`predict_table@sample_index` is always zero.

predict_table
*************
A new :ref:`predict_table-name` is created each time this command is run.
It contains the
:ref:`average integrand<avg_integrand@Average Integrand, A_i>`
values for set of model variables
and each
:ref:`predict_table@avgint_id`
in the
:ref:`predict_table@Avgint Subset` .
{xrst_toc_hidden
   example/get_started/predict_command.py
}
Example
*******
The files :ref:`predict_command.py-name` and
:ref:`user_predict_fit.py-name` contain examples and tests
using this command.

{xrst_end predict_command}
*/

// ----------------------------------------------------------------------------
void predict_command(
   const std::string&                                    source              ,
   sqlite3*                                              db                  ,
   const dismod_at::db_input_struct&                     db_input            ,
   size_t                                                n_var               ,
   dismod_at::data_model&                                avgint_object       ,
   const CppAD::vector<dismod_at::avgint_subset_struct>& avgint_subset_obj   ,
   const pack_prior&                                     var2prior
)
{
   using std::string;
   using CppAD::vector;
   using CppAD::to_string;
   //
   if( source != "sample"
   &&  source != "fit_var"
   &&  source != "truth_var" )
   {  string msg  = "dismod_at predict command source = ";
      msg        += source + " is not one of the following: ";
      msg        += "sample, fit_var, truth_var";
      dismod_at::error_exit(msg);
   }
   // ------------------------------------------------------------------------
   // variable_value
   vector<double> variable_value;
   string table_name = source;
   string column_name;
   if( source == "sample" )
      column_name = "var_value";
   else if( source == "fit_var" )
      column_name = "fit_var_value";
   else
      column_name = "truth_var_value";
   dismod_at::get_table_column(
      db, table_name, column_name, variable_value
   );
   size_t n_sample = variable_value.size() / n_var;
   assert( n_sample * n_var == variable_value.size() );
# ifndef NDEBUG
   // ------------------------------------------------------------------------
   // check sample table
   if( source == "sample" )
   {  // sample table
      vector<dismod_at::sample_struct> sample_table =
         dismod_at::get_sample_table(db);
      size_t sample_id = 0;
      for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
      {  for(size_t var_id = 0; var_id < n_var; var_id++)
         {  size_t sample_check =
               size_t( sample_table[sample_id].sample_index);
            size_t var_check =
               size_t( sample_table[sample_id].var_id);
            if( sample_check != sample_index || var_check != var_id )
            {  string msg = "database modified, restart with init command";
               table_name = "sample";
               dismod_at::error_exit(msg, table_name, sample_id);
            }
            ++sample_id;
         }
      }
   }
# endif
   // -----------------------------------------------------------------------
   // create a new predict table
   string sql_cmd = "drop table if exists predict";
   dismod_at::exec_sql_cmd(db, sql_cmd);
   //
   table_name = "predict";
   size_t n_col      = 3;
   size_t n_subset   = avgint_subset_obj.size();
   size_t n_row      = n_sample * n_subset;
   vector<string> col_name(n_col), col_type(n_col), row_value(n_col * n_row);
   vector<bool>   col_unique(n_col);
   //
   col_name[0]   = "sample_index";
   col_type[0]   = "integer";
   col_unique[0] = false;
   //
   col_name[1]   = "avgint_id";
   col_type[1]   = "integer";
   col_unique[1] = false;
   //
   col_name[2]   = "avg_integrand";
   col_type[2]   = "real";
   col_unique[2] = false;
   //
   // pack_vec
   vector<double> pack_vec(n_var);
   //
   size_t sample_id = 0;
   for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
   {  // copy the variable values for this sample index into pack_vec
      for(size_t var_id = 0; var_id < n_var; var_id++)
         pack_vec[var_id] = variable_value[sample_id++];
      //
      // censor samples to be within limits
      censor_var_limit(
         pack_vec,
         pack_vec,
         var2prior,
         db_input.prior_table
      );
      //
      for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
      {
         int avgint_id  = avgint_subset_obj[subset_id].original_id;
         double avg     = 0.0;
         try
         {  avg = avgint_object.average(subset_id, pack_vec);
         }
         catch(const std::exception& e)
         {  string message("predict_command: std::exception: ");
            message += e.what();
            dismod_at::error_exit(message);
         }
         catch(const CppAD::mixed::exception& e)
         {  string catcher    = "predict_command";
            string message    = e.message(catcher);
            table_name        = "avgint";
            dismod_at::error_exit(message, table_name, avgint_id);
         }
         //
         size_t predict_id = sample_index * n_subset + subset_id;
         if( source == "sample" )
            row_value[n_col * predict_id + 0] = to_string( sample_index );
         else
            row_value[n_col * predict_id + 0] = "";
         row_value[n_col * predict_id + 1] = to_string( avgint_id );
         row_value[n_col * predict_id + 2] = to_string( avg );
      }
   }
   dismod_at::create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   return;
}
} // END_DISMOD_AT_NAMESPACE
