// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin residual_density$$
$spell
	wres
	logden
	enum
	CppAD
	std
	mu
	const
$$

$section Compute the Weighted Residual and Log-Density$$

$head Syntax$$
$icode%wres_logden% = residual_density(%density%, %z%, %mu%, %delta%, %eta%)%$$

$head density$$
This argument has prototype
$codei%
	density_enum %density%
%$$
It specifies one for the following density value
$code uniform_enum$$,
$cref/gaussian_enum/model_density/Gaussian/$$,
$cref/laplace_enum/model_density/Laplace/$$,
$cref/log_gaussian_enum/model_density/Log-Gaussian/$$,
$cref/log_laplace_enum/model_density/Log-Laplace/$$.

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code CppD::AD<double>$$.

$head z$$
This argument has prototype
$codei%
	const %Float%& z
%$$
It specifies the random variable value.

$head mu$$
This argument has prototype
$codei%
	const %Float%& mu
%$$
For the uniform, Gaussian, and Laplace cases,
it specifies the mean of the distribution.
For the Log-Gaussian and Log-Laplace cases,
it specifies the transformed mean.

$head delta$$
This argument has prototype
$codei%
	const %Float%& delta
%$$
For the uniform, Gaussian, and Laplace cases,
it specifies the mean of the standard deviation of the distribution.
For the Log-Gaussian and Log-Laplace cases,
it specifies the transformed standard deviation.

$head eta$$
Is the offset in the Log-Gaussian and Log-Laplace $icode density$$ cases
(and is not used in the other cases).

$head wres_logden$$
The return value has prototype
$codei%
	std::pair<%Float%, %Float%> %wres_logden%
%$$
with the following identifications;
$codei%
	%wres%   = %wres_logden%.first
	%logden% = %wres_logden%.second
%$$
The value $icode wres$$ is the weighted residual
and $icode logden$$ is the log of the density
(see $cref model_density$$).
In the special case where the
$icode density$$ is $code uniform$$,
$icode logden$$ is zero; i.e., 
the normalizing constant is not included in the log-density.


$end
*/
# include <cppad/cppad.hpp>
# include <dismod_at/include/residual_density.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
std::pair<Float, Float> residual_density(
	density_enum density ,
	const Float& z       , 
	const Float& mu      , 
	const Float& delta   ,
	const Float& eta     )
{ 
	Float wres;
	Float sigma;
	switch( density )
	{
		case uniform_enum:
		case gaussian_enum:
		case laplace_enum:
		sigma = delta;
		wres  = ( z - mu) / sigma;
		break;

		case log_gaussian_enum:
		case log_laplace_enum:
		sigma = log( 1.0 + delta / (mu + eta) ); 
		wres  = ( log( z + eta ) - log( mu + eta ) ) / sigma;
		break;

		default:
		assert(false);
	}
	Float logden;
	switch( density )
	{
		case uniform_enum:
		logden = 0.0;
		break;

		case gaussian_enum:
		case log_gaussian_enum:
 		{	double pi2 = 8.0 * std::atan(1.0);
			logden     = - log( sigma * sqrt( pi2 ) ) - wres * wres/ 2.0;
		}
		break;  

		case laplace_enum:
		case log_laplace_enum:
		{	double r2   = sqrt(2.0);
			logden      = - log( sigma * r2 ) - r2 * fabs( wres );
		}
		break;

		default:
		assert(false);
	}
	//
	return std::pair<Float, Float> (wres, logden);
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY(Float) \
	template std::pair<Float, Float> residual_density( \
		density_enum density ,                         \
		const Float& z       ,                         \
		const Float& mu      ,                         \
		const Float& delta   ,                         \
		const Float& eta                               \
	);

// instantiations
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY(double)
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY( CppAD::AD<double> )

} // END DISMOD_AT_NAMESPACE
