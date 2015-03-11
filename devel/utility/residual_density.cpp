// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin residual_density$$
$spell
	struct
	fabs
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
$code double$$, $code AD<double>$$, $code AD< AD<double> >$$,
where $code AD$$ is $code CppAD::AD$$.

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
	residual_density_struct<%Float%> %wres_logden%
%$$

$head Weighted Residual$$
The weighted residual (see $cref model_density$$) is given by the value
$codei%
	%Float% %wres%  = %wres_logden%.wres
%$$

$head Log-Density$$
The log of the density function (see $cref model_density$$) 
is represented by
$codei%
	%Float% %logden_smooth%  = %wres_logden%.logden_smooth
	%Float% %logden_sub_abs% = %wres_logden%.logden_sub_abs
%$$
The values $icode logden_smooth$$ and $icode logden_sub_abs$$
are infinitely differentiable with
respect to the $cref/model_variables/model_variable/$$; i.e., smooth.

$subhead Uniform$$
In the case where the density is uniform,
both $icode logden_smooth$$ and $icode logden_sub_abs$$ are zero.

$subhead Gaussian$$
In the case where the density is  
$cref/Gaussian/model_density/Gaussian/$$ or
$cref/Log-Gaussian/model_density/Log-Gaussian/$$,
the log-density is equal to $icode logden_smooth$$ and
$icode logden_sub_abs$$ is zero.

$subhead Laplace$$
In the case where the density is  
$cref/Laplace/model_density/Laplace/$$ or
$cref/Log-Laplace/model_density/Log-Laplace/$$ likelihoods,
the log-density is equal to
$codei%
	%logden_smooth% - fabs(%logden_sub_abs)%)
%$$
This enables one to express the log-density
in terms of smooth functions (for optimization purposes).

$end
*/
# include <cppad/cppad.hpp>
# include <dismod_at/include/residual_density.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
residual_density_struct<Float> residual_density(
	density_enum density ,
	const Float& z       , 
	const Float& mu      , 
	const Float& delta   ,
	const Float& eta     )
{ 
	Float wres, sigma;
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
	Float logden_smooth, logden_sub_abs;
	switch( density )
	{
		case uniform_enum:
		logden_smooth  = 0.0;
		logden_sub_abs = 0.0;
		break;

		case gaussian_enum:
		case log_gaussian_enum:
 		{	double pi2 = 8.0 * std::atan(1.0);
			logden_smooth  = - log( sigma * sqrt( pi2 ) ) - wres * wres/ 2.0;
			logden_sub_abs = 0.0;
		}
		break;  

		case laplace_enum:
		case log_laplace_enum:
		{	double r2   = sqrt(2.0);
			logden_smooth  = - log( sigma * r2 );
			logden_sub_abs = r2 * wres;
		}
		break;

		default:
		assert(false);
	}
	//
	residual_density_struct<Float> wres_logden;
	wres_logden.wres           = wres;
	wres_logden.logden_smooth  = logden_smooth;
	wres_logden.logden_sub_abs = logden_sub_abs;
	return wres_logden;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY(Float)        \
	template residual_density_struct<Float> residual_density( \
		density_enum density ,                                \
		const Float& z       ,                                \
		const Float& mu      ,                                \
		const Float& delta   ,                                \
		const Float& eta                                      \
	);

// instantiations
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY(double)
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY( CppAD::AD<double> )
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY( CppAD::AD< CppAD::AD<double> > )

} // END DISMOD_AT_NAMESPACE
