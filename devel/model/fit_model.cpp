// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-25 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <cppad/mixed/exception.hpp>
# include <dismod_at/a1_double.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/n_random_const.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/remove_const.hpp>
# include <dismod_at/get_var_limits.hpp>
# include <dismod_at/ran_con_rcv.hpp>
# include <dismod_at/get_str_map.hpp>

# define PRINT_SIZE_MAP 0

namespace dismod_at { // DISMOD_AT_BEGIN_NAMSPACE
/*
{xrst_begin fit_model_ctor dev}
{xrst_spell
  enum
  stderr
}

Fit Model Constructor
#####################

Syntax
******

| ``fit_model`` *fit_object* (
| |tab| *db* ,
| |tab| *simulate_index* ,
| |tab| *warn_on_stderr* ,
| |tab| *bound_random* ,
| |tab| *pack_object* ,
| |tab| *var2prior* ,
| |tab| *start_var* ,
| |tab| *scale_var* ,
| |tab| *prior_table* ,
| |tab| *data_object* ,
| |tab| *prior_object* ,
| |tab| *random_const* ,
| |tab| *quasi_fixed* ,
| |tab| *zero_sum_child_rate* ,
| |tab| *zero_sum_mulcov_group* ,
| |tab| *data_object* ,
| |tab| *trace_init*
| )

fit_object
**********
The ``fit_model`` object being constructed.

db
**
This argument is the database connection for
:ref:`logging<log_message-name>` errors and warnings.

simulate_index
**************
Is the :ref:`data_sim_table@simulate_index`
when simulated data.
This index is ``-1`` when we are fitting the
:ref:`data_table@meas_value` column in the data table.

warn_on_stderr
**************
If true, warnings will be printed on stderr.
(Error messages are always printed on stderr.)

bound_random
************
This is the value of the
:ref:`option_table@Optimize Random Only@bound_random`
in the option table.

pack_object
***********
This argument is the :ref:`pack_info-name` information corresponding to the
:ref:`model_variables-name` .

pack_prior
**********
This argument is the :ref:`pack_prior-name` information corresponding to the
:ref:`model_variables-name` .

var2prior
*********
Mapping from *var_id* to prior information.

start_var
*********
This vector is the starting :ref:`model_variables-name` in the order
specified by :ref:`pack_info-name` .
These values get projected onto the [ lower , upper ] interval
for each variable before being passed to cppad_mixed.

scale_var
*********
The object and constraints are scaled using this value for the
:ref:`model_variables-name` .

prior_table
***********
This argument is the :ref:`get_prior_table@prior_table` .

prior_object
************
This object contains the model for the fixed negative log-likelihood;
see :ref:`prior_model-name` .

remove_const
************
This is a :ref:`remove_const-name` for removing the subset of the
random effects vector that are constant.

quasi_fixed
***********
If this argument is true,
a quasi-Newton method is used when optimizing the fixed effects.
Otherwise a full Newton method is used; see
:ref:`option_table@Optimize Fixed Only@quasi_fixed` .

zero_sum_child_rate
*******************
If this vector has size ``number_rate_enum`` .
If *zero_sum_child_rate* [ *rate_id* ] is true,
for each age, time, and rate,
the sum of the random effects with respect the children
is constrained to be zero.

zero_sum_mulcov_group
*********************
If this vector has size equal to the number of groups in
:ref:`subgroup_table-name` .
If *zero_sum_mulcov_group* [ *group_id* ] is true,
for each age, time, and :ref:`mulcov_table-name` row,
the sum of the random effects with respect to subgroup
is constrained to be zero.

data_object
***********
This object contains the model for the data density;
see :ref:`data_model<devel_data_model-name>` .
It is effectively const.

trace_init
**********
If this argument is true,
a trace of the initialization process is printed on standard output.
This gives one an indication of progress for large problems where
initialization takes a long time.
This argument is optional and its default value is false.

Prototype
*********
{xrst_spell_off}
{xrst_code cpp} */
fit_model::fit_model(
   sqlite3*                              db                    ,
   int                                   simulate_index        ,
   bool                                  warn_on_stderr        ,
   double                                bound_random          ,
   const pack_info&                      pack_object           ,
   const pack_prior&                     var2prior             ,
   const CppAD::vector<double>&          start_var             ,
   const CppAD::vector<double>&          scale_var             ,
   const CppAD::vector<prior_struct>&    prior_table           ,
   const prior_model&                    prior_object          ,
   const remove_const&                   random_const          ,
   bool                                  quasi_fixed           ,
   const CppAD::vector<bool>&            zero_sum_child_rate   ,
   const CppAD::vector<bool>&            zero_sum_mulcov_group ,
   data_model&                           data_object           ,
   bool                                  trace_init            )
/* {xrst_code}
{xrst_spell_on}

{xrst_end fit_model_ctor}
*/
// base class constructor
// (The value of bool_sparsity does not seem to affect speed test results.)
: cppad_mixed(
   // n_fixed
   number_fixed(pack_object),
   // n_random in dismod_at minus number random effects that are constant
   pack_object.random_size() - number_random_const(
      pack_object, var2prior, prior_table
   ),
   // quasi_fixed
   quasi_fixed,
   // bool_sparsity
   false,
   // A_rcv
   ran_con_rcv(
      bound_random,
      zero_sum_child_rate,
      zero_sum_mulcov_group,
      pack_object,
      random_const
   ),
   // trace_init
   trace_init
),
db_            (db)                                 ,
simulate_index_( simulate_index )                   ,
warn_on_stderr_( warn_on_stderr )                   ,
n_fixed_       ( number_fixed(pack_object) )        ,
n_random_      ( pack_object.random_size() )        ,
pack_object_   ( pack_object )                      ,
var2prior_     ( var2prior   )                      ,
start_var_     ( start_var   )                      ,
scale_var_     ( scale_var   )                      ,
prior_table_   ( prior_table )                      ,
prior_object_  ( prior_object )                     ,
random_const_  ( random_const )                     ,
data_object_   ( data_object )
{  if( trace_init )
      std::cout << "Begin dismod_at: fit_model constructor\n";
   //
   assert( bound_random >= 0.0 );
   double inf = std::numeric_limits<double>::infinity();
   // ----------------------------------------------------------------------
   // n_var
   size_t n_var = n_fixed_ + n_random_;
   assert( pack_object.size() == n_var );
   assert( var2prior_.size() == n_var );
   // ----------------------------------------------------------------------
   // random_lower_, random_upper_, n_random_equal_
   random_lower_   = random_const_.lower();
   random_upper_   = random_const_.upper();
   n_random_equal_ = random_const_.n_const();
   // ----------------------------------------------------------------------
   // diff_prior_
   assert( diff_prior_.size() == 0 );
   for(size_t var_id = 0; var_id < n_var; ++var_id)
   {  size_t prior_id = var2prior_.dage_prior_id(var_id);
      if( prior_id != DISMOD_AT_NULL_SIZE_T )
      {  double lower    = prior_table[prior_id].lower;
         double upper    = prior_table[prior_id].upper;
         if( - inf < lower || upper < + inf )
         {  diff_prior_struct entry;
            entry.direction    = diff_prior_struct::dage_enum;
            entry.plus_var_id  = var2prior_.dage_var_id(var_id);
            entry.minus_var_id = var_id;
            entry.prior_id     = prior_id;
            diff_prior_.push_back( entry );
         }
      }
      prior_id = var2prior_.dtime_prior_id(var_id);
      if( prior_id != DISMOD_AT_NULL_SIZE_T )
      {  double lower    = prior_table[prior_id].lower;
         double upper    = prior_table[prior_id].upper;
         if( - inf < lower || upper < + inf )
         {  diff_prior_struct entry;
            entry.direction    = diff_prior_struct::dtime_enum;
            entry.plus_var_id  = var2prior_.dtime_var_id(var_id);
            entry.minus_var_id = var_id;
            entry.prior_id     = prior_id;
            diff_prior_.push_back( entry );
         }
      }
   }
   // ----------------------------------------------------------------------
   // var_id2fixed_
   var_id2fixed_.resize(n_var);
   for(size_t k = 0; k < n_var; k++)
      var_id2fixed_[k] = n_fixed_;
   CppAD::vector<size_t> pack_index(n_fixed_);
   pack_index = fixed2var_id(pack_object);
   for(size_t j = 0; j < n_fixed_; j++)
      var_id2fixed_[ pack_index[j] ] = j;
   // ----------------------------------------------------------------------
   // fixed_is_scaled_, fixed_scale_eta_
   fixed_is_scaled_.resize(n_fixed_);
   fixed_scale_eta_.resize(n_fixed_);
   for(size_t j = 0; j < n_fixed_; j++)
   {  fixed_is_scaled_[j] = false;
      fixed_scale_eta_[j] = - inf;
   }
   for(size_t var_id = 0; var_id < n_var; var_id++)
   {  size_t fixed_id = var_id2fixed_[var_id];
      if( fixed_id < n_fixed_ )
      {  size_t prior_id  = var2prior_.value_prior_id(var_id);
         if( prior_id != DISMOD_AT_NULL_SIZE_T )
         {  prior_struct prior          = prior_table[prior_id];
            fixed_scale_eta_[fixed_id]  = prior.eta;
            fixed_is_scaled_[fixed_id]  = ! std::isnan( prior.eta );
            bool ok = std::isnan(prior.eta);
            ok     |= prior.lower + prior.eta > 0.0;
            if( ! ok  )
            {  std::string  msg = "eta != null, lower + eta <= 0, and\n";
               msg += "this is a value prior for a fixed effect";
               error_exit(msg, "prior", prior_id);
            }
         }
      }
   }
# ifndef NDEBUG
   // check that all the fixed effects scale values have been set
   for(size_t j = 0; j < n_fixed_; j++)
      if( fixed_is_scaled_[j] )
         assert( ! (fixed_scale_eta_[j] == - inf) );
# endif
   // ---------------------------------------------------------------------
   // initialize the cppad_mixed object
   //
   // fixed_vec
   CppAD::vector<double> fixed_vec(n_fixed_);
   unpack_fixed(pack_object, start_var, fixed_vec);
   scale_fixed_effect(fixed_vec, fixed_vec);
   // random_vec
   CppAD::vector<double> random_vec(n_random_);
   unpack_random(pack_object, start_var, random_vec);
   //
   CppAD::vector<double> cppad_mixed_random_vec =
      random_const_.remove( random_vec );
   //
   cppad_mixed_info_ = initialize(fixed_vec, cppad_mixed_random_vec);
# if PRINT_SIZE_MAP
   std::map<std::string, size_t>::iterator itr;
   for(itr = cppad_mixed_info_.begin(); itr != cppad_mixed_info_.end(); itr++)
      std::cout << itr->first << " = " << itr->second << "\n";
# endif
   if( trace_init )
      std::cout << "End dismod_at: fit_model constructor\n";
}
/*
-----------------------------------------------------------------------------
{xrst_begin fit_model_run_fit dev}
{xrst_spell
  frac
}

Run optimization to determine the optimal fixed and random effects
##################################################################

Syntax
******

   *fit_object* . ``run_fit`` ( *random_only* , *option_map* , *warm_start* )

Scaling
*******
During optimization the
:ref:`fixed effects<model_variables@Fixed Effects, theta>` ,
that have *eta* not ``null`` in their value prior, are scaled; see
:ref:`scaling<prior_table@eta@Scaling Fixed Effects>` .

random_only
***********
If this argument is true,
the fixed effects are set to their starting value and only the
random effects are optimized.
Otherwise, both the fixed and random effects are optimized.

option_map
**********
This argument must have the following values:
For *name* equal to
:ref:`option_table@Optimize Fixed and Random@derivative_test` ,
:ref:`option_table@Optimize Fixed and Random@tolerance` ,
:ref:`option_table@Optimize Fixed and Random@max_num_iter` ,
:ref:`option_table@Optimize Fixed and Random@print_level` ,
:ref:`option_table@Optimize Fixed and Random@accept_after_max_steps` ,
:ref:`option_table@asymptotic_rcond_lower` ,
and for *fit* equal to ``fixed`` and ``random``

   *option_map* [" *name* _ *fit* "]

is the value in the :ref:`option_table-name` for the corresponding option.
It must also have

| |tab| *option_map* [ ``"bound_frac_fixed"`` ]
| |tab| *option_map* [ ``"limited_memory_max_history_fixed"`` ]

warm_start
**********
If *warm_start* . ``x_info.size`` () is non-zero,
the ipopt warm start information is in this structure.
In this case, *random_only* must be false.
Note that the default constructor for ``warm_start_struct`` sets
*warm_start* . ``x_info.size`` () to zero.

Prototype
*********
{xrst_spell_off}
{xrst_code cpp} */
void fit_model::run_fit(
   bool                                      random_only    ,
   const std::map<std::string, std::string>& option_map     ,
   const CppAD::mixed::warm_start_struct&    warm_start     )
/* {xrst_code}
{xrst_spell_on}

{xrst_end fit_model_run_fit}
*/
{  assert( warm_start.x_info.size() == 0 || ! random_only );
   //
   size_t n_var = n_fixed_ + n_random_;
   assert( pack_object_.size() == n_var );
   assert( var2prior_.size() == n_var );
   d_vector pack_vec( n_var );
   //
   // var_lower, var_upper
   d_vector var_lower(n_var), var_upper(n_var);
   get_var_limits(
      var_lower, var_upper, var2prior_, prior_table_
   );
   // fixed_lower, fixed_upper
   d_vector fixed_lower(n_fixed_), fixed_upper(n_fixed_);
   unpack_fixed(pack_object_, var_lower, fixed_lower);
   unpack_fixed(pack_object_, var_upper, fixed_upper);
   //
   // fixed_lower_scaled, fixed_upper_scaled
   d_vector fixed_lower_scaled(n_fixed_), fixed_upper_scaled(n_fixed_);
   scale_fixed_effect(fixed_lower, fixed_lower_scaled);
   scale_fixed_effect(fixed_upper, fixed_upper_scaled);

   // fix_constraint_lower, fix_constraint_upper
   // fixed effect constraints are in original space, not scaled space.
   d_vector fix_constraint_lower, fix_constraint_upper;
   for(size_t k = 0; k < diff_prior_.size(); k++)
   {  // make sure these variable ids correspond to fixed effects
      assert( var_id2fixed_[ diff_prior_[k].plus_var_id ] < n_fixed_ );
      assert( var_id2fixed_[ diff_prior_[k].minus_var_id ] < n_fixed_ );
      //
      size_t prior_id = diff_prior_[k].prior_id;
      double lower    = prior_table_[prior_id].lower;
      double upper    = prior_table_[prior_id].upper;
      fix_constraint_lower.push_back(lower);
      fix_constraint_upper.push_back(upper);
   }
   //
   // var_in
   d_vector var_in(n_var);
   for(size_t var_id = 0; var_id < n_var; ++var_id)
   {  var_in[var_id] = std::max(var_lower[var_id], start_var_[var_id]);
      var_in[var_id] = std::min(var_upper[var_id], var_in[var_id]);
   }

   // fixed_in
   d_vector fixed_in(n_fixed_);
   unpack_fixed(pack_object_, var_in, fixed_in);
   scale_fixed_effect(fixed_in, fixed_in);

   // Note that scale_fixed_effect scales argument values.
   // The fixed_scale vector is used by cppad_mixed to scale function values.
   d_vector fixed_scale(n_fixed_);
   unpack_fixed(pack_object_, scale_var_, fixed_scale);
   scale_fixed_effect(fixed_scale, fixed_scale);

   // random_in
   d_vector random_in(n_random_);
   unpack_random(pack_object_, start_var_, random_in);

   // Ipopt fixed effects optimization options
   std::string bound_push = CppAD::to_string(
      std::numeric_limits<double>::min()
   );
   std::string options = "";
   options += "String    sb  yes";
   options += "\nNumeric bound_relax_factor 0.0";
   options += "\nNumeric bound_push " + bound_push;
   options += "\nNumeric tol "
      + get_str_map(option_map, "tolerance_fixed");
   options += "\nNumeric acceptable_tol "
      + get_str_map(option_map, "tolerance_fixed");
   options += "\nInteger print_level "
      + get_str_map(option_map, "print_level_fixed");
   options += "\nInteger max_iter "
      + get_str_map(option_map, "max_num_iter_fixed");
   //
   options += "\nInteger accept_after_max_steps "
      + get_str_map(option_map, "accept_after_max_steps_fixed");
   options += "\nString derivative_test "
      + get_str_map(option_map, "derivative_test_fixed");
   //
   options += "\nNumeric bound_frac "
      + get_str_map(option_map, "bound_frac_fixed");
   options += "\nInteger limited_memory_max_history "
      + get_str_map(option_map, "limited_memory_max_history_fixed");
   options += "\n";
   //
   std::string fixed_options = options;
   //
   // Ipopt random effects optimization options
   options = "";
   options += "String    sb  yes";
   options += "\nNumeric bound_relax_factor 0.0";
   options += "\nNumeric tol "
      + get_str_map(option_map, "tolerance_random");
   options += "\nNumeric acceptable_tol "
      + get_str_map(option_map, "tolerance_random");
   options += "\nInteger print_level "
      + get_str_map(option_map, "print_level_random");
   options += "\nInteger max_iter "
      + get_str_map(option_map, "max_num_iter_random");
   options += "\nString evaluation_method "
      + get_str_map(option_map, "method_random");
   //
   options += "\nInteger accept_after_max_steps "
      + get_str_map(option_map, "accept_after_max_steps_random");
   options += "\nString derivative_test "
      + get_str_map(option_map, "derivative_test_random");
   //
   options += "\nString hessian_approximation exact";
   options += "\n";
   //
   std::string random_options = options;
   //
   // convert from dismod_at random effects to cppad_mixed random effects
   d_vector cppad_mixed_random_lower = random_const_.remove( random_lower_ );
   d_vector cppad_mixed_random_upper = random_const_.remove( random_upper_ );
   d_vector cppad_mixed_random_in    = random_const_.remove( random_in );
   //
   // optimize the fixed effects
   d_vector fixed_opt = fixed_in;
   d_vector fixed_lag(n_fixed_);
   d_vector fixed_con_lag( diff_prior_.size() );
   for(size_t j = 0; j < fixed_lag.size(); j++)
      fixed_lag[j] = 0.0;
   for(size_t j = 0; j < fixed_con_lag.size(); j++)
      fixed_con_lag[j] = 0.0;
   if( ! random_only )
   {  CppAD::mixed::fixed_solution fixed_sol = optimize_fixed(
         fixed_options,
         random_options,
         fixed_lower_scaled,
         fixed_upper_scaled,
         fix_constraint_lower,
         fix_constraint_upper,
         fixed_scale,
         fixed_in,
         cppad_mixed_random_lower,
         cppad_mixed_random_upper,
         cppad_mixed_random_in,
         warm_start
      );
      fixed_opt            = fixed_sol.fixed_opt;
      fixed_lag            = fixed_sol.fixed_lag;
      fixed_con_lag        = fixed_sol.fix_con_lag;
      solution_.trace_vec  = fixed_sol.trace_vec;
      solution_.warm_start = fixed_sol.warm_start;
   }
   // optimal random effects
   d_vector random_opt(n_random_);
   if( n_random_ > n_random_equal_ )
   {  d_vector cppad_mixed_random_opt = optimize_random(
         random_options,
         fixed_opt,
         cppad_mixed_random_lower,
         cppad_mixed_random_upper,
         cppad_mixed_random_in
      );
      random_opt = random_const_.restore( cppad_mixed_random_opt );
   }
   else
   {  assert( n_random_ == n_random_equal_ );
      for(size_t i = 0; i < n_random_; i++)
         random_opt[i] = random_lower_[i];
   }
   // The optimal solution is scaled, but the Lagrange multilpiers are not
   unscale_fixed_effect(fixed_opt, fixed_opt);
   //
   // make sure round off has not violated bounds
   for(size_t j = 0; j < n_fixed_; ++j)
   {  fixed_opt[j] = std::max(fixed_lower[j], fixed_opt[j]);
      fixed_opt[j] = std::min(fixed_upper[j], fixed_opt[j]);
   }
   //
   // size store solution_
   solution_.fit_var_value.resize(n_var);
   solution_.lagrange_value.resize(n_var);
   solution_.lagrange_dage.resize(n_var);
   solution_.lagrange_dtime.resize(n_var);
   //
   // values that are stored by fixed effect index
   pack_fixed(pack_object_, solution_.fit_var_value, fixed_opt);
   pack_fixed(pack_object_, solution_.lagrange_value, fixed_lag);
   //
   // values that are stored by random effect index
   if ( n_random_ > 0 )
   {  d_vector zero(n_random_);
      for(size_t i = 0; i < n_random_; i++)
         zero[i] = 0.0;
      pack_random(pack_object_, solution_.fit_var_value, random_opt);
      pack_random(pack_object_, solution_.lagrange_value, zero);
   }
   //
   // difference constraints
   for(size_t i = 0; i < n_var; i++)
   {  solution_.lagrange_dage[i]  = 0.0;
      solution_.lagrange_dtime[i] = 0.0;
   }
   for(size_t k = 0; k < diff_prior_.size(); k++)
   {  size_t minus_var_id   = diff_prior_[k].minus_var_id;
      assert( var_id2fixed_[minus_var_id] < n_fixed_ );
      if( diff_prior_[k].direction == diff_prior_struct::dage_enum )
         solution_.lagrange_dage[minus_var_id] = fixed_con_lag[k];
      else
         solution_.lagrange_dtime[minus_var_id] = fixed_con_lag[k];
   }
   return;
}
/*
---------------------------------------------------------------------------
{xrst_begin fit_model_get_solution dev}

Get Solution Corresponding to Previous Fit
##########################################

Syntax
******

| *fit_object* . ``get_solution`` (
| |tab| *fit_var_value* ,
| |tab| *lagrange_value* ,
| |tab| *lagrange_dage* ,
| |tab| *lagrange_dtime* ,
| |tab| *warm_start*
| )

fit_object
**********
see :ref:`fit_model_ctor@fit_object` .

fit_var_value
*************
The size of this vector size is zero or
equal to the number of model variables.
The input value of its elements does not matter.
Upon return it is the optimal :ref:`variable values<model_variables-name>` in
:ref:`pack_info-name` order.

lagrange_value
**************
The size of this vector size is zero or
equal to the number of model variables.
The input value of its elements does not matter.
Upon return it is the Lagrange multipliers for the lower and upper
limits on the corresponding model variables.
If there is no limit, or if a limit is not active, the corresponding
element is zero.

lagrange_dage
*************
The size of this vector size is zero or
equal to the number of model variables.
The input value of its elements does not matter.
Upon return it is the Lagrange multipliers for the lower and upper
limits on the forward age difference for all the variables.
If a variable does not have a forward age difference,
if there is no limit, or if a limit is not active, the corresponding
element is zero.

lagrange_dtime
**************
The size of this vector size is zero or
equal to the number of model variables.
The input value of its elements does not matter,
Upon return it is the Lagrange multipliers for the lower and upper
limits on the forward time difference for all the variable.
If a variable does not have a forward time difference,
if there is no limit, or if a limit is not active, the corresponding
element is zero.

warm_start
**********
This is the ipopt warm start information.
It can be used to continue the fit from where it left off; e.g.,
it the maximum number of iterations was reached and one decides to
continue the fit with more iterations.
{xrst_toc_hidden
   example/devel/model/fit_model_xam.cpp
}
Example
*******
The file :ref:`fit_model_xam.cpp-name` contains an example and test
of using this routine.

Prototype
*********
{xrst_spell_off}
{xrst_code cpp} */
void fit_model::get_solution(
   CppAD::vector<double>&           fit_var_value   ,
   CppAD::vector<double>&           lagrange_value  ,
   CppAD::vector<double>&           lagrange_dage   ,
   CppAD::vector<double>&           lagrange_dtime  ,
   CppAD::vector<trace_struct>&     trace_vec       ,
   CppAD::mixed::warm_start_struct& warm_start )
/* {xrst_code}
{xrst_spell_on}

{xrst_end fit_model_get_solution}
*/
{  fit_var_value  = solution_.fit_var_value;
   lagrange_value = solution_.lagrange_value;
   lagrange_dage  = solution_.lagrange_dage;
   lagrange_dtime = solution_.lagrange_dtime;
   trace_vec      = solution_.trace_vec;
   warm_start     = solution_.warm_start;
   return;
}
/*
---------------------------------------------------------------------------
{xrst_begin fit_model_random_obj_hes dev}
{xrst_spell
  rcv
}

Compute Hessian Of Random Effects Objective
###########################################

Syntax
******

   ``random_hes_rcv`` = *fit_object* . ``random_obj_hes`` ( *pack_vec* )

Prototype
*********
{xrst_literal
   // BEGIN_RANDOM_OBJ_HES
   // END_RANDOM_OBJ_HES
}

Constants
*********
The model variables that have upper and lower limits equal
are referred to as constants.

pack_vec
********
Is the value of the vector fixed effects,
in :ref:`pack_info-name` order, at which we are computing the Hessian.
The corresponding fixed effects are referred to as :math:`\theta`
and the random effects are referred to as :math:`u` below.

random_hes_rcv
**************
is a sparse matrix representation of the
Hessian of the random effects objective evaluated
:math:`( \theta , u )`.
The row and column indices in this matrix are relative to the
:ref:`pack_info-name` vector.
Only the lower triangle is returned (column indices are less than or equal
row indices) because the Hessian is symmetric.
Note that the random effects objective does not have any
Laplace density terms.

Constraints
===========
The Hessians of random effects that have upper and lower limits equal
are not included in *random_hes_rcv* ; i.e., they are zero in the
sparse matrix representation.

{xrst_end fit_model_random_obj_hes}
*/
// BEGIN_RANDOM_OBJ_HES
CppAD::mixed::d_sparse_rcv fit_model::random_obj_hes(
   const CppAD::vector<double>&  pack_vec )
// END_RANDOM_OBJ_HES
{  size_t n_var = n_fixed_ + n_random_;
   assert( pack_vec.size() == n_var );
   //
   // fixed_vec, random_vec
   CppAD::vector<double> fixed_vec(n_fixed_), random_vec(n_random_);
   unpack_random(pack_object_, pack_vec, random_vec);
   unpack_fixed(pack_object_,   pack_vec, fixed_vec);
   //
   // convert dismod_at random effects to cppad_mixed random effects
   d_vector cppad_mixed_random_vec = random_const_.remove( random_vec );
   //
   // convert dismod_at fixed effect to cppad_mixed fixed effects
   d_vector cppad_mixed_fixed_vec(n_fixed_);
   scale_fixed_effect(fixed_vec, cppad_mixed_fixed_vec);
   //
   // hes_random_rcv
   // This Hessian uses variable indices in cppad_mixed space
   CppAD::mixed::d_sparse_rcv hes_random_rcv = hes_random_obj(
      cppad_mixed_fixed_vec, cppad_mixed_random_vec
   );
   //
   // random_hes_rcv
   // This Hessian uses variable indices in dismod_at space
   CppAD::vector<size_t> var_id = random2var_id(pack_object_);
   size_t nnz = hes_random_rcv.nnz();
   CppAD::mixed::sparse_rc pattern(n_var, n_var, nnz);
   for(size_t k = 0; k < nnz; ++k)
   {  size_t r = hes_random_rcv.row()[k];
      size_t c = hes_random_rcv.col()[k];
      pattern.set(k, var_id[r], var_id[c]);
   }
   CppAD::mixed::d_sparse_rcv random_hes_rcv( pattern );
   for(size_t k = 0; k < nnz; ++k)
      random_hes_rcv.set(k, hes_random_rcv.val()[k] );
   //
   return random_hes_rcv;
}
/*
---------------------------------------------------------------------------
{xrst_begin fit_model_sample_posterior dev}
{xrst_spell
  uhat
}

Sample From Posterior Distribution for a Fit
############################################

Syntax
******

| *fit_object* . ``sample_posterior`` (
| |tab| *hes_fixed_obj_out* ,
| |tab| *hes_random_obj_out* ,
| |tab| *n_sample* ,
| |tab| *sample_out* ,
| |tab| *fit_var_value* ,
| |tab| *option_map*
| )

Constants
*********
The model variables that have upper and lower limits equal
are referred to as constants.

fit_var_value
*************
This vector has size equal to the number of model variables.
It is the optimal :ref:`variable values<model_variables-name>` in
:ref:`pack_info-name` order.

theta
=====
We use :math:`\theta` to denote the fixed effects vector
corresponding to the values in *fit_var_value* .

uhat
====
We use :math:`\hat{u}` to denote the random effects vector
corresponding to the values in *fit_var_value* .
It is assumed that :math:`\hat{u}` minimizes the random effects objective
:math:`f( \theta , \cdot )`.

hes_fixed_obj_out
*****************
The input value of this argument does not matter.
Upon return it is a sparse matrix representation of the
Hessian of the fixed effects objective evaluated at :math:`\theta`.
The row and column indices in this matrix are relative to the
:ref:`pack_info-name` vector.
Only the lower triangle is returned (column indices are less than or equal
row indices) because the Hessian is symmetric.
Variables that are :ref:`constant<fit_model_sample_posterior@Constants>`
are not included in the Hessian.
The Laplace density terms in the likelihood function are not included
because the Hessian is not defined at zero for an Laplace density.

hes_random_obj_out
******************
The input value of this argument does not matter.
Upon return it is a sparse matrix representation of the
Hessian of the random effects objective evaluated
:math:`( \theta , \hat{u} )`.
The row and column indices in this matrix are relative to the
:ref:`pack_info-name` vector.
Only the lower triangle is returned (column indices are less than or equal
row indices) because the Hessian is symmetric.
Note that the random effects objective does not have any
Laplace density terms.

n_sample
********
Is the number of independent samples to generate.
Each sample contains the entire set of :ref:`model_variables-name` .

sample_out
**********
The input size value of this argument does not matter.
If an error occurs (the samples cannot be calculated)
*sample_out.size* () is zero upon return.
Otherwise upon return *sample_out.size* () is
equal to the number of samples *n_sample* times the number of
:ref:`model_variables-name` *n_var* .
*i* = 0 , ... , *n_sample* ``-1`` ,
*j* = 0 , ... , *n_var* ``-1`` ,

   *sample_out* [ *i* * *n_sample* + *j*  ]

is the *j*-th component of the *i*-th sample of the model variables.
These samples are independent for different *i* .

fixed effects
=============
The fixed effect samples are normal with mean :math:`\theta` and covariance
equal to the inverse of *hes_fixed_obj_out* .
If this Hessian is not positive definite,
all of the samples are set to ``nan`` .

random_effects
==============
The random effect samples are normal with mean :math:`\hat{u}` and covariance
equal to the inverse of *hes_random_obj_out* .
If this Hessian is not positive definite,
all of the samples are set to ``nan`` .

Constraints
===========
The Hessians of the fixed and random effects objectives
ignore all constraints except for constants
(lower and upper limits equal).

Prototype
*********
{xrst_spell_off}
{xrst_code cpp} */
void fit_model::sample_posterior(
   CppAD::mixed::d_sparse_rcv&               hes_fixed_obj_out  ,
   CppAD::mixed::d_sparse_rcv&               hes_random_obj_out ,
   size_t                                    n_sample           ,
   CppAD::vector<double>&                    sample_out         ,
   const CppAD::vector<double>&              fit_var_value      ,
   const std::map<std::string, std::string>& option_map         )
/* {xrst_code}
{xrst_spell_on}

{xrst_end fit_model_sample_posterior}
*/
{  double inf = std::numeric_limits<double>::infinity();
   //
   size_t n_var = n_fixed_ + n_random_;
   assert( fit_var_value.size() == n_var );

   // initialize sample_out as empty
   sample_out.resize(0);

   //
   // fixed_vec, random_opt
   CppAD::vector<double> fixed_vec(n_fixed_), random_opt(n_random_);
   unpack_random(pack_object_, fit_var_value, random_opt);
   unpack_fixed(pack_object_,   fit_var_value, fixed_vec);
   //
   // convert dismod_at random effects to cppad_mixed random effects
   d_vector cppad_mixed_random_opt = random_const_.remove( random_opt );
   //
   // convert dismod_at fixed effect to cppad_mixed fixed effects
   d_vector cppad_mixed_fixed_vec(n_fixed_);
   scale_fixed_effect(fixed_vec, cppad_mixed_fixed_vec);
   //
   // solution.fixed_opt
   CppAD::mixed::fixed_solution solution;
   solution.fixed_opt = cppad_mixed_fixed_vec;
   //
   // hes_fixed_obj_rcv
   CppAD::mixed::d_sparse_rcv hes_fixed_obj_rcv = hes_fixed_obj(
      cppad_mixed_fixed_vec, cppad_mixed_random_opt
   );
   // hes_random_obj_rcv
   CppAD::mixed::d_sparse_rcv hes_random_obj_rcv = hes_random_obj(
      cppad_mixed_fixed_vec, cppad_mixed_random_opt
   );
   //
   // var_lower, var_upper
   d_vector var_lower(n_var), var_upper(n_var);
   get_var_limits(
      var_lower, var_upper, var2prior_, prior_table_
   );
   //
   // hes_fixed_obj_out
   {  CppAD::vector<size_t> var_id = fixed2var_id(pack_object_);
      size_t nnz   = hes_fixed_obj_rcv.nnz();
      CppAD::vector<bool> both_variable(nnz);
      size_t n_both_variable = 0;
      for(size_t k = 0; k < nnz; ++k)
      {  size_t r = hes_fixed_obj_rcv.row()[k];
         size_t c = hes_fixed_obj_rcv.col()[k];
         //
         size_t i = var_id[r];
         size_t j = var_id[c];
         //
         both_variable[k]  = var_lower[i] < var_upper[i];
         both_variable[k] &= var_lower[j] < var_upper[j];
         //
         if( both_variable[k] )
             ++n_both_variable;
      }
      CppAD::mixed::sparse_rc pattern(n_var, n_var, n_both_variable);
      size_t count = 0;
      for(size_t k = 0; k < nnz; ++k) if( both_variable[k] )
      {  size_t r = hes_fixed_obj_rcv.row()[k];
         size_t c = hes_fixed_obj_rcv.col()[k];
         pattern.set(count++, var_id[r], var_id[c]);
      }
      assert( count == n_both_variable);
      CppAD::mixed::d_sparse_rcv info( pattern );
      count = 0;
      for(size_t k = 0; k < nnz; ++k) if( both_variable[k] )
         info.set(count++, hes_fixed_obj_rcv.val()[k] );
      //
      hes_fixed_obj_out = info;
   }
   // hes_random_obj_out
   {  CppAD::vector<size_t> var_id = random2var_id(pack_object_);
      size_t nnz = hes_random_obj_rcv.nnz();
      CppAD::mixed::sparse_rc pattern(n_var, n_var, nnz);
      for(size_t k = 0; k < nnz; ++k)
      {  size_t r = hes_random_obj_rcv.row()[k];
         size_t c = hes_random_obj_rcv.col()[k];
         pattern.set(k, var_id[r], var_id[c]);
      }
      CppAD::mixed::d_sparse_rcv info( pattern );
      for(size_t k = 0; k < nnz; ++k)
         info.set(k, hes_random_obj_rcv.val()[k] );
      //
      hes_random_obj_out = info;
   }
   //
   // cppad_mixed_fixed_lower
   CppAD::vector<double> pack_vec( n_var );
   CppAD::vector<double> fixed_lower(n_fixed_);
   for(size_t i = 0; i < n_var; i++)
   {  size_t prior_id    = var2prior_.value_prior_id(i);
      double const_value = var2prior_.const_value(i);
      if( ! std::isnan(const_value ) )
         pack_vec[i] = const_value;
      else if( prior_id == DISMOD_AT_NULL_SIZE_T )
         pack_vec[i] = -inf;
      else
         pack_vec[i] = prior_table_[prior_id].lower;
   }
   unpack_fixed(pack_object_, pack_vec, fixed_lower);
   CppAD::vector<double> cppad_mixed_fixed_lower(n_fixed_);
   scale_fixed_effect(fixed_lower, cppad_mixed_fixed_lower);

   // cppad_mixed_fixed_upper
   CppAD::vector<double> fixed_upper(n_fixed_);
   for(size_t i = 0; i < n_var; i++)
   {  size_t prior_id    = var2prior_.value_prior_id(i);
      double const_value = var2prior_.const_value(i);
      if( ! std::isnan(const_value) )
         pack_vec[i] = const_value;
      else if( prior_id == DISMOD_AT_NULL_SIZE_T )
         pack_vec[i] = +inf;
      else
         pack_vec[i] = prior_table_[prior_id].upper;
   }
   unpack_fixed(pack_object_, pack_vec, fixed_upper);
   CppAD::vector<double> cppad_mixed_fixed_upper(n_fixed_);
   scale_fixed_effect(fixed_upper, cppad_mixed_fixed_upper);
   //
   // check diagonal of information matrix is positive
   // (except for bound constrained variables)
   std::string msg = "";
   CppAD::vector<size_t> pack_index = fixed2var_id(pack_object_);
   size_t K = hes_fixed_obj_rcv.nnz();
   CppAD::vector<double> hessian_diagonal(n_fixed_);
   for(size_t j = 0; j < n_fixed_; ++j)
      hessian_diagonal[j] = 0.0;
   for(size_t k = 0; k < K; k++)
   {  size_t i = hes_fixed_obj_rcv.row()[k];
      size_t j = hes_fixed_obj_rcv.col()[k];
      double v = hes_fixed_obj_rcv.val()[k];
      if( i == j )
         hessian_diagonal[j] = v;
   }
   for(size_t j = 0; j < n_fixed_; ++j)
   if( fixed_lower[j] != fixed_upper[j] && hessian_diagonal[j] <= 0.0 )
   {  size_t var_id = pack_index[j];
      if( msg == "" )
      {  msg = "sample asymptotic: Following fixed effect's\n"
         "upper bound not equal its lower bound "
         "and diagonal of Hessian is not positive\n";
      }
      msg += "var_id = " + CppAD::to_string( var_id )
         + ", var_value = " + CppAD::to_string(fit_var_value[var_id])
         + ", lower = " + CppAD::to_string(fixed_lower[j])
         + ", upper = " + CppAD::to_string(fixed_upper[j])
         + ", hessian = " + CppAD::to_string( hessian_diagonal[j] ) + "\n"
      ;
   }
   if( msg != "" )
   {  log_message(db_, &std::cerr, "warning", msg);
      assert( sample_out.size() == 0 );
      return;
   }
   //
   // sample_fix, rcond
   CppAD::vector<double> sample_fix(n_sample * n_fixed_);
   msg          = "";
   try {
      // sample fixed effects
      double rcond = std::numeric_limits<double>::quiet_NaN();
      msg = sample_fixed(
         sample_fix,
         hes_fixed_obj_rcv,
         solution,
         cppad_mixed_fixed_lower,
         cppad_mixed_fixed_upper,
         rcond
      );
      std::string str   = get_str_map(option_map, "asymptotic_rcond_lower");
      double      lower = std::stod( str );
      str               = get_str_map(option_map, "print_level_fixed");
      int         level = std::stoi( str );
      if( 0 < level )
         std::cout << "sample_fixd: rcond = " << rcond << "\n";
      if( rcond < lower )
      {  str  = "sample_fixed: rcond = " + CppAD::to_string(rcond);
         str += "\nis less than asymptotic_rcond_lower = ";
         str += CppAD::to_string(lower);
         if( msg == "" )
            msg = str;
         else
            msg += "\n" + str;
      }
   }
   catch(const std::exception& e)
   {  std::string message("sample_command: std::exception: ");
      message += e.what();
      dismod_at::error_exit(message);
   }
   catch(const CppAD::mixed::exception& e)
   {  std::string catcher("sample_command");
      msg = e.message(catcher);
      log_message(db_, &std::cerr, "warning", msg);
      // this case becomes a fatal error after Hessians are stored
      assert( sample_out.size() == 0 );
      return;
   }
   if( msg != "" )
   {  log_message(db_, &std::cerr, "warning", msg);
      // this case becomes a fatal error after Hessians are stored
      assert( sample_out.size() == 0 );
      return;
   }
   //
   // random_options
   // (same as in run_fit)
   std::string options = "";
   options += "String  sb  yes";
   options += "\nNumeric bound_relax_factor 0.0";
   options += "\nNumeric tol "
      + get_str_map(option_map, "tolerance_random");
   options += "\nNumeric acceptable_tol "
      + get_str_map(option_map, "tolerance_random");
   options += "\nInteger max_iter "
      + get_str_map(option_map, "max_num_iter_random");
   options += "\nInteger print_level "
      + get_str_map(option_map, "print_level_random");
   options += "\nString derivative_test "
      + get_str_map(option_map, "derivative_test_random") + "\n";
   std::string random_options = options;
   //
   // random_in
   CppAD::vector<double> random_in(n_random_);
   unpack_random(pack_object_, start_var_, random_in);
   //
   // convert from dismod_at random effects to cppad_mixed random effects
   d_vector cppad_mixed_random_in    = random_const_.remove( random_in );
   //
   // set lower and upper limits to infinity for cppad_mixed random effects
   size_t cppad_mixed_n_random = n_random_ - n_random_equal_;
   d_vector cppad_mixed_random_lower(cppad_mixed_n_random);
   d_vector cppad_mixed_random_upper(cppad_mixed_n_random);
   for(size_t i = 0; i < cppad_mixed_n_random; ++i)
   {  cppad_mixed_random_lower[i] = -inf;
      cppad_mixed_random_upper[i] = +inf;
   }

   // sample_random
   CppAD::vector<double>
      cppad_mixed_sample_random(n_sample * cppad_mixed_n_random);
   msg = "";
   if( cppad_mixed_n_random > 0 )
   {  assert(n_random_ > n_random_equal_ );
      try {
         msg = sample_random(
            cppad_mixed_sample_random,
            random_options,
            cppad_mixed_fixed_vec,
            cppad_mixed_random_lower,
            cppad_mixed_random_upper,
            cppad_mixed_random_in
         );
      }
      catch(const std::exception& e)
      {  std::string message("sample_command: std::exception: ");
         message += e.what();
         log_message(db_, &std::cerr, "error", msg);
         dismod_at::error_exit(message);
      }
      catch(const CppAD::mixed::exception& e)
      {  std::string catcher("sample_command");
         msg = e.message(catcher);
         log_message(db_, &std::cerr, "warning", msg);
         // this case becomes a fatal error after Hessians are stored
         assert( sample_out.size() == 0 );
         return;
      }
      if( msg != "" )
      {  log_message(db_, &std::cerr, "warning", msg);
         // this case becomes a fatal error after Hessians are stored
         assert( sample_out.size() == 0 );
         return;
      }
   }
   // ----------------------------------------------------------------------
   // sample_out
   sample_out.resize( n_sample * n_var );
   CppAD::vector<double> cppad_mixed_one_sample_random(cppad_mixed_n_random);
   CppAD::vector<double> cppad_mixed_one_sample_fixed(n_fixed_);
   CppAD::vector<double> one_sample_random(n_random_);
   CppAD::vector<double> one_sample_fixed(n_fixed_);
   for(size_t i_sample = 0; i_sample < n_sample; i_sample++)
   {  for(size_t j = 0; j < n_fixed_; ++j)
      {  cppad_mixed_one_sample_fixed[j] =
         sample_fix[ i_sample * n_fixed_ + j];
      }
      for(size_t j = 0; j < cppad_mixed_n_random; ++j)
      {  cppad_mixed_one_sample_random[j] =
         cppad_mixed_sample_random[ i_sample * cppad_mixed_n_random + j];
      }
      one_sample_random = random_const_.restore(
         cppad_mixed_one_sample_random
      );
      unscale_fixed_effect(cppad_mixed_one_sample_fixed, one_sample_fixed);
      //
      // pack_vec
      pack_fixed(pack_object_, pack_vec, one_sample_fixed);
      pack_random(pack_object_, pack_vec, one_sample_random);
      //
      // copy to output vector
      for(size_t i = 0; i < n_var; i++)
         sample_out[ i_sample * n_var + i ] = pack_vec[i];
   }
   return;
}
// ===========================================================================
// private virtual functions
// ===========================================================================
// ran_likelihood
fit_model::a1_vector fit_model::ran_likelihood(
   const a1_vector& fixed_vec                ,
   const a1_vector& cppad_mixed_random_vec   )
{  //
   // check for case where all random effects are constrained
   assert( n_random_ >= n_random_equal_ );
   if( n_random_ == n_random_equal_ )
      return a1_vector(0);
   //
   // convert from cppad_mixed random effects to dismod_at random effects
   a1_vector random_vec = random_const_.restore( cppad_mixed_random_vec );
   //
   // packed vector
   a1_vector pack_vec( pack_object_.size() );
   //
   // put the fixed and random effects into pack_vec
   a1_vector fixed_tmp(n_fixed_);
   unscale_fixed_effect(fixed_vec, fixed_tmp);
   pack_fixed(pack_object_, pack_vec, fixed_tmp);
   pack_random(pack_object_, pack_vec, random_vec);
   //
   // evaluate the data and prior residuals that depend on the random effects
   CppAD::vector< residual_struct<a1_double> > data_ran, prior_ran;
   bool hold_out       = true;
   bool random_depend  = true;
   data_ran   = data_object_.like_all(hold_out, random_depend, pack_vec);
   prior_ran  = prior_object_.random(pack_vec);
   //
   // number of data and prior residuals
   size_t n_data_ran    = data_ran.size();
   size_t n_prior_ran   = prior_ran.size();
   //
   // check for the case where we return the empty vector
   if( n_data_ran == 0 && n_prior_ran == 0 )
      return a1_vector(0);
   //
# ifndef NDEBUG
   // no absolute value terms
   for(size_t i = 0; i < n_data_ran; i++)
   {  density_enum density = data_ran[i].density;
      assert( ! nonsmooth_density(density) );
   }
   for(size_t i = 0; i < n_prior_ran; i++)
   {  density_enum density = prior_ran[i].density;
      assert( ! nonsmooth_density(density) );
   }
# endif
   // size ran_den
   a1_vector ran_den(1);
   //
   // initialize summation of smooth part
   ran_den[0] = 0.0;
   //
   // data_ran terms
   for(size_t i = 0; i < n_data_ran; i++)
      ran_den[0] += data_ran[i].logden_smooth;
   //
   // random effects prior
   for(size_t i = 0; i < n_prior_ran; i++)
      ran_den[0] += prior_ran[i].logden_smooth;
   //
   // convert from log-density to negative log density
   ran_den[0] = - ran_den[0];
   //
   return ran_den;
}
// ---------------------------------------------------------------------------
// fix_likelihood
fit_model::a1_vector fit_model::fix_likelihood(
   const a1_vector& fixed_vec   )
{  // local vectors
   a1_vector random_vec( pack_object_.random_size() );
   a1_vector a1_pack_vec( pack_object_.size() );
   //
   // set random_vec
   for(size_t i = 0; i < random_vec.size(); i++)
   {  random_vec[i] = CppAD::nan( a1_double(0.0) );
      if( random_lower_[i] == random_upper_[i] )
         random_vec[i] = random_lower_[i];
   }
   //
   // put the fixed and random effects into pack_vec
   a1_vector fixed_tmp(n_fixed_);
   unscale_fixed_effect(fixed_vec, fixed_tmp);
   pack_fixed(pack_object_, a1_pack_vec, fixed_tmp);
   pack_random(pack_object_, a1_pack_vec, random_vec);
   //
   // evaluate the data and prior residuals that only depend on fixed effects
   // and random effects with bounds that are equal
   CppAD::vector< residual_struct<a1_double> > data_fix, prior_fix;
   bool hold_out      = true;
   bool random_depend = false;
   data_fix  = data_object_.like_all(hold_out, random_depend, a1_pack_vec);
   prior_fix = prior_object_.fixed(a1_pack_vec);
# ifndef NDEBUG
   if( n_random_ == n_random_equal_ )
   {  // ran_likelihood returns the empty vector in this case
      // so we need to include rest of the data here.
      for(size_t i = 0; i < n_random_; i++)
         assert( random_lower_[i] == random_upper_[i] );
      random_depend = true;
      CppAD::vector< residual_struct<a1_double> > data_ran =
         data_object_.like_all(hold_out, random_depend, a1_pack_vec);
      assert( data_ran.size() == 0 );
   }
# endif
   //
   // number of data and prior residuals
   size_t n_data_fix    = data_fix.size();
   size_t n_prior_fix   = prior_fix.size();
   //
   // count the number of absolute value terms
   size_t n_abs = 0;
   for(size_t i = 0; i < n_data_fix; i++)
   {  if( ! Constant(data_fix[i].logden_sub_abs) )
         n_abs++;
   }
   for(size_t i = 0; i < n_prior_fix; i++)
   {  if( ! Constant(prior_fix[i].logden_sub_abs) )
         n_abs++;
   }
   // size fix_den
   a1_vector fix_den(1 + n_abs);
   //
   // initialize summation of smooth part
   fix_den[0] = a1_double(0.0);
   //
   // initialize index for non-smooth part
   size_t i_abs = 0;
   //
   // data_fix terms
   for(size_t i = 0; i < n_data_fix; i++)
   {  fix_den[0] += data_fix[i].logden_smooth;
      if( ! Constant(data_fix[i].logden_sub_abs) )
         fix_den[1 + i_abs++] = data_fix[i].logden_sub_abs;
   }
   //
   // fixed effects prior
   for(size_t i = 0; i < n_prior_fix; i++)
   {  fix_den[0] += prior_fix[i].logden_smooth;
      if( ! Constant(prior_fix[i].logden_sub_abs) )
         fix_den[1 + i_abs++] = prior_fix[i].logden_sub_abs;
   }
   //
   // convert from log-density to negative log density
   fix_den[0] = - fix_den[0];
   //
   return fix_den;
}
// --------------------------------------------------------------------------
// fix_constraint
// pass the fixed effects constraint function to the cppad_mixed base class
fit_model::a1_vector fit_model::fix_constraint(
   const a1_vector& fixed_vec_scaled )
{  // constraints are in original space (not scaled space)
   a1_vector fixed_vec(n_fixed_);
   unscale_fixed_effect(fixed_vec_scaled, fixed_vec);
   //
   // compute differences in original space
   a1_vector result( diff_prior_.size() );
   for(size_t k = 0; k < diff_prior_.size(); k++)
   {  size_t plus_var_id    = diff_prior_[k].plus_var_id;
      size_t minus_var_id   = diff_prior_[k].minus_var_id;
      size_t plus_fixed_id  = var_id2fixed_[ plus_var_id ];
      size_t minus_fixed_id = var_id2fixed_[ minus_var_id ];
      result[k] = fixed_vec[plus_fixed_id] - fixed_vec[minus_fixed_id];
   }
   return result;
}
// ---------------------------------------------------------------------------
// fatal_error
void fit_model::fatal_error(const std::string& error_message)
{
   std::string msg = "cppad_mixed: " + error_message;
   if( simulate_index_ != -1 )
   {  msg += "\nfitting simulate_index = ";
      msg += CppAD::to_string(simulate_index_);
   }
   // prints on std::cerr, logs in database, generates an assert, then exits
   error_exit(msg);
}
// warning
void fit_model::warning(const std::string& warning_message)
{  std::string msg = "cppad_mixed: " + warning_message;
   if( simulate_index_ != -1 )
   {  msg += "\nfitting simulate_index = ";
      msg += CppAD::to_string(simulate_index_);
   }
   // prints on std::cerr as well as logs in database
   if( warn_on_stderr_ )
      log_message(db_, &std::cerr, "warning", msg);
   else
      log_message(db_, DISMOD_AT_NULL_PTR, "warning", msg);
}
} // DISMOD_AT_END_NAMESPACE
