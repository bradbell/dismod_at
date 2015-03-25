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
$begin optimize_random$$
$spell
	vec
	const
	CppAD
	struct
	logden
$$

$section Given the Fixed Effects, Optimize the Random Effects$$

$head Under Construction$$

$head Syntax$$
$icode%random_out% = optimize_random(%fixed_vec%, %random_in%, %logden%)%$$

$head fixed_vec$$
This argument to $code optimize_random$$ has prototype 
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It is the vector of fixed effects values.

$head random_in$$
This argument to $code optimize_random$$ has prototype 
$codei%
	const CppAD::vector<double>& %random_in%
%$$
The input value of this vector is the starting point for the optimization
of the random effects.


$head logden$$
This argument has prototype
$codei%
	%Logden%& %logden%
%$$
where $icode Logden$$ is a special class such that 
$icode logden$$ supports the following syntax:
$codei%
	%logden_vec% = %logden%(%theta%, %u%)
%$$
Up to a constant that does not depend on the fixed or random effects,
the corresponding log-density is for 
the data and the random effects given the fixed effects; i.e.,
$latex \[
	\B{p} ( y, u | \theta ) = \B{p}(y | u , \theta ) \B{p} ( u | \theta )
\] $$
The sequence of operations computed by $icode random_logden$$ 
must be the same for all values of $icode u$$.

$subhead Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, $code AD< AD<double> >$$,
where $code AD$$ is $code CppAD::AD$$.

$subhead theta$$
This argument has prototype 
$codei%
	const CppAD::vector<%Float%>& %theta%
%$$
It has the same size and element values as the vector $icode fixed_vec$$.

$subhead u$$
This argument has prototype 
$codei%
	const CppAD::vector<%Float%>& %u%
%$$
It has the same size as $icode random_vec$$ and its element values will vary.

$subhead logden_vec$$
This result has prototype
$codei%
	CppAD::vector<%Float%> %logden_vec%
%$$
The corresponding log-density function is
$icode%
	%logden_vec%[0]% - abs(%logden_vec%[1]) - %...% - abs(%logden_vec%[%s%-1])
%$$
where $icode%s% = %logden%.size()%$$.

$head random_out$$
This return value has prototype 
$codei%
	CppAD::vector<double> %random_out%
%$$
It maximizes the density for the random effects, given the data
and the fixed effects; i.e., 
$latex \[
\B{p} ( u | y , \theta )
=
C( \theta ) \B{p} ( y | u , \theta ) \B{p} ( u | \theta )
\] $$
where the normalization constant $latex C( \theta )$$
does not depend on the random effects $latex u$$.

$end
------------------------------------------------------------------------------
*/
# include <cppad/cppad.hpp>
# include <cppad/ipopt/solve.hpp>
# include <dismod_at/optimize_random.hpp>

# define DISMOD_AT_IPOPT_INFINITY 1e19

namespace {
	using CppAD::AD;
	using CppAD::vector;
	typedef vector<double>        d_vector;
	typedef vector< AD<double> >  ad_vector;

	template <class Logden>
	class FG_eval {
	private:
		const size_t     n_random_; 
		const d_vector&  fixed_vec_;
		Logden&          logden_;
	public:
		// constructor
		FG_eval(
			size_t           n_random        ,
			const d_vector&  fixed_vec       ,
			Logden&          logden
		) : 
		n_random_( n_random )     ,
		fixed_vec_( fixed_vec )   ,
		logden_( logden )
		{ }
		// evaluate objective and constraints
		void operator()(ad_vector& fg, const ad_vector& x)
		{
			// extract the random effects from x
			ad_vector u(n_random_);
			for(size_t j = 0; j < n_random_; j++)
				u[j] = x[j];

			// compute log-density vector
			ad_vector logden_vec   = logden_(fixed_vec_, u);

			// initialize smooth part of negative log-likelihood
			size_t k = 0;
			fg[k++]  = - logden_vec[0];

			// terms corresponding to data likelihood absolute values
			size_t n_abs   = logden_vec.size()-1;
			for(size_t j = 0; j < n_abs; j++)
			{	// x[ n_random_ + j] >= abs(log_den[1 + j]
				fg[k++] = x[ n_random_ + j] - logden_vec[1 + j]; 	
				fg[k++] = x[ n_random_ + j] + logden_vec[1 + j]; 	
				//
				// smooth contribution to log-likelihood
				fg[0]  += x[ n_random_ + j];
			}

		}	
	};
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

template <class Logden>
vector<double> optimize_random(
	const vector<double>& fixed_vec       ,
	const vector<double>& random_in       ,
	Logden&               logden          )
{
	// number of random effects
	size_t n_random = random_in.size();

	// determine initial density vector
	d_vector logden_vec = logden(fixed_vec, random_in);
	size_t n_abs = logden_vec.size() - 1;

	// number of independent variable is number of random effects
	// plus number of log-density terms that require absolute values
	size_t nx = n_random + n_abs; 

	// set initial x vector
	d_vector xi(nx);
	for(size_t j = 0; j < n_random; j++)
		xi[j] = random_in[j];
	for(size_t j = 0; j < n_abs; j++)
		xi[n_random + j] = CppAD::abs( logden_vec[j] );

	// set lower and upper limits for x
	d_vector xl(nx), xu(nx);
	for(size_t j = 0; j < nx; j++)
	{	xl[j] = - DISMOD_AT_IPOPT_INFINITY;
		xu[j] = + DISMOD_AT_IPOPT_INFINITY;
	}

	// set limits for abs contraint representation
	d_vector gl(n_abs), gu(n_abs);
	for(size_t j = 0; j < n_abs; j++)
	{	gl[j] = 0.0;
		gu[j] = + DISMOD_AT_IPOPT_INFINITY;
	}

	// construct fg_evlution object
	FG_eval<Logden> fg_eval(n_random, fixed_vec, logden);

	// optimizer options
	std::string options;

	// return solution
	CppAD::ipopt::solve_result<d_vector>& solution;


	// solve the optimization problem
	CppAD::ipopt::solve(
		options, xi, xl, xu, gl, gu, fg_eval, solution
	);

	return solution.x;
}
	


} // END_DISMOD_AT_NAMESPACE
