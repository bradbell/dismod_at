// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_AVG_INTEGRAND_HPP
# define DISMOD_AT_AVG_INTEGRAND_HPP
/*
{xrst_begin devel_avg_integrand dev}

Computing Average Integrands
############################

Contents
********
{xrst_toc_table
   devel/model/avg_integrand.cpp
}

{xrst_end devel_avg_integrand}
*/

# include <cppad/utility/vector.hpp>
# include "get_integrand_table.hpp"
# include "get_subgroup_table.hpp"
# include "pack_info.hpp"
# include "a1_double.hpp"
# include "adj_integrand.hpp"
# include "time_line_vec.hpp"
# include "weight_info.hpp"
# include "cov2weight_map.hpp"


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class avg_integrand {
private:
   // constants
   const double                              ode_step_size_;
   const CppAD::vector<double>&              age_table_;
   const CppAD::vector<double>&              time_table_;
   const CppAD::vector<subgroup_struct>&     subgroup_table_;
   const CppAD::vector<integrand_struct>&    integrand_table_;
   const CppAD::vector<weight_info>&         w_info_vec_;

   // temporaries used to avoid memory re-allocation (need constructor)
   time_line_vec<double>                     double_time_line_object_;
   time_line_vec<a1_double>                  a1_double_time_line_object_;
   //
   adj_integrand                             adjint_obj_;

   // other temporaries used to avoid memory re-allocation
   CppAD::vector<double>                     line_age_;
   CppAD::vector<double>                     line_time_;
   CppAD::vector<double>                     line_weight_;
   CppAD::vector<double>                     weight_grid_;
   //
   CppAD::vector<double>                     double_line_adj_;
   CppAD::vector<a1_double>                  a1_double_line_adj_;

   // template version of rectangle
   template <class Float>
   Float rectangle(
      double                           age_lower        ,
      double                           age_upper        ,
      double                           time_lower       ,
      double                           time_upper       ,
      size_t                           weight_id        ,
      size_t                           integrand_id     ,
      size_t                           n_child          ,
      size_t                           child            ,
      size_t                           subgroup_id      ,
      const CppAD::vector<double>&     x                ,
      const CppAD::vector<Float>&      pack_vec         ,
      //
      time_line_vec<Float>&            time_line_object ,
      CppAD::vector<Float>&            line_adj
   );

   // add_cohort
   template <class Float>
   void add_cohort(
      double                       time_ini             ,
      double                       time_lower           ,
      double                       time_upper           ,
      size_t                       weight_id            ,
      size_t                       integrand_id         ,
      size_t                       n_child              ,
      size_t                       child                ,
      size_t                       subgroup_id          ,
      const CppAD::vector<double>& x                    ,
      const CppAD::vector<Float>&  pack_vec             ,
      time_line_vec<Float>&        time_line_object     ,
      CppAD::vector<Float>&        line_adj
   );

public:
   // avg_integrand
   avg_integrand(
      const cov2weight_map&                     cov2weight_obj   ,
      double                                    ode_step_size    ,
      const std::string&                        rate_case        ,
      const CppAD::vector<double>&              age_avg_grid     ,
      const CppAD::vector<double>&              age_table        ,
      const CppAD::vector<double>&              time_table       ,
      const CppAD::vector<subgroup_struct>&     subgroup_table   ,
      const CppAD::vector<integrand_struct>&    integrand_table  ,
      const CppAD::vector<mulcov_struct>&       mulcov_table     ,
      const CppAD::vector<weight_info>&         w_info_vec       ,
      const CppAD::vector<smooth_info>&         s_info_vec       ,
      const pack_info&                          pack_object
   );
   // double version of rectangle
   double rectangle(
      double                           age_lower        ,
      double                           age_upper        ,
      double                           time_lower       ,
      double                           time_upper       ,
      size_t                           weight_id        ,
      size_t                           integrand_id     ,
      size_t                           n_child          ,
      size_t                           child            ,
      size_t                           subgroup_id      ,
      const CppAD::vector<double>&     x                ,
      const CppAD::vector<double>&     pack_vec
   );
   // a1_double version of rectangle
   a1_double rectangle(
      double                           age_lower        ,
      double                           age_upper        ,
      double                           time_lower       ,
      double                           time_upper       ,
      size_t                           weight_id        ,
      size_t                           integrand_id     ,
      size_t                           n_child          ,
      size_t                           child            ,
      size_t                           subgroup_id      ,
      const CppAD::vector<double>&     x                ,
      const CppAD::vector<a1_double>&  pack_vec
   );
};

} // END_DISMOD_AT_NAMESPACE

# endif
