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
$begin variable_vec_xam.cpp$$
$spell
	vec
$$

$section C++ variable_vec: Example and Test$$
$index example, C++ variable_vec$$
$index variable_vec, C++ example$$

$code
$verbatim%example/devel/model/variable_vec_xam.cpp
%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/dismod_at.hpp>

bool variable_vec_xam(void)
{	bool ok = true;
	using CppAD::vector;
	size_t j;

	size_t parent_node_id = 0;
	size_t n_smooth       = 2;
	size_t n_integrand    = 2;
	//
	size_t n_node = 4;
	vector<dismod_at::node_struct> node_table(n_node);
	node_table[0].parent = -1;
	node_table[1].parent = 0;
	node_table[2].parent = 0;
	node_table[3].parent = 2;
	//
	size_t n_data = 5;
	vector<dismod_at::data_struct> data_table(n_data);
	data_table[0].node_id = 1;
	data_table[1].node_id = 1;
	data_table[2].node_id = 2;
	data_table[3].node_id = 3;
	data_table[4].node_id = 3;
	//
	size_t n_mulcov = 2;
	vector<dismod_at::mulcov_struct> mulcov_table(n_mulcov);
	mulcov_table[0].mulcov_type  = "rate_mean";
	mulcov_table[0].rate_id      = 1;
	mulcov_table[0].integrand_id = -1;
	mulcov_table[0].covariate_id = 0;
	mulcov_table[0].smooth_id    = 0;
	mulcov_table[1].mulcov_type  = "meas_mean";
	mulcov_table[1].rate_id      = -1;
	mulcov_table[1].integrand_id = 1;
	mulcov_table[1].covariate_id = 1;
	mulcov_table[1].smooth_id    = 1;
	//
	typedef CppAD::AD<double> Float;
	dismod_at::variable_vec<Float> var(
		parent_node_id, n_smooth, n_integrand, 
		node_table, data_table, mulcov_table
	);
	//
	vector<Float> mulstd_set(3 * n_smooth ), mulstd_get(3 * n_smooth);
	for(size_t i_smooth = 0; i_smooth < n_smooth; i_smooth++)
	{	for(j = 0; j < 3; j++)
			mulstd_set[ 3 * i_smooth + j ] = Float(i_smooth + j);
	}
	var.set_mulstd( mulstd_set );
	var.get_mulstd( mulstd_get );
	for(size_t i_smooth = 0; i_smooth < n_smooth; i_smooth++)
	{	for(j = 0; j < 3; j++)
			ok &= mulstd_get[ 3 * i_smooth + j ] == Float(i_smooth + j);
	}

	return ok;
}
// END C++
