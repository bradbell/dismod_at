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
$begin data_mean_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ data_mean: Example and Test (Under Construction)$$
$index example, C++ data_mean$$
$index data_mean, C++ example$$

$code
$verbatim%example/devel/utility/data_mean_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/include/data_mean.hpp>

namespace {
	double check_avg(const dismod_at::data_struct& data_row)
	{	double a0 = data_row.age_lower;
		double a1 = data_row.age_upper;
		double t0 = data_row.time_lower;
		double t1 = data_row.time_upper;
		// integral of age * time from a0 to a1 and t0 to t1
		// divided by (a1 - a0) time (t1 - t0) is
		return (a0 + a1) * (t0 + t1) / 4.0;
	}
}

bool data_mean_xam(void)
{	bool   ok = true;
	size_t i, j, k;
	using CppAD::abs;	
	using CppAD::vector;	
	using std::cout;
	typedef CppAD::AD<double> Float;
	Float eps = CppAD::numeric_limits<Float>::epsilon();
	Float nan = CppAD::nan( Float(0) );
	//
	// age_table 
	size_t n_age_table   = 11;
	vector<double> age_table(n_age_table);
	for(i = 0; i < n_age_table; i++)
		age_table[i] = 10.0 * i;
	//
	// time_table
	size_t n_time_table  = 4;
	vector<double> time_table(n_time_table);
	for(j = 0; j < n_time_table; j++)
		time_table[j] = 10.0 * j + 1980.0;
	//
	// age_id and time_id
	size_t n_age_grid   = 3;
	size_t n_time_grid  = 2;
	vector<size_t> age_id(n_age_grid), time_id(n_time_grid);
	age_id[0]   = 0;  // age 0
	age_id[1]   = 5;  // age 50
	age_id[2]   = 10; // age 100
	time_id[0]  = 0;  // 1980
	time_id[1]  = 3;  // 2100
	//
	// w_info_vec
	// weight value should not matter when constant
	size_t n_grid = n_age_grid * n_time_grid;
	vector<double> weight(n_grid);
	for(k = 0; k < n_grid; k++)
		weight[k] = 0.5;
	dismod_at::weight_info w_info(age_id, time_id, weight);
	vector<dismod_at::weight_info> w_info_vec(1);
	w_info_vec[0] = w_info;
	//
	// s_info_vec
	vector<size_t> 
	value_like_id(n_grid), dage_like_id(n_grid), dtime_like_id(n_grid);
	size_t mulstd_value = 1, mulstd_dage = 1, mulstd_dtime = 1;
	dismod_at::smooth_info s_info(
		age_id, time_id, value_like_id, dage_like_id, dtime_like_id,
		mulstd_value, mulstd_dage, mulstd_dtime
	);
	vector<dismod_at::smooth_info> s_info_vec(1);
	s_info_vec[0] = s_info;
	//
	// integrand_table
	size_t n_integrand = dismod_at::number_integrand_enum;
	vector<dismod_at::integrand_enum> integrand_table(n_integrand);
	for(i = 0; i < n_integrand; i++)
			integrand_table[i] = dismod_at::integrand_enum(i);
	//
	// ode_step_size
	double ode_step_size = 1.0;
	//
	// n_age_ode
	double age_min       = age_table[0];
	double age_max       = age_table[n_age_table-1];
	size_t n_age_ode     =  1;
	while( age_min + (n_age_ode-1) * ode_step_size < age_max )
			n_age_ode++; 
	//
	// n_time_ode
	double time_min       = time_table[0];
	double time_max       = time_table[n_time_table-1];
	size_t n_time_ode     =  1;
	while( time_min + (n_time_ode-1) * ode_step_size < time_max )
			n_time_ode++; 
	//
	// node_table:    0
	//              1    2
	//                  3  4
	CppAD::vector<dismod_at::node_struct> node_table(5);
	node_table[0].parent = -1;
	node_table[1].parent =  0;
	node_table[2].parent =  0;
	node_table[3].parent =  2;
	node_table[4].parent =  2;
	//
	// parent_node_id
	size_t parent_node_id = 0;
	//
	// data_table
	vector<dismod_at::data_struct> data_table(2);
	//
	size_t data_id = 0;
	data_table[data_id].integrand_id = dismod_at::mtother_enum;
	data_table[data_id].node_id      = 0;
	data_table[data_id].weight_id    = 0;
	data_table[data_id].age_lower    = 40.0;
	data_table[data_id].age_upper    = 90.0;
	data_table[data_id].time_lower   = 1990.0;
	data_table[data_id].time_upper   = 2000.0;
	//
	data_id++;
	data_table[data_id].integrand_id = dismod_at::mtother_enum;
	data_table[data_id].node_id      = 3;
	data_table[data_id].weight_id    = 0;
	data_table[data_id].age_lower    = age_max;
	data_table[data_id].age_upper    = age_max;
	data_table[data_id].time_lower   = 1990.0;
	data_table[data_id].time_upper   = 2000.0;
	//
	dismod_at::data_mean avg_integrand(
		parent_node_id,
		n_age_ode,
		n_time_ode,
		ode_step_size,
		age_table,
		time_table,
		integrand_table,
		node_table,
		data_table,
		w_info_vec,
		s_info_vec
	);

	return ok;
}
// END C++
