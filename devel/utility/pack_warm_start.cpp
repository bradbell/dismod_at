/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin pack_warm_start$$
$spell
$$
$section Packing and Unpacking Warm Start Information in a Vector$$

$head Syntax$$
$icode%vec%        = pack_warm_start(%warm_start%)
%$$
$icode%warm_start% = unpack_warm_start(%vec%)
%$$

$head Prototype$$
$srcthisfile%0%// BEGIN_PACK%//END_PACK%1%$$
$srcthisfile%0%// BEGIN_UNPACK%//END_UNPACK%1%$$

$head vec$$
This is a single vector that contains a copy of the
warm start information.

$head warm_start$$
This is a structure, holding the warm start information
in the form expected by $code cppad_mixed$$.

$end
*/
# include <dismod_at/pack_warm_start.hpp>

namespace dismod_at {

// BEGIN_PACK
CppAD::vector<double> pack_warm_start(
	const CppAD::mixed::warm_start_struct& warm_start )
// END_PACK
{	size_t n    = warm_start.x_info.size();
	size_t m    = warm_start.g_info.size();
	size_t size = 3 + 4 * n + 2 * m;
	CppAD::vector<double> vec(size);
	size_t index = 0;
	vec[index++]   = double(n);
	vec[index++]   = double(m);
	vec[index++]   = warm_start.scale_f;
	for(size_t j = 0; j < n; ++j)
	{	vec[index++] = warm_start.x_info[j].x;
		vec[index++] = warm_start.x_info[j].z_L;
		vec[index++] = warm_start.x_info[j].z_U;
		vec[index++] = warm_start.x_info[j].scale_x;
	}
	for(size_t i = 0; i < m; ++i)
	{	vec[index++] = warm_start.g_info[i].lambda;
		vec[index++] = warm_start.g_info[i].scale_g;
	}
	// check conversion from size_t to double
	assert( size_t(vec[0]) == n );
	assert( size_t(vec[1]) == m );
	return vec;
}
// BEGIN_UNPACK
CppAD::mixed::warm_start_struct unpack_warm_start(
	const CppAD::vector<double>& vec )
// END_UNPACK
{	CppAD::mixed::warm_start_struct warm_start;
	size_t index = 0;
	//
	size_t n = size_t( vec[index++] );
	warm_start.x_info.resize(n);
	//
	size_t m = size_t( vec[index++] );
	warm_start.g_info.resize(m);
	//
	warm_start.scale_f = vec[index++];
	//
	for(size_t j = 0; j < n; ++j)
	{	warm_start.x_info[j].x       = vec[index++];
		warm_start.x_info[j].z_L     = vec[index++];
		warm_start.x_info[j].z_U     = vec[index++];
		warm_start.x_info[j].scale_x = vec[index++];
	}
	for(size_t i = 0; i < m; ++i)
	{	warm_start.g_info[i].lambda  = vec[index++];
		warm_start.g_info[i].scale_g = vec[index++];
	}
	return warm_start;
}

} // END_DISMOD_AT_NAMESPACE
