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
$begin integrate_2d$$
$spell
	dismod
	const
	std
	CppAD
$$

$section Compute Two Dimensional Integration Coefficients$$

$head Syntax$$
$icode%c% = dismod_at::integrate_2d(%a%, %t%, %b%, %s%, %w%)%$$

$head Purpose$$
The function $latex U(a,t)$$ is bilinear and defined on a larger rectangle
that surrounds a rectangle we are integrating over.
This routine computes the integral of
$latex U(a, t)$$ times a weighting $icode w$$ on the smaller rectangle.
$pre

$$
The function $latex U(a, t)$$ is defined on the larger rectangle by
$latex \[
U(a, t) = 
	u_{11} \frac{ ( a_2 - a )( t_2 - t ) }{( a_2 - a_1 )( t_2 - t_1 )} +
	u_{21} \frac{ ( a - a_1 )( t_2 - t ) }{( a_2 - a_1 )( t_2 - t_1 )} +
	u_{12} \frac{ ( a_2 - a )( t - t_1 ) }{( a_2 - a_1 )( t_2 - t_1 )} +
	u_{22} \frac{ ( a - a_1 )( t - t_1 ) }{( a_2 - a_1 )( t_2 - t_1 )}
\] $$
The product of $latex U(a, t)$$ times the weighting is defined
on the smaller rectangle by
$latex \[
\begin{array}{rcl}
V(a, t) 
& = & 
( a_2 - a_1 )^{-1} ( t_2 - t_1 )^{-1} \left[ 
	w_{11} U( b_1 , s_1 )( b_2 - a )( s_2 - t )
	+
	w_{21} U( b_2 , s_1 )( a - b_1 )( s_2 - t )
\right]
\\ & + &
( a_2 - a_1 )^{-1} ( t_2 - t_1 )^{-1} \left[ 
	w_{12} U( b_1, s_2 )( b_2 - a )( t - s_1 )
	+
	w_{22} U( b_2, s_2 )( a - b_1 )( t - s_1 )
\right]
\end{array}
\] $$
where 
$latex a_1 \leq b_1 < b_2 \leq a_2$$ and
$latex t_1 \leq s_1 < s_2 \leq t_2$$.
Note that for $latex i = 1,2$$, $latex j = 1,2$$ we have
$latex U( a_i , t_j ) = u_{ij}$$
and 
$latex V( b_i , s_j ) = w_{ij} U ( b_i , s_j )$$.
This routine computes coefficients
$latex c_{11}, c_{21}, c_{12}, c_{22}$$ such that
$latex \[
	I
	=
	\int_{s(1)}^{s(2)} \int_{a(1)}^{a(2)} V(a, t) \; \B{d} a \; \B{d} t
	=
	c_{11} u_{11} + c_{21} u_{21} + c_{12} u_{12} + c_{22} u_{22}
\] $$
where the coefficients
$latex c_{11}, c_{21}, c_{12}, c_{22}$$ do not depend on the values
$latex u_{11}, u_{21}, u_{12}, u_{22}$$. 


$head a, t, b, s$$
For $icode x$$ in the set of arguments
$codei%{ %a%, %t%, %b%, %s% }%$$,
the argument has prototype
$codei%
	const std::pair<double, double>& %x%
%$$
with the following identifications;
$latex x_1 = $$ $icode%x%.first%$$ and 
$latex x_2 = $$ $icode%x%.second%$$.
 
$head w$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %w%
%$$
with size four and the following identifications:
$latex w_{11} = $$ $icode%w%[0]%$$,
$latex w_{21} = $$ $icode%w%[1]%$$,
$latex w_{12} = $$ $icode%w%[2]%$$, and
$latex w_{22} = $$ $icode%w%[3]%$$.
 
$head c$$
The return value has prototype
$codei%
	CppAD::vector<double>& %c%
%$$
with size four and the following identifications:
$latex c_{11} = $$ $icode%c%[0]%$$,
$latex c_{21} = $$ $icode%c%[1]%$$,
$latex c_{12} = $$ $icode%c%[2]%$$, and
$latex c_{22} = $$ $icode%c%[3]%$$.

$children%example/devel/model/integrate_2d_xam.cpp
%$$
$head Example$$
The file $cref integrate_2d_xam.cpp$$ contains an example and test
of using this routine.

