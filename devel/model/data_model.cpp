// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin data_model_ctor dev}
{xrst_spell
   bool
}

Data Model: Constructor
#######################

Syntax
******

| ``data_model`` *data_object* (
| |tab| *fit_simulated_data* ,
| |tab| *meas_noise_effect* ,
| |tab| *rate_case* ,
| |tab| *bound_random* ,
| |tab| *n_covariate* ,
| |tab| *ode_step_size* ,
| |tab| *age_avg_grid* ,
| |tab| *age_table* ,
| |tab| *time_table* ,
| |tab| *subgroup_table* ,
| |tab| *integrand_table* ,
| |tab| *prior_table* ,
| |tab| *subset_object* ,
| |tab| *subset_cov_value* ,
| |tab| *w_info_vec* ,
| |tab| *s_info_vec* ,
| |tab| *pack_object* ,
| |tab| *child_info4data*
| )

Prototype
*********
{xrst_literal
   // BEGIN_DATA_MODEL_PROTOTYPE
   // END_DATA_MODEL_PROTOTYPE
}

data_object
***********
This is the ``data_model`` object being constructed.

fit_simulated_data
******************
If this is true, we are fitting simulated data that comes from the
:ref:`data_sim_table-name` .

meas_noise_effect
*****************
This is the value of
:ref:`option_table@meas_noise_effect` in the option table.

rate_case
*********
This is the value of
:ref:`option_table@rate_case` in the option table.

bound_random
************
This is the
:ref:`option_table@Optimize Random Only@bound_random` .

n_covariate
***********
This is the number of covariates; i.e., number or rows in
:ref:`covariate_table-name` .

ode_step_size
*************
This is the :ref:`option_table@Age Average Grid@ode_step_size` .

age_avg_grid
************
This is the :ref:`age_avg_table@Age Average Grid` .

age_table
*********
This is the :ref:`get_age_table@age_table` .

time_table
**********
This is the :ref:`get_time_table@time_table` .

subgroup_table
**************
This is the :ref:`get_subgroup_table@subgroup_table` .

integrand_table
***************
This is the :ref:`get_integrand_table@integrand_table` .

subset_object
*************
This is the sub-sampled version of the data or avgint table; see
:ref:`subset_data@subset_data_obj` ,
:ref:`avgint_subset@avgint_subset_obj` .

subset_cov_value
****************
This is the sub-sampled version of the covariates; see
:ref:`subset_data@subset_data_cov_value` ,
:ref:`avgint_subset@avgint_subset_cov_value` .
A reference to *subset_cov_value* is used by *data_object*
(so *age_table* cannot be deleted for as long as
*data_object* is used).

w_info_vec
**********
For each :ref:`weight_table@weight_id` ,

   *w_info_vec* [ *weight_id*  ]

is the corresponding :ref:`weight_info-name` information.
In addition, the constant weight is included at the end of the vector; i.e.,
at index *w_info_vec* . ``size`` () ``-1`` .

s_info_vec
**********
For each :ref:`smooth_table@smooth_id` ,

   *s_info_vec* [ *smooth_id*  ]

is the corresponding :ref:`smooth_info-name` information.
For each *s_info_vec* [ *smooth_id* ] object,
only the following functions are used:
``age_size`` , ``time_size`` , ``age_id`` , ``time_id`` .

pack_object
***********
This is the :ref:`pack_info-name` information corresponding to
the :ref:`model_variables-name` .
A reference to *pack_object* is used by *data_object* .

child_info4data
***************
This is the :ref:`child_info-name` information corresponding to
parent node, node table, and data table.

data_object.n_covariate\_
*************************
set to *n_covariate* .

data_object.ode_step_size\_
***************************
set to *ode_step_size* .

data_object.pack_object\_
*************************
set to *pack_object* .

data_object.minimum_meas_cv\_
*****************************
Set to a vector with the same size as *integrand_table* and

| |tab| ``minimum_meas_cv_`` [ *integrand_id* ] =
| |tab| |tab| ``integrand_table`` [ ``integrand_id`` ]. *minimum_meas_cv*

replace_like_called\_
*********************
initialize as false.

n_child\_
*********
set to number of children.

