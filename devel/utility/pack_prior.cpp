// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/null_int.hpp>

/*
-------------------------------------------------------------------------------
$begin pack_prior$$
$spell
   var
   vec
   struct
   const
   dage
   dtime
   bool
   bnd_mulcov
   covariate
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section Priors in Variable ID Order$$

$head Syntax$$
$codei%pack_prior %var2prior%(
   %bound_random%, %prior_table%, %pack_object%, %s_info_vec%
)
%$$
$icode%size%           = %var2prior%.size()
%$$
$icode%const_value%    = %var2prior%.const_value(%var_id%)
%$$
$icode%value_prior_id% = %var2prior%.value_prior_id(%var_id%)
%$$
$icode%dage_prior_id%  = %var2prior%.dage_prior_id(%var_id%)
%$$
$icode%dtime_prior_id% = %var2prior%.dtime_prior_id(%var_id%)
%$$
$icode%dage_var_id%    = %var2prior%.dage_next(%var_id%)
%$$
$icode%dtime_var_id%   = %var2prior%.dtime_next(%var_id%)
%$$
$icode%fixed_effect%   = %var2prior%.fixed_effect(%var_id%)
%$$
$icode%max_abs%        = %var2prior%.max_abs(%var_id%)
%$$
$icode%var2prior%.set_bnd_mulcov(%bnd_mulcov_table%)
%$$

$head Prototype$$
$srcthisfile%0%// BEGIN CTOR_PROTOTYPE%// END CTOR_PROTOTYPE%1%
%$$

$head Notation$$

$subhead var_id$$
The variable names that end in
$cref/var_id/var_table/var_id/$$
have type $code size_t$$ and are an index in the variable table.

$subhead prior_id$$
The variable names that end in
$cref/prior_id/prior_table/prior_id/$$
have type $code size_t$$ and are an index in the prior table.

$head bound_random$$
This is a bound for the absolute value of the random effects
(which can be infinity).
This bound does not apply for random effects that have equal upper and
lower limits.

$head prior_table$$
is the in memory representation of the
$cref/prior_table/get_prior_table/prior_table/$$.

$head pack_object$$
is the $cref pack_info$$ information corresponding to
the $cref model_variables$$.

$head s_info_vec$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
   %s_info_vec%[ %smooth_id% ]
%$$
is the corresponding $cref smooth_info$$ information.

$head var2prior$$
an object that reports that prior information as a function of
$cref/var_id/var_table/var_id/$$.

$head size$$
This is the number of variables in the model; i.e.,
it is equal to $icode%pack_object%.size()%$$.

$head const_value$$
For each $icode var_id$$,
if $icode const_value$$ is not nan (not null)
$icode const_value$$ is the value at this variable is constrained to.

$head smooth_id$$
This return value has type $code size_t$$.
If $icode smooth_id$$ is null, this variables is a
$cref/smoothing standard deviation multiplier
   /model_variables
   /Fixed Effects, theta
   /Smoothing Standard Deviation Multipliers, lambda
/$$.
In this case there is no standard deviation multiplier for the
value prior for this variables and the difference priors are null.
Otherwise, this is the smoothing used for this variable and the
offset for the corresponding standard deviation multipliers can be computed
using $cref pack_info_mulstd$$.

$head value_prior_id$$
If $icode const_value$$ is null,
$icode value_prior_id$$ must be null and identifies the prior for
the value of this variable.
The null prior for this case corresponds to a uniform
between minus and plus infinity.

$head dage_prior_id$$
If $icode dage_var_id$$ is not null,
$icode dage_prior_id$$ identifies the prior for
the difference between the variable with index
$icode var_id$$ and the variable with index $icode dage_var_id$$.
The null prior for this case corresponds to a uniform
between minus and plus infinity.

$head dtime_prior_id$$
If $icode dtime_var_id$$ is not null,
$icode dtime_prior_id$$ identifies the prior for
the difference between the variable with index
$icode var_id$$ and the variable with index $icode dtime_var_id$$.
The null prior for this case corresponds to a uniform
between minus and plus infinity.

$head dage_var_id$$
If $icode dage_prior_id$$ is not null, this is the
next variable in the age difference.
If $icode var_id$$ corresponds to the maximum age in the grid,
$icode dage_var_id$$ is null.

$head dtime_var_id$$
If $icode dtime_prior_id$$ is not null, this is the
next variable in the time difference.
If $icode var_id$$ corresponds to the maximum time in the grid,
$icode dtime_var_id$$ is null.

$head fixed_effect$$
This $code bool$$ is true (false) if the variable with index
$icode var_id$$ is a
$cref/fixed effect/model_variables/Fixed Effects, theta/$$
(random effect).

$head set_bnd_mulcov$$
This member function sets the maximum upper and minimum lower
limit for the covariate multipliers; i.e., $icode max_abs$$
for covariate multipliers.
If a covariate multiplier is a random effects,
this overrides the $icode bound_random$$ setting.

$head bnd_mulcov_table$$
See$cref/bnd_mulcov_table/get_bnd_mulcov_table/bnd_mulcov_table/$$.

$head max_abs$$
Is the maximum absolute value for this variable.
$list number$$
This constraint is in addition to the upper and lower limits
in the prior for the variables.
$lnext
The initial $icode var2prior$$ corresponds to this maximum being
$icode bound_random$$ for random effects and infinity for fixed effects.
$lnext
If there was a previous call to $code set_bnd_mulcov$$,
it specified $icode max_abs$$ for fixed effect covariate multipliers; i.e.,
$cref/
   group covariate multipliers/
   model_variables/
   Fixed Effects, theta/
   Group Covariate Multipliers
/$$.
$lend

$children%
   example/devel/utility/pack_prior_xam.cpp
%$$
$head Example$$
The file $cref pack_prior_xam.cpp$$
contains an example and test that uses this routine.

$end
-------------------------------------------------------------------------------
*/
namespace dismod_at {

// size
size_t pack_prior::size(void) const
{  return prior_vec_.size(); }

// const_value
double pack_prior::const_value(size_t var_id) const
{  return prior_vec_[var_id].const_value; }

// smooth_id
size_t pack_prior::smooth_id(size_t var_id) const
{  return prior_vec_[var_id].smooth_id; }

// value_prior_id
size_t pack_prior::value_prior_id(size_t var_id) const
{  return prior_vec_[var_id].value_prior_id; }

// dage_prior_id
size_t pack_prior::dage_prior_id(size_t var_id) const
{  return prior_vec_[var_id].dage_prior_id; }

// dtime_prior_id
size_t pack_prior::dtime_prior_id(size_t var_id) const
{  return prior_vec_[var_id].dtime_prior_id; }

// dage_var_id
size_t pack_prior::dage_var_id(size_t var_id) const
{  return var_id + prior_vec_[var_id].n_time; }

// dtime_var_id
size_t pack_prior::dtime_var_id(size_t var_id) const
{  return var_id + 1; }

// fixed_effect
bool pack_prior::fixed_effect(size_t  var_id) const
{  return prior_vec_[var_id].fixed_effect; }

// max_abs
double pack_prior::max_abs(size_t var_id) const
{  return prior_vec_[var_id].max_abs; }

// set_bnd_mulcov
void pack_prior::set_bnd_mulcov(
   const CppAD::vector<bnd_mulcov_struct>& bnd_mulcov_table )
{  for(size_t var_id = 0; var_id < prior_vec_.size(); ++var_id)
   if( prior_vec_[var_id].fixed_effect )
   {  size_t mulcov_id = prior_vec_[var_id].mulcov_id;
      if( mulcov_id != DISMOD_AT_NULL_SIZE_T )
      {  double max_mulcov = bnd_mulcov_table[mulcov_id].max_mulcov;
         prior_vec_[var_id].max_abs = max_mulcov;
      }
   }
}

// set_prior_vec
void pack_prior::set_prior_vec(
   double                                                    bound_random ,
   const CppAD::vector<prior_struct>&                        prior_table  ,
   size_t                                                    offset       ,
   bool                                                      fixed_effect ,
   size_t                                                    mulcov_id    ,
   size_t                                                    smooth_id    ,
   const CppAD::vector<smooth_info>&                         s_info_vec   )
{  //
   double      inf     = std::numeric_limits<double>::infinity();
   smooth_info s_info  = s_info_vec[smooth_id];
   size_t n_age        = s_info.age_size();
   size_t n_time       = s_info.time_size();
   //
   // loop over grid points for this smoothing in age, time order
   for(size_t i = 0; i < n_age; i++)
   {  for(size_t j = 0; j < n_time; j++)
      {  // var_id
         size_t var_id   = offset + i * n_time + j;
         //
         prior_vec_[var_id].fixed_effect = fixed_effect;
         prior_vec_[var_id].mulcov_id    = mulcov_id;
         //
         // const_value
         double const_value            = s_info.const_value(i, j);
         prior_vec_[var_id].const_value = const_value;
         //
         // smooth_id
         prior_vec_[var_id].smooth_id = smooth_id;
         //
         // n_time
         prior_vec_[var_id].n_time = n_time;
         //
         // value prior
         size_t value_prior_id            = s_info.value_prior_id(i, j);
         prior_vec_[var_id].value_prior_id = value_prior_id;
         //
         // dage prior
         prior_vec_[var_id].dage_prior_id = s_info.dage_prior_id(i, j);
         CPPAD_ASSERT_UNKNOWN( i + 1 < n_age ||
            prior_vec_[var_id].dage_prior_id == DISMOD_AT_NULL_SIZE_T
         );
         //
         // dtime_prior
         prior_vec_[var_id].dtime_prior_id = s_info.dtime_prior_id(i, j);
         CPPAD_ASSERT_UNKNOWN( j + 1 < n_time ||
            prior_vec_[var_id].dtime_prior_id == DISMOD_AT_NULL_SIZE_T
         );
         //
         // max_abs
         if( fixed_effect )
            prior_vec_[var_id].max_abs = inf;
         else if ( ! std::isnan( const_value ) )
            prior_vec_[var_id].max_abs = inf;
         else
         {  CPPAD_ASSERT_UNKNOWN(value_prior_id != DISMOD_AT_NULL_SIZE_T);
            double lower = prior_table[value_prior_id].lower;
            double upper = prior_table[value_prior_id].upper;
            if( lower == upper )
               prior_vec_[var_id].max_abs = inf;
            else
               prior_vec_[var_id].max_abs = bound_random;
         }
         //
# ifndef NDEBUG
         bool value_prior_null = value_prior_id == DISMOD_AT_NULL_SIZE_T;
         bool const_value_null = std::isnan( const_value );
         assert( ! (value_prior_null && const_value_null) );
         assert( ! value_prior_null || ! const_value_null );

# endif
      }
   }
   return;
}

// BEGIN CTOR_PROTOTYPE
pack_prior::pack_prior(
   double                             bound_random        ,
   const CppAD::vector<size_t>&       n_child_data_in_fit ,
   const CppAD::vector<prior_struct>& prior_table         ,
   const pack_info&                   pack_object         ,
   const CppAD::vector<smooth_info>&  s_info_vec          )
// END CTOR_PROTOTYPE
{
   //
   pack_info::subvec_info info;
   double nan   = std::numeric_limits<double>::quiet_NaN();
   double inf   = std::numeric_limits<double>::infinity();
   //
   size_t n_var       = pack_object.size();
   size_t n_child     = pack_object.child_size();
   size_t n_integrand = pack_object.integrand_size();
   size_t n_smooth    = s_info_vec.size();
   //
   // -----------------------------------------------------------------------
   // initialize everyting to not defined
   prior_vec_.resize(n_var);
   for(size_t var_id = 0; var_id < n_var; ++var_id)
   {
      prior_vec_[var_id].max_abs        = nan;
      prior_vec_[var_id].const_value    = nan;
      prior_vec_[var_id].n_time         = DISMOD_AT_NULL_SIZE_T;
      prior_vec_[var_id].smooth_id      = DISMOD_AT_NULL_SIZE_T;
      prior_vec_[var_id].mulcov_id      = DISMOD_AT_NULL_SIZE_T;
      prior_vec_[var_id].value_prior_id = DISMOD_AT_NULL_SIZE_T;
      prior_vec_[var_id].dage_prior_id  = DISMOD_AT_NULL_SIZE_T;
      prior_vec_[var_id].dtime_prior_id = DISMOD_AT_NULL_SIZE_T;
      // alternate value (should not matter)
      prior_vec_[var_id].fixed_effect   = bool( var_id % 2 );
   }
   //
   // get priors for smoothing standard deviation multipliers
   for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
   {  // multipliers for this smoothing
      for(size_t k = 0; k < 3; k++)
      {  // value, dage, dtime in that order
         size_t offset     = pack_object.mulstd_offset(smooth_id, k);
         if( offset != DISMOD_AT_NULL_SIZE_T )
         {  size_t prior_id;
            switch(k)
            {  case 0:
               prior_id = s_info_vec[smooth_id].mulstd_value();
               break;

               case 1:
               prior_id = s_info_vec[smooth_id].mulstd_dage();
               break;

               case 2:
               prior_id = s_info_vec[smooth_id].mulstd_dtime();
               break;

               default:
               assert(false);
            }
            // this prior is for a constant; i.e., n_age = n_time = 1
            prior_vec_[offset].max_abs        = inf;
            prior_vec_[offset].n_time         = 1;
            prior_vec_[offset].fixed_effect   = true;
            prior_vec_[offset].value_prior_id = prior_id;
         }
      }
   }
   // ------------------------------------------------------------------------
   // get priors for rate values
   for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
   {  // this rate
      for(size_t j = 0; j <= n_child; j++)
      {  // child if j < n_child, otherwise parent
         info             = pack_object.node_rate_value_info(rate_id, j);
         size_t smooth_id = info.smooth_id;
         // if smooth_id is null this has no variables
         if( smooth_id != DISMOD_AT_NULL_SIZE_T )
         {  size_t offset       = info.offset;
            bool   fixed_effect = j == n_child;
            size_t mulcov_id    = info.mulcov_id;
            if( j < n_child && n_child_data_in_fit[j] == 0 )
            {  // constrain these random effects to be constant
               double zero_bound_random = 0.0;
               set_prior_vec(zero_bound_random, prior_table,
                  offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
               );
            }
            else
            {  set_prior_vec(bound_random, prior_table,
                  offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
               );
            }
         }
      }
   }
   // ------------------------------------------------------------------------
   // get priors for subgroup rate value covariates
   for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
   {  size_t n_cov = pack_object.subgroup_rate_value_n_cov(rate_id);
      for(size_t j = 0; j < n_cov; j++)
      {  size_t n_sub = pack_object.subgroup_rate_value_n_sub(rate_id, j);
         for(size_t k = 0; k < n_sub; ++k)
         {  info   = pack_object.subgroup_rate_value_info(rate_id, j, k);
            size_t offset       = info.offset;
            size_t smooth_id    = info.smooth_id;
            bool   fixed_effect = false;
            size_t mulcov_id    = info.mulcov_id;
            set_prior_vec(bound_random, prior_table,
               offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
            );
         }
      }
   }
   // ------------------------------------------------------------------------
   // get priors for group rate value covariates
   for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
   {  size_t n_cov = pack_object.group_rate_value_n_cov(rate_id);
      for(size_t j = 0; j < n_cov; j++)
      {  info   = pack_object.group_rate_value_info(rate_id, j);
         size_t offset       = info.offset;
         size_t smooth_id    = info.smooth_id;
         bool   fixed_effect = true;
         size_t mulcov_id    = info.mulcov_id;
         set_prior_vec(bound_random, prior_table,
            offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
         );
      }
   }
   // ------------------------------------------------------------------------
   // get priors for subgroup measurement value covariates
   for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
   {  // measurement value covariates for this integrand
      size_t n_cov = pack_object.subgroup_meas_value_n_cov(integrand_id);
      for(size_t j = 0; j < n_cov; j++)
      {  size_t n_sub =
            pack_object.subgroup_meas_value_n_sub(integrand_id, j);
         for(size_t k = 0; k < n_sub; ++k)
         {  info =
               pack_object.subgroup_meas_value_info(integrand_id, j, k);
            size_t offset       = info.offset;
            size_t smooth_id    = info.smooth_id;
            bool   fixed_effect = false;
            size_t mulcov_id    = info.mulcov_id;
            set_prior_vec(bound_random, prior_table,
               offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
            );
         }
      }
   }
   // ------------------------------------------------------------------------
   // get priors for group measurement covariates
   for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
   {  // measurement value covariates for this integrand
      size_t n_cov = pack_object.group_meas_value_n_cov(integrand_id);
      for(size_t j = 0; j < n_cov; j++)
      {  info   = pack_object.group_meas_value_info(integrand_id, j);
         size_t offset       = info.offset;
         size_t smooth_id    = info.smooth_id;
         bool   fixed_effect = true;
         size_t mulcov_id    = info.mulcov_id;
         set_prior_vec(bound_random, prior_table,
            offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
         );
      }
      // measurement std covariates for this integrand
      n_cov = pack_object.group_meas_noise_n_cov(integrand_id);
      for(size_t j = 0; j < n_cov; j++)
      {  info   = pack_object.group_meas_noise_info(integrand_id, j);
         size_t offset       = info.offset;
         size_t smooth_id    = info.smooth_id;
         bool   fixed_effect = true;
         size_t mulcov_id    = info.mulcov_id;
         set_prior_vec(bound_random, prior_table,
            offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
         );
      }
   }
   return;
}

} // END_DISMOD_AT_NAMESPACE
