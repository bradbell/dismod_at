// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_SMOOTH_INFO_HPP
# define DISMOD_AT_SMOOTH_INFO_HPP

# include <sqlite3.h>
# include <cppad/cppad.hpp>
# include "get_smooth_table.hpp"
# include "get_smooth_grid.hpp"
# include "get_prior_table.hpp"

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class smooth_info {
private:
   // grid of age values for this smoothing
   CppAD::vector<size_t> age_id_;
   // grid of time values for this smoothing
   CppAD::vector<size_t> time_id_;
   // prior_id for function values
   CppAD::vector<size_t> value_prior_id_;
   // prior_id for function difference in age direction
   CppAD::vector<size_t> dage_prior_id_;
   // prior_id for function difference in time direction
   CppAD::vector<size_t> dtime_prior_id_;
   // constraint value
   CppAD::vector<double> const_value_;
   // prior_id for multiplier of value prior standard deviations
   size_t mulstd_value_;
   // prior_id for multiplier of dage prior standard deviations
   size_t mulstd_dage_;
   // prior_id for multiplier of dtime prior standard deviations
   size_t mulstd_dtime_;
   // are all the value priors equivalant to a const_value
   bool all_const_value_;
public:
   // assignment operator
   void operator=(const smooth_info& s_info);
   // default constructor
   smooth_info(void);
   // normal constructor
   smooth_info(
      size_t                                   smooth_id         ,
      const CppAD::vector<double>&             age_table         ,
      const CppAD::vector<double>&             time_table        ,
      const CppAD::vector<prior_struct>&       prior_table       ,
      const CppAD::vector<smooth_struct>&      smooth_table      ,
      const CppAD::vector<smooth_grid_struct>& smooth_grid_table
   );
   // testing constructor
   smooth_info(
      const CppAD::vector<double>&  age_table      ,
      const CppAD::vector<double>&  time_table     ,
      const CppAD::vector<size_t>&  age_id         ,
      const CppAD::vector<size_t>&  time_id        ,
      const CppAD::vector<size_t>&  value_prior_id ,
      const CppAD::vector<size_t>&  dage_prior_id  ,
      const CppAD::vector<size_t>&  dtime_prior_id ,
      const CppAD::vector<double>&  const_value    ,
      size_t                        mulstd_value   ,
      size_t                        mulstd_dage    ,
      size_t                        mulstd_dtime   ,
      bool                          all_const_value
   );
   //
   size_t age_size(void)  const;
   size_t time_size(void) const;
   //
   size_t age_id(size_t i)  const;
   size_t time_id(size_t j) const;
   //
   size_t value_prior_id(size_t i, size_t j) const;
   size_t dage_prior_id(size_t i, size_t j)  const;
   size_t dtime_prior_id(size_t i, size_t j) const;
   double const_value(size_t i, size_t j) const;
   bool   all_const_value(void) const;
   //
   size_t mulstd_value(void) const;
   size_t mulstd_dage(void)  const;
   size_t mulstd_dtime(void) const;
};

} // END_DISMOD_AT_NAMESPACE

# endif
