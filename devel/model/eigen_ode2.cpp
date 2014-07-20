// $Id$
/* --------------------------------------------------------------------------
dismod_ode: MCMC Estimation of Disease Rates as Functions of Age
          Copyright (C) 2013 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin eigen_ode2$$
$spell
	tf
	eigen
	const
	yi
	yf
	cpp
$$

$section Eigen Vector Solution of ODE with Two Components$$
$index eigen, ode$$
$index ode, eigen$$

$head Syntax$$
$codei%eigen_ode2(%tf%, %a%, %yi%, %yf%)
%$$

$head Purpose$$
Given $latex y( 0 ) \in \B{R}^2$$, 
$latex A \in \B{R}^{2 \times 2}$$, 
and $latex t_f \in \B{R}_+$$,
this routine uses the eigen vectors of $latex A$$ to solve for
$latex y( t_f )$$ where
$latex \[
	y' (t) = A y(t)
\]$$
We restrict our attention to the case where the off-diagonal elements
of $latex  A$$ have the same sign; i.e. 
$latex \[
	A_{1,2} A_{2,1} \geq 0
\] $$
and there is at least one non-zero entry in the matrix.
(In the case where $latex A = 0$$, we have $latex y( t_f ) = y(0)$$.)

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code CppAD::AD<double>$$

$head tf$$
This argument has prototype
$codei%
	const %Float%& %tf%
%$$
It specifies the final time; i.e. $latex t_f$$.

$head a$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %a%
%$$
and size four.
It specifies the matrix $latex A$$ 
in row-major order; i.e.,
$latex \[
A 
= 
\left( \begin{array}{cc}
	A_{1,1}  & A_{1,2} \\
	A_{2,1}  & A_{2,1}
\end{array} \right)
= 
\left( \begin{array}{cc}
	a_0  & a_1 \\
	a_2  & a_3
\end{array} \right)
\] $$

$head yi$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %yi%
%$$
and size two.
It specifies the vector $latex y( 0 )$$, to be specific,
$pre
	$$
$latex y_1 ( 0 ) =$$ $icode%yi%[0]%$$,
$latex y_2 ( 0 ) =$$ $icode%yi%[1]%$$.

$head yf$$
This argument has prototype
$codei%
	CppAD::vector<%Float%>& %yf%
%$$
and size two.
The input value of its elements does not matter.
Upon return 
$pre
	$$
$icode%yf%[0]%$$ $latex = y_1 ( t_f )$$,
$icode%yf%[1]%$$ $latex = y_2 ( t_f )$$.

$head Method$$

$subhead Eigenvalues$$
A value $latex \lambda$$ 
is an eigen value of $latex A$$ if and only if:
$latex \[
\begin{array}{rcl}
0 & = & ( a_0 - \lambda ) ( a_3 - \lambda )  - a_1 a_2 
\\
0 & = & \lambda^2 - (a_0 + a_3) \lambda + a_0 a_3 - a_1 a_2 
\\
\lambda & = &  \frac{
	(a_0 + a_3) \pm \sqrt{ (a_0 + a_3)^2 - 4 a_0 a_3 + 4 a_1 a_2 }
}{2}
\\
\lambda & = &  \frac{
	(a_0 + a_3) \pm \sqrt{ (a_0 - a_3)^2 + 4 a_1 a_2 }
}{2}
\end{array}
\] $$

$subhead Splitting Case$$
Consider the case where one of the off diagonal elements is zero; 
e.g. $latex a_1 = 0$$.
It follows that
$latex \[
\begin{array}{rcl}
y_1 ( t )   & = & y_1 ( 0 ) \exp ( a_0 t ) 
\\
y_2 ' ( t ) & = & a_3 y_2 (t) + a_2 y_1 ( t )
\\
y_2 (t)     & = & y_2 ( 0 ) \exp ( a_3 t ) +
	\int_0^t \exp [ a_3 ( t - s ) ] a_2 y_1 (s) \B{d} s
\\
y_2 (t)     & = & y_2 ( 0 ) \exp ( a_3 t ) + 
	a_2 y_1 ( 0 ) \int_0^t \exp [ a_3 ( t - s ) + a_0 s  ] \B{d} s
\\
y_2 (t)     & = & y_2 ( 0 ) \exp ( a_3 t ) +
	a_2 y_1 ( 0 ) \exp ( a_3 t ) \int_0^t \exp [ ( a_0 - a_3 ) s ] \B{d} s
\end{array}
\] $$
In the case where $latex a_0 = a_3$$, we have
$latex \[
y_2 (t) = \exp ( a_3 t ) [ y_2 ( 0 ) + a_2 y_1 ( 0 ) t ] 
\] $$
In the case where $latex a_0 \neq a_3$$, we have
$latex \[
y_2 (t) = \exp ( a_3 t ) \left \{ y_2 ( 0 ) +
a_2 y_1 ( 0 ) \frac{ \exp [ ( a_0 - a_3 ) t ] - 1}{ a_0 - a_3 }
\right\}
\] $$

$subhead Eigenvector$$
Without loss of generality we assume that $latex | a_2 | > | a_1 | > 0$$.
If this is not the case, switch to components of $latex y(t)$$
and makes the corresponding changes to $latex A$$, and it will be the case.
We can express an arbitrary left eigenvector of $latex A$$ 
as $latex (1, u)$$ where $latex u \in \B{R}$$,
$latex \[
\begin{array}{rcl}
	\lambda & = & a_0  + a_2 u
	\\
	\lambda u & = & a_1  + a_3 u
\end{array}
\] $$
and $latex \lambda$$ is an eigenvalue of $latex A$$.
Using the first equation to solve for $latex u$$, we have
$latex \[
\lambda \; [ 1 \; , \; ( \lambda - a_0 ) / a_2  ]
=
[ 1 \; , \; ( \lambda - a_0 ) / a_2  ] \; A
\] $$
where 
$latex \[
\lambda =  \frac{
	(a_0 + a_3) \pm \sqrt{ (a_0 - a_3)^2 + 4 a_1 a_2 }
}{2}
\] $$
Define $latex \lambda_+$$ and $latex \lambda_-$$ as corresponding
to the plus and minus the square root above and note
$latex \lambda_+ \neq \lambda_-$$ because $latex a_1 a_2 > 0$$.
We also define
$latex \[
\begin{array}{rcl}
	u_\pm & = & ( \lambda_\pm - a_0 ) / a_1
	\\
	z_\pm (t) & = & y_1 (t) + u_\pm y_2 (t) 
\end{array}
\] $$
It follows that
$latex \[
\begin{array}{rcl}
z_\pm (t) & = &  (1, u_\pm ) y(t)
\\
z_\pm ' (t) 
& = &  (1, u_\pm ) y'(t) =  (1, u_\pm ) A y (t) = \lambda_\pm z(t) 
\\
z_\pm (t) & = & z_\pm (0) \exp( \lambda_\pm t )
\end{array}
\] $$
Which enables us to compute $latex z_\pm (t)$$.
Furthermore
$latex \[
\begin{array}{rcl}
y_2 (t) & = & [ z_+ (t) - z_- (t) ] / ( u_+ - u_- )
\\
& = & [ z_+ (t) - z_- (t) ] a_2 / ( \lambda_+ - \lambda_- )
\\
& = & [ z_+ (t) - z_- (t) ] a_2 / \sqrt{ (a_0 - a_3)^2 + 4 a_1 a_2 }
\\
y_1(t) & = & z_+ (t) - u_+ y_2 (t)
\end{array}
\] $$

$children%
	example/devel/model/eigen_ode2_xam.cpp
%$$
$head Example$$
The file $cref eigen_ode2_xam.cpp$$ contains
and example and test of $code eigen_ode2$$.
It returns true for success and false for failure.

$end 
---------------------------------------------------------------------------
*/
# include <dismod_at/dismod_at.hpp>
# include <cppad/cppad.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

