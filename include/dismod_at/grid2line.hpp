// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GRID2LINE_HPP
# define DISMOD_AT_GRID2LINE_HPP

# include <cppad/cppad.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

template <class Grid_info, class Float>
CppAD::vector<Float> grid2line(
   const CppAD::vector<double>& line_age     ,
   const CppAD::vector<double>& line_time    ,
   const CppAD::vector<double>& age_table    ,
   const CppAD::vector<double>& time_table   ,
   const Grid_info&             g_info       ,
   const CppAD::vector<Float>&  grid_value
);


} // END_DISMOD_AT_NAMESPACE

# endif
