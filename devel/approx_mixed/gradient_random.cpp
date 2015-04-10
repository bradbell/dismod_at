// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/approx_mixed.hpp>
/*
$begin approx_mixed_gradient_random$$
$spell
	vec
	const
	Cpp
	xam
$$

$section Hessian With Respect to Random Effects$$

$head Syntax$$
$icode%grad% = %approx_object%.gradient_random( %fixed_vec%, %random_vec%)%$$

$head Purpose$$
This routine computes the gradient of the negative log of the joint density
$cref/f(theta, u)/approx_mixed_theory/Joint Density, f(theta, u)/$$
with respect to the random effects vector $latex u$$; i.e.
$latex \[
	f_u^{(1)} ( \theta, u )
\] $$

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<a2_double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<a2_double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$.

$head grad$$
The return value has prototype
$codei%
	CppAD::vector<a2_double>& %grad%
%$$
It contains the gradient $latex f_u^{(1)} ( \theta , u )$$.

$children%
	example/devel/approx_mixed/gradient_random_xam.cpp
%$$
$head Example$$
The file $cref gradient_random_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ----------------------------------------------------------------------------
// gradient_random
CppAD::vector<approx_mixed::a2_double> approx_mixed::gradient_random(
	const a2d_vector&        fixed_vec   ,
	const a2d_vector&        random_vec  )
{
	// number of fixed and random effects
	assert( n_fixed_  == fixed_vec.size() );
	assert( n_random_ == random_vec.size() );

	// make sure gradient has been recorded
	if( gradient_.size_var() == 0 )
	{	std::cerr << "approx_mixed::initialize was not called before"
		<< " approx_mixed::gradient_random" << std::endl;
		exit(1);
	}

	// create an a2d_vector containing (theta, u)
	a2d_vector both_vec( n_fixed_ + n_random_ );
	pack(fixed_vec, random_vec, both_vec);

	// compute the gradient
	size_t order = 0;
	return gradient_.Forward(order, both_vec);
}


} // END_DISMOD_AT_NAMESPACE

