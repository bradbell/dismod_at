/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-20 University of Washington
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

$srcthisfile%
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
	// near_equal
	double x = 1.0;
	double y = x + 10.0  * std::numeric_limits<double>::epsilon();
	double z = x + 100.0 * std::numeric_limits<double>::epsilon();
	ok &= dismod_at::time_line_vec<double>::near_equal(x, y);
	ok &= ! dismod_at::time_line_vec<double>::near_equal(x, z);
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
	const CppAD::vector<double>& extend_grid ( vec.extend_grid() );
	// --------------------------------------------------------------------
	// case where age_lower = age_upper = age_grid[2]
	double age_lower = age_grid[2];
	double age_upper = age_grid[2];
	double time_lower = 1990.0;
	double time_upper = 2010.0;
	vec.specialize(age_lower, age_upper, time_lower, time_upper);
	size_t sub_lower = vec.sub_lower();
	size_t sub_upper = vec.sub_upper();
	ok &= sub_lower == 2;
	ok &= sub_upper == 2;
	ok &= extend_grid.size() == n_age_grid;
	for(size_t i = 0; i < n_age_grid; ++i)
		ok &= extend_grid[i] == age_grid[i];
	// --------------------------------------------------------------------
	// case where age_grid[1] < age_lower < age_upper < age_grid[2]
	age_lower = age_grid[1] + (age_grid[2] - age_grid[1]) / 3.0;
	age_upper = age_lower   + (age_grid[2] - age_grid[1]) / 3.0;
	vec.specialize(age_lower, age_upper, time_lower, time_upper);
	sub_lower = vec.sub_lower();
	sub_upper = vec.sub_upper();
	ok &= sub_lower == 2;
	ok &= sub_upper == 3;
	ok &= extend_grid.size() == n_age_grid + 2;
	for(size_t i = 0; i < extend_grid.size(); ++i)
	{	if( i < sub_lower )
			ok &= extend_grid[i] == age_grid[i];
		else if( i == sub_lower )
			ok &= extend_grid[i] == age_lower;
		else if( i == sub_upper )
			ok &= extend_grid[i] == age_upper;
		else
			ok &= extend_grid[i] == age_grid[i-2];
	}
	// --------------------------------------------------------------------
	// add one time point to first time line
	time_point point_00;
	point_00.time   = time_lower;
	point_00.weight = 1.0;
	point_00.value  = 3.0;
	vec.add_point(sub_lower, point_00);
	//
	// add two time points to the second time line
	time_point point_10, point_11;
	point_10.time   = time_lower;
	point_10.weight = 2.0;
	point_10.value  = 4.0;
	point_11.time   = time_upper;
	point_10.weight = 3.0;
	point_11.value  = 5.0;
	vec.add_point(sub_lower+1, point_11); // add second point first time
	vec.add_point(sub_lower+1, point_10); // add second point second time
	//
	// check first time line
	CppAD::vector<time_point> time_line_0 = vec.time_line(sub_lower);
	ok &= time_line_0.size()    == 1;
	ok &= time_line_0[0].time   == point_00.time;
	ok &= time_line_0[0].weight == point_00.weight;
	ok &= time_line_0[0].value  == point_00.value;
	//
	// check second time line
	CppAD::vector<time_point> time_line_1 = vec.time_line(sub_lower+1);
	ok &= time_line_1.size()    == 2;
	ok &= time_line_1[0].time   == point_10.time;
	ok &= time_line_1[0].weight == point_10.weight;
	ok &= time_line_1[0].value  == point_10.value;
	ok &= time_line_1[1].time   == point_11.time;
	ok &= time_line_1[1].weight == point_11.weight;
	ok &= time_line_1[1].value  == point_11.value;
	// --------------------------------------------------------------------
	// add another point to the first line
	time_point point_01;
	point_01.time   = time_upper;
	point_01.weight = 4.0;
	point_01.value  = 6.0;
	vec.add_point(sub_lower, point_01);
	//
	// check age_time_avg
	double eps99 = 99.0 * std::numeric_limits<double>::epsilon();
	double avg      = vec.age_time_avg();
	double sum_w    = point_00.weight + point_01.weight;
	sum_w          += point_10.weight + point_11.weight;
	double sum_wv   = point_00.weight * point_00.value;
	sum_wv         += point_01.weight * point_01.value;
	sum_wv         += point_10.weight * point_10.value;
	sum_wv         += point_11.weight * point_11.value;
	double check = sum_wv / sum_w;
	ok &= std::fabs( 1.0 - avg / check ) <= eps99;
	// ---------------------------------------------------------------------
	// max_time_diff
	size_t age_index;
	size_t time_index;
	double max_diff = vec.max_time_diff(age_index, time_index);
	check = time_upper - time_lower;
	ok   &= std::fabs( 1.0 - max_diff / check ) <= eps99;
	//
	return ok;
}
// END C++
