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
	using  dismod_at::smooth2ode_struct;

	double eps = std::numeric_limits<double>::epsilon();

	// use smooth_grid test constructor
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
	// these values are not used
	vector<size_t> value_like_id(n_age * n_time);
	vector<size_t> dage_like_id(n_age * n_time);
	vector<size_t> dtime_like_id(n_age * n_time);
	// vector constructor
	dismod_at::smooth_grid sg(
		age_id, time_id, value_like_id, dage_like_id, dtime_like_id
	);

	// parameters for the ode grid
	double ode_step_size    = 2.0;
	size_t n_age_ode = 1;
	while( age_table[0] + (n_age_ode-1)*ode_step_size < age_table[n_age-1] )
			n_age_ode++;
	size_t n_time_ode = 1;
	while( time_table[0] + (n_time_ode-1)*ode_step_size < time_table[n_time-1] )
			n_time_ode++;

	// compute the interpolation coefficients on the Ode Grid
	vector<smooth2ode_struct> coefficient = dismod_at::smooth2ode(
		sg, age_table, time_table, n_age_ode, n_time_ode, ode_step_size
	);

	// check minimum age an time
	smooth2ode_struct sg_00 = coefficient[ 0 * n_time_ode + 0 ];
	ok  &= sg_00.i_sg == 0;
	ok  &= sg_00.j_sg == 0;
	ok  &= std::fabs(sg_00.c_00 - 1.0) < 10. * eps;
	ok  &= std::fabs(sg_00.c_10 - 0.0) < 10. * eps;
	ok  &= std::fabs(sg_00.c_01 - 0.0) < 10. * eps;
	ok  &= std::fabs(sg_00.c_11 - 0.0) < 10. * eps;

	// check another point
	size_t i_sg    = n_age - 2;
	size_t j_sg    = n_time - 2;
	i              = size_t(i_sg * 10.0 / ode_step_size + 1.0);
	j              = size_t(j_sg * 10.0 / ode_step_size + 1.0);
	double age     = i * ode_step_size + age_table[0];
	double time    = j * ode_step_size + time_table[0];
	double age_sg  = i_sg * 10.0 + age_table[0];
	double time_sg = j_sg * 10.0 + time_table[0];
	double check;
	//
	smooth2ode_struct sg_ij = coefficient[ i * n_time_ode + j ];
	ok   &= sg_ij.i_sg == i_sg;
	ok   &= sg_ij.j_sg == j_sg;
	check = (age_sg + 10.0 - age) * (time_sg + 10 - time) / 100.0;
	ok   &= std::fabs(sg_ij.c_00 - check) < 10. * eps;
	check = (age - age_sg) * (time_sg + 10 - time) / 100.0;
	ok   &= std::fabs(sg_ij.c_10 - check) < 10. * eps;
	check = (age_sg + 10.0 - age) * (time - time_sg) / 100.0;
	ok   &= std::fabs(sg_ij.c_01 - check) < 10. * eps;
	check = (age - age_sg) * (time - time_sg) / 100.0;
	ok   &= std::fabs(sg_ij.c_11 - check) < 10. * eps;

	return ok;
}
// END C++
