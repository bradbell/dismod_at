// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <cppad/mixed/exception.hpp>
# include <dismod_at/adj_integrand.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/a1_double.hpp>
# include <dismod_at/grid2line.hpp>
# include <dismod_at/cohort_ode.hpp>
# include <dismod_at/cohort_ode.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_subgroup_table.hpp>

/*
{xrst_begin adj_integrand dev}
{xrst_spell
   adj
   adjint
}

Compute Adjusted Integrand On a Line
####################################

Syntax
******

| ``adj_integrand`` *adjint_obj* (
| |tab| *n_covariate*,
| |tab| *rate_case* ,
| |tab| *age_table* ,
| |tab| *time_table* ,
| |tab| *subgroup_table* ,
| |tab| *integrand_table* ,
| |tab| *s_info_vec* ,
| |tab| *pack_object*
| );
| *adj_line* = *adjint_obj* . ``line`` (
| |tab| *line_age* ,
| |tab| *line_time* ,
| |tab| *integrand_id* ,
| |tab| *n_child* ,
| |tab| *child* ,
| |tab| *subgroup_id* ,
| |tab| *x* ,
| |tab| *pack_vec*
| )

Prototype
*********
{xrst_literal
   // BEGIN_ADJ_INTEGRAND_PROTOTYPE
   // END_ADJ_INTEGRAND_PROTOTYPE
}
{xrst_literal
   // BEGIN_LINE_PROTOTYPE
   // END_LINE_PROTOTYPE
}

n_covariate
***********
is the number of covariates; i.e., the size of the
:ref:`get_covariate_table@covariate_table` .

rate_case
*********
This is the value of
:ref:`option_table@rate_case` in the option table.

age_table
*********
This argument is the :ref:`age_table-name` .
A reference to *age_table* is used by *adjint_obj*
( *age_table* must no be deleted for as long as
*adjint_obj* is used).

time_table
**********
This argument is the :ref:`time_table-name` .
A reference to *time_table* is used by *adjint_obj* .

subgroup_table
**************
This argument is the :ref:`subgroup_table-name` .
A reference to *subgroup_table* is used by *adjint_obj* .

integrand_table
***************
This argument is the :ref:`integrand_table-name` .
A reference to *integrand_table* is used by *adjint_obj* .

s_info_vec
**********
For each :ref:`smooth_table@smooth_id` ,

   *s_info_vec* [ *smooth_id*  ]

is the corresponding :ref:`smooth_info-name` information.
None of the prior information in *s_info_vec* is used.
A reference to *s_info_vec* is used by *adjint_obj* .

pack_object
***********
This is the :ref:`pack_info-name` information corresponding to
the :ref:`model_variables-name` .

line_age
********
This vector is the age points at which the adjusted integrand is computed.
It is faster if successive points have close values in age; e.g.,

   *line_age* [ *k* ] <= *line_age* [ *k* +1]

n_line
======
We use the notation *n_line* = *line_age* . ``size`` () for the number
points at which the approximate solution is returned.

line_time
*********
This vector has size *n_line* and is
the time points at which the adjusted integrand is computed.
It is faster if successive points have close values in time; e.g.,

   *line_time* [ *k* ] >= *line_time* [ *k* +1]

ODE
===
In the case where the integrand (specified by *integrand_id* )
requires solving the
:ref:`integrand_table@integrand_name@ODE` ,
the line must be a cohort; i.e., it must satisfy the following properties:

#. The first line age must be the minimum value in the age table; i.e.,

      ``line_age`` [0] = ``min_`` *i* *table_age* [ *i* ]

#. The line ages must be monotone increasing; i.e.,
   for *k* = 1 , ..., *n_line* ``-1``

      *line_age* [ *k* ``-1`` ] < *line_age* [ *k* ]

#. The difference between the line ages and times is constant; i.e.,
   for *k* = 1 , ..., *n_line* ``-1``

      *line_time* [ *k* ] ``-`` *line_age* [ *k* ] == *line_time* [0] ``-`` *line_age* [0]

In this case
*cohort_age* and *cohort_time* are better names for the
arguments *line_age* and *line_time* .

integrand_id
************
This is the :ref:`integrand_table@integrand_id`
in the integrand table.

n_child\_
*********
set to number of :ref:`option_table@Parent Node@Children` .

child
*****
Is the :ref:`child_info@table_id2child@child` corresponding
to this adjustment of the integrand.

subgroup_id
***********
is the :ref:`avgint_table@subgroup_id` corresponding
to this adjustment of the integrand.

pack_vec
********
is all the :ref:`model_variables-name` in the order
specified by *pack_object* .

Float
=====
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

adj_line
********
The return value is a vector with size *n_line*
and *adj_line* [ *i* ] is the
:ref:`avg_integrand@Adjusted Integrand`
at age *line_age* [ *i* ]
and time *line_time* [ *i* ] .
{xrst_toc_hidden
   example/devel/model/adj_integrand_xam.cpp
}
Example
*******
The file :ref:`adj_integrand_xam.cpp-name` contains an example and test
of using this routine.

{xrst_end adj_integrand}
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_ADJ_INTEGRAND_PROTOTYPE
adj_integrand::adj_integrand(
   size_t                                    n_covariate      ,
   const std::string&                        rate_case        ,
   const CppAD::vector<double>&              age_table        ,
   const CppAD::vector<double>&              time_table       ,
   const CppAD::vector<subgroup_struct>&     subgroup_table   ,
   const CppAD::vector<integrand_struct>&    integrand_table  ,
   const CppAD::vector<mulcov_struct>&       mulcov_table     ,
   const CppAD::vector<smooth_info>&         s_info_vec       ,
   const pack_info&                          pack_object      )
// END_ADJ_INTEGRAND_PROTOTYPE
:
rate_case_         (rate_case)        ,
age_table_         (age_table)        ,
time_table_        (time_table)       ,
subgroup_table_     (subgroup_table)  ,
integrand_table_   (integrand_table)  ,
s_info_vec_        (s_info_vec)       ,
pack_object_       (pack_object)      ,
node_cov_map_      (n_covariate)      ,
double_rate_       (number_rate_enum) ,
a1_double_rate_    (number_rate_enum)
{  // set mulcov_pack_info_
   size_t n_integrand = integrand_table.size();
   mulcov_pack_info_.resize( mulcov_table.size() );
   CppAD::vector<size_t> rate_value_index(number_rate_enum);
   CppAD::vector<size_t> meas_value_index(n_integrand);
   CppAD::vector<size_t> meas_noise_index(n_integrand);
   for(size_t i = 0; i < number_rate_enum; ++i)
      rate_value_index[i] = 0;
   for(size_t i = 0; i < n_integrand; ++i)
   {  meas_value_index[i] = 0;
      meas_noise_index[i] = 0;
   }
   for(size_t mulcov_id = 0; mulcov_id < mulcov_table.size(); ++mulcov_id)
   {
      size_t rate_id      = size_t( mulcov_table[mulcov_id].rate_id );
      size_t integrand_id = size_t(mulcov_table[mulcov_id].integrand_id);
      //
      // only group covariate multipliers can be integrands
      int    smooth_id    = mulcov_table[mulcov_id].group_smooth_id;
      if( smooth_id == DISMOD_AT_NULL_INT )
         mulcov_pack_info_[mulcov_id].smooth_id = size_t(smooth_id);
      //
      else switch( mulcov_table[mulcov_id].mulcov_type )
      {  case rate_value_enum:
         mulcov_pack_info_[mulcov_id] =
            pack_object.group_rate_value_info(
               rate_id, rate_value_index[rate_id]
         );
         ++rate_value_index[rate_id];
         break;

         case meas_value_enum:
         mulcov_pack_info_[mulcov_id] =
            pack_object.group_meas_value_info(
               integrand_id, meas_value_index[integrand_id]
         );
         ++meas_value_index[integrand_id];
         break;

         case meas_noise_enum:
         mulcov_pack_info_[mulcov_id] =
            pack_object.group_meas_noise_info(
               integrand_id, meas_noise_index[integrand_id]
         );
         ++meas_noise_index[integrand_id];
         break;

         default:
         assert(false);
      }
      assert( mulcov_pack_info_[mulcov_id].smooth_id == size_t(smooth_id) );
   }
}

// BEGIN_LINE_PROTOTYPE
template <class Float>
CppAD::vector<Float> adj_integrand::line(
   const CppAD::vector<double>&                       line_age         ,
   const CppAD::vector<double>&                       line_time        ,
   size_t                                             integrand_id     ,
   size_t                                             n_child          ,
   size_t                                             child            ,
   size_t                                             subgroup_id      ,
   const CppAD::vector<double>&                       x                ,
   const CppAD::vector<Float>&                        pack_vec         ,
// END_LINE_PROTOTYPE
   CppAD::vector<Float>&                              mulcov           ,
   CppAD::vector< CppAD::vector<Float> >&             rate             )
{  using CppAD::vector;
   //
   // node_id
   // This will be an extra argument to adj_integrand::line
   size_t node_id = 0;
   //
   // some temporaries
   pack_info::subvec_info info;
   vector<Float> smooth_value;
   // ---------------------------------------------------------------------
   // integrand for this average
   integrand_enum integrand = integrand_table_[integrand_id].integrand;
   //
   // group for this average
   size_t group_id = subgroup_table_[subgroup_id].group_id;
   //
   // first subgroup for this group
   size_t first_subgroup_id = pack_object_.first_subgroup_id(group_id);
   assert( first_subgroup_id <= subgroup_id );
   //
   // initialize other values for this average
   bool need_ode     = false;
   bool need_mulcov  = false;
   vector<bool> need_rate(number_rate_enum);
   for(size_t k = 0; k < number_rate_enum; ++k)
      need_rate[k] = false;
   switch( integrand )
   {
      // -----------------------------------------------------------------
      // need_ode = true;
      case susceptible_enum:
      case withC_enum:
      case prevalence_enum:
      case Tincidence_enum:
      case mtspecific_enum:
      case mtall_enum:
      case mtstandard_enum:
      need_ode = true;
      //
      // need_rate = true
      for(size_t k = 0; k < number_rate_enum; ++k)
         need_rate[k] = true;
      break;

      // -----------------------------------------------------------------
      case Sincidence_enum:
      need_rate[iota_enum] = true;
      break;

      case remission_enum:
      need_rate[rho_enum] = true;
      break;

      case mtexcess_enum:
      need_rate[chi_enum] = true;
      break;

      case mtother_enum:
      need_rate[omega_enum] = true;
      break;

      case mtwith_enum:
      need_rate[omega_enum] = true;
      need_rate[chi_enum]   = true;
      break;

      case relrisk_enum:
      need_rate[chi_enum]   = true;
      need_rate[omega_enum] = true;
      break;

      case mulcov_enum:
      need_mulcov = true;
      break;

      // -----------------------------------------------------------------
      default:
      assert( false);
   }
   // number of points in line
   size_t n_line = line_age.size();
   //
   // vector of effects
   vector<Float> effect(n_line), temp_1(n_line), temp_2(n_line), weight_grid;
   //
   // Effect (for error reporting)
   vector< vector<Float> > effect_mul(number_rate_enum);
   // -----------------------------------------------------------------------
   // mulcov is special case: no ode and no effects
   if( need_mulcov )
   {  mulcov.resize(n_line);
      //
      int mulcov_id    = integrand_table_[integrand_id].mulcov_id;
      info             = mulcov_pack_info_[mulcov_id];
      size_t smooth_id = info.smooth_id;
      if( info.smooth_id == DISMOD_AT_NULL_SIZE_T )
      {  for(size_t k = 0; k < n_line; ++k)
            mulcov[k] = 0.0;
      }
      else
      {  // interpolate covariate multiplier from smoothing grid to line
         smooth_value.resize(info.n_var);
         for(size_t k = 0; k < info.n_var; ++k)
            smooth_value[k] = pack_vec[info.offset + k];
         const smooth_info& s_info = s_info_vec_[smooth_id];
         mulcov = grid2line(
            line_age,
            line_time,
            age_table_,
            time_table_,
            s_info,
            smooth_value
         );
      }
      return mulcov;
   }
   // -----------------------------------------------------------------------
   // get value for each rate that is needed
   for(size_t rate_id = 0; rate_id < number_rate_enum; ++rate_id)
   if( need_rate[rate_id] )
   {  rate[rate_id].resize(n_line);
      effect_mul[rate_id].resize(n_line);
      //
      // parent rate for each point in the line
      info             = pack_object_.node_rate_value_info(rate_id, n_child);
      size_t smooth_id = info.smooth_id;
      //
      if( smooth_id == DISMOD_AT_NULL_SIZE_T )
      {  for(size_t k = 0; k < n_line; ++k)
            rate[rate_id][k] = 0.0;
      }
      else
      {  // interpolate this rate from smoothing grid to line
         smooth_value.resize(info.n_var);
         for(size_t k = 0; k < info.n_var; ++k)
            smooth_value[k] = pack_vec[info.offset + k];
         const smooth_info& s_info = s_info_vec_[smooth_id];
         rate[rate_id] = grid2line(
            line_age,
            line_time,
            age_table_,
            time_table_,
            s_info,
            smooth_value
         );
      }
      //
      // initialize effect as zero
      for(size_t k = 0; k < n_line; ++k)
         effect[k] = 0.0;
      //
      // include the child effect
      if( child < n_child )
      {  // child effect rate for each point in the line
         info      = pack_object_.node_rate_value_info(rate_id, child);
         smooth_id = info.smooth_id;
         if( smooth_id != DISMOD_AT_NULL_SIZE_T )
         {  // interpolate from smoothing grid to line
            smooth_value.resize(info.n_var);
            for(size_t k = 0; k < info.n_var; ++k)
               smooth_value[k] = pack_vec[info.offset + k];
            const smooth_info& s_info = s_info_vec_[smooth_id];
            //
            // temp_1 = child random effect
            temp_1 = grid2line(
               line_age,
               line_time,
               age_table_,
               time_table_,
               s_info,
               smooth_value
            );
            for(size_t k = 0; k < n_line; ++k)
               effect[k] += temp_1[k];
         }
      }
      //
      // include the group covariate effects on this rate
      size_t n_cov    = pack_object_.group_rate_value_n_cov(rate_id);
      for(size_t j = 0; j < n_cov; ++j)
      {  info        = pack_object_.group_rate_value_info(rate_id, j);
         if( info.group_id == group_id )
         {  smooth_id   = info.smooth_id;
            // interpolate from smoothing grid to line
            smooth_value.resize(info.n_var);
            for(size_t k = 0; k < info.n_var; ++k)
               smooth_value[k] = pack_vec[info.offset + k];
            const smooth_info& s_info = s_info_vec_[smooth_id];
            //
            // temp_1 = covariate multiplier fixed effect
            temp_1 = grid2line(
               line_age,
               line_time,
               age_table_,
               time_table_,
               s_info,
               smooth_value
            );
            //
            // temp_2 = covariate value
            size_t covariate_id = info.covariate_id;
            if( node_cov_map_[covariate_id].size() == 0 )
            {  for(size_t k = 0; k < n_line; ++k)
                  temp_2[k] = x[ info.covariate_id ];
            }
            else
            {  size_t weight_id   = node_cov_map_[covariate_id][node_id];
               const weight_info& w_info = w_info_vec_[weight_id];
               size_t n_age        = w_info.age_size();
               size_t n_time       = w_info.time_size();
               weight_grid.resize(n_age * n_time);
               for(size_t i = 0; i < n_age; i++)
               {  for(size_t ell = 0; ell < n_time; ++ell)
                     weight_grid[i * n_time + ell] = w_info.weight(i, ell);
               }
               temp_2 = grid2line(
                  line_age,
                  line_time,
                  age_table_,
                  time_table_,
                  w_info,
                  weight_grid
               );
            }
            for(size_t k = 0; k < n_line; ++k)
               effect[k] += temp_1[k] * temp_2[k];
         }
      }
      //
      // include the subgroup covariate effects on this rate
      n_cov = pack_object_.subgroup_rate_value_n_cov(rate_id);
      for(size_t j = 0; j < n_cov; ++j)
      {  info  = pack_object_.subgroup_rate_value_info(rate_id, j, 0);
         if( info.group_id == group_id )
         {  size_t k = subgroup_id - first_subgroup_id;
            assert( k < pack_object_.subgroup_size(group_id) );
            info  = pack_object_.subgroup_rate_value_info(rate_id, j, k);
            smooth_id   = info.smooth_id;
            // interpolate from smoothing grid to line
            smooth_value.resize(info.n_var);
            for(size_t ell = 0; ell < info.n_var; ++ell)
               smooth_value[ell] = pack_vec[info.offset + ell];
            const smooth_info& s_info = s_info_vec_[smooth_id];
            //
            // temp_1 = covariate multiplier random effect
            temp_1 = grid2line(
               line_age,
               line_time,
               age_table_,
               time_table_,
               s_info,
               smooth_value
            );
            //
            // temp_2 = covariate value
            size_t covariate_id = info.covariate_id;
            if( node_cov_map_[covariate_id].size() == 0 )
            {  for(size_t ell = 0; ell < n_line; ++ell)
                  temp_2[ell] = x[ info.covariate_id ];
            }
            else
            {  size_t weight_id   = node_cov_map_[covariate_id][node_id];
               const weight_info& w_info = w_info_vec_[weight_id];
               size_t n_age        = w_info.age_size();
               size_t n_time       = w_info.time_size();
               weight_grid.resize(n_age * n_time);
               for(size_t i = 0; i < n_age; i++)
               {  for(size_t ell = 0; ell < n_time; ++ell)
                     weight_grid[i * n_time + ell] = w_info.weight(i, ell);
               }
               temp_2 = grid2line(
                  line_age,
                  line_time,
                  age_table_,
                  time_table_,
                  w_info,
                  weight_grid
               );
            }
            for(size_t ell = 0; ell < n_line; ++ell)
               effect[ell] += temp_1[ell] * temp_2[ell];
         }
      }
      //
      // multiply parent rate by exponential of the total effect
      for(size_t k = 0; k < n_line; ++k)
      {  Float mul              = exp( effect[k] );
         effect_mul[rate_id][k] = mul;
         rate[rate_id][k]      *= mul;
      }
   }
   // -----------------------------------------------------------------------
   // solve the ode on the cohort specified by line_age and line_time[0]
   vector<Float> s_out(n_line), c_out(n_line);
   if( need_ode )
   {
# ifndef NDEBUG
      Float eps99  = 99.0 * CppAD::numeric_limits<Float>::epsilon();
      Float check  = line_time[0] - line_age[0];
      for(size_t k = 1; k < n_line; ++k)
      {  Float diff = line_time[k] - line_age[k];
         CppAD::NearEqual(diff, check, eps99, eps99);
      }
# endif
      Float pini = rate[pini_enum][0];
      cohort_ode(
         rate_case_,
         line_age,
         pini,
         rate[iota_enum],
         rate[rho_enum],
         rate[chi_enum],
         rate[omega_enum],
         s_out,
         c_out
      );
   }
# ifndef NDEBUG
   else
   {  for(size_t k = 0; k < n_line; ++k)
      {  s_out[k] = CppAD::numeric_limits<Float>::quiet_NaN();
         c_out[k] = CppAD::numeric_limits<Float>::quiet_NaN();
      }
   }
# endif
   // -----------------------------------------------------------------------
   // value of the integrand on the line
   vector<Float> result(n_line);
   Float infinity = std::numeric_limits<double>::infinity();
   Float zero     =  0.0;
   for(size_t k = 0; k < n_line; ++k)
   {  Float P = c_out[k] / ( s_out[k] + c_out[k] );
      bool need_P = false;
      switch(integrand)
      {  // ----------------------------------------------------------------
         // no ode cases
         // ----------------------------------------------------------------

         case Sincidence_enum:
         result[k] = rate[iota_enum][k];
         break;

         case remission_enum:
         result[k] = rate[rho_enum][k];
         break;

         case mtexcess_enum:
         result[k] = rate[chi_enum][k];
         break;

         case mtother_enum:
         result[k] = rate[omega_enum][k];
         break;

         case mtwith_enum:
         result[k] = rate[omega_enum][k] + rate[chi_enum][k];
         break;

         case relrisk_enum:
         result[k] = 1.0 + rate[chi_enum][k] / rate[omega_enum][k];
         break;

         // ----------------------------------------------------------------
         // need_ode cases
         // ----------------------------------------------------------------

         case susceptible_enum:
         result[k] = s_out[k];
         break;

         case withC_enum:
         result[k] = c_out[k];
         break;

         case prevalence_enum:
         result[k] = P;
         need_P    = true;
         break;

         case Tincidence_enum:
         result[k] = rate[iota_enum][k] * (1.0 - P);
         need_P    = true;
         break;

         case mtspecific_enum:
         result[k] = rate[chi_enum][k] * P;
         need_P    = true;
         break;

         case mtall_enum:
         result[k] = rate[omega_enum][k] + rate[chi_enum][k] * P;
         need_P    = true;
         break;

         case mtstandard_enum:
         result[k]  = rate[omega_enum][k] + rate[chi_enum][k];
         result[k] /= rate[omega_enum][k] + rate[chi_enum][k] * P;
         need_P    = true;
         break;

         // ---------------------------------------------------------------
         default:
         assert(false);
         // ---------------------------------------------------------------
      }
      bool ok = - infinity < result[k] && result[k] < infinity;
      if( need_P )
         ok &= zero <= P;
      if( ! ok )
      {  std::string msg = "Cannot compute the ";
         msg += integrand_enum2name[integrand];
         msg += " integrand.\n";
         if( need_P )
         {  msg += "age = " + CppAD::to_string(line_age[k]);
            msg += ", S = " + CppAD::to_string(s_out[k]);
            msg += ", C = " + CppAD::to_string(c_out[k]);
            msg += "\n";
         }
         bool first_rate = true;
         for(size_t rate_id = 0; rate_id < number_rate_enum; ++rate_id)
         {  if( need_rate[rate_id] )
            {   if( ! first_rate )
                  msg += ", ";
               Float mul = effect_mul[rate_id][k];
               msg += get_rate_name(rate_id) ;
               msg += " = " + CppAD::to_string( rate[rate_id][k] / mul );
               if( mul != 1.0 )
               {  msg += " adjusted = ";
                  msg += CppAD::to_string( rate[rate_id][k] );
               }
               first_rate = false;
            }
         }
         throw CppAD::mixed::exception( "adj_integrand", msg);
      }
   }
   // -----------------------------------------------------------------------
   // include measurement effects
   //
   // initialize effect as zero
   for(size_t k = 0; k < n_line; ++k)
      effect[k] = 0.0;
   //
   // group_meas_value covariates
   size_t n_cov = pack_object_.group_meas_value_n_cov(integrand_id);
   for(size_t j = 0; j < n_cov; ++j)
   {  info  = pack_object_.group_meas_value_info(integrand_id, j);
      if( info.group_id == group_id )
      {  size_t smooth_id = info.smooth_id;
         double x_j       = x[ info.covariate_id ];
         // interpolate from smoothing grid to cohort
         smooth_value.resize(info.n_var);
         for(size_t k = 0; k < info.n_var; ++k)
            smooth_value[k] = pack_vec[info.offset + k];
         const smooth_info& s_info = s_info_vec_[smooth_id];
         //
         // temp_1 = covariate multiplier fixed effects
         temp_1 = grid2line(
            line_age,
            line_time,
            age_table_,
            time_table_,
            s_info,
            smooth_value
         );
         for(size_t k = 0; k < n_line; ++k)
            effect[k] += temp_1[k] * x_j;
      }
   }
   //
   // subgroup_meas_value covariates
   n_cov = pack_object_.subgroup_meas_value_n_cov(integrand_id);
   for(size_t j = 0; j < n_cov; ++j)
   {  info  = pack_object_.subgroup_meas_value_info(integrand_id, j, 0);
      if( info.group_id == group_id )
      {  size_t k = subgroup_id - first_subgroup_id;
         info  = pack_object_.subgroup_meas_value_info(integrand_id, j, k);
         size_t smooth_id = info.smooth_id;
         double x_j       = x[ info.covariate_id ];
         // interpolate from smoothing grid to cohort
         smooth_value.resize(info.n_var);
         for(size_t ell = 0; ell < info.n_var; ++ell)
            smooth_value[ell] = pack_vec[info.offset + ell];
         const smooth_info& s_info = s_info_vec_[smooth_id];
         //
         // temp_1 = covariate multiplier random effects
         temp_1 = grid2line(
            line_age,
            line_time,
            age_table_,
            time_table_,
            s_info,
            smooth_value
         );
         for(size_t ell = 0; ell < n_line; ++ell)
            effect[ell] += temp_1[ell] * x_j;
      }
   }
   // -----------------------------------------------------------------------
   // multiply by exponential of total effect
   for(size_t k = 0; k < n_line; ++k)
       result[k] *= exp( effect[k] );
   //
   return result;
}

# define DISMOD_AT_INSTANTIATE_ADJ_INTEGTAND_LINE(Float)                    \
   template                                                                \
   CppAD::vector<Float> adj_integrand::line(                               \
      const CppAD::vector<double>&                  line_age         ,    \
      const CppAD::vector<double>&                  line_time        ,    \
      size_t                                        integrand_id     ,    \
      size_t                                        n_child          ,    \
      size_t                                        child            ,    \
      size_t                                        subgroup_id      ,    \
      const CppAD::vector<double>&                  x                ,    \
      const CppAD::vector<Float>&                   pack_vec         ,    \
      CppAD::vector<Float>&                         mulcov           ,    \
      CppAD::vector< CppAD::vector<Float> >&        rate                  \
   );                                                                      \
\
   CppAD::vector<Float> adj_integrand::line(                               \
      const CppAD::vector<double>&                  line_age         ,    \
      const CppAD::vector<double>&                  line_time        ,    \
      size_t                                        integrand_id     ,    \
      size_t                                        n_child          ,    \
      size_t                                        child            ,    \
      size_t                                        subgroup_id      ,    \
      const CppAD::vector<double>&                  x                ,    \
      const CppAD::vector<Float>&                   pack_vec         )    \
   {  return line(                                                        \
         line_age,                                                       \
         line_time,                                                      \
         integrand_id,                                                   \
         n_child,                                                        \
         child,                                                          \
         subgroup_id,                                                    \
         x,                                                              \
         pack_vec,                                                       \
         Float ## _mulcov_,                                              \
         Float ## _rate_                                                 \
      );                                                                  \
   }

// instantiations
DISMOD_AT_INSTANTIATE_ADJ_INTEGTAND_LINE( double )
DISMOD_AT_INSTANTIATE_ADJ_INTEGTAND_LINE( a1_double )

} // END_DISMOD_AT_NAMESPACE
