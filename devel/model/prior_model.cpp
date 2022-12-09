// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin prior_model_ctor dev}

Construct Fixed Negative Log-Likelihood Object
##############################################

Syntax
******

| ``prior_model`` *prior_object* (
| |tab| *pack_object* , *var2prior* , *age_table* , *time_table* , *prior_table*
| )

prior_object
************
This object has prototype

   ``prior_model`` *prior_object*

pack_object
***********
This argument has prototype

   ``const pack_info&`` *pack_object*

and is the :ref:`pack_info-name` information corresponding to the
:ref:`model_variables-name` .

var2prior
*********
This argument has prototype

   ``const pack_prior&`` *var2prior*

and is the :ref:`pack_prior-name` information corresponding to the
:ref:`model_variables-name` .

age_table
*********
This argument has prototype

   ``const CppAD::vector<double>&`` *age_table*

and is the :ref:`get_age_table@age_table` .

time_table
**********
This argument has prototype

   ``const CppAD::vector<double>&`` *time_table*

and is the :ref:`get_time_table@time_table` .

prior_table
***********
This argument has prototype

   ``const CppAD::vector<prior_struct>&`` *prior_table*

and is the :ref:`get_prior_table@prior_table` .
Only to following fields are used
:ref:`prior_table@density_id` ,
:ref:`prior_table@mean` ,
:ref:`prior_table@std` ,
:ref:`prior_table@eta` ,
:ref:`prior_table@nu` .

{xrst_end prior_model_ctor}
*/

# include <utility>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/residual_density.hpp>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/smooth_info.hpp>
# include <dismod_at/a1_double.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

prior_model::prior_model(
   const pack_info&                       pack_object     ,
   const pack_prior&                      var2prior       ,
   const CppAD::vector<double>&           age_table       ,
   const CppAD::vector<double>&           time_table      ,
   const CppAD::vector<prior_struct>&     prior_table     ,
   const CppAD::vector<density_enum>&     density_table   )
:
pack_object_(pack_object)     ,
var2prior_(var2prior)         ,
age_table_(age_table)         ,
time_table_(time_table)       ,
prior_table_(prior_table)     ,
density_table_(density_table)
{  // Set prior_mean_ to values in prior table (default)
   //
   size_t n_var = var2prior_.size();
   prior_mean_.resize( 3 * n_var );
   for(size_t var_id = 0; var_id < n_var; ++var_id)
   {
      // value prior
      double const_value = var2prior_.const_value(var_id);
      size_t prior_id    = var2prior_.value_prior_id(var_id);
      if( ! std::isnan(const_value) )
      {  assert( prior_id == DISMOD_AT_NULL_SIZE_T );
         prior_mean_[var_id * 3 + 0] = const_value;
      }
      else
      {  assert( prior_id != DISMOD_AT_NULL_SIZE_T );
         prior_mean_[var_id * 3 + 0] = prior_table[prior_id].mean;
      }
      //
      // dage prior
      prior_id = var2prior_.dage_prior_id(var_id);
      if( prior_id == DISMOD_AT_NULL_SIZE_T )
         prior_mean_[var_id * 3 + 1] = 0.0;
      else
         prior_mean_[var_id * 3 + 1] = prior_table[prior_id].mean;
      //
      // dtime prior
      prior_id = var2prior_.dtime_prior_id(var_id);
      if( prior_id == DISMOD_AT_NULL_SIZE_T )
         prior_mean_[var_id * 3 + 1] = 0.0;
      else
         prior_mean_[var_id * 3 + 2] = prior_table[prior_id].mean;
   }
   return;
}

