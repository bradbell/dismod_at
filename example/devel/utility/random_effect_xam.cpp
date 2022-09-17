// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin random_effect_xam.cpp$$
$spell
	var
$$

$section C++ random_effect: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/random_effect.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>

bool random_effect_xam(void)
{	bool ok = true;
	using CppAD::vector;

	size_t n_integrand     = 1;
	size_t n_child         = 2;
	//
	// initialize
	size_t n_random_effect = 0;
	//
	// subgroup_table
	size_t n_subgroup = 1;
	vector<dismod_at::subgroup_struct> subgroup_table(n_subgroup);
	subgroup_table[0].subgroup_name = "world";
	subgroup_table[0].group_id      = 0;
	subgroup_table[0].group_name    = "world";
	//
	size_t n_smooth = 2;
	vector<dismod_at::smooth_struct> smooth_table(n_smooth);
	smooth_table[0].n_age  = 1;
	smooth_table[0].n_time = 3;
	smooth_table[1].n_age  = 2;
	smooth_table[1].n_time = 3;
	//
	size_t n_mulcov = 0;
	vector<dismod_at::mulcov_struct> mulcov_table(n_mulcov);
	//
	vector<dismod_at::rate_struct> rate_table(dismod_at::number_rate_enum);
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	size_t parent_smooth_id = 0;
		size_t child_smooth_id  = 1;
		if( rate_id == dismod_at::pini_enum )
			child_smooth_id  = 0;
		rate_table[rate_id].parent_smooth_id =  int( parent_smooth_id );
		rate_table[rate_id].child_smooth_id  =  int( child_smooth_id );
		rate_table[rate_id].child_nslist_id  =  int( DISMOD_AT_NULL_INT );
		size_t n_age  = smooth_table[child_smooth_id].n_age;
		size_t n_time = smooth_table[child_smooth_id].n_time;
		n_random_effect += n_child * n_age * n_time;
	}
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
	// check n_random_effect
	ok &= n_random_effect == pack_object.random_size();

	// pack_vec
	CppAD::vector<double> pack_vec( pack_object.size() );

	// random_vec
	CppAD::vector<double> random_vec(n_random_effect);

	// set value of random effects in pack_vec
	for(size_t i = 0; i < n_random_effect; i++)
		random_vec[i] = double(i + 1);
	dismod_at::pack_random(pack_object, pack_vec, random_vec);

	// clear random_vec
	for(size_t i = 0; i < n_random_effect; i++)
		random_vec[i] = 0.0;

	// get the random effects in pack_info
	dismod_at::unpack_random(pack_object, pack_vec, random_vec);

	// check value of random effects
	for(size_t i = 0; i < n_random_effect; i++)
		ok &= random_vec[i] == double(i + 1);

	return ok;
}
// END C++
