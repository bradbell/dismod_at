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
$begin cppad_conjgrad$$
$spell
	CppAD
	xout
	cppad
	xin
	const
	conjgrad
	Taylor
$$

$section A CppAD Linear or Nonlinear Conjugate Gradient Method$$

$head Syntax$$
$icode%xout% = cppad_conjgrad(%fun%, %xin%, %n_step%)%$$

$head Under Construction$$

$head Purpose$$
Uses a CppAD function object,
and the 
$href%http://en.wikipedia.org/wiki/Nonlinear_conjugate_gradient_method
	nonlinear conjugate gradient method
%$$,
to minimize a function.
This is accomplished in a way so that
the CppAD sequence of operations does not depend on the argument
$icode xin$$.

$head fun$$
This argument has prototype
$codei%
	CppAD::ADFun<%Float%>& %fun%
%$$
It corresponds to a function $latex f:\B{R}^n \rightarrow \B{R}$$; i.e.
its range dimension is one.
As set of Forward mode calls are made to this function
and the state of its Taylor coefficients upon return is not specified.

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, $code AD< AD<double> >$$,
where $code AD$$ is $code CppAD::AD$$.

$head xin$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %xin%
%$$
and is the starting point for the algorithm.
It is assumed that this is close enough to the true minimum
so that the algorithm will converge.

$head n_step$$
Is the number of steps of the algorithm to execute.

$head Example$$
The file $code cppad_conjgrad.cpp$$ is an example and test of this routine.
It returns $code true$$, if the test passes, and $code false$$ otherwise.

$end
*/
# include <cppad/cppad.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

template <class Float>
CppAD::vector<Float> cppad_conjgrad(
	CppAD::ADFun<Float>&          fun    ,
	const CppAD::vector<Float>&   xin    ,
	size_t                        n_step )
{	using CppAD::vector;

	assert( fun.Domain() == xin.size() );
	assert( fun.Range()  == 1 );
	size_t n = fun.Domain();
	size_t m = fun.Range();
	
	// initial x vector
	vector<Float> xcur = xin;

	// initial function value 
	vector<Float> fcur = fun.Forward(0, xcur);

	// initial gradient
	vector<Float> w(m);
	w[0] = Float(1.0);
	vector<Float> gcur = fun.Reverse(1, w);

	// initial search direction
	vector<Float> scur(n);
	for(size_t j = 0; j < n; j++)
		scur[j] = - gcur[j];

	// version of length n with zeros
	vector<Float> zero(n);
	for(size_t j = 0; j < n; j++)
		zero[j] = Float(0.0);

	vector<Float> df1(m), df2(m), fnext(m), xnext(n), gnext(n);
	for(size_t j_step = 0; j_step < n_step; j_step++)
	{	// first derivative of function in scur direction
		df1 = fun.Forward(1, scur);

		// second derivative of function in scur direction
		df2 = fun.Forward(2, zero);

		// newton step for minimizing fun( xcur + alpha * scur ) w.r.t alpha
		Float alpha = - df1[0] / df2[0];

		// next x value
		for(size_t j = 0; j < n; j++)
			xnext[j] = xcur[j] + alpha * scur[j];

		// next function value 
		fnext = fun.Forward(0, xnext);

		// next gradient value
		gnext = fun.Reverse(1, w);

		// Polak-Ribiere formula for beta
		Float numerator   = Float(0.0);
		Float denominator = Float(0.0);
		for(size_t j = 0; j < n; j++)
		{	numerator   += gnext[j] * (gnext[j] - gcur[j]);
			denominator += gcur[j] * gcur[j];
		}
		Float beta = numerator / denominator;

		// update current values
		for(size_t j = 0; j < n; j++)
		{	xcur[j] = xnext[j];
			scur[j] = beta * scur[j] - gnext[j];
			gcur[j] = gnext[j];
		}
	}
	return xcur;
}

# define DISMOD_AT_INSTANTIATE_CPPAD_CONJGRAD(Float)                 \
	template  CppAD::vector<Float> cppad_conjgrad(                   \
		CppAD::ADFun<Float>&          fun    ,                       \
		const CppAD::vector<Float>&   xin    ,                       \
		size_t                        n_step                         \
	);

// instantiations
DISMOD_AT_INSTANTIATE_CPPAD_CONJGRAD(double)
DISMOD_AT_INSTANTIATE_CPPAD_CONJGRAD(CppAD::AD<double>)
DISMOD_AT_INSTANTIATE_CPPAD_CONJGRAD(CppAD::AD< CppAD::AD<double> >)

} // END_DISMOD_AT_NAMESPACE
