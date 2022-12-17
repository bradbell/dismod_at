// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin censor_var_limit$$
$spell
   var
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section Censor Variables to be Within Lower and Upper Limits$$

$head Syntax$$
$codei%censor_var_limit(
   %var_out%, %var_in%, %var2prior%, %prior_table%
)%$$

$head Prototype$$
$srcthisfile%
   0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1
%$$

$head lower, upper$$
Let $icode lower$$ ($icode upper$$)
denote a vector in $cref pack_info$$ order
containing the corresponding variable lower (upper) limits.

$head var_out$$
This vector must have size $icode%var2prior%.size()%$$.
The input value of its elements does not matter.
Upon return,
$codei%
   if %var_in%[%i%] < %lower%[%i%] then
      %var_out%[%i%] = %lower%[%i%]
   else if %upper%[%i%] < %var_in%[%i%] then
      %var_out%[%i%] = %upper%[%i%]
   else
      %var_out%[%i%] = %var_in%[%i%]
%$$

$head var_in$$
This the variable values before censoring.
It is OK if $icode var_in$$ is the same vector as $icode var_out$$.

$head var2prior$$
This maps each variable index to the corresponding prior.

$head prior_table$$
This is the prior table.

$end
*/
# include <dismod_at/censor_var_limit.hpp>
# include <dismod_at/null_int.hpp>
# include <cppad/utility/nan.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
void censor_var_limit(
   CppAD::vector<double>&                    var_out       ,
   const CppAD::vector<double>&              var_in        ,
   const pack_prior&                         var2prior     ,
   const CppAD::vector<prior_struct>&        prior_table   )
// END_PROTOTYPE
{  //
   assert( var_out.size() == var2prior.size() );
   assert( var_in.size() == var2prior.size() );
   //
   for(size_t var_id = 0; var_id < var_in.size(); ++var_id)
   {  double const_value      = var2prior.const_value(var_id);
      size_t value_prior_id   = var2prior.value_prior_id(var_id);
      double lower = const_value;
      double upper = const_value;
      if( CppAD::isnan(const_value) )
      {  assert( value_prior_id != DISMOD_AT_NULL_SIZE_T );
         lower = prior_table[value_prior_id].lower;
         upper = prior_table[value_prior_id].upper;
      }
      assert( lower <= upper );
      double value    = var_in[var_id];
      value           = std::max(value, lower);
      value           = std::min(value, upper);
      var_out[var_id] = value;
   }
   return;
}

} // END DISMOD_AT_NAMESPACE
