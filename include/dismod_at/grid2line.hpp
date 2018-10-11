// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GRID2LINE_HPP
# define DISMOD_AT_GRID2LINE_HPP

# include <cppad/cppad.hpp>
# include "smooth_info.hpp"
# include "weight_info.hpp"

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