subset_data_obj\_
*****************
for each *subset_id* , set ``subset_data_obj_`` [ *subset_id* ]
fields that are command both data_subset and avgint_subset.

data_info\_
***********
for each *subset_id* , set
``data_info_`` [ *subset_id* ]
is extra information for each data point.
Each of the fields in
``data_info_`` [ *subset_id* ]
is described below:

density
=======
Is the
:ref:`get_density_table@density_enum` corresponding
to the *subset_id* .

child
=====
This ``size_t`` value is the
:ref:`child_info@table_id2child@child` index corresponding
to this *subset_id* .
Note that if it is equal to ``n_child_`` ,
this data point corresponds to th parent node.

depend_on_ran_var
=================
This ``bool`` value is true (false) if the data point corresponding to
*subset_id* depends (does not depend) on a random effect
that is a variable; i.e., not constrained to be a constant.

avgint_obj\_
************
The ``avg_integrand`` :ref:`constructor<avg_integrand_ctor-name>`
is used to create this argument.
The arguments to this constructor are a subset of the
arguments to the ``data_model`` constructor and are passed through
(with the same name) to the ``avg_integrand`` constructor.

{xrst_end data_model_ctor}
-----------------------------------------------------------------------------
*/
# include <cppad/mixed/exception.hpp>
# include <dismod_at/min_max_vector.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/residual_density.hpp>
# include <dismod_at/a1_double.hpp>
# include <dismod_at/avgint_subset.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// destructor
data_model::~data_model(void)
{ }

// BEGIN_DATA_MODEL_PROTOTYPE
template <class SubsetStruct>
data_model::data_model(
   bool                                     fit_simulated_data ,
   const std::string&                       meas_noise_effect  ,
   const std::string&                       rate_case          ,
   double                                   bound_random       ,
   size_t                                   n_covariate        ,
   double                                   ode_step_size      ,
   const CppAD::vector<double>&             age_avg_grid       ,
   const CppAD::vector<double>&             age_table          ,
   const CppAD::vector<double>&             time_table         ,
   const CppAD::vector<subgroup_struct>&    subgroup_table     ,
   const CppAD::vector<integrand_struct>&   integrand_table    ,
   const CppAD::vector<mulcov_struct>&      mulcov_table       ,
   const CppAD::vector<prior_struct>&       prior_table        ,
   const CppAD::vector<SubsetStruct>&       subset_object      ,
   const CppAD::vector<double>&             subset_cov_value   ,
   const CppAD::vector<weight_info>&        w_info_vec         ,
   const CppAD::vector<smooth_info>&        s_info_vec         ,
   const pack_info&                         pack_object        ,
   const child_info&                        child_info4data    )
