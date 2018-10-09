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
	typedef dismod_at::time_line_vec<double>::time_point time_point;
	//
	// age_grid
	size_t n_age_grid = 4;
	CppAD::vector<double> age_grid(n_age_grid);
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
	double time_lower = 1990.0;
	double time_upper = 2010.0;
	vec.specialize(age_lower, age_upper, time_lower, time_upper);
	ok &= vec_age.size() == 1;
	ok &= vec_age[0] == age_lower;
	// --------------------------------------------------------------------
	// case where age_grid[1] < age_lower < age_upper < age_grid[2]
	age_lower = age_grid[1] + (age_grid[2] - age_grid[1]) / 3.0;
	age_upper = age_lower   + (age_grid[2] - age_grid[1]) / 3.0;
	vec.specialize(age_lower, age_upper, time_lower, time_upper);
	ok &= vec_age.size() == 2;
	ok &= vec_age[0] == age_lower;
	ok &= vec_age[1] == age_upper;
	// --------------------------------------------------------------------
	// add one time point to first time line
	time_point point_00;
	point_00.time  = time_lower;
	point_00.value = 3.0;
	vec.add_point(0, point_00);
	//
	// add two time points to the second time line
	time_point point_10, point_11;
	point_10.time  = time_lower;
	point_10.value = 4.0;
	point_11.time  = time_upper;
	point_11.value = 5.0;
	vec.add_point(1, point_11); // add second point first
	vec.add_point(1, point_10); // add first point second
	//
	// check first time line
	CppAD::vector<time_point> time_line_0 = vec.time_line(0);
	ok &= time_line_0.size() == 1;
	ok &= time_line_0[0].time  == point_00.time;
	ok &= time_line_0[0].value == point_00.value;
	//
	// check second time line
	CppAD::vector<time_point> time_line_1 = vec.time_line(1);
	ok &= time_line_1.size() == 2;
	ok &= time_line_1[0].time  == point_10.time;
	ok &= time_line_1[0].value == point_10.value;
	ok &= time_line_1[1].time  == point_11.time;
	ok &= time_line_1[1].value == point_11.value;
	// --------------------------------------------------------------------
	// add another point too the first line
	time_point point_01;
	point_01.time  = time_upper;
	point_01.value = 6.0;
	vec.add_point(0, point_01);
	//
	// check age_time_avg
	double eps99 = 99.0 * std::numeric_limits<double>::epsilon();
	double avg   = vec.age_time_avg();
	double sum   = point_00.value+point_01.value+point_10.value+point_11.value;
	double check = sum / 4.0;
	ok &= std::fabs( 1.0 - avg / check ) <= eps99;
	//
	return ok;
}
// END C++
