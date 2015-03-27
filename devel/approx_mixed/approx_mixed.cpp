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
$begin approx_mixed_notation$$
$spell
	CppAD
	vec
$$

$section Notation Used to Document approx_mixed Base Class$$

$head Float$$
For any function or operation,
the type $icode Float$$ is one of the following:
$code double$$, $code AD<double>$$, $code AD< AD<double> >$$,
where $code AD$$ is $code CppAD::AD$$.

$head Negative Log-Density Vector$$
If $icode vec$$ is a density vector corresponding to $latex \B{p}(x)$$,
the corresponding negative log-density is given by
$pre
	$$
$latex - \log [ \B{p} (x) ] = $$
$icode%vec%[0] + CppAD::abs(%vec%[1]) + %...% CppAD::abs(%vec%[%s%-1])
%$$
where $icode%s% = %vec%.size()%$$.

$head Fixed Effects, theta$$
For each $code approx_mixed$$ object, there is a vector of fixed effects.
The number of the fixed effects, and their order in the vector, does not vary.
We sometimes use $latex \theta$$ to denote the vector of fixed effects.

$head Random Effects, u$$
For each $code approx_mixed$$ object, there is a vector of random effects.
The number of the random effects, and their order in the vector, does not vary.
We sometimes use $latex u$$ to denote the vector of random effects.

$end
-------------------------------------------------------------------------------
$begin approx_mixed_joint_density$$
$spell
	vec
	CppAD
	const
$$

$section Compute Joint Density for the Data and Random Effect$$

$head Syntax$$
$icode%vec% = %approx_object%.joint_density(%fixed_vec%, %random_vec%)%$$

$head approx_object$$
This is an object of a class that is derived from the
$code approx_mixed$$ base class.

$head Virtual Function$$
This is a pure virtual function and must be implemented
in the derived class with prototype
$codei%
	virtual CppAD::vector<%Float%> joint_density(
		const vector<%Float%>& fixed_vec  ,
		const vector<%Float%>& random_vec )
%$$
for each of the possible
$cref/Float/approx_mixed_notation/Float/$$ cases.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %fixed_vec%
%$$
It contains a value for the
$cref/fixed effects/approx_mixed_notation/Fixed Effects, theta/$$ vector.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %random_vec%
%$$
It contains a value for the
$cref/random effects/approx_mixed_notation/Random Effects, u/$$ vector.

$head vec$$
This result has prototype
$codei%
	CppAD::vector<%Float%> %vec%
%$$
It is a
$cref/negative log-density vector
	/approx_mixed_notation
	/Negative Log-Density Vector
/$$
corresponding to the joint density of the data and random effects; i.e.,
$latex \[
	\B{p}( y, u | \theta) = \B{p}( y | u , \theta) \B{p} ( u | \theta )
\] $$

$end
-------------------------------------------------------------------------------
*/
# include <dismod_at/approx_mixed.hpp>
# include <cppad/ipopt/solve.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ----------------------------------------------------------------------------
// helper class used by optimize_random
class optimize_random_eval {
public:
	typedef CppAD::vector<double>              Dvector;
	typedef CppAD::vector< CppAD::AD<double> > ADvector;
private:
	const size_t     n_abs_;
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
	n_abs_        ( n_abs )           ,
	n_random_     ( n_random )        ,
	approx_object_( approx_object )
	{	fixed_vec_.resize( fixed_vec.size() );
		for(size_t i = 0; i < fixed_vec.size(); i++)
			fixed_vec_[i] = fixed_vec[i];
	}

	// evaluate objective and constraints
	void operator()(ADvector& fg, const ADvector& x)
	{	assert( fg.size() == 1 + 2 * n_abs_ );

		// extract the random effects from x
		ADvector random_vec(n_random_);
		for(size_t j = 0; j < n_random_; j++)
			random_vec[j] = x[j];

		// compute log-density vector
		ADvector vec = approx_object_.joint_density(fixed_vec_, random_vec);

		// initialize smooth part of negative log-likelihood
		size_t k = 0;
		fg[k++]  = vec[0];

		// terms corresponding to data likelihood absolute values
		size_t n_abs   = vec.size()-1;
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
	const CppAD::vector<double>& fixed_vec       ,
	const CppAD::vector<double>& random_in       )
{	typedef CppAD::vector<double>   Dvector;

	// number of random effects
	size_t n_random = random_in.size();

	// determine initial density vector
	Dvector vec = joint_density(fixed_vec, random_in);

	// number of absolute value terms in objective
	size_t n_abs = vec.size() - 1;

	// number of independent variable is number of random effects
	// plus number of log-density terms that require absolute values
	size_t nx = n_random + n_abs;

	// set initial x vector
	Dvector xi(nx);
	for(size_t j = 0; j < n_random; j++)
		xi[j] = random_in[j];
	for(size_t j = 0; j < n_abs; j++)
		xi[n_random + j] = CppAD::abs( vec[j] );

	// ipopts default value for infinity (use options to change it)
	double ipopt_infinity = 1e19;

	// set lower and upper limits for x
	Dvector xl(nx), xu(nx);
	for(size_t j = 0; j < nx; j++)
	{	xl[j] = - ipopt_infinity;
		xu[j] = + ipopt_infinity;
	}

	// set limits for abs contraint representation
	Dvector gl(2*n_abs), gu(2*n_abs);
	for(size_t j = 0; j < 2*n_abs; j++)
	{	gl[j] = 0.0;
		gu[j] = + ipopt_infinity;
	}

	// construct fg_eval  object
	optimize_random_eval fg_eval(n_abs, n_random, fixed_vec, *this);

	// optimizer options
	std::string options;
	options += "Integer print_level 0\n";
	options += "String  sb          yes\n";
	options += "String  derivative_test second-order\n";

	// return solution
	CppAD::ipopt::solve_result<Dvector> solution;

	// solve the optimization problem
	CppAD::ipopt::solve(
		options, xi, xl, xu, gl, gu, fg_eval, solution
	);

	return solution.x;
}



} // END_DISMOD_AT_NAMESPACE

