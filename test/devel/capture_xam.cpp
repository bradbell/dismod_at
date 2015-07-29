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
$begin capture_xam.cpp$$
$spell
	Biometrics
	Royle
	Resample
	Poisson
	covariate
$$

$section A Capture Re-capture Model$$

$head Under Construction$$
This example is under construction and has not yet been fully implemented.

$head Notation$$
$table
$latex T$$     $cnext number of sampling times
$rnext
$latex I$$     $cnext number of sampling locations
$rnext
$latex K$$     $cnext practical bound on population size (used to truncate sum)
$rnext
$latex N_i$$   $cnext size of the population at $th i$$ location
$rnext
$latex y_{i,t}$$ $cnext
	number of captures at location $latex i$$ and time $latex t$$
	($latex y_{i,t} < K$$)
$rnext
$latex M_i$$   $cnext
	maximum of the measured number of captures at $th i$$ location
$rnext
$latex p_{i,t}$$ $cnext
	probability of capture at location $latex i$$ and time $latex t$$
$rnext
$latex u_t$$   $cnext random effect for each sampling time
$rnext
$latex x_i$$   $cnext covariate value for each location
$rnext
$latex \theta_0$$ $cnext constant multiplier in covariate relation
$rnext
$latex \theta_1$$ $cnext linear multiplier in covariate relation
$rnext
$latex \theta_2$$ $cnext
	log of mean for $latex N_i$$ given $latex \theta$$
$rnext
$latex \theta_3$$ $cnext
	log of the variance of the random effects $latex u_t$$
$tend

$head Count Data$$
We use a binomial distribution to model the
probability of $latex y_{i,t}$$ given $latex N_i$$ and $latex p_{i,t}$$; i.e,
$latex \[
\B{P} ( y_{i,t} | N_i , p_{i,t} )
=
\left( \begin{array}{c} N_i \\ y_{i,t} \end{array} \right)
p_{i,t}^{y(i,t)} \left( 1 - p_{i,t}^{y(i,t)} \right)
\] $$
Furthermore, we assume that this probability
is independent for each $latex (i, t)$$.

$head Capture Probability$$
Section 2.4 of the reference below suggest a covariate
model for the probability of capture.
We add random effects to this model as follows:
$latex \[
	\R{logit} ( p_{i,t} ) = - u_t - \theta_0 - \theta_1 x_i
\] $$
It follows that
$latex \[
	p_{i,t} = [ 1 + \exp(  u_t + \theta_0 + \theta_1 x_i ) ]^{-1}
\] $$
Note that the covariate vector $latex x$$ is assumed to be known.

$head Population Probability$$
We use a Poisson distribution to model the
probability of $latex N_i$$ given $latex \theta_2$$; i.e.,
$latex \[
\B{P} [ N_i | \exp( \theta_2 ) ]
=
\exp( \theta_2 )^{N(i)} \frac{ \exp[ - \exp( \theta_2 ) ] }{ N_i ! }
\] $$
We assume that this probability
is independent for each $latex i$$.
Furthermore, we assume that for $latex \lambda = \exp( \theta_2 ) $$,
$latex \[
	\exp( \lambda )
	\approx
	\sum_{k = 0}^{K-1} \frac{ \lambda^k }{ k ! }
\] $$

$head p(u | theta)$$
We use a normal distribution, with mean zero and variance
$latex \exp( \theta_3 )$$,
for the distribution of the random effects $latex u$$
given the fixed effects $latex \theta$$; i.e.,
$latex \[
\B{p} ( u | \theta )
=
\prod_{t=0}^{T-1}
	\frac{1}{ \sqrt{ \exp( \theta_3 ) 2 \pi } }
		\exp \left[ - \frac{1}{2} \frac{ u_t^2 }{ \exp( \theta_3 ) } \right]
\] $$

$head p(y | theta , u)$$
We define the vector of maximum measurement for each location by
$latex \[
	M_i = \max \left\{ y_{i,0} , \cdots , y_{i, T-1} \right\}
\] $$
We define a probability of capture function for each location and time by
$latex \[
p_{i,t} ( \theta , u ) =  [ 1 + \exp( u_t + \theta_0 + \theta_1 x_i ) ]^{-1}
\] $$
The joint probability of $latex N_i$$, and $latex y_{i, t}$$ given
$latex \theta$$, and $latex u$$ is
$latex \[
J_{i,t} ( N_i, y_{i,t} , \theta , u )
=
\exp( \theta_2 )^{N(i)} \frac{ \exp[ - \exp( \theta_2 ) ] }{ {N(i)} ! }
\left( \begin{array}{c} {N(i)} \\ y_{i,t} \end{array} \right)
	p_{i,t} ( \theta , u)^{y(i,t)}
	\left( 1 - p_{i,t}( \theta , u)^{y(i,t)} \right)
\] $$
We do not know the population at each location $latex N_i$$
so we define the population summed probability
function for each location by
$latex \[
S_i ( y, \theta , u )
=
\sum_{k=M(i)}^{K-1} \prod_{t=0}^{T-1} J_{i,t} ( k , y_{i,t}, \theta , u )
\] $$
Our model for the density of the data,
that depends on the fixed and random effects, is
$latex \[
\B{p}( y | \theta , u )
=
\prod_{i=0}^{I-1} S_i ( y, \theta , u )
\] $$