// END_DATA_MODEL_PROTOTYPE
:
// const
fit_simulated_data_ ( fit_simulated_data)           ,
n_covariate_        (n_covariate)                   ,
ode_step_size_      (ode_step_size)                 ,
n_child_            ( child_info4data.child_size() )   ,
subset_cov_value_   (subset_cov_value)              ,
pack_object_        (pack_object)                   ,
avgint_obj_(
   ode_step_size,
   rate_case,
   age_avg_grid,
   age_table,
   time_table,
   subgroup_table,
   integrand_table,
   mulcov_table,
   w_info_vec,
   s_info_vec,
   pack_object
),
avg_noise_obj_(
   ode_step_size,
   age_avg_grid,
   age_table,
   time_table,
   subgroup_table,
   integrand_table,
   w_info_vec,
   s_info_vec,
   pack_object
)
{  assert( bound_random >= 0.0 );
   assert( n_child_ == pack_object.child_size() );
   // ------------------------------------------------------------------------
   //
   using std::string;
   //
   // meas_noise_effect_
   // add_std
   if( meas_noise_effect == "add_std_scale_all" )
      meas_noise_effect_ = add_std_scale_all_enum;
   else if( meas_noise_effect == "add_std_scale_none" )
      meas_noise_effect_ = add_std_scale_none_enum;
   else if( meas_noise_effect == "add_std_scale_log" )
      meas_noise_effect_ = add_std_scale_log_enum;
   // add_var
   else if( meas_noise_effect == "add_var_scale_all" )
      meas_noise_effect_ = add_var_scale_all_enum;
   else if( meas_noise_effect == "add_var_scale_none" )
      meas_noise_effect_ = add_var_scale_none_enum;
   else if( meas_noise_effect == "add_var_scale_log" )
      meas_noise_effect_ = add_var_scale_log_enum;
   else
      assert(false);
   //
   // minimum_meas_cv_
   minimum_meas_cv_.resize( integrand_table.size() );
   for(size_t i = 0; i < integrand_table.size(); ++i)
      minimum_meas_cv_[i] = integrand_table[i].minimum_meas_cv;
   //
   // replace_like_called_: initialize
   replace_like_called_ = false;
   //
   // -----------------------------------------------------------------------
   // subset_data_obj_
   //
   // only set values that are in subset_data_struct and avgint_subset_struct
   size_t n_subset = subset_object.size();
   subset_data_obj_.resize(n_subset);
   assert( subset_cov_value.size() == n_covariate * n_subset );
   for(size_t i = 0; i < n_subset; i++)
   {  subset_data_obj_[i].original_id  = subset_object[i].original_id;
      subset_data_obj_[i].integrand_id = subset_object[i].integrand_id;
      subset_data_obj_[i].node_id      = subset_object[i].node_id;
      subset_data_obj_[i].subgroup_id  = subset_object[i].subgroup_id;
      subset_data_obj_[i].weight_id    = subset_object[i].weight_id;
      subset_data_obj_[i].age_lower    = subset_object[i].age_lower;
      subset_data_obj_[i].age_upper    = subset_object[i].age_upper;
      subset_data_obj_[i].time_lower   = subset_object[i].time_lower;
      subset_data_obj_[i].time_upper   = subset_object[i].time_upper;
   }
   // -----------------------------------------------------------------------
   // data_info_
   //
   // has same size as subset_data_obj
   data_info_.resize( n_subset );
   //
   for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
   {  // information for this data point
      size_t original_id = subset_object[subset_id].original_id;

      // integrand as an enum instead of integrand_id
      size_t  integrand_id     = subset_object[subset_id].integrand_id;
      integrand_enum integrand = integrand_table[integrand_id].integrand;

      // child of parent node that this data is associated with
      size_t  child            = child_info4data.table_id2child(original_id);

      // group_id for this data point
      size_t subgroup_id       = subset_object[subset_id].subgroup_id;
      size_t group_id          = subgroup_table[subgroup_id].group_id;

      // set child information for this data point
      data_info_[subset_id].child     = child;

      // Does this data point depend on the random effects
      // that do not have equal bounds
      bool depend_on_ran_var = false;
      if( bound_random > 0.0 )
      {  CppAD::vector<size_t> rate_id_vec;
         switch( integrand )
         {  case Sincidence_enum:
            rate_id_vec.push_back( size_t(iota_enum) );
            break;

            case remission_enum:
            rate_id_vec.push_back( size_t(rho_enum) );
            break;

            case mtexcess_enum:
            rate_id_vec.push_back( size_t(chi_enum) );
            break;

            case mtother_enum:
            rate_id_vec.push_back( size_t(omega_enum) );
            break;

            case mtwith_enum:
            case relrisk_enum:
            rate_id_vec.push_back( size_t(chi_enum) );
            rate_id_vec.push_back( size_t(omega_enum) );
            break;

            default:
            for(size_t ell = 0; ell < number_rate_enum; ell++)
               rate_id_vec.push_back( ell );
            break;
         }
         // change depend_on_ran_var for rate effects that are not constant
         if( child < n_child_ )
         for(size_t ell = 0; ell < rate_id_vec.size(); ell++)
         {  size_t rate_id = rate_id_vec[ell];
            // check if any random effects for this rate are not constant
            size_t smooth_id =
               pack_object.node_rate_value_info(rate_id, child).smooth_id;
            if( smooth_id != DISMOD_AT_NULL_SIZE_T )
            {  const smooth_info& s_info = s_info_vec[smooth_id];
               depend_on_ran_var |= ! s_info.all_const_value();
            }
         }
         // change depend_on_ran_var for subgroup_rate_value covariates
         for(size_t ell = 0; ell < rate_id_vec.size(); ell++)
         {  size_t rate_id = rate_id_vec[ell];
            size_t n_cov = pack_object.subgroup_rate_value_n_cov(rate_id);
            for(size_t j = 0; j < n_cov; ++j)
            {  const pack_info::subvec_info info_0 =
                  pack_object.subgroup_rate_value_info(rate_id, j, 0);
               size_t n_sub =
                  pack_object.subgroup_rate_value_n_sub(rate_id, j);
               if( info_0.group_id == group_id )
               for(size_t k = 0; k < n_sub; ++k)
               {  const pack_info::subvec_info info =
                  pack_object.subgroup_rate_value_info(rate_id, j, k);
                  //
                  assert( info.group_id  == info_0.group_id );
                  assert( info.smooth_id == info_0.smooth_id );
                  //
                  size_t smooth_id = info.smooth_id;
                  assert( smooth_id != DISMOD_AT_NULL_SIZE_T );
                  const smooth_info& s_info = s_info_vec[smooth_id];
                  depend_on_ran_var |= ! s_info.all_const_value();
               }
            }
         }
         // change depend_on_ran_var for subgroup_meas_value covariates
         size_t n_cov = pack_object.subgroup_meas_value_n_cov(integrand_id);
         for(size_t j = 0; j < n_cov; ++j)
         {  const pack_info::subvec_info info_0 =
               pack_object.subgroup_meas_value_info(integrand_id, j, 0);
            size_t n_sub =
               pack_object.subgroup_meas_value_n_sub(integrand_id, j);
            if( info_0.group_id == group_id )
            for(size_t k = 0; k < n_sub; ++k)
            {  const pack_info::subvec_info info =
               pack_object.subgroup_meas_value_info(integrand_id, j, k);
               //
               assert( info.group_id  == info_0.group_id );
               assert( info.smooth_id == info_0.smooth_id );
               //
               size_t smooth_id = info.smooth_id;
               assert( smooth_id != DISMOD_AT_NULL_SIZE_T );
               const smooth_info& s_info = s_info_vec[smooth_id];
               depend_on_ran_var |= ! s_info.all_const_value();
            }
         }
      }
      data_info_[subset_id].depend_on_ran_var = depend_on_ran_var;
   }
}
/*
{xrst_begin data_model_replace_like dev}

Set Value Necessary for Likelihood (not for Average Integrand)
##############################################################

Syntax
******
*data_object* . ``replace_like`` ( *subset_data_obj* )

Purpose
*******
The values
:ref:`data_table@density_id` ,
:ref:`data_table@hold_out` ,
:ref:`data_table@meas_value` ,
:ref:`data_table@meas_std` ,
:ref:`data_table@eta` ,
:ref:`data_table@nu` ,
are not necessary to calculate the average integrand.
However, the are necessary to use the functions
:ref:`data_object.like_one<data_model_like_one-name>` and
:ref:`data_object.like_all<data_model_like_all-name>` .
If the :ref:`data_sim_table@data_sim_value`
values in *subset_data_obj* are not nan, these
are simulated values used in calculating the likelihood.

data_object
***********
This object has prototype

   ``data_model`` *data_object*

see :ref:`data_object constructor<data_model_ctor@data_object>` .

subset_object
=============
We use *subset_object*
for the :ref:`subset_object<data_model_ctor-name>` in the *data_object*
constructor.

n_subset
========
We use the notation

   *n_subset* = *subset_object* . ``size`` ()

subset_id
=========
This an index between zero and *n_subset* ``- 1`` .
It is used to refer to the corresponding element of
*subset_object* .

subset_data_obj
***************
This argument has prototype

   ``const CppAD::vector<subset_data_struct>&`` *subset_data_obj*

and has size *n_subset* .
For each *subset_id* ,
and for *field* = ``density_id`` , ``hold_out`` , ``meas_value`` , ``meas_std`` ,
*subset_data_obj* [ *subset_id* ]. *field* ,
is used as a replacement for
*subset_object* [ ``subset_id`` ]. *field* .

Laplace Density
***************
The Laplace density is only allowed if the model for the
:ref:`avg_integrand-name` does not depend on the value of the random effects.
If this assumption is violated, an error messages is generated and the
program exits.

{xrst_end data_model_replace_like}
*/
void data_model::replace_like(
      const CppAD::vector<subset_data_struct>&  subset_data_obj )
{
   // n_subset
   size_t n_subset = subset_data_obj_.size();
   assert( subset_data_obj.size() == n_subset );
   //
   // replace density_id, hold_out, meas_value, meas_std, eta, nu
   for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
   {  subset_data_obj_[subset_id].density =
         subset_data_obj[subset_id].density;
      subset_data_obj_[subset_id].hold_out =
         subset_data_obj[subset_id].hold_out;
      subset_data_obj_[subset_id].meas_value =
         subset_data_obj[subset_id].meas_value;
      subset_data_obj_[subset_id].meas_std =
         subset_data_obj[subset_id].meas_std;
      subset_data_obj_[subset_id].eta =
         subset_data_obj[subset_id].eta;
      subset_data_obj_[subset_id].nu =
         subset_data_obj[subset_id].nu;
      subset_data_obj_[subset_id].data_sim_value =
         subset_data_obj[subset_id].data_sim_value;
      //
      data_info_[subset_id].density = subset_data_obj[subset_id].density;
      //
      bool laplace = data_info_[subset_id].density == laplace_enum;
      laplace     |= data_info_[subset_id].density == log_laplace_enum;
      if( laplace && data_info_[subset_id].depend_on_ran_var )
      {  std::string msg, table_name;
         size_t data_id = subset_data_obj_[subset_id].original_id;
         table_name = "data";
         msg  = "density_id corresponds to laplace or log_laplace and\n";
         msg += "model depends on random effects that are not constrained";
         error_exit(msg, table_name, data_id);
      }
   }
   replace_like_called_ = true;
   return;
}

