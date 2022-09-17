// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_TIME_LINE_VEC_HPP
# define DISMOD_AT_TIME_LINE_VEC_HPP

# include <cppad/cppad.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

template <class Float>
class time_line_vec {
public:
   // structure corresponding to one point in a time line
   struct time_point {double time; double weight; Float value; };
private:
   // fixed age grid for numerical averages w.r.t. age and time
   const CppAD::vector<double> age_grid_;
   //
   // Set by specialize to include all the points in age_grid_ as well as
   // age_lower and  age_upper
   CppAD::vector<double> extend_grid_;
   //
   // Set by specialize: exend_grid_[ sub_lower_ ] is age_lower
   size_t sub_lower_;
   //
   // Set by specialize: exend_grid_[ sub_upper_ ] is age_upper
   size_t sub_upper_;
   //
   // Set by specialize
   double time_lower_;
   double time_upper_;
   //
   // vec_ has the same size as sub_grid_ and
   // vec_[j] contains the line for sub_grid_[j]
   CppAD::vector< CppAD::vector<time_point> >  vec_;
public:
   // near_equal
   static bool near_equal(double x, double y);
   //
   // constructor
   time_line_vec(const CppAD::vector<double>& age_grid);
   //
   // specialize
   void specialize(
      const double& age_lower  ,
      const double& age_upper  ,
      const double& time_lower ,
      const double& time_upper
   );
   // extend_grid
   const CppAD::vector<double>& extend_grid(void) const;
   //
   // sub_lower
   size_t sub_lower(void) const;
   //
   // sub_upper
   size_t sub_upper(void) const;
   //
   // add_point
   void add_point(
      const size_t&     age_index ,
      const time_point& point
   );
   // time_line
   const CppAD::vector<time_point>& time_line(
      const size_t& age_index
   ) const;
   //
   // max_time_diff
   double max_time_diff(size_t& age_index, size_t& time_index) const;
   //
   // age_time_avg
   Float age_time_avg(void) const;
};

} // END_DISMOD_AT_NAMESPACE

# endif
