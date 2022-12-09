// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <cppad/mixed/exception.hpp>
# include <dismod_at/avg_noise_effect.hpp>
# include <dismod_at/grid2line.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

/*
------------------------------------------------------------------------------
{xrst_begin avg_noise_effect_ctor}

Constructing An Average Noise Effect Object
###########################################

Syntax
******

| ``avg_noise_effect`` *avg_noise_obj* (
| |tab| *ode_step_size* ,
| |tab| *age_avg_grid* ,
| |tab| *age_table* ,
| |tab| *time_table* ,
| |tab| *subgroup_table* ,
| |tab| *integrand_table* ,
| |tab| *w_info_vec* ,
| |tab| *s_info_vec* ,
| |tab| *pack_object*
| );

Prototype
*********
{xrst_literal
   // BEGIN_AVG_NOISE_EFFECT_PROTOTYPE
   // END_AVG_NOISE_EFFECT_PROTOTYPE
}

ode_step_size
*************
This is the value of
:ref:`option_table@Age Average Grid@ode_step_size` in the option table.

age_avg_grid
************
This is the :ref:`age_avg_grid-name` .
A reference to *age_avg_grid* is used by *avg_noise_obj*
( *age_avg_grid* must no be deleted for as long as
*avg_noise_obj* is used).

age_table
*********
This argument is the :ref:`age_table-name` .
A reference to *age_table* is used by *avg_noise_obj*
( *age_table* must no be deleted for as long as
*avg_noise_obj* is used).

time_table
**********
This argument is the :ref:`time_table-name` .
A reference to *time_table* is used by *avg_noise_obj* .

subgroup_table
**************
This argument is the :ref:`subgroup_table-name` .
A reference to *subgroup_table* is used by *avg_noise_obj* .

integrand_table
***************
This argument is the :ref:`integrand_table-name` .
A reference to *integrand_table* is used by *avg_noise_obj* .

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
None of the prior information in *s_info_vec* is used.
A reference to *s_info_vec* is used by *avg_noise_obj* .

pack_object
***********
This is the :ref:`pack_info-name` information corresponding to
the :ref:`model_variables-name` .

Example
*******
The file :ref:`avg_noise_effect_xam.cpp-name` contains an example and test
of using this routine.

{xrst_end avg_noise_effect_ctor}
*/
// BEGIN_AVG_NOISE_EFFECT_PROTOTYPE
avg_noise_effect::avg_noise_effect(
      double                                    ode_step_size    ,
      const CppAD::vector<double>&              age_avg_grid     ,
      const CppAD::vector<double>&              age_table        ,
      const CppAD::vector<double>&              time_table       ,
      const CppAD::vector<subgroup_struct>&     subgroup_table   ,
      const CppAD::vector<integrand_struct>&    integrand_table  ,
      const CppAD::vector<weight_info>&         w_info_vec       ,
      const CppAD::vector<smooth_info>&         s_info_vec       ,
      const pack_info&                          pack_object      )
// END_AVG_NOISE_EFFECT_PROTOTYPE
:
ode_step_size_             ( ode_step_size )   ,
age_table_                 ( age_table )       ,
time_table_                ( time_table )      ,
subgroup_table_            ( subgroup_table )  ,
integrand_table_           ( integrand_table ) ,
w_info_vec_                ( w_info_vec )      ,
s_info_vec_                ( s_info_vec )      ,
pack_object_               ( pack_object )     ,
double_time_line_object_   ( age_avg_grid )    ,
a1_double_time_line_object_( age_avg_grid )
{ }
/*
------------------------------------------------------------------------------
{xrst_begin avg_noise_effect_rectangle}

Computing One Average Integrand
###############################

Syntax
******

| *avg* = *avg_noise_obj* . ``rectangle`` (
| |tab| *age_lower* ,
| |tab| *age_upper* ,
| |tab| *time_lower* ,
| |tab| *time_upper* ,
| |tab| *weight_id* ,
| |tab| *subgroup_id* ,
| |tab| *integrand_id* ,
| |tab| *x* ,
| |tab| *pack_vec*
| )

Prototype
*********
{xrst_literal
   // BEGIN_RECTANGLE_PROTOTYPE
   // END_RECTANGLE_PROTOTYPE
}

age_lower
*********
the lower age in the rectangle.

age_upper
*********
the upper age in the rectangle; *age_lower* <= *age_upper* .

time_lower
**********
the lower time in the rectangle.

time_upper
**********
the upper time in the rectangle; *time_lower* <= *time_upper* .

weight_id
*********
This is the :ref:`weight_table@weight_id`
in the weight table corresponding to this average integrand.

subgroup_id
***********
This is the :ref:`subgroup_table@subgroup_id`
in the subgroup table corresponding to this average integrand.

integrand_id
************
This is the :ref:`integrand_table@integrand_id`
in the integrand table corresponding to this average integrand.

x
*
This is the vector of covariates for this average.

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

pack_vec
********
is all the :ref:`model_variables-name` in the order
specified by *pack_object* .

avg
***
The return value *avg* is the average of the integrand
using the specified weighting over the specified rectangle
{xrst_toc_hidden
   example/devel/model/avg_noise_effect_xam.cpp
}

Example
*******
The file :ref:`avg_noise_effect_xam.cpp-name` contains an example and test
of using this routine.

{xrst_end avg_noise_effect_rectangle}
*/

