// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin variable_limits dev}

Determine Limits in Variable ID Order
#####################################

Syntax
******

| ``get_var_limits`` (
| |tab| ``lower_bound`` ,
| |tab| ``upper_bound`` ,
| |tab| ``bound_random`` ,
| |tab| ``var2prior`` ,
| |tab| ``prior_table``
| )

Prototype
*********
{xrst_literal
   // BEGIN PROTOTYPE
   // END PROTOTYPE
}

lower_limit, upper_limit
************************
*lower_limit* ( *upper_limit* )
is a vector with size zero or equal to the number of :ref:`model_variables-name` .
The input value of its elements does not matter.
Upon return it contains the lower (upper) limit for each variable.
This include the prior limits and the :ref:`bnd_mulcov_table-name` limits.

bound_random
************
This is the value of the
:ref:`option_table@Optimize Random Only@bound_random`
in the option table.

var2prior
*********
This argument is the :ref:`pack_prior-name` information corresponding to the
:ref:`model_variables-name` .

prior_table
***********
This argument is the :ref:`get_prior_table@prior_table` .

{xrst_end variable_limits}
*/
# include <dismod_at/get_var_limits.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at {
   // BEGIN PROTOTYPE
   void get_var_limits(
      CppAD::vector<double>&               lower_limit  ,
      CppAD::vector<double>&               upper_limit  ,
       const pack_prior&                    var2prior    ,
      const CppAD::vector<prior_struct>&   prior_table  )
   // END PROTOTYPE
   {
      double inf   = std::numeric_limits<double>::infinity();
      size_t n_var = var2prior.size();
      std::string msg;
      //
      assert( n_var == lower_limit.size() );
      assert( n_var == upper_limit.size() );
      //
      for(size_t var_id = 0; var_id < n_var; ++var_id)
      {  size_t prior_id    = var2prior.value_prior_id(var_id);
         double const_value = var2prior.const_value(var_id);
         if( ! std::isnan(const_value ) )
         {  lower_limit[var_id] = const_value;
            upper_limit[var_id] = const_value;
         }
         else if( prior_id == DISMOD_AT_NULL_SIZE_T )
         {  lower_limit[var_id] = -inf;
            upper_limit[var_id] = +inf;
         }
         else
         {
            lower_limit[var_id] = prior_table[prior_id].lower;
            upper_limit[var_id] = prior_table[prior_id].upper;
         }
         // enforce the max_abs table values
         double lower        = lower_limit[var_id];
         double upper        = upper_limit[var_id];
         double max_abs      = var2prior.max_abs(var_id);
         if( upper < - max_abs )
         {  msg  = "var_id = " + CppAD::to_string(var_id);
            msg += ": variable upper limit < - maximum absolute value\n";
            error_exit(msg);
         }
         if(  max_abs < lower )
         {  msg  = "var_id = " + CppAD::to_string(var_id);
            msg += ": maximum absolute value < variable lower limit\n";
            error_exit(msg);
         }
         if( lower < upper )
         {  lower_limit[var_id] = std::max(lower, - max_abs);
            upper_limit[var_id] = std::min(upper, max_abs);
         }
      }
   }
}
