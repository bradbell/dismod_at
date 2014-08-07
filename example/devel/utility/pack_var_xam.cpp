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
$verbatim%example/devel/utility/pack_var_xam.cpp
%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/include/pack_var.hpp>

bool pack_var_xam(void)
{	bool ok = true;
	using CppAD::vector;

	size_t n_integrand     = 4;
	size_t n_child         = 2;
	size_t pini_smooth_id  = 2;
	//
	size_t n_smooth = 4;
	vector<dismod_at::smooth_struct> smooth_table(n_smooth);
	smooth_table[0].n_age  = 2;
	smooth_table[0].n_time = 3;
	smooth_table[1].n_age  = 2;
	smooth_table[1].n_time = 3;
	smooth_table[2].n_age  = 1;
	smooth_table[2].n_time = 1;
	smooth_table[3].n_age  = 3;
	smooth_table[3].n_time = 3;
	//
	size_t n_mulcov = 4;
	vector<dismod_at::mulcov_struct> mulcov_table(n_mulcov);
	mulcov_table[0].mulcov_type  = dismod_at::meas_mean_enum;
	mulcov_table[0].rate_id      = -1;
	mulcov_table[0].integrand_id = 0;
	mulcov_table[0].covariate_id = 0;
	mulcov_table[0].smooth_id    = 0;
	mulcov_table[1].mulcov_type  = dismod_at::meas_mean_enum;
	mulcov_table[1].rate_id      = -1;
	mulcov_table[1].integrand_id = 1;
	mulcov_table[1].covariate_id = 1;
	mulcov_table[1].smooth_id    = 1;
	mulcov_table[2].mulcov_type  = dismod_at::meas_std_enum;
	mulcov_table[2].rate_id      = -1;
	mulcov_table[2].integrand_id = 2;
	mulcov_table[2].covariate_id = 2;
	mulcov_table[2].smooth_id    = 2;
	mulcov_table[3].mulcov_type  = dismod_at::rate_mean_enum;
	mulcov_table[3].rate_id      = 3;
	mulcov_table[3].integrand_id = 3;
	mulcov_table[3].covariate_id = 3;
	mulcov_table[3].smooth_id    = 3;
	//
	vector<dismod_at::rate_struct> rate_table(dismod_at::number_rate_enum);
	rate_table[0].parent_smooth_id = 0;
	rate_table[0].child_smooth_id  = 1;
	rate_table[0].parent_smooth_id = 0;
	rate_table[0].child_smooth_id  = 1;
	rate_table[0].parent_smooth_id = 0;
	rate_table[0].child_smooth_id  = 1;
	rate_table[0].parent_smooth_id = 0;
	rate_table[0].child_smooth_id  = 1;
	//
	// constructor
	dismod_at::pack_var var_info(
		n_integrand, n_child, pini_smooth_id, 
		smooth_table, mulcov_table, rate_table
	);
	//
	// packed vector
	size_t size = var_info.size();
	CppAD::vector<double> var_vec(size);

	// some temporary variables
	dismod_at::pack_var::subvec_info   info;
	size_t n_var, offset;

	// ---------------------------------------------------------------------
	// set pini
	info   = var_info.pini_info();
	n_var  = info.n_var;
	offset = info.offset;
	for(size_t j = 0; j < n_var; j++)
		var_vec[offset + j] = 1.0 / double(j + 2.0); 

	// set mulstd
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	size_t offset    =	var_info.mulstd_offset(smooth_id);
		var_vec[offset + 0 ] = smooth_id + 0; // value multiplier
		var_vec[offset + 1 ] = smooth_id + 1; // dage  multiplier
		var_vec[offset + 2 ] = smooth_id + 2; // dtime multiplier
	}
	// set rates 
	size_t n_rate = dismod_at::number_rate_enum;
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	n_var = var_info.rate_n_var(rate_id);
		for(size_t j = 0; j < std::max(n_child, size_t(1));  j++)
		{	offset = var_info.rate_offset(rate_id, j);
			for(size_t k = 0; k < n_var; k++)
				var_vec[offset + k] = rate_id + 3 + k;
		}
	}
	// set meas_mean_mulcov
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = var_info.meas_mean_mulcov_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = var_info.meas_mean_mulcov_info(integrand_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				var_vec[offset + k] = integrand_id + 4 + k;
		}
	}
	// set meas_std_mulcov
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = var_info.meas_std_mulcov_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = var_info.meas_std_mulcov_info(integrand_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				var_vec[offset + k] = integrand_id + 5 + k;
		}
	}
	// set rate_mean_mulcov
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	size_t n_cov = var_info.rate_mean_mulcov_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = var_info.rate_mean_mulcov_info(rate_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				var_vec[offset + k] = rate_id + 6 + k;
		}
	}
	// ---------------------------------------------------------------------
	// check pini
	info   = var_info.pini_info();
	ok    &= info.smooth_id == pini_smooth_id;
	n_var  = info.n_var;
	offset = info.offset;
	for(size_t j = 0; j < n_var; j++)
		ok &= var_vec[offset + j] == 1.0 / double(j + 2.0); 
	// check mulstd
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	offset =	var_info.mulstd_offset(smooth_id);
		ok &= var_vec[ offset + 0 ] == smooth_id + 0;
		ok &= var_vec[ offset + 1 ] == smooth_id + 1;
		ok &= var_vec[ offset + 2 ] == smooth_id + 2;
	}
	// check rates 
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	n_var = var_info.rate_n_var(rate_id);
		for(size_t j = 0; j < std::max(n_child, size_t(1));  j++)
		{	offset = var_info.rate_offset(rate_id, j);
			for(size_t k = 0; k < n_var; k++)
				ok &= var_vec[offset + k] == rate_id + 3 + k;
		}
	}
	// check meas_mean_mulcov
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = var_info.meas_mean_mulcov_n_cov(integrand_id);
		size_t check = 0;
		if( integrand_id < 2 )
			check = 1;
		ok &= n_cov == check;
		for(size_t j = 0; j < n_cov; j++)
		{	info   = var_info.meas_mean_mulcov_info(integrand_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				ok &= var_vec[offset + k] == integrand_id + 4 + k;
			size_t smooth_id = info.smooth_id;
			size_t n_age     = smooth_table[smooth_id].n_age;
			size_t n_time    = smooth_table[smooth_id].n_time;
			ok &= n_var == n_age * n_time;
		}
	}
	// check meas_std_mulcov
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = var_info.meas_std_mulcov_n_cov(integrand_id);
		size_t check = 0;
		if( integrand_id == 2 )
			check = 1;
		ok &= n_cov == check;
		for(size_t j = 0; j < n_cov; j++)
		{	info   = var_info.meas_std_mulcov_info(integrand_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				ok &= var_vec[offset + k] == integrand_id + 5 + k;
			size_t smooth_id = info.smooth_id;
			size_t n_age     = smooth_table[smooth_id].n_age;
			size_t n_time    = smooth_table[smooth_id].n_time;
			ok &= n_var == n_age * n_time;
		}
	}
	// check rate_mean_mulcov
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	size_t n_cov = var_info.rate_mean_mulcov_n_cov(rate_id);
		size_t check = 0;
		if( rate_id == 3 )
			check = 1;
		ok &= n_cov == check;
		for(size_t j = 0; j < n_cov; j++)
		{	info   = var_info.rate_mean_mulcov_info(rate_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				ok &= var_vec[offset + k] == rate_id + 6 + k;
			size_t smooth_id = info.smooth_id;
			size_t n_age     = smooth_table[smooth_id].n_age;
			size_t n_time    = smooth_table[smooth_id].n_time;
			ok &= n_var == n_age * n_time;
		}
	}

	return ok;
}
// END C++