template <class Float>
void eigen_ode2(
	const Float&                 tf  , 
	const CppAD::vector<Float>&  a   , 
	const CppAD::vector<Float>&  yi  ,
	      CppAD::vector<Float>&  yf  )
{	using CppAD::abs;
	using CppAD::exp;
	using CppAD::sqrt;
	using CppAD::CondExpGt;
	using CppAD::CondExpEq;
	using CppAD::numeric_limits;
	//
	assert( a.size() == 4 );
	assert( yi.size() == 2 );
	assert( yf.size() == 2 );
	assert( a[1] * a[2] >= 0.0 );

	size_t i;
	Float a0 = a[0], a1 = a[1], a2 = a[2], a3 = a[3];
	Float yi1 = yi[0], yi2 = yi[1];

	// sqrt root of Float machine epsilon
	Float eps = sqrt( numeric_limits<Float>::epsilon() );

	// determine maximum abosolute value in matrix
	Float norm_a = abs( a0 );
	for(i = 1; i < 4; i++)
		norm_a += abs(a[i]);

	// discriminant in the quadratic equation for eigen-values
	Float disc = (a0 - a3)*(a0 - a3) + 4.0*a1*a2;
	assert( disc >= 0.0 );
	Float root_disc = sqrt( disc );

	// in this case will will use splitting; i.e., approxiamte
	Float order_1  = tf;
	Float a_diff   = CondExpGt(abs(a1), abs(a2), a3 - a0, a0 - a3);
	Float order_2  = order_1 * a_diff * tf / 2.0; 
	Float approx   = order_1 + order_2;
	Float exact    = (exp( a_diff * tf ) - 1.0) / a_diff;
	Float term     = CondExpGt(abs(order_2), eps*order_1, exact, approx);

	// a[1] = 0: term = { exp[ (a0 - a3)*tf ] - 1 } / (a0 - a3)
	Float yf1_a1_0 = yi1 * exp( a0 * tf );
	Float yf2_a1_0 = exp( a3 * tf )*( yi2 + a2 * yi1 * term );

	// a[2] = 0: term = { exp[ (a3 - a0)*tf ] - 1 } / (a3 - a0)
	Float yf2_a2_0 = yi2 * exp( a3 * tf );
	Float yf1_a2_0 = exp( a0 * tf )*( yi1 + a1 * yi2 * term );

	Float yf1_split = CondExpGt(abs(a1), abs(a2), yf1_a2_0, yf1_a1_0);
	Float yf2_split = CondExpGt(abs(a1), abs(a2), yf2_a2_0, yf2_a1_0);

	// use eigen vectors
	Float lambda_plus  = ((a0 + a3) + root_disc) / 2.0;
	Float lambda_minus = ((a0 + a3) - root_disc) / 2.0;
	//
	Float u_plus       = (lambda_plus  - a0) / a2;
	Float u_minus      = (lambda_minus - a0) / a2;
	//
	Float zi_plus      = yi1 + u_plus  * yi2;
	Float zi_minus     = yi1 + u_minus * yi2;
	//
	Float zf_plus      = zi_plus  * exp( lambda_plus  * tf );
	Float zf_minus     = zi_minus * exp( lambda_minus * tf );
	//
	Float yf2_eigen    = (zf_plus - zf_minus) * a2 / root_disc;
 	Float yf1_eigen    = zf_plus - u_plus * yf2_eigen;
	//
	Float zero(0.0);
	yf[0] = CondExpGt( sqrt(a1*a2), eps*norm_a, yf1_eigen, yf1_split);
	yf[0] = CondExpEq(      norm_a,       zero,     yi[0],     yf[0]);
	//
	yf[1] = CondExpGt( sqrt(a1*a2), eps*norm_a, yf2_eigen, yf2_split);
	yf[1] = CondExpEq(      norm_a,       zero,     yi[1],     yf[1]);
	//
	return;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_EIGEN_ODE2(Float)  \
	template void eigen_ode2<Float>(             \
		const Float&                 tf  ,       \
		const CppAD::vector<Float>&  a   ,       \
		const CppAD::vector<Float>&  yi  ,       \
	      	CppAD::vector<Float>&    yf          \
	);

// instantiations
DISMOD_AT_INSTANTIATE_EIGEN_ODE2(double)
DISMOD_AT_INSTANTIATE_EIGEN_ODE2( CppAD::AD<double> )

} // END DISMOD_AT_NAMESPACE
