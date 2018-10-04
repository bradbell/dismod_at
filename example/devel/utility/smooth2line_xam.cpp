// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin smooth2line_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ smooth2line: Example and Test$$
$index example, C++ smooth2line$$
$index smooth2line, C++ example$$

$srcfile%example/devel/utility/smooth2ode_xam.cpp%
	0%// BEGIN C++%// END C++%1
%$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/smooth2line.hpp>
# include <dismod_at/bilinear_interp.hpp>
# include <dismod_at/a1_double.hpp>

bool smooth2line_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;
	typedef CppAD::AD<double> Float;
	//
	double eps99 = 99.0 * std::numeric_limits<double>::epsilon();

	// use smooth_info test constructor
	size_t n_age  = 4;
	size_t n_time = 3;
	vector<size_t> age_id(n_age),    time_id(n_time);
	vector<double> age_table(n_age), time_table(n_time);
	for(size_t i = 0; i < n_age; i++)
	{	// smoothing ages are entire age_table
		age_id[i]    = i;
		age_table[i] = double(i) / double(n_age);
	}
	for(size_t j = 0; j < n_time; j++)
	{	// smoothing times are entire time_table
		time_id[j]    = j;
		time_table[j] = 2.0 * double(j) / double(n_time);
	}
	// these values are not used
	vector<size_t> value_prior_id(n_age * n_time);
	vector<size_t> dage_prior_id(n_age * n_time);
	vector<size_t> dtime_prior_id(n_age * n_time);
	vector<double> const_value;
	size_t mulstd_value   = 1;
	size_t mulstd_dage    = 1;
	size_t mulstd_dtime   = 1;

	// testing constructor
	dismod_at::smooth_info s_info(
		age_table,
		time_table,
		age_id,
		time_id,
		value_prior_id,
		dage_prior_id,
		dtime_prior_id,
		const_value,
		mulstd_value,
		mulstd_dage,
		mulstd_dtime
	);

	// variable values on smoothing grid
	CppAD::vector<Float>  smooth_value(n_age * n_time);
	CppAD::vector<double> smooth_check(n_age * n_time);
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; j++)
		{	double age  = age_table[i];
			double time = time_table[j];
			smooth_check[i * n_time + j] = age * age + time * time;
			smooth_value[i * n_time + j] = smooth_check[i * n_time + j];
		}
	}

	// line on which to interpolate the value
	size_t n_line = 6;
	CppAD::vector<double> line_age(n_line), line_time(n_line);
	CppAD::vector<Float> line_value(n_line);
	//
	// minimum and maximum age in both tables and grid
	double age_min   = age_table[0];
	double age_max   = age_table[ n_age - 1 ];
	double age_diff  = age_max - age_min;
	//
	// minimum and maximum time in both tables and grid
	double time_min  = time_table[0];
	double time_max  = time_table[ n_time - 1 ];
	double time_diff = time_max - time_min;

	// case we are testing
	for(size_t k = 0; k < n_line; k++)
	{	line_age[k]  = age_min  + age_diff  * double(k) / double(n_line - 1);
		line_time[k] = time_min + time_diff * double(k) / double(n_line - 1);
		//
		// last age is above the maximum age
		line_age[k]  += age_diff / double(n_line - 1);
		//
		// first time is below the minimum time
		line_time[k] -=  time_diff / double(n_line - 1);
	}

	// smooth2line calculation
	line_value = smooth2line(
		line_age, line_time, age_table, time_table, s_info, smooth_value
	);


	// check the result
	for(size_t k = 0; k < n_line; k++)
	{	double age    = line_age[k];
		double time   = line_time[k];
		Float  result = line_value[k];
		//
		size_t i = 0;
		while( i < n_age - 1 && age_table[i+1] < age )
			++i;
		//
		size_t j = 0;
		while( j < n_time - 1 && time_table[j+1] < time )
			++j;
		//
		double  check = dismod_at::bilinear_interp(
			age, time, age_table, time_table, smooth_check, i, j
		);
		//
		if( check == 0.0 )
			ok &= fabs( result ) < eps99;
		else
			ok  &= fabs( 1.0 - result / Float(check) ) < eps99;
		//
		// std::cout << ", check = " << check;
		// std::cout << ", result = " << result << std::endl;
	}
	return ok;
}
// END C++
