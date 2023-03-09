// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_ADJ_INTEGRAND_HPP
# define DISMOD_AT_ADJ_INTEGRAND_HPP

# include <cppad/utility/vector.hpp>
# include "get_integrand_table.hpp"
# include "get_covariate_table.hpp"
# include "get_subgroup_table.hpp"
# include "pack_info.hpp"
# include "a1_double.hpp"
# include "weight_info.hpp"
# include "cov2weight_map.hpp"


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class adj_integrand {
private:
   // constants
   const std::string&                         rate_case_;
   const CppAD::vector<double>&               age_table_;
   const CppAD::vector<double>&               time_table_;
   const CppAD::vector<covariate_struct>&     covariate_table_;
   const CppAD::vector<subgroup_struct>&      subgroup_table_;
   const CppAD::vector<integrand_struct>&     integrand_table_;
   const CppAD::vector<smooth_info>&          s_info_vec_;
   const pack_info&                           pack_object_;
   const cov2weight_map&                      cov2weight_obj_;
   const CppAD::vector<weight_info>&          w_info_vec_;

   // Maps each mulcov_id to corresponding packing information.
   // Set by constructor and effectory const
   CppAD::vector<pack_info::subvec_info>      mulcov_pack_info_;

   // temporaries
   CppAD::vector<double>                      double_mulcov_;
   CppAD::vector<a1_double>                   a1_double_mulcov_;
   //
   CppAD::vector< CppAD::vector<double> >     double_rate_;
   CppAD::vector< CppAD::vector<a1_double> >  a1_double_rate_;

   // template version of line
   template <class Float>
   CppAD::vector<Float> line(
      size_t                                    node_id          ,
      const CppAD::vector<double>&              line_age         ,
      const CppAD::vector<double>&              line_time        ,
      size_t                                    integrand_id     ,
      size_t                                    n_child          ,
      size_t                                    child            ,
      size_t                                    subgroup_id      ,
      const CppAD::vector<double>&              x                ,
      const CppAD::vector<Float>&               pack_vec         ,
      CppAD::vector<Float>&                     mulcov           ,
      CppAD::vector< CppAD::vector<Float> >&    rate
   );
public:
   // adj_integrand
   adj_integrand(
      const cov2weight_map&                     cov2wight_obj    ,
      const CppAD::vector<weight_info>&         w_info_vec       ,
      const std::string&                        rate_case        ,
      const CppAD::vector<double>&              age_table        ,
      const CppAD::vector<double>&              time_table       ,
      const CppAD::vector<covariate_struct>&    covariate_table  ,
      const CppAD::vector<subgroup_struct>&     subgroup_table   ,
      const CppAD::vector<integrand_struct>&    integrand_table  ,
      const CppAD::vector<mulcov_struct>&       mulcov_table     ,
      const CppAD::vector<smooth_info>&         s_info_vec       ,
      const pack_info&                          pack_object
   );
   // double version of line
   CppAD::vector<double> line(
      size_t                                    node_id          ,
      const CppAD::vector<double>&              line_age         ,
      const CppAD::vector<double>&              line_time        ,
      size_t                                    integrand_id     ,
      size_t                                    n_child          ,
      size_t                                    child            ,
      size_t                                    subgroup_id      ,
      const CppAD::vector<double>&              x                ,
      const CppAD::vector<double>&              pack_vec
   );
   // a1_double version of line
   CppAD::vector<a1_double> line(
      size_t                                    node_id          ,
      const CppAD::vector<double>&              line_age         ,
      const CppAD::vector<double>&              line_time        ,
      size_t                                    integrand_id     ,
      size_t                                    n_child          ,
      size_t                                    child            ,
      size_t                                    subgroup_id      ,
      const CppAD::vector<double>&              x                ,
      const CppAD::vector<a1_double>&           pack_vec
   );
};

} // END_DISMOD_AT_NAMESPACE

# endif
