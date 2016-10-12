// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin random_limits$$
$spell
	vec
$$

$section Limits for Random Effects Smoothing$$

$head Syntax$$
$codei%get_random_limits(
	%pack_object%, %prior_table%, %s_info_vec%, %random_lower%, %random_upper%
)%$$

$head Prototype$$
$srcfile%devel/utility/random_limits.cpp
	%4%// BEGIN PROTOTYPE%// END PROTOTYPE%1%$$

$head Remark$$
Currently, these lower and upper limits are minus and plus infinity
respectively. This routine is used in the expectation that in the future
one may be able to constrain the random effects to be specific values; see
$cref/constrain random effects/wish_list/Constrain Random Effects/$$
in the wish list.

$head Order of Random Effects$$
The order of the random effects is unspecified, except for the fact that
$cref/pack_random/random_effect/pack_random/$$ and
$cref/unpack_random/random_effect/unpack_random/$$
are inverses of each other.

$head pack_object$$
It is the $cref pack_info$$ information corresponding
to the $cref model_variables$$.

$head prior_table$$
This argument is the $cref/prior_table/get_prior_table/prior_table/$$.

$head s_info_vec$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%s_info_vec%[ %smooth_id% ]
%$$
is the corresponding $cref smooth_info$$ information.

$head random_lower$$
The input size and contents of this vector does not matter.
Upon return it has size equal to the number of random effects
and contains the corresponding prior lower limits.

$head random_upper$$
The input size and contents of this vector does not matter.
Upon return it has size equal to the number of random effects
and contains the corresponding prior upper limits.

$end
*/
# include <dismod_at/random_limits.hpp>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/random_effect.hpp>

namespace dismod_at {
	// BEGIN PROTOTYPE
	void random_limits(
		const pack_info&                   pack_object   ,
		const CppAD::vector<prior_struct>& prior_table   ,
		const CppAD::vector<smooth_info>&  s_info_vec    ,
		CppAD::vector<double>&             random_lower  ,
		CppAD::vector<double>&             random_upper  )
	// END PROTOTYPE
	{	// prior in dismod_at variable order
		CppAD::vector<size_t> value_prior = pack_value_prior(
			pack_object, s_info_vec
		);
		// number of dismod_at varaibles
		size_t n_var = value_prior.size();
		//
		// packed vector containg all the limits
		CppAD::vector<double> packed_lower(n_var), packed_upper(n_var);
		for(size_t var_id = 0; var_id < n_var; var_id++)
		{	size_t prior_id = value_prior[var_id];
			prior_struct prior   = prior_table[prior_id];
			packed_lower[var_id] = prior.lower;
			packed_upper[var_id] = prior.upper;
		}
		//
		// number of random effects
		size_t n_random = size_random_effect(pack_object);
		random_lower.resize(n_random);
		random_upper.resize(n_random);
		//
		// unpack the random effect limits
		unpack_random(pack_object, packed_lower, random_lower);
		unpack_random(pack_object, packed_upper, random_upper);
		//
	}
}