// BEGIN_RECTANGLE_PROTOTYPE
template <class Float>
Float avg_noise_effect::rectangle(
   double                           age_lower        ,
   double                           age_upper        ,
   double                           time_lower       ,
   double                           time_upper       ,
   size_t                           weight_id        ,
   size_t                           subgroup_id      ,
   size_t                           integrand_id     ,
   const CppAD::vector<double>&     x                ,
   const CppAD::vector<Float>&      pack_vec         ,
// END_RECTANGLE_PROTOTYPE
   time_line_vec<Float>&            time_line_object ,
   CppAD::vector<Float>&            effect           )
{  using CppAD::vector;
   typedef typename time_line_vec<Float>::time_point  time_point;

   // numerical precision
   double eps99 = 99.0 * std::numeric_limits<double>::epsilon();

   // weight information for this average
   // constant weighting is at the end of w_info_vec_
   size_t weight_index = w_info_vec_.size() - 1;
   if( weight_id != DISMOD_AT_NULL_SIZE_T )
   {  assert( weight_id < weight_index );
      weight_index = weight_id;
   }
   const weight_info& w_info( w_info_vec_[weight_index] );

   // number of ages and time in the weight grid
   size_t n_age  = w_info.age_size();
   size_t n_time = w_info.time_size();

   // weight_grid_
   weight_grid_.resize(n_age * n_time);
   for(size_t i = 0; i < n_age; i++)
   {  for(size_t j = 0; j < n_time; ++j)
         weight_grid_[i * n_time + j] = w_info.weight(i, j);
   }


   // specialize the time_line object for this rectangle
   time_line_object.specialize(
      age_lower, age_upper, time_lower, time_upper
   );

   // The extended age grid
   const vector<double>& extend_grid = time_line_object.extend_grid();
   size_t                sub_lower   = time_line_object.sub_lower();
   size_t                sub_upper   = time_line_object.sub_upper();

   // age_lower == extend_grid[sub_lower]
   assert(time_line_vec<Float>::near_equal(extend_grid[sub_lower],age_lower));

   // age_upper == extend_grid[sub_upper]
   assert(time_line_vec<Float>::near_equal(extend_grid[sub_upper],age_upper));

   // n_age: number of ages (time line for each time line)
   n_age = sub_upper - sub_lower + 1;

   // one_time
   bool one_time = time_line_vec<double>::near_equal(time_lower, time_upper);

   // n_time: number times in each time line
   // d_time: spacing between time points
   n_time        = 1;
   double d_time = 0.0;
   if( ! one_time )
   {  n_time = size_t(
         2.0 - eps99 + (time_upper - time_lower) / ode_step_size_
      );
      d_time = (time_upper - time_lower) / double(n_time - 1);
   }

   // n_line: total number of age, time points
   size_t n_line = n_age * n_time;

   // resize temporaris
   line_age_.resize(n_line);
   line_time_.resize(n_line);

   // line_age_
   // line_time_
   for(size_t i = 0; i < n_age; ++i)
   {  for(size_t j = 0; j < n_time; ++j)
      {  size_t k =  i * n_time + j;
         size_t age_index = sub_lower + i;
         line_age_[k]     = extend_grid[age_index];
         line_time_[k]    = time_lower + double(j) * d_time;
      }
   }

   // -----------------------------------------------------------------------
   // effect
   // -----------------------------------------------------------------------
   //
   // some temporaries
   pack_info::subvec_info info;
   vector<Float> temp(n_line), smooth_value;
   //
   // initialize effect as zero
   effect.resize(n_line);
   for(size_t k = 0; k < n_line; ++k)
      effect[k] = 0.0;
   //
   // sum over the noise covariates for this integrand
   size_t n_cov = pack_object_.group_meas_noise_n_cov(integrand_id);
   for(size_t j = 0; j < n_cov; ++j)
   {  info  = pack_object_.group_meas_noise_info(integrand_id, j);
      size_t group_id  = info.group_id;
      size_t smooth_id = info.smooth_id;
      double x_j       = x[ info.covariate_id ];
      if( group_id == size_t( subgroup_table_[subgroup_id].group_id ) )
      {  // interpolate from smoothing grid to cohort
         smooth_value.resize(info.n_var);
         for(size_t k = 0; k < info.n_var; ++k)
            smooth_value[k] = pack_vec[info.offset + k];
         const smooth_info& s_info = s_info_vec_[smooth_id];
         temp = grid2line(
            line_age_,
            line_time_,
            age_table_,
            time_table_,
            s_info,
            smooth_value
         );
         // add in this multiplier times covariate effect
         for(size_t k = 0; k < n_line; ++k)
            effect[k] += temp[k] * x_j;
      }
   }
   // -----------------------------------------------------------------------
   // line_weight_
   line_weight_.resize(n_line);
   line_weight_ = grid2line(
      line_age_,
      line_time_,
      age_table_,
      time_table_,
      w_info,
      weight_grid_
   );
   for(size_t i = 0; i < n_age; ++i)
   {  for(size_t j = 0; j < n_time; ++j)
      {  time_point point;
         size_t k         = i * n_time + j;
         size_t age_index = sub_lower + i;
         point.time       = line_time_[k];
         point.weight     = line_weight_[k];
         point.value      = effect[k];
         time_line_object.add_point(age_index, point);
      }
   }
   Float avg = time_line_object.age_time_avg();
   return avg;
}

