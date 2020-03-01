// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-20 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin pack_info_xam.cpp$$
$spell
	var
$$

$section C++ pack_info: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>

bool pack_info_xam(void)
{	bool ok = true;
	using CppAD::vector;

	size_t n_integrand     = 4;
	size_t n_child         = 2;
	//
	// subgroup_table
	size_t n_subgroup = 1;
	vector<dismod_at::subgroup_struct> subgroup_table(n_subgroup);
	subgroup_table[0].subgroup_name = "world";
	subgroup_table[0].group_id      = 0;
	subgroup_table[0].group_name    = "world";
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
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	smooth_table[smooth_id].mulstd_value_prior_id = 1;
		smooth_table[smooth_id].mulstd_dage_prior_id = 1;
		smooth_table[smooth_id].mulstd_dtime_prior_id = 1;
	}
	//
	size_t n_mulcov = 4;
	vector<dismod_at::mulcov_struct> mulcov_table(n_mulcov);
	mulcov_table[0].mulcov_type        = dismod_at::meas_value_enum;
	mulcov_table[0].rate_id            = DISMOD_AT_NULL_INT;
	mulcov_table[0].integrand_id       = 0;
	mulcov_table[0].covariate_id       = 0;
	mulcov_table[0].group_smooth_id    = 0;
	mulcov_table[0].subgroup_smooth_id = DISMOD_AT_NULL_INT;
	//
	mulcov_table[1].mulcov_type        = dismod_at::meas_value_enum;
	mulcov_table[1].rate_id            = DISMOD_AT_NULL_INT;
	mulcov_table[1].integrand_id       = 1;
	mulcov_table[1].covariate_id       = 1;
	mulcov_table[1].group_smooth_id    = 1;
	mulcov_table[1].subgroup_smooth_id = DISMOD_AT_NULL_INT;
	//
	mulcov_table[2].mulcov_type        = dismod_at::meas_noise_enum;
	mulcov_table[2].rate_id            = DISMOD_AT_NULL_INT;
	mulcov_table[2].integrand_id       = 2;
	mulcov_table[2].covariate_id       = 2;
	mulcov_table[2].group_smooth_id    = 2;
	mulcov_table[2].subgroup_smooth_id = DISMOD_AT_NULL_INT;
	//
	mulcov_table[3].mulcov_type        = dismod_at::rate_value_enum;
	mulcov_table[3].rate_id            = 3;
	mulcov_table[3].integrand_id       = DISMOD_AT_NULL_INT;
	mulcov_table[3].covariate_id       = 3;
	mulcov_table[3].group_smooth_id          = 3;
	mulcov_table[3].subgroup_smooth_id = DISMOD_AT_NULL_INT;
	//
	size_t n_random = 0;
	vector<dismod_at::rate_struct> rate_table(dismod_at::number_rate_enum);
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	size_t parent_smooth_id = 0;
		size_t child_smooth_id   = 1;
		if( rate_id == dismod_at::pini_enum )
		{	parent_smooth_id = 2;
			child_smooth_id  = 2;
		}
		size_t n_age  = smooth_table[child_smooth_id].n_age;
		size_t n_time = smooth_table[child_smooth_id].n_time;
		n_random += n_child * n_age * n_time;
		rate_table[rate_id].parent_smooth_id =  int( parent_smooth_id );
		rate_table[rate_id].child_smooth_id  =  int( child_smooth_id );
		rate_table[rate_id].child_nslist_id  =  int( DISMOD_AT_NULL_INT );
	}
	//
	// pack_object
	// values in child_id2node_id do not matter because child_nslist_id is null
	vector<size_t> child_id2node_id(n_child);
	vector<dismod_at::nslist_pair_struct> nslist_pair(0);
	dismod_at::pack_info pack_object(
		n_integrand,
		child_id2node_id,
		subgroup_table,
		smooth_table,
		mulcov_table,
		rate_table,
		nslist_pair
	);
	//
	// check integrand_size, child_size, smooth_size, and random_size
	ok &= n_child == pack_object.child_size();
	ok &= n_integrand == pack_object.integrand_size();
	ok &= n_smooth == pack_object.smooth_size();
	ok &= n_random == pack_object.random_size();
	//
	// packed vector
	size_t size = pack_object.size();
	CppAD::vector<double> pack_vec(size);

	// some temporary variables
	dismod_at::pack_info::subvec_info   info;
	size_t n_var, offset;

	// ---------------------------------------------------------------------
	size_t count = 0;

	// set mulstd
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	for(size_t k = 0; k < 3; k++)
		{	offset = pack_object.mulstd_offset(smooth_id, k);
			if( offset != DISMOD_AT_NULL_SIZE_T )
			{	pack_vec[offset] = double(smooth_id + k);
				count++;
			}
		}
	}
	// set rates
	size_t n_rate = dismod_at::number_rate_enum;
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	for(size_t j = 0; j <= n_child;  j++)
		{	info = pack_object.node_rate_value_info(rate_id, j);
			for(size_t k = 0; k < info.n_var; k++)
			{	pack_vec[info.offset + k] = double(rate_id + 3 + j + k);
				count++;
			}
		}
	}
	// set mulcov_meas_value
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object.group_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.group_meas_value_info(integrand_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
			{	pack_vec[offset + k] = double(integrand_id + 4 + k);
				count++;
			}
		}
	}
	// set mulcov_meas_noise
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object.group_meas_noise_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.group_meas_noise_info(integrand_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
			{	pack_vec[offset + k] = double(integrand_id + 5 + k);
				count++;
			}
		}
	}
	// set mulcov_rate_value
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	size_t n_cov = pack_object.group_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.group_rate_value_info(rate_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
			{	pack_vec[offset + k] = double(rate_id + 6 + k);
				count++;
			}
		}
	}
	// ---------------------------------------------------------------------
	// check size
	ok &= size == count;
	//
	// check mulstd
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	for(size_t k = 0; k < 3; k++)
		{	offset =	pack_object.mulstd_offset(smooth_id, k);
			if( offset != DISMOD_AT_NULL_SIZE_T )
				ok &= pack_vec[offset] == double(smooth_id + k);
		}
	}
	// check rates
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	for(size_t j = 0; j <= n_child;  j++)
		{	info = pack_object.node_rate_value_info(rate_id, j);
			for(size_t k = 0; k < info.n_var; k++)
				ok &= pack_vec[info.offset + k] == double(rate_id + 3 + j + k);
		}
	}
	// check mulcov_meas_value
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object.group_meas_value_n_cov(integrand_id);
		size_t check = 0;
		if( integrand_id < 2 )
			check = 1;
		ok &= n_cov == check;
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.group_meas_value_info(integrand_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				ok &= pack_vec[offset + k] == double(integrand_id + 4 + k);
			size_t smooth_id = info.smooth_id;
			size_t n_age     = smooth_table[smooth_id].n_age;
			size_t n_time    = smooth_table[smooth_id].n_time;
			ok &= n_var == n_age * n_time;
		}
	}
	// check mulcov_meas_noise
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object.group_meas_noise_n_cov(integrand_id);
		size_t check = 0;
		if( integrand_id == 2 )
			check = 1;
		ok &= n_cov == check;
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.group_meas_noise_info(integrand_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				ok &= pack_vec[offset + k] == double(integrand_id + 5 + k);
			size_t smooth_id = info.smooth_id;
			size_t n_age     = smooth_table[smooth_id].n_age;
			size_t n_time    = smooth_table[smooth_id].n_time;
			ok &= n_var == n_age * n_time;
		}
	}
	// check mulcov_rate_value
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	size_t n_cov = pack_object.group_rate_value_n_cov(rate_id);
		size_t check = 0;
		if( rate_id == 3 )
			check = 1;
		ok &= n_cov == check;
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.group_rate_value_info(rate_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				ok &= pack_vec[offset + k] == double(rate_id + 6 + k);
			size_t smooth_id = info.smooth_id;
			size_t n_age     = smooth_table[smooth_id].n_age;
			size_t n_time    = smooth_table[smooth_id].n_time;
			ok &= n_var == n_age * n_time;
		}
	}

	// check copy constructor
	dismod_at::pack_info pack_copy(pack_object);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	size_t n_cov = pack_copy.group_rate_value_n_cov(rate_id);
		size_t check = 0;
		if( rate_id == 3 )
			check = 1;
		ok &= n_cov == check;
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_copy.group_rate_value_info(rate_id, j);
			offset = info.offset;
			n_var  = info.n_var;
			for(size_t k = 0; k < n_var; k++)
				ok &= pack_vec[offset + k] == double(rate_id + 6 + k);
			size_t smooth_id = info.smooth_id;
			size_t n_age     = smooth_table[smooth_id].n_age;
			size_t n_time    = smooth_table[smooth_id].n_time;
			ok &= n_var == n_age * n_time;
		}
	}

	return ok;
}
// END C++