/*
-----------------------------------------------------------------------------
{xrst_begin data_model_average dev}

Data Model: Compute One Average Integrand
#########################################

Syntax
******
*avg* = *data_object* . ``average`` ( *subset_id* , *pack_vec* )

data_object
***********
This object has prototype

   ``data_model`` *data_object*

see :ref:`data_object constructor<data_model_ctor@data_object>` .
The object *data_object* is effectively const.

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

subset_id
*********
This argument has prototype

   ``size_t`` *subset_id*

and is the :ref:`subset_data@subset_data_obj@subset_id`
we are computing the average integrand for.

pack_vec
********
This argument has prototype

   ``const CppAD::vector<`` *Float* >& *pack_vec*

and is all the :ref:`model_variables-name` in the order
specified by :ref:`pack_info-name` .

avg
***
The return value has prototype

   *Float* ``avg``

This is the
:ref:`average integrand<avg_integrand@Average Integrand, A_i>`
for the specified data point.
{xrst_toc_hidden
   example/devel/model/avg_no_ode_xam.cpp
   example/devel/model/avg_yes_ode_xam.cpp
}
Example
*******
The files
:ref:`avg_no_ode_xam.cpp-name` and :ref:`avg_yes_ode_xam.cpp-name`
contain examples using this routine.

{xrst_end data_model_average}
*/
template <class Float>
Float data_model::average(
   size_t                        subset_id ,
   const CppAD::vector<Float>&   pack_vec  )
{
   // arguments to avg_integrand::rectangle
   const subset_data_struct& data_item = subset_data_obj_[subset_id];
   double age_lower    = data_item.age_lower;
   double age_upper    = data_item.age_upper;
   double time_lower   = data_item.time_lower;
   double time_upper   = data_item.time_upper;
   size_t weight_id    = size_t( data_item.weight_id );
   size_t integrand_id = size_t( data_item.integrand_id );
   size_t subgroup_id  = size_t( data_item.subgroup_id );
   size_t child        = size_t( data_info_[subset_id].child );
   CppAD::vector<double> x(n_covariate_);
   for(size_t j = 0; j < n_covariate_; j++)
      x[j] = subset_cov_value_[subset_id * n_covariate_ + j];
   //
   // compute average integrand
   Float result = avgint_obj_.rectangle(
      age_lower,
      age_upper,
      time_lower,
      time_upper,
      weight_id,
      integrand_id,
      n_child_,
      child,
      subgroup_id,
      x,
      pack_vec
   );
   //
   assert( ! CppAD::isnan(result) );
   return result;
}

