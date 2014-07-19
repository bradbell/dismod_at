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
$begin interp_weight_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ interp_weight: Example and Test$$
$index example, C++ interp_weight$$
$index interp_weight, C++ example$$

$code
$verbatim%example/devel/model/interp_weight_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/dismod_at.hpp>

bool interp_weight_xam(void)
{
	bool   ok = true;
	size_t i, j, k;
	using  std::string;
	using  CppAD::vector;	

	double eps = std::numeric_limits<double>::epsilon();

	// use weight_grid test constructor
	size_t n_age  = 4;
	size_t n_time = 3;
	vector<size_t> age_id(n_age),    time_id(n_time);
	vector<double> age_table(n_age), time_table(n_time);
	for(i = 0; i < n_age; i++)
	{	age_id[i]    = i;
		age_table[i] = 10.0 * i;
	}
	for(j = 0; j < n_time; j++)
	{	time_id[j]    = j;
		time_table[j] = 1990.0 + 10 * j;
	}
	vector<double> weight(n_age * n_time);
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
		{	k = i * n_time + j;
			weight[i * n_time + j] = i * j + 1;
		}
	}
	dismod_at::weight_grid wg(age_id, time_id, weight);

	// point at which to interpolate the function
	double age = 10.0 * 2.5;            // i = 2.5
	double time = 1990.0 + 10 * 1.5;    // j = 1.5
	size_t i_wg = 2;
	size_t j_wg = 1;

	// evaluate the interpolant
	double w_value = dismod_at::interp_weight(
		age, time, wg, age_table, time_table, i_wg, j_wg
	);
	double w_check  = 2.5 * 1.5 + 1.0;

	ok = fabs( 1.0 - w_value / w_check  ) < 10.0 * eps;

	return ok;
}
// END C++
