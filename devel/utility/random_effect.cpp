// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin random_effect$$
$spell
	vec
	CppAD
	var
	cpp
	const
$$

$section Setting and Getting the Random Effect Vector$$

$head Syntax$$
$icode%size_random% = size_random_effect(%pack_info%)
%$$
$codei%set_random_effect(%pack_info%, %pack_vec%, %random_vec%)
%$$
$codei%get_random_effect(%pack_info%, %pack_vec%, %random_vec%)
%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, $code AD< AD<double> >$$,
where $code AD$$ is $code CppAD::AD$$.

$head Order of Random Effects$$
The order of the random effects is unspecified, except for the
fact that $code set_random_effect$$ and $code get_random_effect$$
are inverses of each other; i.e., if you set the random effects to a 
specific vector, and then do a get, you will get that vector back.

$head pack_info$$
This argument has prototype
$codei%
	const pack_var& %pack_info%
%$$
It is the $cref pack_var$$ information corresponding
to the $cref/model_variables/model_variable/$$.

$head size_random$$
This return value has prototype
$codei%
	size_t %size_random%
%$$
It is the number of
$cref/random effects/model_variable/Random Effects, u/$$ in the model.

$head set_random_effect$$

$subhead pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and its size is $icode%pack_info%.size()%$$.
It specifies the value for all the $cref/model_variables/model_variable/$$.

$subhead random_vec$$
This argument has prototype
$codei%
	CppAD::vector<%Float%>& %random_vec%
%$$
and its size is $icode size_random$$.
It is the set of random effects in $icode pack_vec$$ as one contiguous vector.


$head get_random_effect$$

$subhead pack_vec$$
This argument has prototype
$codei%
	CppAD::vector<%Float%>& %pack_vec%
%$$
and its size is $icode%pack_info%.size()%$$.
The input value of its random effects does not matter.
Upon return, the correspond to the values in $icode random_vec$$.

$subhead random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %random_vec%
%$$
and its size is $icode size_random$$.
It specifies the value for the random effects as one contiguous vector.

$children%
	example/devel/utility/random_effect_xam.cpp
%$$
$head Example$$
The file $cref random_effect_xam.cpp$$
contains an example and test that uses this routine.

$end
*/

# include <dismod_at/include/pack_var.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

size_t size_random_effect(const pack_var&  pack_info)
{	size_t n_child = pack_info.child_size();
	if( n_child == 0 )
		return 0;
	//
	size_t sum = 0;
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_var = pack_info.rate_info(rate_id, 0).n_var;
		sum += n_var * n_child;
# ifndef NDEBUG
		for(size_t j = 0; j < n_child; j++)
		{	pack_var::subvec_info info = pack_info.rate_info(rate_id, j);
			assert( n_var == info.n_var );
		}
# endif
	}
	return sum;
}

} // END DISMOD_AT_NAMESPACE
