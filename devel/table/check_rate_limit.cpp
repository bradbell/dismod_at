// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin check_rate_limit dev}

Check Lower and Upper Iota and Rho Limits
#########################################

syntax
******

| ``check_rate_limit`` (
| |tab| *db* , *rate_case* , *rate_table* , *prior_table* , *smooth_grid*
| )

db
**
This argument has prototype

   ``sqlite3`` * *db*

and is the database connection for :ref:`logging<log_message-name>` errors.

rate_case
*********
This is the option table information about the limits for
*iota* and *rho* ; see
:ref:`option_table@rate_case` .

rate_table
**********
This argument has prototype

   ``const CppAD::vector<rate_struct>&`` *rate_table*

and it is the
:ref:`get_rate_table@rate_table` .
For this table,
only the ``parent_smooth_id`` column is used.

prior_table
***********
This argument has prototype

   ``const CppAD::vector<prior_struct>&`` *prior_table*

and it is the
:ref:`get_prior_table@prior_table` .
For this table, only the ``lower`` and ``upper`` fields are used.

smooth_grid
***********
This argument has prototype

   ``const CppAD::vector<smooth_grid_struct>&`` *smooth_grid*

and it is the
:ref:`smooth_grid_table<get_smooth_grid@smooth_grid>` .
For this table, only the ``value_prior_id`` field is used.

{xrst_end check_rate_limit}
*/
# include <dismod_at/check_rate_limit.hpp>
# include <dismod_at/error_exit.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void check_rate_limit(
   sqlite3*                                  db            ,
   const std::string&                        rate_case     ,
   const CppAD::vector<rate_struct>&         rate_table    ,
   const CppAD::vector<prior_struct>&        prior_table   ,
   const CppAD::vector<smooth_grid_struct>&  smooth_grid   )
{  assert( rate_table.size()   == number_rate_enum );
   using CppAD::to_string;
   //
   CppAD::vector<bool> rate_zero(number_rate_enum);
   if( rate_case == "no_ode" || rate_case == "trapezoidal" )
   {  // no restrictions on the rates
      return;
   }
   else if( rate_case == "iota_zero_rho_zero" )
   {  rate_zero[iota_enum] = true;
      rate_zero[rho_enum]  = true;
   }
   else if( rate_case == "iota_pos_rho_zero" )
   {  rate_zero[iota_enum] = false;
      rate_zero[rho_enum]  = true;
   }
   else if( rate_case == "iota_zero_rho_pos" )
   {  rate_zero[iota_enum] = true;
      rate_zero[rho_enum]  = false;
   }
   else if( rate_case == "iota_pos_rho_pos" )
   {  rate_zero[iota_enum] = false;
      rate_zero[rho_enum]  = false;
   }
   else
   {  assert(false);
   }
   size_t n_grid = smooth_grid.size();
   size_t error_rate_id     = number_rate_enum;
   size_t error_smooth_id   = 0;
   size_t error_grid_id     = 0;
   size_t error_prior_id    = 0;
   double error_const_value = 0.0;
   bool   error_zero       = false;
   for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
   if( rate_enum(rate_id) == iota_enum || rate_enum(rate_id) == rho_enum )
   {  size_t smooth_id = rate_table[rate_id].parent_smooth_id;
      if( smooth_id == DISMOD_AT_NULL_SIZE_T )
      {  if( ! rate_zero[rate_id] )
         {  std::string msg = "parent_smooth_id for this rate is null";
            msg += "\nbut this rate is expected to be positive.";
            std::string table_name = "rate";
            error_exit(msg, table_name, rate_id);
         }
      }
      else
      {  for(size_t grid_id = 0; grid_id < n_grid; grid_id++)
         if( size_t( smooth_grid[grid_id].smooth_id ) == smooth_id )
         {  double lower    = smooth_grid[grid_id].const_value;
            double upper    = smooth_grid[grid_id].const_value;
            size_t prior_id = smooth_grid[grid_id].value_prior_id;
            if( prior_id != DISMOD_AT_NULL_SIZE_T )
            {  assert( std::isnan(lower) );
               lower    = prior_table[prior_id].lower;
               upper    = prior_table[prior_id].upper;
            }
            bool error;
            if( rate_zero[rate_id] )
               error = upper != 0.0;
            else
               error = lower <= 0.0;
            if( error )
            {
               error_rate_id     = rate_id;
               error_smooth_id   = smooth_id;
               error_grid_id     = grid_id;
               error_prior_id    = prior_id;
               error_const_value = smooth_grid[grid_id].const_value;
               error_zero        = rate_zero[rate_id];
            }
         }
      }
   }
   if( error_rate_id < number_rate_enum )
   {
      std::string message;
      if( error_zero)
         message = "expected prior upper limit to be == 0.0 for";
      else
         message = "expected prior lower limit to be > 0.0 for";
      if( rate_enum(error_rate_id) == iota_enum )
         message += " iota\n";
      else
         message += " rho\n";
      message += "rate_case = " + rate_case;
      message += ", parent_smooth_id = " + to_string(error_smooth_id);
      message += "\nsmooth_grid_id = " + to_string(error_grid_id);
      if( error_prior_id == DISMOD_AT_NULL_SIZE_T )
         message += ", const_value = " + to_string(error_const_value);
      else
         message += ", value_prior_id = " + to_string(error_prior_id);
      std::string table_name = "rate";
      error_exit(message, table_name, error_rate_id);
   }
}

} // END DISMOD_AT_NAMESPACE
