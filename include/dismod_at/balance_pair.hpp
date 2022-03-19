/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-22 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_BALANCE_PAIR_HPP
# define DISMOD_AT_BALANCE_PAIR_HPP
/*
$begin balance_pair$$
$spell
	vec
	dismod
$$

$section Balanced Sampling from Vector of Standard Pairs$$

$head Syntax$$
$icode%sample_vec% = dismod_at::balance_pair(
	%n_sample%, %pair_vec%, %second_1%, %second_2%
)%$$

$head Prototype$$
$srcthisfile%0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1%$$

$head Balancing$$
This routine samples randomly from a set of pairs with the following
extra condition:
For each chosen pair, if there is another pair
with the same first value, opposite second value, and not yet chosen,
such another pair is also chosen.
If there is more that one such pair, one is chosen at random.
The samples with opposite second value will be sequential.

$head n_sample$$
Is the number of pairs to choose. It is possible that one extra pair
is chosen to meet the balancing condition above.

$head pair_vec$$
is the vector of pairs to choose from.

$head second_1$$
Is the smaller of the two opposite second values in a pair.

$head second_2$$
Is the larger of the two opposite second values in a pair.

$head sample_vec$$
is the set of indices in $icode pair_vec$$ corresponding to the
chosen set of pairs.

$end
*/
# include <gsl/gsl_randist.h>
# include <cppad/utility/vector.hpp>
# include <cppad/utility/index_sort.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
template <class First, class Second>
CppAD::vector<size_t> balance_pair(
	size_t                                           n_sample  ,
	const CppAD::vector< std::pair<First, Second> >& pair_vec  ,
	const Second&                                    second_1 ,
	const Second&                                    second_2 )
// END_PROTOTYPE
{	//
	// pair_t
	typedef std::pair<First, Second> pair_t;
	//
	// second_1 < second_2
	assert( second_1 < second_2 );
	//
	// n_pair
	size_t n_pair = pair_vec.size();
	//
	// n_sample <= n_pair
	assert( n_sample <= n_pair );
	//
	// rng
	gsl_rng* rng = CppAD::mixed::get_gsl_rng();
	//
	// random_order
	CppAD::vector<size_t> random_order(n_pair);
	CppAD::vector<double> random_key(n_pair);
	for(size_t i = 0; i < n_pair; ++i)
		random_key[i] = gsl_ran_flat(rng, 0., 1.);
	CppAD::index_sort(random_key, random_order);
	//
	// pair_order
	// sort by (first, second, random)
	CppAD::vector<size_t>                      pair_order(n_pair);
	CppAD::vector< std::pair<pair_t, double> > pair_key(n_pair);
	for(size_t i = 0; i < n_pair; ++i)
		pair_key[i] = std::pair<pair_t, double>(pair_vec[i], random_key[i]);
	CppAD::index_sort(pair_key, pair_order);
	//
	// inv_pair_order
	CppAD::vector<size_t> inv_pair_order(n_pair);
	for(size_t i = 0; i < n_pair; ++i)
		inv_pair_order[ pair_order[i] ] = i;
	//
	// used
	// which indices are already in sample_vec
	CppAD::vector<bool> used(n_pair);
	for(size_t i = 0; i < n_pair; ++i)
		used[i] = false;
	//
	// sample_vec
	CppAD::vector<size_t> sample_vec(n_sample);
	//
	// i_random, i_sample
	size_t i_random = 0;
	size_t i_sample = 0;
	while(i_sample < n_sample)
	{	// i_random
		while( used[ random_order[i_random] ] )
			++i_random;
		assert( i_random <= i_sample);
		//
		// sample, sample_vec, used
		size_t sample        = random_order[i_random];
		sample_vec[i_sample] = sample;
		used[sample]         = true;
		//
		// i_sample, i_random
		++i_sample;
		++i_random;
		//
		if( i_sample < n_sample )
		{	//
			// first, equal_1, equal_2
			pair_t pair = pair_vec[sample];
			First first  = pair.first;
			bool equal_1 = pair.second == second_1;
			bool equal_2 = pair.second == second_2;
			//
			// i_pair_order
			size_t i_pair_order = inv_pair_order[sample];
			//
			// sample, match_pair
			pair_t match_pair;
			if( equal_1 )
			{	match_pair = pair_t(first, second_2);
				while( i_pair_order + 1 < n_pair && pair < match_pair )
				{	++i_pair_order;
					sample = pair_order[i_pair_order];
					pair   = pair_vec[sample];
					if( used[sample] )
						pair.second = second_1;
				}
			}
			if( equal_2 )
			{	match_pair = pair_t(first, second_1);
				while( 0 < i_pair_order && match_pair < pair )
				{	--i_pair_order;
					sample = pair_order[i_pair_order];
					pair   = pair_vec[sample];
					if( used[sample] )
						pair.second = second_2;
				}
			}
			if( pair == match_pair)
			{	assert( ! used[sample] );
				//
				// sample_vec, used, i_sample
				sample_vec[i_sample] = sample;
				used[sample]         = true;
				++i_sample;
			}
		}
	}
	return sample_vec;
}

} // END_DISMOD_AT_NAMESPACE

# endif
