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
$begin smooth2ode_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ smooth2ode: Example and Test$$
$index example, C++ smooth2ode$$
$index smooth2ode, C++ example$$

$code
$verbatim%example/devel/model/smooth2ode_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/dismod_at.hpp>

bool smooth2ode_xam(void)
{
	bool   ok = true;
	size_t i, j;
	using  std::string;
	using  CppAD::vector;	
	typedef CppAD::AD<double> Float;

	double eps = std::numeric_limits<double>::epsilon();

	// use smooth_info test constructor
	size_t n_age_si  = 4;
	size_t n_time_si = 3;
	vector<size_t> age_id(n_age_si),    time_id(n_time_si);
	vector<double> age_table(n_age_si), time_table(n_time_si);
	for(i = 0; i < n_age_si; i++)
	{	age_id[i]    = i;
		age_table[i] = 10.0 * i;
	}
	for(j = 0; j < n_time_si; j++)
	{	time_id[j]    = j;
		time_table[j] = 1990.0 + 10 * j;
	}
	// these values are not used
	vector<size_t> value_like_id(n_age_si * n_time_si);
	vector<size_t> dage_like_id(n_age_si * n_time_si);
	vector<size_t> dtime_like_id(n_age_si * n_time_si);
	size_t mulstd_value   = -1;
	size_t mulstd_dage    = -1;
	size_t mulstd_dtime   = -1;

	// testing constructor
	dismod_at::smooth_info s_info(
		age_id, 
		time_id, 
		value_like_id, 
		dage_like_id, 
		dtime_like_id,
		mulstd_value,
		mulstd_dage,
		mulstd_dtime
	);

	// parameters for the ode grid
	double ode_step_size    = 2.0;
	size_t n_age_ode = 1;
	double age_min = age_table[0];
	double age_max = age_table[ age_table.size() - 1 ];
	while(age_min + (n_age_ode-1)*ode_step_size < age_max )
		n_age_ode++;
	size_t n_time_ode = 1;
	double time_min = time_table[0];
	double time_max = time_table[ time_table.size() - 1 ];
	while(time_min + (n_time_ode-1)*ode_step_size < time_max )
		n_time_ode++;

	// construct the interpolation object
	dismod_at::smooth2ode si2ode(
		s_info, age_table, time_table, n_age_ode, n_time_ode, ode_step_size
	);

	// variable values on smoothing grid
	CppAD::vector<Float> var_si(n_age_si * n_time_si);
	for(i = 0; i < n_age_si; i++)
	{	for(j = 0; j < n_time_si; j++)
			var_si[i * n_time_si + j] = i*i + j*j;
	}

	// interpolate from smoothing to ode grid
	CppAD::vector<Float> var_ode = si2ode.interpolate(var_si);

	// check one point
	size_t i_ode = 6;
	size_t j_ode = 7;
	double age   = age_table[0] + ode_step_size * i_ode;
	double time  = time_table[0] + ode_step_size * j_ode;
	Float  v_ode = var_ode[ i_ode * n_time_ode + j_ode ];
	size_t i_si  = 0;
	while( age_table[ age_id[i_si+1] ] < age )
		i_si++;
	size_t j_si  = 0;
	while( time_table[ time_id[j_si+1] ] < time )
		j_si++;
	Float sum = 0.0;
	double a0  = age_table[ age_id[i_si] ];
	double a1  = age_table[ age_id[i_si+1] ];
	double t0  = time_table[ time_id[j_si] ];
	double t1  = time_table[ time_id[j_si+1] ];
	sum  += (a1-age)*(t1-time)*var_si[i_si * n_time_si + j_si];
	sum  += (age-a0)*(t1-time)*var_si[(i_si+1) * n_time_si + j_si];
	sum  += (a1-age)*(time-t0)*var_si[i_si * n_time_si + (j_si+1)];
	sum  += (age-a0)*(time-t0)*var_si[(i_si+1) * n_time_si + (j_si+1)];
	sum /= (a1 - a0) * (t1 - t0);
	//
	ok  &= abs( 1.0 - sum / v_ode ) < 10. * eps;
	//
	// std::cout << std::endl;
	// std::cout << "sum = " << sum << std::endl;
	// std::cout << "v_ode = " << v_ode << std::endl;

	return ok;
}
// END C++
