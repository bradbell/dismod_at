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
$icode%c% = dismod_at::integrate_1d(%q%, %r%, %w%)%$$

$head Purpose$$
Define the functions
$latex \[
	v(s) = ( q_2 - q_1 )^{-1} [ ( q_2 - s ) v_1 + ( s - q_1 ) v_2 ] 
\] $$
$latex \[
	w(s) = ( r_2 - r_1 )^{-1} [ ( r_2 - s ) w_1 + ( s - r_1 ) w_2 ] 
\] $$
where $latex q_1 \leq r_1 < r_2 \leq q_2$$.
Note that 
$latex v(q_1) = v_1$$,
$latex v(q_2) = v_2$$,
$latex w(r_1) = w_1$$, and
$latex w(r_2) = w_2$$,
We wish to compute the coefficients $latex c_1, c_2$$ such that
$latex \[
I( v_1, v_2 ) = \int_{r(0)}^{r(1)} v(s) w(s) \; \B{d} s = c_1 v_1 + c_2 v_2
\] $$
where the coefficients $latex c_1, c_2$$ do not depend on the value of 
$latex v_1$$ or $latex v_2$$.

$head Arguments$$
For $icode x$$ in the set of arguments
$codei%{ %q%, %r%, %w% }%$$,
the argument has prototype
$codei%
	const std::pair<double, double>& %x%
%$$
with the following identification;
$latex x_0 = $$ $icode%x%.first%$$ and 
$latex x_1 = $$ $icode%x%.second%$$.

$head Return$$
The return value has prototype
$codei%
	std::pair<double, double> %c%
%$$
with the following identification;
$latex c_1 = $$ $icode%c%.first%$$ and 
$latex c_2 = $$ $icode%c%.second%$$.

$children%example/devel/integrate_1d_xam.cpp
%$$
$head Example$$
The file $cref integrate_1d_xam.cpp$$ contains an example and test
of using this routine.

$head Method$$
Using the notation $latex d = r_2 - r_1$$,
the integral is given by
$latex \[
I( v_1, v_2 ) = \int_0^d v(s + r_2) w(s + r_2) \; \B{d} s 
\] $$
Using the fact that the functions 
$latex v(s)$$ and $latex w(s)$$ are linear, we have
$latex \[
I( v_1, v_2) =  d^{-2} \int_0^d 
	[ ( w_2 - w_1 ) s + w_1 d ] 
		( [ v( r_2 ) - v( r_1 ) ] s + v( r_1 ) d )
			\; \B{d} s
\] $$
Grouping terms that have the same order in $latex s$$, we obtain
$latex \[
I( v_1, v_2) =  d^{-2} \int_0^d  \left(
	[ ( w_2 - w_1 ) [ v( r_2 ) - v( r_1) ]  s^2 
	+
	w_1 d [ v( r_2 ) - v( r_1) ]  s 
	+
	v( r_1 ) d ( w_2 - w_1 )  s 
	+
	w_1 d v( r_1 ) d
\right) \; \B{d} s
\] $$
Doing the integration we have
$latex \[
d^{-1} I( v_1, v_2) =  
	( w_2 - w_1 ) [ v( r_2 ) - v( r_1) ] / 3
	+
	w_1 [ v( r_2 ) - v( r_1) ] / 2
	+
	v( r_1 ) ( w_2 - w_1 )  / 2
	+
	w_1 v( r_1 ) 
\] $$
$latex \[
d^{-1} I( v_1, v_2) =  
	( w_2 - w_1 ) [ v( r_2 ) - v( r_1) ] / 3
	+
	[ w_1 v( r_2 ) +  v( r_1 ) w_2 ] / 2
\] $$
$latex \[
d^{-1} I( v_1, v_2) =  
	[ w_2 / 2 - ( w_2 - w_1 ) / 3 ] v( r_1 )
	+
	[ w_1 / 2 + ( w_2 - w_1 ) / 3 ] v( r_2 )
\] $$
We define
$latex \[
\begin{array}{rcl}
b_1 & = & w_2 / 2 - ( w_2 - w_1 ) / 3 
= ( w_2 + 2 w_1 ) / 6
\\
b_2 & = & w_1 / 2 + ( w_2 - w_1 ) / 3 
= ( w_1 + 2 w_2 ) / 6
\end{array}
\] $$
Using the definition of $latex v(s)$$ to replace 
$latex v( r_1 )$$ and $latex v( r_2 )$$, we have
$latex \[
d^{-1} I( v_1, v_2) =  
b_1 \frac{( q_2 - r_1 ) v_1 + ( r_1 - q_1 ) v_2 }{ q_2 - q_1 }
+
b_2 \frac{( q_2 - r_2 ) v_1 + ( r_2 - q_1 ) v_2 }{ q_2 - q_1 }
\] $$
Finally, making the definitions
$latex \[
\begin{array}{rcl}
c_1 & = &
[  b_1 ( q_2 - r_1 ) + b_2 ( q_2 - r_2 ) ] \; d / ( q_2 - q_1 )
\\
c_2 & = &
[ b_1 ( r_1 - q_1 ) + b_2 ( r_2 - q_1 ) ] \; d / ( q_2 - q_1 )
\end{array}
\] $$
We obtain the desired conclusion
$latex \[
I( v_1 , v_2 ) = c_1 v_1 + c_2 v_2
\] $$
$end
------------------------------------------------------------------------------
*/
# include <utility>
# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DIMSOD_AT_NAMESPACE

std::pair<double,double> integrate_1d(
	const std::pair<double,double>&  q ,
	const std::pair<double,double>&  r ,
	const std::pair<double,double>&  w )
{
	assert( q.first  <= r.first  );
	assert( r.first  <  r.second );
	assert( r.second <= q.second );

	double dr = r.second - r.first;
	double dq = q.second - q.first;
	double b1 = ( w.second + 2.0 * w.first  ) / 6.0;
	double b2 = ( w.first  + 2.0 * w.second ) / 6.0;

	std::pair<double,double> c;
	c.first  = ( b1*(q.second-r.first) + b2*(q.second-r.second) ) * dr / dq;
	c.second = ( b1*(r.first-q.first)  + b2*(r.second-q.first)  ) * dr / dq;

	return c;
}

} // END DISMOD_AT_NAMESPACE
