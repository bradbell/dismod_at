// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <gsl/gsl_randist.h>
# include <cppad/utility/to_string.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/get_mulcov_table.hpp>
# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/get_bnd_mulcov_table.hpp>
# include <dismod_at/bnd_mulcov_command.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
{xrst_begin bnd_mulcov_command}
{xrst_spell
   mul
}

Bound The Covariate Multiplier Absolute Data Effect
###################################################

Syntax
******

| ``dismod_at`` *database* ``bnd_mulcov`` *max_abs_effect*
| ``dismod_at`` *database* ``bnd_mulcov`` *max_abs_effect* *covariate_name*

Purpose
*******
This command is used to set the maximum absolute effect
in the model for the data values.
This is done by changing the lower and upper bounds
for the covariate multipliers (ignoring bounds in the corresponding priors).
The :ref:`mulcov_table@mulcov_type@meas_noise` multipliers and
:ref:`model_variables@Random Effects, u@Subgroup Covariate Multipliers`
are not included.
The subgroup covariate multipliers are random effects and
:ref:`option_table@Optimize Random Only@bound_random`
set the absolute bound for all the random effects.

database
********
Is an
http://www.sqlite.org/sqlite/ database containing the
``dismod_at`` :ref:`input-name` tables which are not modified.

max_abs_effect
**************
is either ``inf`` (for infinity) or
a non-negative value that bounds absolute covariate effects.
A covariate multiplier is defined by each row of the :ref:`mulcov_table-name` .
We use the notation *mul_value* for a value of the multiplier.
We use the notation *cov_value* for a value of the
:ref:`covariate<data_table@Covariates>` in the data table.
We use the notation *cov_ref* for the
:ref:`covariate_table@reference` for the covariate.
The maximum effect condition is

   | *mul_value* * ( *cov_value* ``-`` *cov_ref* ) | <= *max_abs_effect*

Note that the limits on the covariate multiplier in its prior have units
and the *max_abs_effect* does not have units.

covariate_name
**************
If this argument is present, it is a
:ref:`covariate_table@covariate_name` .
In this case, the inequality above only refers to
covariate multipliers that use this covariate.

bnd_mulcov_table
****************
The table :ref:`bnd_mulcov_table-name` is an input and output for this command.

max_cov_diff
============
The :ref:`bnd_mulcov_table@max_cov_diff` column is not changed.

max_mulcov
==========
The :ref:`bnd_mulcov_table@max_mulcov` column is set so the
inequality above is true for all the data that is modeled using this
covariate multiplier and that is included in the fit.
To be specific, for each covariate multiplier

   *max_mulcov* = *max_abs_effect* / *max_cov_diff*

If *covariate_name* is present,
*max_mulcov* the bound is only changed
for multipliers that use that covariate.
The *max_mulcov* value for
:ref:`mulcov_table@mulcov_type@meas_noise` covariates
are not changed.

Infinite Case
*************
The case where *max_abs_effect* is ``inf`` or
*max_cov_diff* is zero,
*max_mulcov* is set to null (which corresponds to plus infinity).

{xrst_comment 2DO: create the user_bnd_mulcov.py example}
Example
*******
The file ``user_bnd_mulcov.py`` contains an example and test
using this command.

{xrst_end bnd_mulcov_command}
*/
void bnd_mulcov_command(
   sqlite3*                                      db                ,
   const std::string&                            max_abs_effect    ,
   const std::string&                            covariate_name    ,
   const CppAD::vector<covariate_struct>&        covariate_table   ,
   const CppAD::vector<mulcov_struct>&           mulcov_table      )
{  using std::string;
   using CppAD::vector;
   using CppAD::to_string;
   double inf = std::numeric_limits<double>::infinity();
   //
   // max_abs_effect
   double max_effect = inf;
   if( max_abs_effect != "inf" )
      max_effect = std::atoi( max_abs_effect.c_str() );
   //
   // covariate_id
   int covariate_id = DISMOD_AT_NULL_INT;
   if( covariate_name != "" )
   {  for(size_t id = 0; id < covariate_table.size(); ++id)
      {  if( covariate_name == covariate_table[id].covariate_name )
            covariate_id = int(id);
      }
      if( covariate_id == DISMOD_AT_NULL_INT )
      {  string msg = "bnd_mulcov_command: covariate_name = ";
         msg       += covariate_name + " is not in covariate table";
         error_exit(msg);
      }
   }
   //
   // n_mulcov
   size_t n_mulcov    = mulcov_table.size();
   //
   // get the current bnd_mulcov table
   vector<bnd_mulcov_struct> bnd_mulcov_table = get_bnd_mulcov_table(db);
   //
   // drop old bnd_mulcov table
   string sql_cmd    = "drop table bnd_mulcov";
   exec_sql_cmd(db, sql_cmd);
   //
   // write new data_subset table
   string table_name = "bnd_mulcov";
   size_t n_col      = 2;
   vector<string> col_name(n_col), col_type(n_col);
   vector<string> row_value(n_col * n_mulcov);
   vector<bool>   col_unique(n_col);
   //
   col_name[0]       = "max_cov_diff";
   col_type[0]       = "real";
   col_unique[0]     = false;
   //
   col_name[1]       = "max_mulcov";
   col_type[1]       = "real";
   col_unique[1]     = false;
   //
   for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
   {  // max_cov_diff  does not change
      double max_cov_diff = bnd_mulcov_table[mulcov_id].max_cov_diff;
      row_value[n_col * mulcov_id + 0] = to_string( max_cov_diff );
      //
      bool change = covariate_id == DISMOD_AT_NULL_INT;
      change     |= covariate_id == mulcov_table[mulcov_id].covariate_id;
      change     &= mulcov_table[mulcov_id].mulcov_type != meas_noise_enum;
      if( ! change )
      {  double max_mulcov = bnd_mulcov_table[mulcov_id].max_mulcov;
         if( std::isnan( max_mulcov ) )
            row_value[n_col * mulcov_id + 1] = "";
         else
            row_value[n_col * mulcov_id + 1] = to_string( max_mulcov );
      }
      else
      {  if( max_cov_diff == 0.0 || max_effect == inf )
            row_value[n_col * mulcov_id + 1] = "";
         else
         {  double max_mulcov = max_effect / max_cov_diff;
            row_value[n_col * mulcov_id + 1] = to_string( max_mulcov );
         }
      }
   }
   create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   return;
}
} // END_DISMOD_AT_NAMESPACE
