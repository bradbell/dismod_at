// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
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
$icode%residual% = residual_density(
	%density%, %z%, %mu%, %delta%, %eta%
)%$$

$head density$$
This argument has prototype
$codei%
	density_enum %density%
%$$
It specifies one for the following density value
$code uniform_enum$$,
$cref/gaussian_enum/wres_density/Gaussian/$$,
$cref/laplace_enum/wres_density/Laplace/$$,
$cref/log_gaussian_enum/wres_density/Log-Gaussian/$$,
$cref/log_laplace_enum/wres_density/Log-Laplace/$$.

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a2_double$$.

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
For the Gaussian, and Laplace cases,
it specifies the mean of the distribution.
For the Log-Gaussian and Log-Laplace cases,
it specifies the transformed mean.

$head delta$$
This argument has prototype
$codei%
	const %Float%& delta
%$$
For Gaussian, and Laplace cases,
it specifies the standard deviation of the distribution.
For the Log-Gaussian and Log-Laplace cases,
it specifies the transformed standard deviation.

$head eta$$
Is the offset in the Log-Gaussian and Log-Laplace $icode density$$ cases
(and is not used in the other cases).

$head residual$$
The return value has prototype
$codei%
	residual_struct<%Float%> %residual%
%$$

$head residual_struct$$
This structure has the following fields:
$table
Type $cnext Field $cnext Description $rnext
$icode Float$$ $cnext
	$code wres$$ $cnext
	$cref/weighted residual/wres_density/$$
$rnext
$icode Float$$ $cnext
	$code logden_smooth$$ $cnext
	this smooth term is in $cref/log-density/wres_density/$$
$rnext
$icode Float$$ $cnext
	$code logden_sub_abs$$ $cnext
	absolute value of this smooth term is in log-density
$rnext
$icode density_enum$$ $cnext
	$code density$$ $cnext
	type of density function; see
	$cref/density_enum/get_density_table/density_enum/$$
$tend

$head Uniform$$
In the case where the $icode density$$ is uniform,
$icode wres$$,
$icode logden_smooth$$,
and $icode logden_sub_abs$$ are all set zero.

$head Gaussian$$
In the case where the $icode density$$ is
$cref/Gaussian/wres_density/Gaussian/$$ or
$cref/Log-Gaussian/wres_density/Log-Gaussian/$$,
the log-density is equal to $icode logden_smooth$$ and
$icode logden_sub_abs$$ is zero.

$head Laplace$$
In the case where the $icode density$$ is
$cref/Laplace/wres_density/Laplace/$$ or
$cref/Log-Laplace/wres_density/Log-Laplace/$$ likelihoods,
the log-density is equal to
$codei%
	%logden_smooth% - fabs(%logden_sub_abs)%)
%$$
This enables one to express the log-density
in terms of smooth functions (for optimization purposes).

$end
*/
# include <cppad/cppad.hpp>
# include <dismod_at/residual_density.hpp>
# include <dismod_at/a2_double.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
residual_struct<Float> residual_density(
	density_enum       density ,
	const Float&       z       ,
	const Float&       mu      ,
	const Float&       delta   ,
	const Float&       eta     )
{	Float nan(std::numeric_limits<double>::quiet_NaN());

	Float wres = nan;
	Float sigma = nan;
	switch( density )
	{
		case uniform_enum:
		wres = 0.0;
		break;

		case gaussian_enum:
		case laplace_enum:
		assert( delta > 0.0 );
		sigma = delta;
		wres  = ( z - mu) / sigma;
		break;

		case log_gaussian_enum:
		case log_laplace_enum:
		assert( delta > 0.0 );
		sigma = log( 1.0 + delta / (mu + eta) );
		wres  = ( log( z + eta ) - log( mu + eta ) ) / sigma;
		break;

		default:
		assert(false);
	}
	Float logden_smooth = nan;
	Float logden_sub_abs = nan;
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
	residual_struct<Float> residual;
	residual.wres           = wres;
	residual.logden_smooth  = logden_smooth;
	residual.logden_sub_abs = logden_sub_abs;
	residual.density        = density;
	return residual;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY(Float)        \
	template residual_struct<Float> residual_density(         \
		density_enum       density ,                          \
		const Float&       z       ,                          \
		const Float&       mu      ,                          \
		const Float&       delta   ,                          \
		const Float&       eta                                \
	);

// instantiations
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY(double)
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY( CppAD::AD<double> )
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY( a2_double )

} // END DISMOD_AT_NAMESPACE