# define DISMOD_AT_INSTANTIATE_AVG_NOISE_EFFECT_RECTANGLE(Float) \
   template                                                   \
   Float avg_noise_effect::rectangle(                           \
      double                           age_lower        ,    \
      double                           age_upper        ,    \
      double                           time_lower       ,    \
      double                           time_upper       ,    \
      size_t                           weight_id        ,    \
      size_t                           subgroup_id      ,    \
      size_t                           integrand_id     ,    \
      const CppAD::vector<double>&     x                ,    \
      const CppAD::vector<Float>&      pack_vec         ,    \
      time_line_vec<Float>&            time_line_object ,    \
      CppAD::vector<Float>&            line_adj              \
   );                                                         \
\
   Float avg_noise_effect::rectangle(                           \
      double                           age_lower        ,    \
      double                           age_upper        ,    \
      double                           time_lower       ,    \
      double                           time_upper       ,    \
      size_t                           weight_id        ,    \
      size_t                           subgroup_id      ,    \
      size_t                           integrand_id     ,    \
      const CppAD::vector<double>&     x                ,    \
      const CppAD::vector<Float>&      pack_vec         )    \
   {  return rectangle(                                      \
         age_lower,                                         \
         age_upper,                                         \
         time_lower,                                        \
         time_upper,                                        \
         weight_id,                                         \
         subgroup_id,                                       \
         integrand_id,                                      \
         x,                                                 \
         pack_vec,                                          \
         Float ## _time_line_object_,                       \
         Float ## _effect_                                  \
      );                                                     \
   }

// instantiations
DISMOD_AT_INSTANTIATE_AVG_NOISE_EFFECT_RECTANGLE( double )
DISMOD_AT_INSTANTIATE_AVG_NOISE_EFFECT_RECTANGLE( a1_double )

} // END_DISMOD_AT_NAMESPACE
