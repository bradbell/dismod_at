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
$begin interp_weight_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ interp_weight: Example and Test$$
$index example, C++ interp_weight$$
$index interp_weight, C++ example$$

$code
$srcfile%example/devel/utility/interp_weight_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/interp_weight.hpp>
# include <dismod_at/bilinear_interp.hpp>

bool interp_weight_xam(void)
{
	bool   ok = true;
	using  std::string;
	using  CppAD::vector;

	double eps = std::numeric_limits<double>::epsilon();

	// use weight_grid test constructor
	size_t n_age  = 4;
	size_t n_time = 3;
	vector<size_t> age_id(n_age),    time_id(n_time);
	vector<double> age_table(n_age), time_table(n_time);
	for(size_t i = 0; i < n_age; i++)
	{	age_id[i]    = i;
		age_table[i] = 10.0 * double(i);
	}
	for(size_t j = 0; j < n_time; j++)
	{	time_id[j]    = j;
		time_table[j] = 1990.0 + 10 * double(j);
	}
	vector<double> weight(n_age * n_time);
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; j++)
			weight[i * n_time + j] = double(i * j + 1);
	}
	dismod_at::weight_info w_info(
		age_table, time_table, age_id, time_id, weight
	);

	// case where interpolating in both directions
	double age     = 10.0 * 2.5;
	double time    = 1990.0 + 10 * 1.5;
	size_t i_wi    = 0;
	size_t j_wi    = 0;
	double w_value = dismod_at::interp_weight(
		age, time, w_info, age_table, time_table, i_wi, j_wi
	);
	size_t i_check = 0;
	size_t j_check = 0;
	double w_check  = dismod_at::bilinear_interp(
		age, time, age_table, time_table, weight, i_check, j_check
	);
	ok &= i_wi == i_check;
	ok &= j_wi == j_check;
	ok &= std::fabs( 1.0 - w_value / w_check  ) < 10.0 * eps;

	// case where interpolate in time only
	age     = age_table[n_age-1] + 10;
	time    = 1990.0 + 10 * 1.5;
	w_value = dismod_at::interp_weight(
		age, time, w_info, age_table, time_table, i_wi, j_wi
	);
	w_check  = dismod_at::bilinear_interp(
		age, time, age_table, time_table, weight, i_check, j_check
	);
	ok &= i_wi == i_check;
	ok &= j_wi == j_check;
	ok &= std::fabs( 1.0 - w_value / w_check  ) < 10.0 * eps;

	// case where interpolate in age only
	age     = 10.0 * 2.5;
	time    = time_table[0] - 10.0;
	w_value = dismod_at::interp_weight(
		age, time, w_info, age_table, time_table, i_wi, j_wi
	);
	w_check  = dismod_at::bilinear_interp(
		age, time, age_table, time_table, weight, i_check, j_check
	);
	ok &= i_wi == i_check;
	ok &= j_wi == j_check;
	ok &= std::fabs( 1.0 - w_value / w_check  ) < 10.0 * eps;

	// case where no interpolation is done
	age     = age_table[0] - 10.0;
	time    = time_table[n_time-1] + 10.0;
	w_value = dismod_at::interp_weight(
		age, time, w_info, age_table, time_table, i_wi, j_wi
	);
	w_check  = dismod_at::bilinear_interp(
		age, time, age_table, time_table, weight, i_check, j_check
	);
	ok &= i_wi == i_check;
	ok &= j_wi == j_check;
	ok &= std::fabs( 1.0 - w_value / w_check  ) < 10.0 * eps;

	return ok;
}
// END C++