/*
-----------------------------------------------------------------------------
{xrst_begin data_model_like_one dev}
{xrst_spell
   subvectors
}

One Weighted Residual and Log-Likelihood for any Integrands
###########################################################

Syntax
******

| *residual* = *data_object* . ``like_one`` (
| |tab| *subset_id* , *pack_vec* , *avg* , *delta_out*
| )

Requirement
***********
One must call :ref:`replace_like<data_model_replace_like-name>`
before calling this function.

Log-likelihood
**************
We use :ref:`data_like@Notation@y_i` to denote the
:ref:`data_table@meas_value` corresponding
to this :ref:`subset_data@subset_data_obj@subset_id` .
The log-likelihood computed by ``like_one`` is the mapping

.. math::

   \ell (u, \theta) = C + \log [ \B{p} ( y_i | u , \theta ) ]

where :math:`u` are the random effects,
:math:`\theta` are the fixed effects, and
:math:`C` is a constant that does
not depend on :math:`( u , \theta )`.

data_object
***********
This object has prototype

   ``data_model`` *data_object*

see :ref:`data_object constructor<data_model_ctor@data_object>` .
It is effectively const
(some internal arrays are used for temporary work space and kept between
calls to avoid re-allocating memory).

fit_simulated_data
==================
If :ref:`data_model_ctor@fit_simulated_data` is true,
we are fitting simulated data *data_sim_value* ,
otherwise we are fitting the actual data *meas_value* .

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

subset_id
*********
This argument has prototype

   ``size_t`` *subset_id*

and is the :ref:`subset_data@subset_data_obj@subset_id`
we are computing the weighted residual and log-likelihood for.

pack_vec
********
This argument has prototype

   ``const CppAD::vector<`` *Float* >& *pack_vec*

and is all the :ref:`model_variables-name` in the order
specified by :ref:`pack_info-name` ; i.e.,
:math:`(u , \theta)`.
Only the :ref:`pack_info_group_meas-name` subvectors of *pack_vec* are used
by ``like_one`` (note that other components of :math:`(u, \theta )`
are used to compute *avg* documented below).

avg
***
This argument has prototype

   ``const`` *Float* & *avg*

and is the
:ref:`average integrand<avg_integrand@Average Integrand, A_i>` ,
:math:`A_i ( u , \theta )`, for the specified data point.
This can be calculated using :ref:`data_model_average-name` .

delta_out
*********
The input value of *delta_out* does not matter.
Upon return it is the adjusted standard deviation
:ref:`delta<data_like@Adjusted Standard Deviation, delta_i(theta)>`
for this data point.

residual
********
The return value has prototype

   ``residual_struct<`` *Float* > *residual*

see :ref:`residual_density@residual@residual_struct` .
It contains the weighted residual and the corresponding log-density.

index
=====
Note that the :ref:`residual_density@index` field is set to
the value *subset_id* .
{xrst_toc_hidden
   example/devel/model/like_one_xam.cpp
}
Example
*******
The file :ref:`like_one_xam.cpp-name` contains an example and test
of using this routine.

{xrst_end data_model_like_one}
*/
template <class Float>
residual_struct<Float> data_model::like_one(
   size_t                        subset_id ,
   const CppAD::vector<Float>&   pack_vec  ,
   const Float&                  avg       ,
   Float&                        delta_out )
{
   assert( pack_object_.size() == pack_vec.size() );
   assert( replace_like_called_ );

   // covariate information for this data point
   CppAD::vector<double> x(n_covariate_);
   for(size_t j = 0; j < n_covariate_; j++)
      x[j] = subset_cov_value_[subset_id * n_covariate_ + j];
   double eta          = subset_data_obj_[subset_id].eta;
   double nu           = subset_data_obj_[subset_id].nu;
   double meas_value   = subset_data_obj_[subset_id].meas_value;
   double meas_std     = subset_data_obj_[subset_id].meas_std;
   double age_lower    = subset_data_obj_[subset_id].age_lower;
   double age_upper    = subset_data_obj_[subset_id].age_upper;
   double time_lower   = subset_data_obj_[subset_id].time_lower;
   double time_upper   = subset_data_obj_[subset_id].time_upper;
   size_t weight_id    = size_t( subset_data_obj_[subset_id].weight_id );
   size_t subgroup_id  = size_t( subset_data_obj_[subset_id].subgroup_id );
   size_t integrand_id = size_t( subset_data_obj_[subset_id].integrand_id );
   //
   double data_sim_value   = subset_data_obj_[subset_id].data_sim_value;
   //
   // average noise effect
   Float std_effect = avg_noise_obj_.rectangle(
      age_lower,
      age_upper,
      time_lower,
      time_upper,
      weight_id,
      subgroup_id,
      integrand_id,
      x,
      pack_vec
   );
   //
   assert( meas_std  > 0.0 );
   double meas_cv = minimum_meas_cv_[integrand_id];
   double Delta   = std::max(meas_std, meas_cv * std::fabs(meas_value) );
   density_enum density = data_info_[subset_id].density;
   //
   // transformed standard deviation
   double sigma = Delta;
   if( log_density(density) )
      sigma = log( meas_value + eta + Delta ) - log( meas_value + eta );
   //
   // Compute the adusted standard deviation, delta_out
   switch( meas_noise_effect_ )
   {  // add_std
      case add_std_scale_all_enum:
      delta_out  = sigma * (1.0  + std_effect);
      break;
      case add_std_scale_none_enum:
      delta_out  = sigma + std_effect;
      break;
      case add_std_scale_log_enum:
      if( log_density(density) )
         delta_out  = sigma * (1.0  + std_effect);
      else
         delta_out  = sigma + std_effect;
      break;
      // -----------------------------------------------------------------
      // add_var
      case add_var_scale_all_enum:
      delta_out  = sigma * sqrt(1.0  + std_effect);
      break;
      case add_var_scale_none_enum:
      delta_out  = sqrt( sigma * sigma + std_effect );
      break;
      case add_var_scale_log_enum:
      if( log_density(density) )
         delta_out  = sigma * sqrt(1.0  + std_effect);
      else
         delta_out  = sqrt( sigma * sigma + std_effect );
      break;
      // -----------------------------------------------------------------
      default:
      assert(false);
   }
   //
   bool diff  = false;
   bool prior = false;
# ifndef NDEBUG
   if( fit_simulated_data_ )
      assert( ! std::isnan(data_sim_value) );
   else
      assert( std::isnan(data_sim_value) );
# endif
   struct residual_struct<Float> residual = residual_density(
      Float(data_sim_value),
      Float(meas_value),
      avg,
      delta_out,
      density,
      Float(eta),
      Float(nu),
      subset_id,
      diff,
      prior
   );
   //
   return residual;
}
/*
-------------------------------------------------------------------------------
{xrst_begin data_model_like_all dev}
{xrst_spell
   bool
}

All the Weighted Residuals and Log-Likelihoods
##############################################

Syntax
******

| *residual_vec* = *data_object* . ``like_all`` (
| |tab| *hold_out* , *random_depend* , *pack_vec*
| )

Requirement
***********
One must call :ref:`replace_like<data_model_replace_like-name>`
before calling this function.

data_object
***********
This object has prototype

   ``data_model`` *data_object*

see :ref:`data_object constructor<data_model_ctor@data_object>` .
It is effectively const
(some internal arrays are used for temporary work space and kept between
calls to avoid re-allocating memory).

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

hold_out
********
This argument has prototype

   ``bool`` *hold_out*

If it is true, the flagged data will be held out,
otherwise it will not.

random_depend
*************
This argument has prototype

   ``bool`` *random_depend*

If it is true (false), only residuals that depend on the random effects
that have non-equal (equal) bounds, are included.

pack_vec
********
This argument has prototype

   ``const CppAD::vector<`` *Float* >& *pack_vec*

and is all the :ref:`model_variables-name` in the order
specified by :ref:`pack_info-name` ; i.e., :math:`(u , \theta)`.
If *random_depend* is false,
only the components of *u* that have equal lower and upper
bounds are defined (the other components are unspecified).

residual_vec
************
The return value has prototype

   ``CppAD::vector< residual_struct<`` *Float* > > *residual_vec*

index
=====
For each element of *residual_vec* ,
the residual :ref:`residual_density@index`
is set to the :ref:`data_subset_table@data_subset_id`
for the corresponding residual.

Include Hold Outs
=================
If *hold_out* is false,
the size of *residual* is equal the number of subset data values
:ref:`subset_data@subset_data_obj@n_subset` .
The order of the residuals is the same as in
:ref:`data_model_ctor@subset_object` in the
*data_object* constructor.

No Hold Outs
============
If *hold_out* is true,
the size of *residual* is equal the number of subset data values
:ref:`subset_data@subset_data_obj@n_subset` ,
minus the number for which
:ref:`data_table@hold_out` is one.
The order of the residuals is not specified in this case.

Log Density
***********
The log of the density
:math:`\B{p}( y | u , \theta )`,
for the data that is included,
is the sum of the log of the densities corresponding to all the
:ref:`residuals<residual_density-name>` in *residual_vec* .
{xrst_toc_hidden
   example/devel/model/like_all_xam.cpp
}
Example
*******
:ref:`like_all_xam.cpp-name`

{xrst_end data_model_like_all}
*/
template <class Float>
CppAD::vector< residual_struct<Float> > data_model::like_all(
   bool                        hold_out      ,
   bool                        random_depend ,
   const CppAD::vector<Float>& pack_vec      )
{  assert( replace_like_called_ );
   //
   // loop over the subsampled data
   CppAD::vector< residual_struct<Float> > residual_vec;
   for(size_t subset_id = 0; subset_id < subset_data_obj_.size(); subset_id++)
   {  bool keep = hold_out == false;
      keep     |= subset_data_obj_[subset_id].hold_out == 0;
      if( random_depend )
         keep &= data_info_[subset_id].depend_on_ran_var == true;
      else
         keep &= data_info_[subset_id].depend_on_ran_var == false;
      assert( data_info_[subset_id].child <= n_child_ );
      if( keep )
      {  Float avg = average(subset_id, pack_vec);

         // compute its residual and log likelihood
         Float not_used;
         residual_struct<Float> residual =
            like_one(subset_id, pack_vec, avg, not_used);
         residual_vec.push_back( residual );
      }
   }
   return residual_vec;
}

