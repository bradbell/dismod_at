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
$begin integrate_1d$$
$spell
	dismod
	const
	std
$$

$section Compute One Dimensional Integration Coefficients$$

$head Syntax$$
$icode%c% = integrate_1d(%r%, %s%, %w%)%$$

$head See Also$$
$cref integrate_2d$$

$head Purpose$$
The function $latex U(r)$$ is linear and defined on a larger
interval that surrounds an interval we are integrating over.
This routine computes the integral of
$latex U(r)$$ times weighting $icode w$$ on the smaller interval.
$pre

$$
The function $latex U(r)$$ is linear and define on the larger interval by
$latex \[
	U(r) = ( r_2 - r_1 )^{-1} [ ( r_2 - r ) u_1 + ( r - r_1 ) u_2 ]
\] $$
The product of $latex U(r)$$ times the weighting is defined on the
smaller interval by
$latex \[
V(r) = ( s_2 - s_1 )^{-1} [
	w_1 U( s_1 ) ( s_2 - r ) + w_2 U( s_2 ) ( r - s_1 )
]
\] $$
where $latex r_1 \leq s_1 < s_2 \leq r_2 $$.
Note that
$latex U( r_1 ) = u_1$$, $latex U( r_2 ) = u_2 $$,
$latex V( s_1 ) = w_1 U( s_1 )$$, and $latex V( r_2 ) = w_2 U( s_2 ) $$.
This routine computes coefficients
$latex c_1, c_2$$ such that
$latex \[
	I  = \int_{s(1)}^{s(2)} V(r) \; \B{d} r = c_1 u_1 + c_2 u_2
\] $$
where the coefficients
$latex c_1, c_2$$ do not depend on the values
$latex u_1, u_2$$.

$head r, s, w$$
For $icode x$$ in the set of arguments
$codei%{ %r%, %s%, %w% }%$$,
the argument has prototype
$codei%
	const std::pair<double, double>& %x%
%$$
with the following identifications;
$latex x_1 = $$ $icode%x%.first%$$ and
$latex x_2 = $$ $icode%x%.second%$$.

$head c$$
The return value has prototype
$codei%
	std::pair<double, double> %c%
%$$
with the following identifications;
$latex c_1 = $$ $icode%c%.first%$$ and
$latex c_2 = $$ $icode%c%.second%$$.

$children%example/devel/utility/integrate_1d_xam.cpp
%$$
$head Example$$
The file $cref integrate_1d_xam.cpp$$ contains an example and test
of using this routine.

$head Method$$
Using the notation $latex d = s_2 - s_1$$,
the integral is given by
$latex \[
I = \int_0^d V(r + s_1) \; \B{d} r
\] $$
Using the notation
$latex v_1 = w_1 U( s_1 )$$ and
$latex v_2 = w_2 U( s_2 )$$,
$latex \[
I
=  d^{-1} \int_0^d [ v_1 (d - r) + v_2 d ] \; \B{d} r
= d ( v_1 + v_2 ) / 2
\] $$
Using the definition for $latex v_1$$ and $latex v_2$$ we have
$latex \[
	I = (d / 2) [  w_1 U( s_1 ) + w_2 U( s_2 ) ]
\] $$
Using the definition for $latex U(r)$$ we obtain
We define
$latex \[
I = \frac{1}{2} \frac{s_2 - s_1}{r_2 - r_1} [
	w_1 ( r_2 - s_1 ) u_1 + w_1 ( s_1 - r_1 ) u_2 +
	w_2 ( r_2 - s_2 ) u_1 + w_2 ( s_2 - r_1 ) u_2
]
\] $$
Using the definitions
$latex \[
c_1 = \frac{1}{2} \frac{s_2 - s_1}{r_2 - r_1}
	[ w_1 ( r_2 - s_1 ) + w_2 ( r_2 - s_2 ) ]
\] $$
$latex \[
c_2 = \frac{1}{2} \frac{s_2 - s_1}{r_2 - r_1}
	[ w_1 ( s_1 - r_1 ) + w_2 ( s_2 - r_1 ) ]
\] $$
We obtain the desired conclusion
$latex \[
I = c_1 u_1 + c_2 u_2
\] $$
$end
------------------------------------------------------------------------------
*/
# include <utility>
# include <cassert>
# include <dismod_at/integrate_1d.hpp>

namespace dismod_at { // BEGIN DIMSOD_AT_NAMESPACE

std::pair<double,double> integrate_1d(
	const std::pair<double,double>&  r ,
	const std::pair<double,double>&  s ,
	const std::pair<double,double>&  w )
{
	double r1 = r.first;
	double r2 = r.second;
	double s1 = s.first;
	double s2 = s.second;
	double w1 = w.first;
	double w2 = w.second;

	assert( r1 <= s1 && s1 < s2 && s2 <= r2 );

	double q = (s2 - s1) / ( 2.0 * (r2 - r1) );

	std::pair<double,double> c;
	c.first  = q * ( w1 * (r2 - s1) + w2 * (r2 - s2) );
	c.second = q * ( w1 * (s1 - r1) + w2 * (s2 - r1) );


	return c;
}

} // END DISMOD_AT_NAMESPACE
