// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <dismod_at/get_prior_mean.hpp>
# include <dismod_at/null_int.hpp>
/*
{xrst_begin get_prior_mean}

Get Prior Mean For Each Variable
################################

Syntax
******

| *prior_mean* = ``get_prior_mean`` (
| |tab| *prior_table* , *pack_prior* , *var2prior*
| )

Prototype
*********
{xrst_literal
   // BEGIN PROTOTYPE
   // END PROTOTYPE
}

prior_table
***********
Is the :ref:`get_prior_table@prior_table` .

var2prior
*********
Is the prior information as a mapping from variable index to
prior table index.

prior_mean
**********
The return value *prior_main*
is the prior mean for each variable by
:ref:`var_table@var_id` .

{xrst_end get_prior_mean}
*/
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN PROTOTYPE
CppAD::vector<double> get_prior_mean(
   const CppAD::vector<dismod_at::prior_struct>& prior_table ,
   const dismod_at::pack_prior&                  var2prior   )
// END PROTOTYPE
{
   // put means in return value
   size_t n_var = var2prior.size();
   CppAD::vector<double> result(n_var);
   for(size_t var_id = 0; var_id < n_var; var_id++)
   {  double var_value = var2prior.const_value(var_id);
      if( std::isnan(var_value) )
      {  size_t prior_id = var2prior.value_prior_id(var_id);
         assert( prior_id != DISMOD_AT_NULL_SIZE_T );
         var_value = prior_table[prior_id].mean;
      }
      result[var_id] = var_value;
   }
   return result;
}

} // END_DISMOD_AT_NAMESPACE