// private
template <class Float>
residual_struct<Float> prior_model::log_prior(
   const prior_struct&   prior      ,  // prior for this residual
   const Float&          mulstd     ,  // multiplies prior std
   const Float&          z          ,  // first random variable
   const Float&          y          ,  // second random variable
   size_t                index      ,  // the index for this residual
   bool                  difference ) const // is this a difference residual
{  assert ( 0 <= prior.density_id  );
   assert ( size_t(prior.density_id) < density_table_.size()  );

   density_enum density = density_table_[prior.density_id];
   Float        mu      = Float(prior.mean);
   Float        delta   = mulstd * Float(prior.std);
   Float        eta     = Float(prior.eta);
   Float        nu      = Float(prior.nu);
   bool prior_density = true;
   return residual_density(
      z, y, mu, delta, density, eta, nu, index, difference, prior_density
   );
}
/*
{xrst_begin replace_mean dev}

Replace Prior Means
###################

Syntax
******
*prior_object* . ``replace_mean`` ( *prior_mean* ) .

Prototype
*********
{xrst_literal
   // BEGIN_REPLACE_MEAN_PROTOTYPE
   // END_REPLACE_MEAN_PROTOTYPE
}

Purpose
*******
By default, the :ref:`prior_table@mean` values in the prior table
are used when computing the prior for the
:ref:`fixed<prior_fixed_effect-name>` and :ref:`random<prior_random_effect-name>` .
This routine can be used to replace these means
(which is useful for posterior :ref:`posterior@Simulation` ).

This object has prototype

   ``const prior_model`` *prior_object*

prior_mean
**********
This vector has size *n_var* * 3 where *n_var*
is the number of variables in the :ref:`var_table-name` .
For *var_id* equal zero to *n_var* ``-1`` ,

   *prior_mean* [ 3 * *var_id* + *k*  ]

replace the mean in the
:ref:`value prior<pack_prior@value_prior_id>` ( *k* =0 ),
:ref:`dage prior<pack_prior@dage_prior_id>` ( *k* =1 ),
:ref:`dtime prior<pack_prior@dtime_prior_id>` ( *k* =2 ).

{xrst_end replace_mean}
*/
// BEGIN_REPLACE_MEAN_PROTOTYPE
void prior_model::replace_mean(const CppAD::vector<double>& prior_mean)
// END_REPLACE_MEAN_PROTOTYPE
{  prior_mean_ = prior_mean;
}

