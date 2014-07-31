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
	vec
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

	size_t parent_node_id = 0;
	size_t n_smooth       = 2;
	size_t n_integrand    = 2;
	//
	// constructor
	dismod_at::pack_var var(
		parent_node_id, n_smooth, n_integrand
	);
	//
	// packed vector
	size_t size = var.size();
	CppAD::vector<double> vec(size);

	// set values
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	vec[ var.value_mulstd(smooth_id) ] = smooth_id + 0;
		vec[ var.dage_mulstd(smooth_id) ]  = smooth_id + 1;
		vec[ var.dtime_mulstd(smooth_id) ] = smooth_id + 2;
	}

	// check values
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	ok &= vec[ var.value_mulstd(smooth_id) ] == smooth_id + 0;
		ok &= vec[ var.dage_mulstd(smooth_id) ]  == smooth_id + 1;
		ok &= vec[ var.dtime_mulstd(smooth_id) ] == smooth_id + 2;
	}


	return ok;
}
// END C++
