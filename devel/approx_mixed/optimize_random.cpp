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
# include <cppad/ipopt/solve.hpp>
/*
$begin approx_mixed_optimize_random$$
$spell
	vec
	const
	CppAD
$$

$section Optimize Random Effects$$

$head Syntax$$
$icode%random_out% =%$$
$icode%approx_object%.optimize_random(%fixed_vec%, %random_in%)%$$

$head Purpose$$
This routine maximizes the
$cref/joint density/approx_mixed_joint_density/$$ corresponding to the
object $icode approx_object$$.

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$.

$head random_in$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_in%
%$$
It specifies the initial value used for the optimization of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$.

$head random_out$$
This argument has prototype
$codei%
	CppAD::vector<double> %random_out%
%$$
It is the final value (obtained by optimization) of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$.

$head Laplace Likelihood$$
This optimization properly handles the case where
the joint density contains Laplace likelihood terms.

$children%
	example/devel/approx_mixed/optimize_random_xam.cpp
%$$
$head Example$$
The file $cref optimize_random_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ----------------------------------------------------------------------------
// helper class used by optimize_random
class optimize_random_eval {
public:
	// same as approx_mixed::d_vector
	typedef CppAD::vector<double>              Dvector;

	// same as approx_mixed::a1d_vector
	typedef CppAD::vector< CppAD::AD<double> > ADvector;
private:
	const size_t     n_abs_;
	const size_t     n_fixed_;
	const size_t     n_random_;
	ADvector         fixed_vec_;
	approx_mixed&    approx_object_;
public:
	// constructor
	optimize_random_eval(
		size_t           n_abs           ,
		size_t           n_random        ,
		const Dvector&   fixed_vec       ,
		approx_mixed&    approx_object
	) :
	n_abs_        ( n_abs )            ,
	n_fixed_      ( fixed_vec.size() ) ,
	n_random_     ( n_random )         ,
	approx_object_( approx_object )
	{	fixed_vec_.resize( n_fixed_ );
		for(size_t i = 0; i < n_fixed_; i++)
			fixed_vec_[i] = fixed_vec[i];
	}

	// evaluate objective and constraints
	void operator()(ADvector& fg, const ADvector& x)
	{	assert( fg.size() == 1 + 2 * n_abs_ );

		// extract the random effects from x
		ADvector both(n_fixed_ + n_random_);
		for(size_t j = 0; j < n_fixed_; j++)
			both[j] = fixed_vec_[j];
		for(size_t j = 0; j < n_random_; j++)
			both[n_fixed_ + j] = x[j];

		// compute log-density vector
		ADvector vec = approx_object_.a1_joint_density_.Forward(0, both);

		// initialize smooth part of negative log-likelihood
		size_t k = 0;
		fg[k++]  = vec[0];

		// terms corresponding to data likelihood absolute values
		size_t n_abs   = vec.size() - 1;
		for(size_t j = 0; j < n_abs; j++)
		{	// x[ n_random_ + j] >= abs(log_den[1 + j]
			fg[k++] = x[ n_random_ + j] - vec[1 + j];
			fg[k++] = x[ n_random_ + j] + vec[1 + j];
			//
			// smooth contribution to log-likelihood
			fg[0]  += x[ n_random_ + j];
		}
	}
};

// ----------------------------------------------------------------------------
// optimize_random
CppAD::vector<double> approx_mixed::optimize_random(
	const d_vector& fixed_vec       ,
	const d_vector& random_in       )
{
	// number of fixed and random effects
	assert( n_fixed_  == fixed_vec.size() );
	assert( n_random_ == random_in.size() );

	// determine initial density vector
	d_vector both(n_fixed_ + n_random_);
	for(size_t j = 0; j < n_fixed_; j++)
		both[j] = fixed_vec[j];
	for(size_t j = 0; j < n_random_; j++)
		both[n_fixed_ + j] = random_in[j];
	d_vector vec = a0_joint_density_.Forward(0, both);

	// number of absolute value terms in objective
	size_t n_abs = vec.size() - 1;

	// number of independent variable is number of random effects
	// plus number of log-density terms that require absolute values
	size_t nx = n_random_ + n_abs;

	// set initial x vector
	d_vector xi(nx);
	for(size_t j = 0; j < n_random_; j++)
		xi[j] = random_in[j];
	for(size_t j = 0; j < n_abs; j++)
		xi[n_random_ + j] = CppAD::abs( vec[j] );

	// ipopts default value for infinity (use options to change it)
	double ipopt_infinity = 1e19;

	// set lower and upper limits for x
	d_vector xl(nx), xu(nx);
	for(size_t j = 0; j < nx; j++)
	{	xl[j] = - ipopt_infinity;
		xu[j] = + ipopt_infinity;
	}

	// set limits for abs contraint representation
	d_vector gl(2*n_abs), gu(2*n_abs);
	for(size_t j = 0; j < 2*n_abs; j++)
	{	gl[j] = 0.0;
		gu[j] = + ipopt_infinity;
	}

	// construct fg_eval  object
	optimize_random_eval fg_eval(n_abs, n_random_, fixed_vec, *this);

	// optimizer options
	std::string options;
	options += "Integer print_level 0\n";
	options += "String  sb          yes\n";
	options += "String  derivative_test second-order\n";

	// return solution
	CppAD::ipopt::solve_result<d_vector> solution;

	// solve the optimization problem
	CppAD::ipopt::solve(
		options, xi, xl, xu, gl, gu, fg_eval, solution
	);

	return solution.x;
}


} // END_DISMOD_AT_NAMESPACE