$head p(theta)$$
$index no prior$$
$index fix_like, none$$
For this example there is no prior $latex \B{p}(\theta)$$.

$head Reference$$
J. Andrew Royle,
Biometrics 60, 108-115 March 2004,
$italic
N-Mixture Models for Estimating Population Size
from Spatially Replicated Counts.
$$

$end
-----------------------------------------------------------------------------
*/

# include <gsl/gsl_randist.h>
# include <cppad/vector.hpp>
# include <dismod_at/approx_mixed.hpp>
# include <dismod_at/manage_gsl_rng.hpp>

namespace { // BEGIN_EMPTY_NAMESPACE

using CppAD::vector;
using std::exp;
using std::log;

// simulate covariates, x, and data, y
void simulate_xy(
	size_t                 I     ,
	size_t                 T     ,
	const vector<double>&  theta ,
	vector<double>&        x     ,
	vector<size_t>&        y     )
{	assert( theta.size() == 4 );
	assert( x.size() == I );
	assert( y.size() == I * T );
	// random number generator
	gsl_rng* rng = dismod_at::get_gsl_rng();
	//
	// simulate population sizes
	vector<double> N(I);
	double mu =  exp( theta[2] );
	for(size_t i = 0; i < I; i++)
		N[i] = gsl_ran_poisson(rng, mu );
	//
	// simulate random effects
	vector<double> u(T);
	double sigma = std::sqrt( exp( theta[3] ) );
	for(size_t t = 0; t < T; t++)
		u[t] = gsl_ran_gaussian(rng, sigma);
	//
	// simulate covariate values
	for(size_t i = 0; i < I; i++)
		x[i] = gsl_ran_gaussian(rng, sigma);
	//
	// simulate data
	for(size_t i = 0; i < I; i++)
	{	for(size_t t = 0; t < T; t++)
		{	// probability of capture
			double ex = exp( u[t] + theta[0] + theta[1] * x[i] );
			double p = 1.0 /( 1.0  + ex );
			y[ i * T + t ] = gsl_ran_binomial(rng, p, N[i]);
		}
	}
	//
	return;
}

// approx_mixed derived class
class approx_derived : public dismod_at::approx_mixed {
private:
	const size_t          K_; // practical bound on population size
	const size_t          I_; // number of locations
	const size_t          T_; // number of times
	const vector<double>& x_; // referece to covariate values
	const vector<size_t>& y_; // reference to data values
	// -----------------------------------------------------------------
	// set by constructor and then effectively const
	vector<size_t>        M_;      // max number of captures at each location
	vector<double>        logfac_; // logfac_[k] = log( k! )
// ------------------------------------------------------------------------
public:
	// constructor
	approx_derived(
		size_t                 K     ,
		size_t                 I     ,
		size_t                 T     ,
		vector<double>&        x     ,
		vector<size_t>&        y     )
		:
		dismod_at::approx_mixed(4, T) , // n_fixed = 4, n_random = T
		K_(K)            ,
		I_(I)            ,
		T_(T)            ,
		x_(x)            ,
		y_(y)
	{	// set M_
		M_.resize(I);
		for(size_t i = 0; i < I; i++)
		{	M_[i] = 0;
			for(size_t t = 0; t < T; t++)
				M_[i] = std::max( M_[i], y[ i * T + t] );
			assert( M_[i] < K );
		}
		assert( K >= 2 );
		logfac_.resize(K);
		logfac_[0]  = 0.0;
		logfac_[1]  = 0.0;
		for(size_t k = 2; k < K; k++)
			logfac_[k] = log( double(k) ) + logfac_[k-1];
	}
	// implementaion of ran_like
	template <class Float>
	vector<Float> implement_ran_like(
		const vector<Float>&  theta  ,
		const vector<Float>&  u      )
	{	vector<Float> vec(1);
		Float pi2( 8.0 * std::atan(1.0) );
		Float one( 1.0 );
		Float two( 2.0 );
		Float eps = Float( 10.0 * std::numeric_limits<double>::epsilon() );
		//  ------------------------------------------------------------
		// log [ p(u | theta) ]
		//  ------------------------------------------------------------
		vec[0] = Float(0.0);
		Float sigsq = exp( theta[3] );
		for(size_t t = 0; t < T_; t++)
		{	vec[0] -= log( pi2 * sigsq ) / two;
			vec[0] -= u[t] * u[t] / (two * sigsq);
		}
		//  ------------------------------------------------------------
		// log [ p(y | theta, u) ]
		//  ------------------------------------------------------------
		// log of k !
		//
		// p_{i,t}
		vector<Float> p(I_ * T_);
		for(size_t i = 0; i < I_; i++)
		{	for(size_t t = 0; t < T_; t++)
			{	Float ex     = exp( u[t] + theta[0] + theta[1] * x_[i] );
				p[i * T_ + t] = Float(1.0) / (Float(1.0) + ex );
			}
		}
		//
		// loop over locations
		for(size_t i = 0; i < I_; i++)
		{	// initialize sum that defines S_i
			Float Si = Float(0.0);
			for(size_t k = M_[i]; k < K_; k++)
			{	// compute log( J_{it} )
				//
				// terms that do not depend on i, t
				Float sumlog = Float(T_) * theta[2] * Float(k);
				sumlog      -= Float(T_) * exp( theta[2] );
				//
				// now compute terms that depend on t
				for(size_t t = 0; t < T_; t++)
				{	size_t yit = y_[ i * T_ + t ];
					Float  pit = p[ i * T_ + t ];
					//
					// log [ (k choose yit) / k ! ]
					Float term_1 = - Float( logfac_[yit] + logfac_[k - yit] );
					// log [ pit^yit ]
					Float term_2 = Float( yit ) * log(pit + eps);
					// log [ 1 - pit^yit ]
					Float term_3 = Float(k - yit) * log(one + eps - pit);
					//
					sumlog       += term_1 + term_2 + term_3;
				}
				Si += exp( sumlog );
			}
			vec[0] += log( Si );
		}
		vec[0] = - vec[0];
		return vec;
	}
// ------------------------------------------------------------------------
public:
	virtual vector<a5_double> ran_like(
		const vector<a5_double>& fixed_vec  ,
		const vector<a5_double>& random_vec )
	{	return implement_ran_like(fixed_vec, random_vec); }
	//
	virtual vector<a1_double> fix_like(
		const vector<a1_double>& fixed_vec  )
	{	return vector<a1_double>(0); } // empty vector
	//
	virtual vector<a1_double> constraint(
		const vector<a1_double>& fixed_vec  )
	{	return vector<a1_double>(0); } // empty vector
	//
	virtual void fatal_error(const std::string& error_message)
	{	std::cerr << "Error: " << error_message << std::endl;
		std::exit(1);
	}
	//
	virtual void warning(const std::string& warning_message)
	{	std::cerr << "Warning: " << warning_message << std::endl;
	}
};
} // END_EMPTY_NAMESPACE

