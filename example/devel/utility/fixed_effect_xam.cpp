// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin fixed_effect_xam.cpp$$
$spell
	var
$$

$section C++ fixed_effect: Example and Test$$

$code
$srcfile%example/devel/utility/fixed_effect_xam.cpp
%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/fixed_effect.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/open_connection.hpp>

bool fixed_effect_xam(void)
{	bool ok = true;
	using CppAD::vector;

	size_t n_integrand     = 1;
	size_t n_child         = 0;
	//
	// initialize
	size_t n_fixed_effect = 0;
	//
	size_t n_smooth = 2;
	vector<dismod_at::smooth_struct> smooth_table(n_smooth);
	smooth_table[0].n_age  = 1;
	smooth_table[0].n_time = 3;
	smooth_table[1].n_age  = 2;
	smooth_table[1].n_time = 3;
	//
	// mulstd
	n_fixed_effect += n_smooth * 3;
	//
	// parent rates
	vector<dismod_at::rate_struct> rate_table(dismod_at::number_rate_enum);
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	size_t parent_smooth_id = 1;
		if( rate_id == dismod_at::pini_enum )
			parent_smooth_id  = 0;
		rate_table[rate_id].parent_smooth_id = parent_smooth_id;
		size_t n_age    = smooth_table[parent_smooth_id].n_age;
		size_t n_time   = smooth_table[parent_smooth_id].n_time;
		n_fixed_effect += n_age * n_time;
	}
	//
	// covariate multipliers
	size_t n_mulcov = 3;
	vector<dismod_at::mulcov_struct> mulcov_table(n_mulcov);
	//
	// mulcov_meas_value
	mulcov_table[0].mulcov_type  = dismod_at::meas_value_enum;
	mulcov_table[0].rate_id      = -1;
	mulcov_table[0].integrand_id = 0;
	mulcov_table[0].covariate_id = 0;
	mulcov_table[0].smooth_id    = 1;
	n_fixed_effect += smooth_table[1].n_age * smooth_table[1].n_time;
	//
	// mulcov_meas_std
	mulcov_table[1].mulcov_type  = dismod_at::meas_std_enum;
	mulcov_table[1].rate_id      = -1;
	mulcov_table[1].integrand_id = 0;
	mulcov_table[1].covariate_id = 1;
	mulcov_table[1].smooth_id    = 0;
	n_fixed_effect += smooth_table[0].n_age * smooth_table[0].n_time;
	//
	// mulcov_rate_value
	mulcov_table[2].mulcov_type  = dismod_at::rate_value_enum;
	mulcov_table[2].rate_id      = 3;
	mulcov_table[2].integrand_id = 0;
	mulcov_table[2].covariate_id = 2;
	mulcov_table[2].smooth_id    = 1;
	n_fixed_effect += smooth_table[1].n_age * smooth_table[1].n_time;
	//
	// construct pack_object, pack_vec, and subvec_info
	bool new_file = true;
	std::string file_name = "example.db";
	sqlite3* db = dismod_at::open_connection(file_name, new_file);
	dismod_at::pack_info pack_object(
		n_integrand, n_child, smooth_table, mulcov_table, rate_table
	);
	//
	// check n_fixed_effect
	ok &= n_fixed_effect == dismod_at::number_fixed(pack_object);

	// pack_index
	CppAD::vector<size_t> pack_index = dismod_at::fixed2var_id(pack_object);

	// pack_vec
	CppAD::vector<double> pack_vec( pack_object.size() );

	// fixed_vec
	CppAD::vector<double> fixed_vec(n_fixed_effect);

	// set value of fixed effects in pack_vec
	for(size_t i = 0; i < n_fixed_effect; i++)
		fixed_vec[i] = double(i + 1);
	dismod_at::pack_fixed(pack_object, pack_vec, fixed_vec);

	// check pack_index values
	for(size_t i = 0; i < n_fixed_effect; i++)
		ok &= pack_vec[ pack_index[i] ] == double(i+1);

	// clear fixed_vec
	for(size_t i = 0; i < n_fixed_effect; i++)
		fixed_vec[i] = 0.0;

	// get the fixed effects in pack_info
	dismod_at::unpack_fixed(pack_object, pack_vec, fixed_vec);

	// check value of fixed effects
	for(size_t i = 0; i < n_fixed_effect; i++)
		ok &= fixed_vec[i] == double(i + 1);

	return ok;
}
// END C++
