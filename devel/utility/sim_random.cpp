// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin sim_random$$
$spell
	Dismod
	sim
	mu
	enum
	gsl_rng
$$

$section Simulate a Dismod_at Random Distribution$$

$head Syntax$$
$icode%z% = sim_random(%density%, %mu%, %delta%, %eta%, %nu%)%$$

$head manage_gsl_rng$$
The routine $cref manage_gsl_rng$$ sets up and controls the underlying
simulated random number generator.

$head density$$
This argument has prototype
$codei%
	density_enum %density%
%$$
It specifies the $cref/density/get_density_table/density_enum/$$
for the distribution that we are simulating.
The following table specifies the meaning of this choice:
$table
$code uniform_enum$$ $pre $$ $cnext
	not allowed $rnext
$code gaussian_enum$$ $pre  $$ $cnext
	$cref/Gaussian/statistic/Log-Density Function, D/Gaussian/$$ $rnext
$code laplace_enum$$ $pre  $$ $cnext
	$cref/Laplace/statistic/Log-Density Function, D/Laplace/$$ $rnext
$code students_enum$$ $pre  $$ $cnext
	$cref/Student's-t/statistic/Log-Density Function, D/Student's-t/$$ $rnext
$code log_gaussian_enum$$ $pre  $$ $cnext
	$cref/Log-Gaussian/statistic/Log-Density Function, D/Log-Gaussian/$$ $rnext
$code log_laplace_enum$$ $pre  $$ $cnext
	$cref/Log-Laplace/statistic/Log-Density Function, D/Log-Laplace/$$ $rnext
$code log_students_enum$$ $pre  $$ $cnext
	$cref/Log-Student's-t/statistic/Log-Density Function, D/Log-Student's-t/$$
$tend

$head mu$$
This argument has prototype
$codei%
	double %mu%
%$$
In the case were $icode density$$ is
$code gaussian_enum$$, $code laplace_enum$$ or $code students_enum$$,
it is the mean for the distribution that we are simulating.
Otherwise $codei%log( %mu% + %eta% )%$$ is the mean of the
log-transformed data.

$head delta$$
This argument has prototype
$codei%
	double %delta%
%$$
In the case were $icode density$$ is
$code gaussian_enum$$ or $code laplace_enum$$,
it is the standard deviation for the distribution that we are simulating.
Otherwise,
$codei%
	log( %mu% + %eta% + %delta% ) - log( %mu% + %eta% )
%$$
it is the standard deviation of
$codei%
	log( %z% + %eta% ) - log( %mu% + %eta% )
%$$
It is assumed $icode delta$$ is greater than zero and not infinity.

$head eta$$
This argument has prototype
$codei%
	double %eta%
%$$
In the case were $icode density$$ is
$code log_gaussian_enum$$, $code log_laplace_enum$$
or $code log_students_enum$$,
this is the offset in the log transformation for the distribution.
In this case, it is assumed that $icode%mu% + %eta% > 0%$$.
Otherwise, $icode eta$$ is not used.

$head nu$$
This argument has prototype
$codei%
	double %nu%
%$$
In the case were $icode density$$ is
$code students_enum$$ or $code log_students_enum$$, it is
the degrees of freedom in the Student's-t distribution.
Otherwise it is not used.

$head z$$
The return value has prototype
$codei%
	double %z%
%$$
It simulates a sample from the specified distribution that is independent
for the any previous return values.

$children%example/devel/utility/sim_random_xam.cpp
%$$
$head Example$$
The file $cref sim_random_xam.cpp$$ is an example and test of this simulation.
------------------------------------------------------------------------------
$end
*/

# include <cmath>
# include <gsl/gsl_randist.h>
# include <dismod_at/sim_random.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

double sim_random(
	density_enum density, double mu, double delta, double eta, double nu )
{	gsl_rng* rng = CppAD::mixed::get_gsl_rng();
	//
	assert( density != uniform_enum );
	assert( delta > 0.0 );
	//
	if( density == gaussian_enum )
		return mu + gsl_ran_gaussian(rng, delta);
	//
	if( density == laplace_enum )
	{	double width = delta / std::sqrt(2.0);
		return mu + gsl_ran_laplace(rng, width);
	}
	if( density == students_enum )
	{	assert( nu > 2.0 );
		double x = gsl_ran_tdist(rng, nu);
		return  mu +  x * std::sqrt( (nu - 2.0) / nu ) * delta;
	}
	//
	assert( mu + eta > 0.0 );
	//
	// standard deviation in transformed space
	double sigma = std::log(mu + eta + delta) - std::log(mu + eta);
	//
	// difference from mean in transformed space
	double d_log;
	if( density == log_gaussian_enum )
		d_log = gsl_ran_gaussian(rng, sigma);
	else if( density == log_laplace_enum )
	{	assert( density == log_laplace_enum );
		double width = sigma / std::sqrt(2.0);
		d_log = gsl_ran_laplace(rng, width);
	}
	else
	{	assert( density == log_students_enum );
		assert( nu > 2.0 );
		double x = gsl_ran_tdist(rng, nu);
		d_log = x * std::sqrt( (nu - 2.0) / nu ) * sigma;
	}
	//
	// d_log = log(z + eta) - log(mu + eta)
	double z = std::exp( d_log ) * (mu + eta) - eta;
	//
	return z;
}

} // END_DISMOD_AT_NAMESPACE