bool capture_xam(void)
{	bool ok = true;
	size_t n_fixed = 4;
	size_t random_seed = dismod_at::new_gsl_rng(0);

	// simulation parameters
	size_t I = 50;
	size_t T = 1;
	vector<double> theta_sim(n_fixed);
	theta_sim[0] =   1.00;      // constant term in covariate model
	theta_sim[1] =   0.00;      // linear term in covariate model
	theta_sim[2] =   log(5.0);  // log of mean population size
	theta_sim[3] =   log(0.10); // log of variance of random effects

	// set x, y
	vector<double> x(I);
	vector<size_t> y(I * T);
	simulate_xy(I, T, theta_sim, x, y);

	// practical bound for population size is 5 times mean
	double lambda = exp( theta_sim[2] );
	double sigma  = std::sqrt( lambda );
	size_t K      = size_t ( lambda + 2.0 * sigma );
	assert( K >= 2 );

	// create derived object
	approx_derived approx_object(K, I, T, x, y);

	// initialize point to start optimization at
	vector<double> theta_in( n_fixed ), u_in(T);
	for(size_t j = 0; j < n_fixed; j++)
		theta_in[j] = theta_sim[j];
	for(size_t t = 0; t < T; t++)
		u_in[t] = 0.0;
	approx_object.initialize(theta_in, u_in);

	// lower and upper limits
	vector<double> constraint_lower, constraint_upper;
	vector<double> theta_lower(n_fixed), theta_upper(n_fixed);
	for(size_t j = 0; j < n_fixed; j++)
	{	theta_lower[j] = theta_sim[j];
		theta_upper[j] = theta_sim[j];
	}
	// limit on constant term
	theta_lower[0] = -2.0;
	theta_upper[0] =  2.0;

	// optimize the fixed effects
	std::string options =
		"Integer print_level               0\n"
		"String  sb                        yes\n"
		"String  derivative_test           second-order\n"
		"String  derivative_test_print_all yes\n"
		"Numeric tol                       1e-8\n"
	;
	vector<double> theta_out = approx_object.optimize_fixed(
		options,
		theta_lower,
		theta_upper,
		constraint_lower,
		constraint_upper,
		theta_in,
		u_in
	);
	for(size_t j = 0; j < n_fixed; j++)
		std::cout << j << ", " << theta_sim[j] << " , " << theta_out[j] << std::endl;
	//
	if( ! ok )
		std::cout << "capture_xam:: random_seed = " << random_seed << std::endl;
	dismod_at::free_gsl_rng();
	return ok;
}
