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
	v(s) = ( q_1 - q_0 )^{-1} [ ( q_1 - s ) v_0 + ( s - q_0 ) v_1 ] 
\] $$
$latex \[
	w(s) = ( r_1 - r_0 )^{-1} [ ( r_1 - s ) w_0 + ( s - r_0 ) w_1 ] 
\] $$
We wish to compute the coefficients $latex c_0, c_1$$ such that
$latex \[
I( v_0, v_1 ) = \int_{r(0)}^{r(1)} v(s) w(s) \; \B{d} s = c_0 v_0 + c_1 v_1
\] $$
where the coefficients $latex c_0, c_1$$ do not depend on the value of 
$latex v_0$$ or $latex v_1$$.

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
$latex c_0 = $$ $icode%c%.first%$$ and 
$latex c_1 = $$ $icode%c%.second%$$.

$head Method$$
Using the notation $latex d = r_1 - r_0$$,
the integral is given by
$latex \[
I( v_0, v_1 ) = \int_0^d v(s + r_1) w(s + r_1) \; \B{d} s 
\] $$
Using the fact that the functions 
$latex v(s)$$ and $latex w(s)$$ are linear, we have
$latex \[
I( v_0, v_1) =  d^{-2} \int_0^d 
	[ ( w_1 - w_0 ) s + w_0 d ] 
		( [ v( r_1 ) - v( r_0 ) ] s + v( r_0 ) d )
			\; \B{d} s
\] $$
Grouping terms that have the same order in $latex s$$, we obtain
$latex \[
I( v_0, v_1) =  d^{-2} \int_0^d  \left(
	[ ( w_1 - w_0 ) [ v( r_1 ) - v( r_0) ]  s^2 
	+
	w_0 d [ v( r_1 ) - v( r_0) ]  s 
	+
	v( r_0 ) d ( w_1 - w_0 )  s 
	+
	w_0 d v( r_0 ) d
\right) \; \B{d} s
\] $$
Doing the integration we have
$latex \[
d^{-1} I( v_0, v_1) =  
	( w_1 - w_0 ) [ v( r_1 ) - v( r_0) ] / 3
	+
	w_0 [ v( r_1 ) - v( r_0) ] / 2
	+
	v( r_0 ) ( w_1 - w_0 )  / 2
	+
	w_0 v( r_0 ) 
\] $$
$latex \[
d^{-1} I( v_0, v_1) =  
	( w_1 - w_0 ) [ v( r_1 ) - v( r_0) ] / 3
	+
	[ w_0 v( r_1 ) +  v( r_0 ) w_1 ] / 2
\] $$
$latex \[
d^{-1} I( v_0, v_1) =  
	[ w_1 / 2 - ( w_1 - w_0 ) / 3 ] v( r_0 )
	+
	[ w_0 / 2 + ( w_1 - w_0 ) / 3 ] v( r_1 )
\] $$
We define
$latex \[
\begin{array}{rcl}
b_0 & = & w_1 / 2 - ( w_1 - w_0 ) / 3 
= ( w_1 + 2 w_0 ) / 6
\\
b_1 & = & w_0 / 2 + ( w_1 - w_0 ) / 3 
= ( w_0 + 2 w_1 ) / 6
\end{array}
\] $$
Using the definition of $latex v(s)$$ to replace 
$latex v( r_0 )$$ and $latex v( r_1 )$$, we have
$latex \[
d^{-1} I( v_0, v_1) =  
b_0 \frac{( q_1 - r_0 ) v_0 + ( r_0 - q_0 ) v_1 }{ q_1 - q_0 }
+
b_1 \frac{( q_1 - r_1 ) v_0 + ( r_1 - q_0 ) v_1 }{ q_1 - q_0 }
\] $$
Finally, making the definitions
$latex \[
\begin{array}{rcl}
c_0 & = &
[  b_0 ( q_1 - r_0 ) + b_1 ( q_1 - r_1 ) ] \; d / ( q_1 - q_0 )
\\
c_1 & = &
[ b_0 ( r_0 - q_0 ) + b_1 ( r_1 - q_0 ) ] \; d / ( q_1 - q_0 )
\end{array}
\] $$
We obtain the desired conclusion
$latex \[
I( v_0 , v_1 ) = c_0 v_0 + c_1 v_1
\] $$
$end
------------------------------------------------------------------------------
*/
namespace dismod_at { // BEGIN DIMSOD_AT_NAMESPACE

std::pair<double,double> integrate_1d(
	const std::pair<double,double>&  q ,
	const std::pair<double,double>&  r ,
	const std::pair<double,double>&  w )
{
}

} // END DISMOD_AT_NAMESPACE
