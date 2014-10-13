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
$begin devel_density$$

$section Computing the Density Functions$$

$head Syntax$$
$icode%g% = log_gaussian(%mu%, %sigma%, %z%)
%$$
$icode%v% = log_laplace(%mu%, %sigma%, %z%)
%$$

$head Float$$
The type $icode Float$$ must be one of hte following:
$code double$$, $code CppAD::AD<double>$$.

$head mu$$
This argument has prototype
$codei%
	const %Float%& %mu%
%$$
and is the mean for the density function.

$head sigma$$
This argument has prototype
$codei%
	const %Float%& %sigma%
%$$
and is the standard deviation for the density function.

$head z$$
This argument has prototype
$codei%
	const %Float%& %z%
%$$
and is the point at which we are computing the density function.

$head r$$
We use $icode r$$ below to denote the value
$codei%
	%r% = (%z% - %mu%) / %sigma%
%$$

$head pi$$
We use $icode pi$$ below to denote the ratio of a circle's circumfenrence
divide by its diameter.

$head log_gaussian$$
The return value has prototype
$codei%
	%Float% %g%
%$$
and is the log of the corresponding 
$cref/Gaussian/model_density/Gaussian/Log/$$ density function; i.e.,
$latex \[
g = - \log \left( \sigma \sqrt{2 \pi} \right)  - \frac{1}{2} r^2
\] $$

$head log_laplace$$
The return value has prototype
$codei%
	std::pair<%Float%, %Float%> %p%
%$$
If we define the value
$codei%
	%q% = - %p%.first - std::max( %p%.second , - %p%.second )
%$$
The log of the corresponding
$cref/Laplace/model_density/Laplace/Log/$$ density function is
$latex \[
q 
=
- \log \left( \sigma \sqrt{2} \right) 
- \max \left( \sqrt{2} r , - \sqrt{2} r \right)
\] $$
Note that the functions corresponding to 
$icode%p%.first%$$ and $icode%p%.second%$$
are infinitely differentiable w.r.t. the arguments
$icode mu$$, $icode sigma$$, and $icode z$$.

$head Example$$$
This is just a plan for the density fucntions.
We are holding off on an example until these functions get used.

$end
*/

# include <dismod_at/include/density.hpp>
# include <cppad/cppad.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
Float log_gaussian(const Float& mu, const Float& sigma, const Float& z)
{	// http://en.wikipedia.org/wiki/Pi#Approximate_value
	Float pi = 3.14159265358979323846264338327950288419716939937510;
	Float r  = ( z - mu ) / sigma;
	Float g  = - log( sigma * sqrt(2.0 * pi) ) - r * r / 2.0;
	return g;
}

template <class Float>
std::pair<Float, Float> log_laplace(
	const Float& mu    , 
	const Float& sigma ,
	const Float& z     )
{	std::pair<Float, Float> p;
	Float r  = ( z - mu ) / sigma;
	p.first  = log( sigma * sqrt(2.0) );
	p.second = sqrt(2.0) * r;
	return p;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_DENSITY(Float)             \
	template Float log_gaussian<Float>(                   \
		const Float&  mu       ,                          \
		const Float&  sigma    ,                          \
		const Float&  z                                   \
	);                                                    \
	template std::pair<Float, Float> log_laplace<Float>(  \
		const Float&  mu       ,                          \
		const Float&  sigma    ,                          \
		const Float&  z                                   \
	);

// instantiations
DISMOD_AT_INSTANTIATE_DENSITY(double)
DISMOD_AT_INSTANTIATE_DENSITY( CppAD::AD<double> )

} // END DISMOD_AT_NAMESPACE
