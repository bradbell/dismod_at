/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin time_line_vec_xam.cpp$$
$spell
	interp
	xam
	vec
$$

$section C++ time_line_vec: Example and Test$$

$srcfile%example/devel/utility/time_line_vec_xam.cpp%
	0%// BEGIN C++%// END C++%1
%$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/time_line_vec.hpp>

bool time_line_vec_xam(void)
{	bool   ok = true;
	using CppAD::vector;
	//
	// age_grid
	size_t n_age_grid = 4;
	vector<double> age_grid(n_age_grid);
	age_grid[0] = 0.0;
	age_grid[1] = 0.1;
	age_grid[2] = 1.0;
	age_grid[3] = 10.0;
	//
	// vec
	dismod_at::time_line_vec<double> vec(age_grid);
	const CppAD::vector<double>& vec_age ( vec.vec_age() );
	// --------------------------------------------------------------------
	// case where age_lower = age_upper = age_grid[2]
	double age_lower = age_grid[2];
	double age_upper = age_grid[2];
	vec.specialize(age_lower, age_upper);
	ok &= vec_age.size() == 1;
	ok &= vec_age[0] == age_grid[2];
	// --------------------------------------------------------------------
	return ok;
}
// END C++
