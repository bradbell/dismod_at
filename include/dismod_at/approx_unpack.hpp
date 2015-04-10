// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_APPROX_UNPACK_HPP
# define DISMOD_AT_APPROX_UNPACK_HPP
# include <dismod_at/approx_mixed.hpp>

/*
$begin approx_mixed_unpack$$
$spell
	vec
	const
	Cpp
	dismod
	hpp
$$

$section approx_mixed: Pack Fixed Effect and Random Effects Into One Vector$$

$head Syntax$$
$codei%unpack(%fixed_vec%, %random_vec%, %both_vec%)%$$

$head Private$$
This function is $code private$$ to the $code approx_mixed$$ class
and cannot be used by a derived
$cref/approx_object/approx_mixed_derived_ctor/approx_object/$$.

$head Float_pack$$
This can be any type.

$head Float_unpack$$
If $icode x$$ has type $icode Float_pack$$,
the syntax $icode%Float_unpack%(%x%)%$$ must convert $icode x$$
to the type $icode Float_unpack$$.

$head fixed_vec$$
This argument has prototype
$codei%
	CppAD::vector<%Float_unpack%>& %fixed_vec%
%$$
The input value of its elements does not matter.
Upon return, it contains the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$.
corresponding to $icode both_vec$$.

$head random_vec$$
This argument has prototype
$codei%
	CppAD::vector<%Float_unpack%>& %random_vec%
%$$
The input value of its elements does not matter.
Upon return, it contains the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$.
corresponding to $icode both_vec$$.

$head both_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float_pack%>& %both_vec%
%$$
The size of this vector must be equal to
$cref/n_fixed_/approx_mixed_private/n_fixed_/$$
+
$cref/n_random_/approx_mixed_private/n_random_/$$.

$end
*/
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

template <class Float_unpack, class Float_pack>
void approx_mixed::unpack(
	CppAD::vector<Float_unpack>&      fixed_vec  ,
	CppAD::vector<Float_unpack>&      random_vec ,
	const CppAD::vector<Float_pack>&  both_vec   ) const
{
	assert( fixed_vec.size() == n_fixed_ );
	assert( random_vec.size() == n_random_ );
	assert( both_vec.size() == n_fixed_ + n_random_ );
	for(size_t j = 0; j < n_fixed_; j++)
		fixed_vec[j] = Float_unpack( both_vec[j] );
	for(size_t j = 0; j < n_random_; j++)
		random_vec[j] = Float_unpack( both_vec[n_fixed_ + j] );
}


} // END_DISMOD_AT_NAMESPACE

# endif
