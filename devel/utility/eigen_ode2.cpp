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
$codei%eigen_ode2(%tf%, %b%, %yi%, %yf%)
%$$

$head Purpose$$
Given $latex y( 0 ) \in \B{R}^2$$, 
$latex B \in \B{R}^{2 \times 2}$$, 
and $latex t_f \in \B{R}_+$$,
this routine uses the eigen vectors of $latex B$$ to solve for
$latex y( t_f )$$ where
$latex \[
	y' (t) = B y(t)
\]$$
We restrict our attention to the case where the off-diagonal elements
of $latex  B$$ have the same sign; i.e. 
$latex \[
	B_{1,2} B_{2,1} \geq 0
\] $$
and there is at least one non-zero entry in the matrix.
(In the case where $latex B = 0$$, we have $latex y( t_f ) = y(0)$$.)

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code CppAD::AD<double>$$

$head tf$$
This argument has prototype
$codei%
	const %Float%& %tf%
%$$
It specifies the final time; i.e. $latex t_f$$.

$head b$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %b%
%$$
and size four.
It specifies the matrix $latex B$$ 
in row-major order; i.e.,
$latex \[
B
= 
\left( \begin{array}{cc}
	B_{1,1}  & B_{1,2} \\
	B_{2,1}  & B_{2,1}
\end{array} \right)
= 
\left( \begin{array}{cc}
	b_0  & b_1 \\
	b_2  & b_3
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
is an eigen value of $latex B$$ if and only if:
$latex \[
\begin{array}{rcl}
0 & = & ( b_0 - \lambda ) ( b_3 - \lambda )  - b_1 b_2 
\\
0 & = & \lambda^2 - (b_0 + b_3) \lambda + b_0 b_3 - b_1 b_2 
\\
\lambda & = &  \frac{
	(b_0 + b_3) \pm \sqrt{ (b_0 + b_3)^2 - 4 b_0 b_3 + 4 b_1 b_2 }
}{2}
\\
\lambda & = &  \frac{
	(b_0 + b_3) \pm \sqrt{ (b_0 - b_3)^2 + 4 b_1 b_2 }
}{2}
\end{array}
\] $$

$subhead Splitting Case$$
Consider the case where one of the off diagonal elements is zero; 
e.g. $latex b_1 = 0$$.
It follows that
$latex \[
\begin{array}{rcl}
y_1 ( t )   & = & y_1 ( 0 ) \exp ( b_0 t ) 
\\
y_2 ' ( t ) & = & b_3 y_2 (t) + b_2 y_1 ( t )
\\
y_2 (t)     & = & y_2 ( 0 ) \exp ( b_3 t ) +
	\int_0^t \exp [ b_3 ( t - s ) ] b_2 y_1 (s) \B{d} s
\\
y_2 (t)     & = & y_2 ( 0 ) \exp ( b_3 t ) + 
	b_2 y_1 ( 0 ) \int_0^t \exp [ b_3 ( t - s ) + b_0 s  ] \B{d} s
\\
y_2 (t)     & = & y_2 ( 0 ) \exp ( b_3 t ) +
	b_2 y_1 ( 0 ) \exp ( b_3 t ) \int_0^t \exp [ ( b_0 - b_3 ) s ] \B{d} s
\end{array}
\] $$
In the case where $latex b_0 = b_3$$, we have
$latex \[
y_2 (t) = \exp ( b_3 t ) [ y_2 ( 0 ) + b_2 y_1 ( 0 ) t ] 
\] $$
In the case where $latex b_0 \neq b_3$$, we have
$latex \[
y_2 (t) = \exp ( b_3 t ) \left \{ y_2 ( 0 ) +
b_2 y_1 ( 0 ) \frac{ \exp [ ( b_0 - b_3 ) t ] - 1}{ b_0 - b_3 }
\right\}
\] $$

$subhead Eigenvector$$
Without loss of generality we assume that $latex | b_2 | > | b_1 | > 0$$.
If this is not the case, switch to components of $latex y(t)$$
and makes the corresponding changes to $latex B$$, and it will be the case.
We can express an arbitrary left eigenvector of $latex B$$ 
as $latex (1, u)$$ where $latex u \in \B{R}$$,
$latex \[
\begin{array}{rcl}
	\lambda & = & b_0  + b_2 u
	\\
	\lambda u & = & b_1  + b_3 u
\end{array}
\] $$
and $latex \lambda$$ is an eigenvalue of $latex B$$.
Using the first equation to solve for $latex u$$, we have
$latex \[
\lambda \; [ 1 \; , \; ( \lambda - b_0 ) / b_2  ]
=
[ 1 \; , \; ( \lambda - b_0 ) / b_2  ] \; A
\] $$
where 
$latex \[
\lambda =  \frac{
	(b_0 + b_3) \pm \sqrt{ (b_0 - b_3)^2 + 4 b_1 b_2 }
}{2}
\] $$
Define $latex \lambda_+$$ and $latex \lambda_-$$ as corresponding
to the plus and minus the square root above and note
$latex \lambda_+ \neq \lambda_-$$ because $latex b_1 b_2 > 0$$.
We also define
$latex \[
\begin{array}{rcl}
	u_\pm & = & ( \lambda_\pm - b_0 ) / b_1
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
& = & [ z_+ (t) - z_- (t) ] b_2 / ( \lambda_+ - \lambda_- )
\\
& = & [ z_+ (t) - z_- (t) ] b_2 / \sqrt{ (b_0 - b_3)^2 + 4 b_1 b_2 }
\\
y_1(t) & = & z_+ (t) - u_+ y_2 (t)
\end{array}
\] $$

$children%
	example/devel/utility/eigen_ode2_xam.cpp
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

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
void eigen_ode2(
	const Float&                 tf  , 
	const CppAD::vector<Float>&  b   , 
	const CppAD::vector<Float>&  yi  ,
	      CppAD::vector<Float>&  yf  )
{	using CppAD::abs;
	using CppAD::exp;
	using CppAD::sqrt;
	using CppAD::CondExpGt;
	using CppAD::CondExpEq;
	using CppAD::numeric_limits;
	//
	assert( b.size() == 4 );
	assert( yi.size() == 2 );
	assert( yf.size() == 2 );
	assert( b[1] * b[2] >= 0.0 );

	size_t i;
	Float b0 = b[0], b1 = b[1], b2 = b[2], b3 = b[3];
	Float yi1 = yi[0], yi2 = yi[1];

	// sqrt root of Float machine epsilon
	Float eps = sqrt( numeric_limits<Float>::epsilon() );

	// determine maximum abosolute value in matrix
	Float norm_b = abs( b0 );
	for(i = 1; i < 4; i++)
		norm_b += abs(b[i]);

	// discriminant in the quadratic equation for eigen-values
	Float disc = (b0 - b3)*(b0 - b3) + 4.0*b1*b2;
	assert( disc >= 0.0 );
	Float root_disc = sqrt( disc );

	// in this case will will use splitting; i.e., approxiamte
	Float order_1  = tf;
	Float b_diff   = CondExpGt(abs(b1), abs(b2), b3 - b0, b0 - b3);
	Float order_2  = order_1 * b_diff * tf / 2.0; 
	Float approx   = order_1 + order_2;
	Float exact    = (exp( b_diff * tf ) - 1.0) / b_diff;
	Float term     = CondExpGt(abs(order_2), eps*order_1, exact, approx);

	// b[1] = 0: term = { exp[ (b0 - b3)*tf ] - 1 } / (b0 - b3)
	Float yf1_b1_0 = yi1 * exp( b0 * tf );
	Float yf2_b1_0 = exp( b3 * tf )*( yi2 + b2 * yi1 * term );

	// b[2] = 0: term = { exp[ (b3 - b0)*tf ] - 1 } / (b3 - b0)
	Float yf2_b2_0 = yi2 * exp( b3 * tf );
	Float yf1_b2_0 = exp( b0 * tf )*( yi1 + b1 * yi2 * term );

	Float yf1_split = CondExpGt(abs(b1), abs(b2), yf1_b2_0, yf1_b1_0);
	Float yf2_split = CondExpGt(abs(b1), abs(b2), yf2_b2_0, yf2_b1_0);

	// use eigen vectors
	Float lambda_plus  = ((b0 + b3) + root_disc) / 2.0;
	Float lambda_minus = ((b0 + b3) - root_disc) / 2.0;
	//
	Float u_plus       = (lambda_plus  - b0) / b2;
	Float u_minus      = (lambda_minus - b0) / b2;
	//
	Float zi_plus      = yi1 + u_plus  * yi2;
	Float zi_minus     = yi1 + u_minus * yi2;
	//
	Float zf_plus      = zi_plus  * exp( lambda_plus  * tf );
	Float zf_minus     = zi_minus * exp( lambda_minus * tf );
	//
	Float yf2_eigen    = (zf_plus - zf_minus) * b2 / root_disc;
 	Float yf1_eigen    = zf_plus - u_plus * yf2_eigen;
	//
	Float zero(0.0);
	yf[0] = CondExpGt( sqrt(b1*b2), eps*norm_b, yf1_eigen, yf1_split);
	yf[0] = CondExpEq(      norm_b,       zero,     yi[0],     yf[0]);
	//
	yf[1] = CondExpGt( sqrt(b1*b2), eps*norm_b, yf2_eigen, yf2_split);
	yf[1] = CondExpEq(      norm_b,       zero,     yi[1],     yf[1]);
	//
	return;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_EIGEN_ODE2(Float)   \
	template void eigen_ode2<Float>(              \
		const Float&                 tf  ,       \
		const CppAD::vector<Float>&  b   ,       \
		const CppAD::vector<Float>&  yi  ,       \
		      CppAD::vector<Float>&  yf          \
	);

// instantiations
DISMOD_AT_INSTANTIATE_EIGEN_ODE2(double)
DISMOD_AT_INSTANTIATE_EIGEN_ODE2( CppAD::AD<double> )

} // END DISMOD_AT_NAMESPACE
