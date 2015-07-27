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
$begin capture.cpp$$
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
$latex N_i$$   $cnext size of the population at the $th i$$ location
$rnext
$latex y_{i,t}$$ $cnext
	number of individuals captured at location $latex i$$ and time $latex t$$
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

$head Data Probability$$
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
	\R{logit} ( p_{i,t} ) = u_t + \theta_0 + \theta_1 x_i
\] $$
It follows that
$latex \[
	p_{i,t} = [ 1 + \exp( - u_t - \theta_0 - \theta_1 x_i ) ]^{-1}
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
Furthermore, we assume that this probability
is independent for each $latex i$$.

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
p_{i,t} ( \theta , u ) =  [ 1 + \exp( - u_t - \theta_0 - \theta_1 x_i ) ]^{-1}
\] $$
We do not know the population at each location,
so we define the population summed probability
function for each location and time by
$latex \[
S_{i,t} ( y, \theta , u )
=
\sum_{k=M(i)}^{+\infty}
\exp( \theta_2 )^k \frac{ \exp[ - \exp( \theta_2 ) ] }{ k ! }
\left( \begin{array}{c} k \\ y_{i,t} \end{array} \right)
	p_{i,t} ( \theta , u)^{y(i,t)}
	\left( 1 - p_{i,t}( \theta , u)^{y(i,t)} \right)
\] $$
Our model for the density of the data,
that depends on the fixed and random effects, is
$latex \[
\B{p}( y | \theta , u )
=
\sum_{i=0}^{I-1} \sum_{t=0}^{T-1} S_{i,t} ( y, \theta , u )
\] $$

$head p(theta)$$
$index no prior$$
$index prior_like, none$$
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
namespace {
	using CppAD::vector;

	void simulate_xy(
		size_t                 I     ,
		size_t                 T     ,
		const vector<double>&  theta ,
		vector<double>&        x     ,
		vector<double>&        y     )
	{	assert( theta.size() == 4 );
		assert( x.size() == I );
		assert( y.size() == I * T );
		// random number generator
		gsl_rng* rng = get_gsl_rng();
		//
		// simulate population sizes
		vector<double> N(I);
		double mu =  std::exp( theta[2] );
		for(size_t i = 0; i < I; i++)
			N[i] = gsl_ran_poisson(gsl_rng, mu );
		//
		// simulate random effects
		vector<double> u(T);
		double sigma = std::sqrt( std::exp( theta[3] ) );
		for(size_t t = 0; t < T; t++)
			u[t] = gsl_ran_gaussian(gsl_rng, sigma);
		//
		// simulate covariate values
		for(size_t i = 0; i < I; i++)
			x[i] = gsl_ran_gaussian(gsl_rng, sigma);
		//
		// simulate data
		for(size_t i = 0; i < I; i++)
		{	for(size_t t = 0; t < T; t++)
			{	// probability of capture
				double ex = std::exp( - u[t] - theta[0] - theta[1] * x[i] );
				double p = 1.0 /( 1.0  + ex );
				y[ i * T + t ] = gsl_ran_binomial(gsl_rng, p, N[i]);
			}
		}
		//
		return;
	}

}
bool capture(void)
{	bool ok = true;

	// simulation parameters
	size_t I = 10;
	size_t T = 10;
	const vector<double> theta(4);
	theta[0] =   0.75; // constant term in covariate model
	theta[1] =   1.00; // linear term in covariate model
	theta[2] =   2.50; // log of mean population size
	theta[3] = - 1.00; // log of variance of random effects

	// set x, y
	vector<double> x(I), y(I * T);
	simulate_xy(I, T, theta, x, y);

	return ok;
}
