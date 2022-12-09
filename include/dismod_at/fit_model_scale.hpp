// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_FIT_MODEL_SCALE_HPP
# define DISMOD_AT_FIT_MODEL_SCALE_HPP

/*
{xrst_begin fit_model_scale}
{xrst_spell
   unscale
}

fit_model Class Scaling of Fixed Effects
########################################

Syntax
******

| *fit_object* . ``scale_fixed_effect`` ( *fixed_before* , *fixed_after* )
| *fit_object* . ``unscale_fixed_effect`` ( *fixed_after* , *fixed_before* )

Prototype
*********
{xrst_literal
   // BEGIN_SCALE_FIXED_EFFECT
   // END_SCALE_FIXED_EFFECT
}
{xrst_literal
   // BEGIN_UNSCALE_FIXED_EFFECT
   // END_UNSCALE_FIXED_EFFECT
}

n_fixed\_
*********
This ``fit_model`` member variable
is the number of fixed effects in this model.

fixed_is_scaled\_
*****************
This ``fit_model`` member variable
is a vector with length ``n_fixed_`` .
The *j*-th component of fixed effects is scaled
if and only if *fixed_is_scaled_* [ ``j`` ] is true.

fixed_scale_eta\_
*****************
This ``fit_model`` member variable
is a vector with length *n_fixed_* .
If *fixed_is_scaled_* [ ``j`` ] is true,
*fixed_scale_eta_* [ ``j`` ] is the value of *eta*
used to scale this component of the fixed effects vector.

fixed_before
************
This vector contains the value of the fixed effects before the scaling.

fixed_after
***********
This vector contains the value of the fixed effects before the scaling.

Result
******
The ``scale_fixed_effect`` (``unscale_fixed_effect`` ) function
sets *fixed_after* ( *fixed_before* ) so that
for *j* = 0 ,..., ``n_fixed_-1`` ,

| |tab| ``if`` ( ``fixed_is_scaled_`` [ *j* ] )
| |tab| |tab| *fixed_after* [ *j* ] == ``log`` ( *fixed_before* [ *j* ] + ``fixed_is_scaled_`` [ *j* ])
| |tab| ``else``
| |tab| |tab| *fixed_after* [ *j* ] == *fixed_before* [ *j* ]

Note that this equality is only true up to numerical precision.

{xrst_end fit_model_scale}
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