// ------------------------------------------------------------------------
# define DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(SubsetStruct)       \
template data_model::data_model(                                   \
   bool                                     fit_simulated_data ,  \
   const std::string&                       meas_noise_effect  ,  \
   const std::string&                       rate_case          ,  \
   double                                   bound_random       ,  \
   size_t                                   n_covariate        ,  \
   double                                   ode_step_size      ,  \
   const CppAD::vector<double>&             age_avg_grid       ,  \
   const CppAD::vector<double>&             age_table          ,  \
   const CppAD::vector<double>&             time_table         ,  \
   const CppAD::vector<subgroup_struct>&    subgroup_table     ,  \
   const CppAD::vector<integrand_struct>&   integrand_table    ,  \
   const CppAD::vector<mulcov_struct>&      mulcov_table       ,  \
   const CppAD::vector<prior_struct>&       prior_table        ,  \
   const CppAD::vector<SubsetStruct>&       subset_object      ,  \
   const CppAD::vector<double>&             subset_cov_value   ,  \
   const CppAD::vector<weight_info>&        w_info_vec         ,  \
   const CppAD::vector<smooth_info>&        s_info_vec         ,  \
   const pack_info&                         pack_object        ,  \
   const child_info&                        child_info4data       \
);
DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(subset_data_struct)
DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(avgint_subset_struct)
// ------------------------------------------------------------------------
# define DISMOD_AT_INSTANTIATE_DATA_MODEL(Float)            \
   template Float data_model::average(                     \
      size_t                        subset_id,            \
      const CppAD::vector<Float>&   pack_vec              \
   );                                                      \
   template residual_struct<Float>                         \
   data_model::like_one(                                   \
      size_t                        subset_id,            \
      const CppAD::vector<Float>&   pack_vec ,            \
      const Float&                  avg      ,            \
      Float&                        delta_out             \
   );                                                      \
   template CppAD::vector< residual_struct<Float> >        \
   data_model::like_all(                                   \
      bool                          hold_out ,            \
      bool                          parent   ,            \
      const CppAD::vector<Float>&   pack_vec              \
   );                                                      \

// instantiations
DISMOD_AT_INSTANTIATE_DATA_MODEL( double )
DISMOD_AT_INSTANTIATE_DATA_MODEL( a1_double )


} // END DISMOD_AT_NAMESPACE
