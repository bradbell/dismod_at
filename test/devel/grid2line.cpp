// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
// BEGIN C++
# include <limits>
# include <dismod_at/grid2line.hpp>
# include <dismod_at/bilinear_interp.hpp>
# include <dismod_at/a1_double.hpp>

bool grid2line(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;
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
	// weight value on the grid
	vector<double> weight_value(n_age * n_time);
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; j++)
		{	double age  = age_table[i];
			double time = time_table[j];
			weight_value[i * n_time + j] = age * age + time * time;
		}
	}

	dismod_at::weight_info w_info(
		age_table, time_table, age_id, time_id, weight_value
	);

	// line on which to interpolate the value
	size_t n_line = 6;
	CppAD::vector<double> line_age(n_line), line_time(n_line);
	CppAD::vector<double> line_value(n_line);
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

	// grid2line calculation
	line_value = grid2line(
		line_age, line_time, age_table, time_table, w_info, weight_value
	);

	// check the result
	for(size_t k = 0; k < n_line; k++)
	{	double age    = line_age[k];
		double time   = line_time[k];
		double result = line_value[k];
		//
		size_t i = 0;
		while( i < n_age - 1 && age_table[i+1] < age )
			++i;
		//
		size_t j = 0;
		while( j < n_time - 1 && time_table[j+1] < time )
			++j;
		//
		double check = dismod_at::bilinear_interp(
			age, time, age_table, time_table, weight_value, i, j
		);
		//
		if( check == 0.0 )
			ok &= fabs( result ) < eps99;
		else
			ok  &= fabs( 1.0 - result / check ) < eps99;
		//
		// std::cout << ", check = " << check;
		// std::cout << ", result = " << result << std::endl;
	}
	return ok;
}
// END C++
