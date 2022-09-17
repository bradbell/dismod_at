// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_FIT_MODEL_SCALE_HPP
# define DISMOD_AT_FIT_MODEL_SCALE_HPP

/*
$begin fit_model_scale$$
$spell
   unscale
$$

$section fit_model Class Scaling of Fixed Effects$$

$head Syntax$$
$icode%fit_object%.scale_fixed_effect(%fixed_before%, %fixed_after%)
%$$
$icode%fit_object%.unscale_fixed_effect(%fixed_after%, %fixed_before%)
%$$

$head Prototype$$
$srcthisfile%
   0%// BEGIN_SCALE_FIXED_EFFECT%// END_SCALE_FIXED_EFFECT%1
%$$
$srcthisfile%
   0%// BEGIN_UNSCALE_FIXED_EFFECT%// END_UNSCALE_FIXED_EFFECT%1
%$$

$head n_fixed_$$
This $code fit_model$$ member variable
is the number of fixed effects in this model.

$head fixed_is_scaled_$$
This $code fit_model$$ member variable
is a vector with length $code n_fixed_$$.
The $th j$$ component of fixed effects is scaled
if and only if $icode%fixed_is_scaled_[%j%]%$$ is true.

$head fixed_scale_eta_$$
This $code fit_model$$ member variable
is a vector with length $icode n_fixed_$$.
If $icode%fixed_is_scaled_[%j%]%$$ is true,
$icode%fixed_scale_eta_[%j%]%$$ is the value of $icode eta$$
used to scale this component of the fixed effects vector.

$head fixed_before$$
This vector contains the value of the fixed effects before the scaling.

$head fixed_after$$
This vector contains the value of the fixed effects before the scaling.

$head Result$$
The $code scale_fixed_effect$$ ($code unscale_fixed_effect$$) function
sets $icode fixed_after$$ ($icode fixed_before$$) so that
for $icode%j% = 0 ,%...%, n_fixed_-1%$$,
$codei%
   if( fixed_is_scaled_[%j%] )
      %fixed_after%[%j%] == log(%fixed_before%[%j%] + fixed_is_scaled_[%j%])
   else
      %fixed_after%[%j%] == %fixed_before%[%j%]
%$$
Note that this equality is only true up to numerical precision.

$end
*/
# include <dismod_at/fit_model.hpp>

// BEGIN_SCALE_FIXED_EFFECT
template <class Float>
void dismod_at::fit_model::scale_fixed_effect(
   const CppAD::vector<Float>& fixed_before  ,
   CppAD::vector<Float>&       fixed_after ) const
// END_SCALE_FIXED_EFFECT
{  assert( fixed_before.size()  == n_fixed_ );
   assert( fixed_after.size() == n_fixed_ );
   for(size_t j = 0; j < n_fixed_; j++)
   {  if( fixed_is_scaled_[j] )
         fixed_after[j] = log( fixed_before[j] + fixed_scale_eta_[j] );
      else
         fixed_after[j] = fixed_before[j];
   }
}
// BEGIN_UNSCALE_FIXED_EFFECT
template <class Float>
void dismod_at::fit_model::unscale_fixed_effect(
   const CppAD::vector<Float>& fixed_after  ,
   CppAD::vector<Float>&       fixed_before ) const
// END_UNSCALE_FIXED_EFFECT
{  assert( fixed_after.size()  == n_fixed_ );
   assert( fixed_before.size() == n_fixed_ );
   for(size_t j = 0; j < n_fixed_; j++)
   {  if( fixed_is_scaled_[j] )
         fixed_before[j] = exp( fixed_after[j] ) - fixed_scale_eta_[j];
      else
         fixed_before[j] = fixed_after[j];
   }
}

# endif
