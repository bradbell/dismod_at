// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin random_effect_xam.cpp$$
$spell
	var
$$

$section C++ random_effect: Example and Test$$

$code
$verbatim%example/devel/utility/random_effect_xam.cpp
%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/include/random_effect.hpp>

bool random_effect(void)
{	bool ok = true;
	using CppAD::vector;

	size_t n_integrand     = 1;
	size_t n_child         = 2;
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
	size_t n_random_effect = 0;
	vector<dismod_at::rate_struct> rate_table(dismod_at::number_rate_enum);
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{	size_t parent_smooth_id = 0;
		size_t child_smooth_id  = 1;
		if( rate_id == dismod_at::pini_enum )
			child_smooth_id  = 0;
		rate_table[rate_id].parent_smooth_id = parent_smooth_id;
		rate_table[rate_id].child_smooth_id  = child_smooth_id;
		size_t n_age  = smooth_table[child_smooth_id].n_age;
		size_t n_time = smooth_table[child_smooth_id].n_time;
		n_random_effect += n_age * n_time;
	}
	//
	// constructor
	dismod_at::pack_var pack_info(
		n_integrand, n_child,
		smooth_table, mulcov_table, rate_table
	);
	//
	// check number of random effects
	ok &= n_random_effect == size_random_effect(pack_info);

	return ok;
}
// END C++
