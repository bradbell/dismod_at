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
$verbatim%example/devel/utility/avg_integrand_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/include/avg_integrand.hpp>

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

bool avg_integrand_xam(void)
{	bool   ok = true;
	size_t i, j, k;
	using CppAD::abs;	
	using CppAD::vector;	
	using std::cout;
	typedef CppAD::AD<double> Float;
	Float eps = CppAD::numeric_limits<Float>::epsilon();
	Float nan = CppAD::nan( Float(0) );
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
	age_id[1]   = 5;  // age 50
	age_id[2]   = 10; // age 100
	time_id[0]  = 0;  // 1980
	time_id[1]  = 3;  // 2100
	//
	// value of weight should not matter (if constant)
	vector<double> weight(n_age_grid * n_time_grid );
	for(k = 0; k < n_age_grid * n_time_grid; k++)
		weight[k] = 0.5;
	//
	// w_info_vec
	dismod_at::weight_info w_info(age_id, time_id, weight);
	vector<dismod_at::weight_info> w_info_vec(1);
	w_info_vec[0] = w_info;
	//
	size_t n_integrand = dismod_at::number_integrand_enum;
	vector<dismod_at::integrand_enum> integrand_table(n_integrand);
	for(i = 0; i < n_integrand; i++)
			integrand_table[i] = dismod_at::integrand_enum(i);
	//
	double age_min       = age_table[0];
	double age_max       = age_table[n_age_table-1];
	double ode_step_size = 1.0;
	size_t n_age_ode     =  1;
	while( age_min + (n_age_ode-1) * ode_step_size < age_max )
			n_age_ode++; 
	//
	double time_min       = time_table[0];
	double time_max       = time_table[n_time_table-1];
	size_t n_time_ode     =  1;
	while( time_min + (n_time_ode-1) * ode_step_size < time_max )
			n_time_ode++; 
	//
	vector<dismod_at::data_struct> data_table(3);
	size_t data_id = 0;
	data_table[data_id].integrand_id = dismod_at::mtother_enum;
	data_table[data_id].weight_id    = 0;
	data_table[data_id].age_lower    = 40.0;
	data_table[data_id].age_upper    = 90.0;
	data_table[data_id].time_lower   = 1990.0;
	data_table[data_id].time_upper   = 2000.0;
	//
	data_id++;
	data_table[data_id].integrand_id = dismod_at::mtother_enum;
	data_table[data_id].weight_id    = 0;
	data_table[data_id].age_lower    = age_max;
	data_table[data_id].age_upper    = age_max;
	data_table[data_id].time_lower   = 1990.0;
	data_table[data_id].time_upper   = 2000.0;
	//
	data_id++;
	data_table[data_id].integrand_id = dismod_at::prevalence_enum;
	data_table[data_id].weight_id    = 0;
	data_table[data_id].age_lower    = 40.0;
	data_table[data_id].age_upper    = 90.0;
	data_table[data_id].time_lower   = time_min;
	data_table[data_id].time_upper   = time_min;
	
	dismod_at::avg_integrand avg(
		n_age_ode,
		n_time_ode,
		ode_step_size,
		age_table,
		time_table,
		integrand_table,
		data_table,
		w_info_vec
	);

	// normally S and C solve ODE but that is not necessary for these tests.

	// test measuring omega
	size_t n_ode = n_age_ode * n_time_ode;
	vector<Float> iota(n_ode), rho(n_ode), chi(n_ode), omega(n_ode);
	vector<Float> S(n_ode), C(n_ode);
	for(k = 0; k < n_ode; k++)
		iota[k] = rho[k] = chi[k] = omega[k] = S[k] = C[k] = nan;
	for(i = 0; i < n_age_ode; i++)
	{	double age = age_min + i * ode_step_size;	
		for(j = 0; j < n_time_ode; j++)
		{	double time     = time_min + j * ode_step_size;
			k               = i * n_time_ode + j;
			omega[k]        = age * time / (age_max * time_max);
			C[k]            = omega[k];
			S[k]            = 1.0 - C[k];
		}
	} 
	for(data_id = 0; data_id < data_table.size(); data_id++)
	{	Float data_mean = avg.compute(data_id, iota, rho, chi, omega, S, C);
		double check = check_avg(data_table[data_id]) / (age_max * time_max);
		ok          &= abs( 1.0 - data_mean / check ) <= 1e-2;
		// cout << std::endl;
		// cout << "relerr    = " << 1.0 - data_mean / check  << std::endl;
	}


	return ok;
}
// END C++