$head Method$$
Using the notation 
$latex d = b_2 - b_1$$, and $latex e = s_2 - s_1$$,
$latex \[
I  = \int_0^e \int_0^d V( a + b1 , t + s_1 ) \; \B{d} a \; \B{d} t
\] $$
Using the notation $latex v_{ij} = w_{ij} U( b_i , s_j )$$,
$latex \[
(d e) I  = \int_0^e \int_0^d  \left[
	v_{11} (d - a)(e - t) + v_{21} a (e - t) + v_{12} (d - a) t + v_{22} at
\right]
\; \B{d} a \; \B{d} t
\] $$
Doing the integration w.r.t. $latex a$$ we have
$latex \[
e I  = ( d  / 2 ) \int_0^e \left[
	v_{11} (e - t) + v_{21} (e - t) + v_{12} t + v_{22} t
\right]
\; \B{d} t
\] $$
Doing the integration w.r.t. $latex t$$ we have
$latex \[
I  = ( d e / 4 ) ( v_{11} + v_{21} + v_{12} + v_{22} )
\] $$
Using the definitions for $latex v_{ij}$$ we obtain
$latex \[
I = ( d e / 4 ) \left[  
	w_{11} U( b_1 , s_1 ) + 
	w_{21} U( b_2 , s_1 ) + 
	w_{21} U( b_1 , s_2 ) + 
	w_{22} U( b_2 , s_2 )
\right]
\] $$
We use the following notation
$latex \[
r = \frac{1}{4} \frac{ b_2 - b_1 }{ a_2 - a_1} \frac{ s_2 - s_1 }{ t_2 - t_1}
\] $$
together with definition for $latex U(a, t)$$ to obtain
$latex \[
I = c_{11} u_{11} + c_{21} u_{21} + c_{12} u_{12} + c_{22} u_{22}
\] $$
where
$latex \[
\begin{array}{rcl}
c_{11} & = &
r [ 
	w_{11} ( a_2 - b_1 ) ( t_2 - s_1 ) +
	w_{21} ( a_2 - b_2 ) ( t_2 - s_1 ) +
	w_{12} ( a_2 - b_1 ) ( t_2 - s_2 ) +
	w_{22} ( a_2 - b_2 ) ( t_2 - s_2 )
]
\\
c_{21} & = &
r [ 
	w_{11} ( b_1 - a_1 ) ( t_2 - s_1 ) +
	w_{21} ( b_2 - a_1 ) ( t_2 - s_1 ) +
	w_{12} ( b_1 - a_1 ) ( t_2 - s_2 ) +
	w_{22} ( b_2 - a_1 ) ( t_2 - s_2 )
]
\\
c_{12} & = &
r [ 
	w_{11} ( a_2 - b_1 ) ( s_1 - t_1 ) +
	w_{21} ( a_2 - b_2 ) ( s_1 - t_1 ) +
	w_{12} ( a_2 - b_1 ) ( s_2 - t_1 ) +
	w_{22} ( a_2 - b_2 ) ( s_2 - t_1 )
]
\\
c_{22} & = &
r [ 
	w_{11} ( b_1 - a_1 ) ( s_1 - t_1 ) +
	w_{21} ( b_2 - a_1 ) ( s_1 - t_1 ) +
	w_{12} ( b_1 - a_1 ) ( s_2 - t_1 ) +
	w_{22} ( b_2 - a_1 ) ( s_2 - t_1 )
]
\end{array}
\] $$


$end
------------------------------------------------------------------------------
*/
# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DIMSOD_AT_NAMESPACE

CppAD::vector<double> integrate_2d(
	const std::pair<double,double>&   a ,
	const std::pair<double,double>&   t ,
	const std::pair<double,double>&   b ,
	const std::pair<double,double>&   s ,
	const CppAD::vector<double>&      w )
{
	double a1 = a.first;
	double a2 = a.second;
	double t1 = t.first;
	double t2 = t.second;
	double b1 = b.first;
	double b2 = b.second;
	double s1 = s.first;
	double s2 = s.second;
	//
	assert( a1 <= b1 && b1 < b2 && b2 <= a2 );
	assert( t1 <= s1 && s1 < s2 && s2 <= t2 );
	//
	double w11 = w[0];
	double w21 = w[1];
	double w12 = w[2];
	double w22 = w[3];
	//
	double r   = (b2 - b1)*(s2 - s1) / (4 * (a2 - a1)*(t2 - t1) );
	//
	double c11 = w11 * (a2 - b1) * (t2 - s1);
	c11       += w21 * (a2 - b2) * (t2 - s1);
	c11       += w12 * (a2 - b1) * (t2 - s2);
	c11       += w22 * (a2 - b2) * (t2 - s2);
	c11       *= r;
	//
	double c21 = w11 * (b1 - a1) * (t2 - s1);
	c21       += w21 * (b2 - a1) * (t2 - s1);
	c21       += w12 * (b1 - a1) * (t2 - s2);
	c21       += w22 * (b2 - a1) * (t2 - s2);
	c21       *= r;
	//
	double c12 = w11 * (a2 - b1) * (s1 - t1);
	c12       += w21 * (a2 - b2) * (s1 - t1);
	c12       += w12 * (a2 - b1) * (s2 - t1);
	c12       += w22 * (a2 - b2) * (s2 - t1);
	c12       *= r;
	//
	double c22 = w11 * (b1 - a1) * (s1 - t1);
	c22       += w21 * (b2 - a1) * (s1 - t1);
	c22       += w12 * (b1 - a1) * (s2 - t1);
	c22       += w22 * (b2 - a1) * (s2 - t1);
	c22       *= r;
	//
	CppAD::vector<double> c(4);
	c[0] = c11;
	c[1] = c21;
	c[2] = c12;
	c[3] = c22;
	//
	return c;
}
} // END DISMOD_AT_NAMESPACE
