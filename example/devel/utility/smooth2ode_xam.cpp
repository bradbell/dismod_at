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
$verbatim%example/devel/utility/smooth2ode_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/include/smooth2ode.hpp>
# include <dismod_at/include/bilinear_interp.hpp>

bool smooth2ode_xam(void)
{
	bool   ok = true;
	size_t i, j, k;
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
	size_t mulstd_value   = 1;
	size_t mulstd_dage    = 1;
	size_t mulstd_dtime   = 1;

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
	dismod_at::smooth2ode s2ode(
		n_age_ode, n_time_ode, ode_step_size, age_table, time_table, 
		s_info
	);

	// variable values on smoothing grid
	CppAD::vector<Float> var_si(n_age_si * n_time_si);
	for(i = 0; i < n_age_si; i++)
	{	for(j = 0; j < n_time_si; j++)
			var_si[i * n_time_si + j] = i*i + j*j;
	}

	// ode grid points at which to interpolate
	CppAD::vector<size_t> ode_index;
	i = 0;
	j = 0;
	while( i < n_age_ode && j < n_time_ode )
	{	ode_index.push_back( i * n_time_ode + j );
		i++;
		j++;
	}

	// interpolate from smoothing to ode grid
	CppAD::vector<Float> var_ode = s2ode.interpolate(var_si, ode_index);

	// check result
	ok &= var_ode.size() == ode_index.size();
	//
	CppAD::vector<double> vdbl_si(n_age_si * n_time_si);
	CppAD::vector<double> age_si(n_age_si), time_si(n_time_si);
	for(i = 0; i < n_age_si; i++)
	{	age_si[i] = age_table[ age_id[i] ]; 	
		for(j = 0; j < n_time_si; j++)
			vdbl_si[i * n_time_si + j] = i*i + j*j;
	}
	for(j = 0; j < n_time_si; j++)
		time_si[j] = time_table[ time_id[j] ];
	//
	size_t i_si = 0, j_si = 0;
	for(k = 0; k < ode_index.size(); k++)
 	{
		Float  v_ode = var_ode[k];
		//
		size_t i_ode  = ode_index[k] / n_time_ode;
		size_t j_ode  = ode_index[k] % n_time_ode;
		double age    = age_table[0]  + ode_step_size * i_ode;
		double time   = time_table[0] + ode_step_size * j_ode;
		double  check = dismod_at::bilinear_interp(
			age, time, age_si, time_si, vdbl_si, i_si, j_si
		);
		//
		if( check == 0.0 )
			ok &= abs( v_ode ) <= 10. * eps ;
		else
			ok  &= abs( 1.0 - v_ode / Float(check) ) < 10. * eps;
		//
		// std::cout << std::endl;
		// std::cout << "check = " << check << std::endl;
		// std::cout << "v_ode = " << v_ode << std::endl;
	}
	return ok;
}
// END C++
