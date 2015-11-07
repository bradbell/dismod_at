// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
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
$icode%size_random% = size_random_effect(%pack_object%)
%$$
$codei%put_random_effect(%pack_object%, %pack_vec%, %random_vec%)
%$$
$codei%get_random_effect(%pack_object%, %pack_vec%, %random_vec%)
%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a2_double$$.

$head Order of Random Effects$$
The order of the random effects is unspecified, except for the
fact that $code put_random_effect$$ and $code get_random_effect$$
are inverses of each other; i.e., if you pack the random effects using a
$icode random_vec$$, and then do a unpack, you will get that
$icode random_vec$$ back.

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
It is the $cref pack_info$$ information corresponding
to the $cref model_variables$$.

$head size_random$$
This return value has prototype
$codei%
	size_t %size_random%
%$$
It is the number of
$cref/random effects/model_variables/Random Effects, u/$$ in the model.

$head put_random_effect$$
This functions copies information from $icode pack_vec$$
to $icode random_vec$$.

$subhead pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and its size is $icode%pack_object%.size()%$$.
It specifies the value for all the $cref model_variables$$
in $cref pack_info$$ format.

$subhead random_vec$$
This argument has prototype
$codei%
	CppAD::vector<%Float%>& %random_vec%
%$$
and its size is $icode size_random$$.
It is a copy of the random effects in $icode pack_vec$$
as one contiguous vector in an unspecified order.


$head get_random_effect$$
This functions copies information from $icode random_vec$$
to $icode pack_vec$$.

$subhead pack_vec$$
This argument has prototype
$codei%
	CppAD::vector<%Float%>& %pack_vec%
%$$
and its size is $icode%pack_object%.size()%$$.
It specifies the value for all the $cref model_variables$$
in $cref pack_info$$ format.
The input value of its random effects does not matter.  Upon return,
the random effects correspond to the values in $icode random_vec$$.

$subhead random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %random_vec%
%$$
and its size is $icode size_random$$.
It contains the random effects
as one contiguous vector in an unspecified order.

$children%
	example/devel/utility/random_effect_xam.cpp
%$$
$head Example$$
The file $cref random_effect_xam.cpp$$
contains an example and test that uses this routine.

$end
*/

# include <dismod_at/pack_info.hpp>
# include <dismod_at/a2_double.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

size_t size_random_effect(const pack_info&  pack_object)
{	size_t n_child = pack_object.child_size();
	if( n_child == 0 )
		return 0;
	//
	size_t sum = 0;
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	pack_info::subvec_info info = pack_object.rate_info(rate_id, 0);
		if( info.smooth_id != size_t(DISMOD_AT_NULL_INT) )
		{	size_t n_var = pack_object.rate_info(rate_id, 0).n_var;
			sum += n_var * n_child;
# ifndef NDEBUG
			for(size_t j = 0; j < n_child; j++)
			{	info = pack_object.rate_info(rate_id, j);
				assert( n_var == info.n_var );
			}
		}
# endif
	}
	return sum;
}

template <class Float>
void get_random_effect(
	const pack_info&              pack_object  ,
	const CppAD::vector<Float>&  pack_vec   ,
	CppAD::vector<Float>&        random_vec )
{
	assert( random_vec.size() == size_random_effect(pack_object) );
	assert( pack_vec.size()   == pack_object.size() );
	//
	size_t n_child = pack_object.child_size();

	// empty vector case
	if( n_child == 0 )
		return;

	size_t random_index = 0;
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	pack_info::subvec_info info = pack_object.rate_info(rate_id, 0);
		if( info.smooth_id != size_t(DISMOD_AT_NULL_INT) )
		{	for(size_t j = 0; j < n_child; j++)
			{	info = pack_object.rate_info(rate_id, j);
				size_t pack_index = info.offset;
				for(size_t k = 0; k < info.n_var; k++)
					random_vec[random_index++] = pack_vec[pack_index++];
			}
		}
	}
	return;
}

template <class Float>
void put_random_effect(
	const pack_info&              pack_object  ,
	CppAD::vector<Float>&        pack_vec   ,
	const CppAD::vector<Float>&  random_vec )
{
	assert( random_vec.size() == size_random_effect(pack_object) );
	assert( pack_vec.size()   == pack_object.size() );
	//
	size_t n_child = pack_object.child_size();

	// empty vector case
	if( n_child == 0 )
		return;

	size_t random_index = 0;
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	pack_info::subvec_info info = pack_object.rate_info(rate_id, 0);
		if( info.smooth_id != size_t(DISMOD_AT_NULL_INT) )
		{	for(size_t j = 0; j < n_child; j++)
			{	info = pack_object.rate_info(rate_id, j);
				size_t pack_index = info.offset;
				for(size_t k = 0; k < info.n_var; k++)
					pack_vec[pack_index++] = random_vec[random_index++];
			}
		}
	}
	return;
}

# define DISMOD_AT_INSTANTIATE_RANDOM_EFFECT(Float)           \
	template void get_random_effect(                       \
	const pack_info&              pack_object  ,                 \
	const CppAD::vector<Float>&  pack_vec   ,                 \
	CppAD::vector<Float>&        random_vec                   \
	);                                                        \
	template void put_random_effect(                         \
	const pack_info&              pack_object  ,                 \
	CppAD::vector<Float>&        pack_vec   ,                 \
	const CppAD::vector<Float>&  random_vec                   \
	);

// instantiations
DISMOD_AT_INSTANTIATE_RANDOM_EFFECT(double)
DISMOD_AT_INSTANTIATE_RANDOM_EFFECT( CppAD::AD<double> )
DISMOD_AT_INSTANTIATE_RANDOM_EFFECT( a2_double )


} // END DISMOD_AT_NAMESPACE
