// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin avg_integrand_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ avg_integrand: Example and Test (Under Construction)$$
$index example, C++ avg_integrand$$
$index avg_integrand, C++ example$$

$code
$verbatim%example/devel/model/avg_integrand_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/dismod_at.hpp>

bool avg_integrand_xam(void)
{	bool   ok = true;
	size_t i, j, k;
	using  CppAD::abs;	
	using  CppAD::vector;	
	typedef CppAD::AD<double> Float;
	Float eps = CppAD::numeric_limits<Float>::epsilon();
	//
	size_t n_age_table   = 11;
	size_t n_time_table  = 4;
	//
	vector<double> age_table(n_age_table), time_table(n_time_table);
	for(i = 0; i < n_age_table; i++)
		age_table[i] = 10.0 * i;
	//
	for(j = 0; j < n_time_table; j++)
		time_table[j] = 10.0 * j + 1980.0;
	//
	size_t n_age_grid   = 3;
	size_t n_time_grid  = 2;
	vector<size_t> age_id(n_age_grid), time_id(n_time_grid);
	age_id[0]   = 0;  // age 0
	age_id[1]   = 6;  // age 50
	age_id[2]   = 11; // age 100
	time_id[0]  = 0;  // 1980
	time_id[1]  = 3;  // 2100
	//
	// value of weight should not matter (if constant)
	vector<double> weight(n_age_grid * n_time_grid );
	for(k = 0; k < n_age_grid * n_time_grid; k++)
		weight[k] = 0.5;
	//
	// wg_vec
	dismod_at::weight_grid wg(age_id, time_id, weight);
	vector<dismod_at::weight_grid> wg_vec(1);
	wg_vec[0] = wg;
	//
	vector<dismod_at::data_struct> data_table(1);
	data_table[0].integrand_id = 0;
	data_table[0].weight_id    = 0;
	data_table[0].age_lower    = 10.0;
	data_table[0].age_upper    = 90.0;
	data_table[0].time_lower   = 1990.0;
	data_table[0].time_lower   = 2000.0;
	
	
	return ok;
}
// END C++
