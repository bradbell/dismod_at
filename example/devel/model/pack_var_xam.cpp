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
$begin pack_var_xam.cpp$$
$spell
	var
$$

$section C++ pack_var: Example and Test$$
$index example, C++ pack_var$$
$index pack_var, C++ example$$

$code
$verbatim%example/devel/model/pack_var_xam.cpp
%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/dismod_at.hpp>

bool pack_var_xam(void)
{	bool ok = true;
	using CppAD::vector;

	size_t n_integrand    = 2;
	//
	size_t n_smooth = 2;
	vector<dismod_at::smooth_struct> smooth_table(n_smooth);
	smooth_table[0].n_age  = 2;
	smooth_table[0].n_time = 1;
	smooth_table[1].n_age  = 2;
	smooth_table[1].n_time = 3;
	//
	size_t n_mulcov = 2;
	vector<dismod_at::mulcov_struct> mulcov_table(n_mulcov);
	mulcov_table[0].rate_id = -1;
	mulcov_table[0].integrand_id = 0;
	mulcov_table[0].covariate_id = 0;
	mulcov_table[0].smooth_id    = 0;
	mulcov_table[1].rate_id = -1;
	mulcov_table[1].integrand_id = 1;
	mulcov_table[1].covariate_id = 1;
	mulcov_table[1].smooth_id    = 1;
	//
	// constructor
	dismod_at::pack_var var(
		n_integrand, smooth_table, mulcov_table
	);
	//
	// packed vector
	size_t size = var.size();
	CppAD::vector<double> vec(size);

	// some temporary variables
	dismod_at::pack_var::mulcov_info info;

	// ---------------------------------------------------------------------
	// set mulstd
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	size_t offset    =	var.mulstd(smooth_id);
		vec[offset + 0 ] = smooth_id + 0; // value multiplier
		vec[offset + 1 ] = smooth_id + 1; // dage  multiplier
		vec[offset + 2 ] = smooth_id + 2; // dtime multiplier
	}
	// set meas_mean_mulcov
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = var.meas_mean_mulcov_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info          = var.meas_mean_mulcov_info(integrand_id, j);
			size_t offset = info.offset;
			size_t n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				vec[offset + k] = integrand_id + 3 + k;
		}
	}
	// ---------------------------------------------------------------------
	// check mulstd
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	size_t offset =	var.mulstd(smooth_id);
		ok &= vec[ offset + 0 ] == smooth_id + 0;
		ok &= vec[ offset + 1 ] == smooth_id + 1;
		ok &= vec[ offset + 2 ] == smooth_id + 2;
	}
	// check meas_mean_mulcov
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = var.meas_mean_mulcov_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info          = var.meas_mean_mulcov_info(integrand_id, j);
			size_t offset = info.offset;
			size_t n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				ok &= vec[offset + k] == integrand_id + 3 + k;
		}
	}

	return ok;
}
// END C++