/*
------------------------------------------------------------------------------
{xrst_begin prior_fixed_effect dev}

Evaluate Fixed Negative Log-Likelihood for the Fixed Effects
############################################################

Syntax
******
*residual_vec* = *prior_object* . ``fixed`` ( *pack_vec* )

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

prior_object
************
This object has prototype

   ``const prior_model`` *prior_object*

see :ref:`prior_object constructor<prior_model_ctor@prior_object>` .

pack_vec
********
This vector has prototype

   ``const CppAD::vector<`` *Float* >& *pack_vec*

and is all the :ref:`model_variables-name` in the order
specified by :ref:`pack_info-name` .

residual_vec
************
The return value has prototype

   ``CppAD::vector< residual_struct<`` *Float* > > *residual_vec*

The size of *residual* is not equal to the number of fixed effects
because there are priors on smoothing differences as well as values.
The order of the residuals is unspecified.

index
=====
The :ref:`residual_density@index` for each residual is
3 * *var_id* + *k* where
:ref:`var_table@var_id` is the variable index,
*k*  = 0 for value priors,
*k*  = 1 for age difference priors, and
*k*  = 2 for time difference priors.

p(theta)
********
The log of the fixed negative log-likelihood for the
:ref:`fixed effects<model_variables@Fixed Effects, theta>` ,
:math:`\log \; \B{p}( \theta )`,
is the sum of all the log densities corresponding to the
:ref:`residuals<residual_density-name>` .
{xrst_toc_hidden
   example/devel/model/prior_fixed_xam.cpp
}
Example
*******
The file :ref:`prior_fixed_xam.cpp-name` contains an example and test
of using this routine.

{xrst_end prior_fixed_effect}
*/
template <class Float>
CppAD::vector< residual_struct<Float> >
prior_model::fixed(const CppAD::vector<Float>& pack_vec ) const
{  Float nan = Float( std::numeric_limits<double>::quiet_NaN() );
   //
   // initialize the log of the fixed negative log-likelihood as zero
   CppAD::vector< residual_struct<Float> > residual_vec;
   assert( residual_vec.size() == 0 );
   //
   // for computing one residual
   residual_struct<Float> residual;

   // n_var
   assert( pack_vec.size()   == pack_object_.size() );
   assert( var2prior_.size() == pack_object_.size() );
   size_t n_var = pack_vec.size();

   for(size_t var_id = 0; var_id < n_var; ++var_id)
   {  // This variable value
      Float y = pack_vec[var_id];
      //
      // prior information
      size_t smooth_id      = var2prior_.smooth_id(var_id);
      size_t value_prior_id = var2prior_.value_prior_id(var_id);
      size_t dage_prior_id  = var2prior_.dage_prior_id(var_id);
      size_t dtime_prior_id = var2prior_.dtime_prior_id(var_id);
      bool   fixed_effect   = var2prior_.fixed_effect(var_id);
# ifndef NDEBUG
      double const_value = var2prior_.const_value(var_id);
      if( ! std::isnan(const_value) )
         assert( value_prior_id == DISMOD_AT_NULL_SIZE_T );
      else
         assert( value_prior_id != DISMOD_AT_NULL_SIZE_T );
# endif
      //
      if( smooth_id == DISMOD_AT_NULL_SIZE_T )
      {  // standard deviation multipliers are fixed effects and do not
         // have a smoothing, hence the following
         assert( fixed_effect );
         assert( value_prior_id != DISMOD_AT_NULL_SIZE_T );
         assert( dage_prior_id  == DISMOD_AT_NULL_SIZE_T );
         assert( dtime_prior_id == DISMOD_AT_NULL_SIZE_T );
         //
         // value prior
         prior_struct prior = prior_table_[value_prior_id];
         //
         // replacement for prior mean
         prior.mean = prior_mean_[var_id * 3 + 0];
         //
         // no standard deviation multiplier for this prior
         Float mulstd = Float(1.0);
         //
         // this is not a difference prior
         bool  difference  = false;
         Float z           = nan;
         //
         // index used for value priors
         size_t index              = 3 * var_id + 0;
         //
         // negative log-likelihood of value prior for this multiplier
         residual  = log_prior(prior, mulstd, z, y, index, difference);
         if( residual.density != uniform_enum )
            residual_vec.push_back(residual);
      }
      else if( fixed_effect )
      {  if( value_prior_id != DISMOD_AT_NULL_SIZE_T )
         {  prior_struct prior = prior_table_[value_prior_id];
            bool   difference = false;
            Float  z          = nan;
            size_t k          = 0;
            size_t index      = 3 * var_id + k;
            size_t offset     = pack_object_.mulstd_offset(smooth_id, k);
            Float mulstd      = Float(1.0);
            prior.mean        = prior_mean_[var_id * 3 + k];
            if( offset != DISMOD_AT_NULL_SIZE_T )
               mulstd        = pack_vec[offset];
            //
            residual = log_prior(prior, mulstd, z, y, index, difference);
            if( residual.density != uniform_enum )
               residual_vec.push_back(residual);
         }
         if( dage_prior_id != DISMOD_AT_NULL_SIZE_T )
         {  prior_struct prior = prior_table_[dage_prior_id];
            bool   difference = true;
            Float  z          = pack_vec[var2prior_.dage_var_id(var_id)];
            size_t k          = 1;
            size_t index      = 3 * var_id + k;
            size_t offset     = pack_object_.mulstd_offset(smooth_id, k);
            Float mulstd      = Float(1.0);
            prior.mean        = prior_mean_[var_id * 3 + k];
            if( offset != DISMOD_AT_NULL_SIZE_T )
               mulstd        = pack_vec[offset];
            //
            residual = log_prior(prior, mulstd, z, y, index, difference);
            if( residual.density != uniform_enum )
               residual_vec.push_back(residual);
         }
         if( dtime_prior_id != DISMOD_AT_NULL_SIZE_T )
         {  prior_struct prior = prior_table_[dtime_prior_id];
            bool   difference = true;
            Float  z          = pack_vec[var2prior_.dtime_var_id(var_id)];
            size_t k          = 2;
            size_t index      = 3 * var_id + k;
            size_t offset     = pack_object_.mulstd_offset(smooth_id, k);
            Float mulstd      = Float(1.0);
            prior.mean        = prior_mean_[var_id * 3 + k];
            if( offset != DISMOD_AT_NULL_SIZE_T )
               mulstd        = pack_vec[offset];
            //
            residual = log_prior(prior, mulstd, z, y, index, difference);
            if( residual.density != uniform_enum )
               residual_vec.push_back(residual);
         }
      }
   }
   return residual_vec;
}
/*
------------------------------------------------------------------------------
{xrst_begin prior_random_effect dev}

Evaluate Fixed Negative Log-Likelihood for the Random Effects
#############################################################

Syntax
******
*residual_vec* = *prior_object* . ``random`` ( *pack_vec* )

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

prior_object
************
This object has prototype

   ``const prior_model`` *prior_object*

see :ref:`prior_object constructor<prior_model_ctor@prior_object>` .

pack_vec
********
This vector has prototype

   ``const CppAD::vector<`` *Float* >& *pack_vec*

and is all the :ref:`model_variables-name` in the order
specified by :ref:`pack_info-name` .

residual_vec
************
The return value has prototype

   ``CppAD::vector< residual_struct<`` *Float* > > *residual_vec*

The size of *residual* is not equal to the number of random effects
because there are priors on smoothing differences as well as values.
The order of the residuals is unspecified (at this time).

index
=====
The :ref:`residual_density@index` for each residual is
3 * *var_id* + *k* where
:ref:`var_table@var_id` is the variable index,
*k*  = 0 for value priors,
*k*  = 1 for age difference priors, and
*k*  = 2 for time difference priors.

p(u|theta)
**********
The log of the fixed negative log-likelihood for the
and :ref:`random effects<model_variables@Random Effects, u>`
given the fixed effects,
:math:`\log \; \B{p}( u | \theta )`,
is the sum of the all the log densities corresponding to the
:ref:`residuals<residual_density-name>` .
{xrst_toc_hidden
   example/devel/model/prior_random_xam.cpp
}
Example
*******
The file :ref:`prior_random_xam.cpp-name` contains an example and test
of using this routine.

{xrst_end prior_random_effect}
*/
template <class Float>
CppAD::vector< residual_struct<Float> >
prior_model::random(const CppAD::vector<Float>& pack_vec ) const
{  Float nan = Float( std::numeric_limits<double>::quiet_NaN() );
   //
   // initialize the log of the fixed negative log-likelihood as zero
   CppAD::vector< residual_struct<Float> > residual_vec;
   assert( residual_vec.size() == 0 );
   //
   // for computing one residual
   residual_struct<Float> residual;

   // n_var
   assert( pack_vec.size()   == pack_object_.size() );
   assert( var2prior_.size() == pack_object_.size() );
   size_t n_var = pack_vec.size();

   for(size_t var_id = 0; var_id < n_var; ++var_id)
   {  // This variable value
      Float y = pack_vec[var_id];
      //
      // prior information
      size_t smooth_id      = var2prior_.smooth_id(var_id);
      size_t value_prior_id = var2prior_.value_prior_id(var_id);
      size_t dage_prior_id  = var2prior_.dage_prior_id(var_id);
      size_t dtime_prior_id = var2prior_.dtime_prior_id(var_id);
      bool   fixed_effect   = var2prior_.fixed_effect(var_id);
      //
      if( ! fixed_effect )
      {  assert( smooth_id != DISMOD_AT_NULL_SIZE_T );
         //
         if( value_prior_id != DISMOD_AT_NULL_SIZE_T )
         {
# ifndef NDEBUG
            double const_value    = var2prior_.const_value(var_id);
            assert( std::isnan(const_value)  );
# endif
            const prior_struct& prior = prior_table_[value_prior_id];
            bool   difference = false;
            Float  z          = nan;
            size_t k          = 0;
            size_t index      = 3 * var_id + k;
            size_t offset     = pack_object_.mulstd_offset(smooth_id, k);
            Float mulstd      = Float(1.0);
            if( offset != DISMOD_AT_NULL_SIZE_T )
               mulstd        = pack_vec[offset];
            //
            residual = log_prior(prior, mulstd, z, y, index, difference);
            if( residual.density != uniform_enum )
               residual_vec.push_back(residual);
         }
         if( dage_prior_id != DISMOD_AT_NULL_SIZE_T )
         {  const prior_struct& prior = prior_table_[dage_prior_id];
            bool   difference = true;
            Float  z          = pack_vec[var2prior_.dage_var_id(var_id)];

            size_t k          = 1;
            size_t index      = 3 * var_id + k;
            size_t offset     = pack_object_.mulstd_offset(smooth_id, k);
            Float mulstd      = Float(1.0);
            if( offset != DISMOD_AT_NULL_SIZE_T )
               mulstd        = pack_vec[offset];
            //
            residual = log_prior(prior, mulstd, z, y, index, difference);
            if( residual.density != uniform_enum )
               residual_vec.push_back(residual);
         }
         if( dtime_prior_id != DISMOD_AT_NULL_SIZE_T )
         {  const prior_struct& prior = prior_table_[dtime_prior_id];
            bool   difference = true;
            Float  z          = pack_vec[var2prior_.dtime_var_id(var_id)];
            size_t k          = 2;
            size_t index      = 3 * var_id + k;
            size_t offset     = pack_object_.mulstd_offset(smooth_id, k);
            Float mulstd      = Float(1.0);
            if( offset != DISMOD_AT_NULL_SIZE_T )
               mulstd        = pack_vec[offset];
            //
            residual = log_prior(prior, mulstd, z, y, index, difference);
            if( residual.density != uniform_enum )
               residual_vec.push_back(residual);
         }
      }
   }
   return residual_vec;
}

# define DISMOD_AT_INSTANTIATE_PRIOR_DENSITY(Float)                       \
   template                                                              \
   CppAD::vector< residual_struct<Float> > prior_model::fixed<Float>(  \
      const CppAD::vector<Float>&   pack_vec                            \
   ) const;                                                              \
   template                                                              \
   CppAD::vector< residual_struct<Float> > prior_model::random<Float>( \
      const CppAD::vector<Float>&   pack_vec                            \
   ) const;

// instantiations
DISMOD_AT_INSTANTIATE_PRIOR_DENSITY( double )
DISMOD_AT_INSTANTIATE_PRIOR_DENSITY( a1_double )

} // BEGIN_DISMOD_AT_NAMESPACE
