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

$head Syntax$$
$icode%yf% = eigen_ode2(%case_number%, %b%, %yi%, %tf%)
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
Furthermore, the operation sequence only depends on the value of
$icode case_number$$ and not on the value of the particular $icode b$$
for which this function is recorded.

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a2_double$$.


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
	B_{0,0}  & B_{0,1} \\
	B_{1,0}  & B_{1,1}
\end{array} \right)
=
\left( \begin{array}{cc}
	b_0  & b_1 \\
	b_2  & b_3
\end{array} \right)
\] $$

$head Assumption$$
We restrict our attention to the case where the off-diagonal elements
of $latex  B$$ have the same sign; i.e.
$latex \[
	B_{0,1} B_{1,0} = b_1 b_2 \geq 0
\] $$

$head yi$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %yi%
%$$
and size two.
It specifies the vector $latex y( 0 )$$, to be specific,
$pre
	$$ $latex y_0 ( 0 ) =$$ $icode%yi%[0]%$$
$pre
	$$ $latex y_1 ( 0 ) =$$ $icode%yi%[1]%$$.

$head tf$$
This argument has prototype
$codei%
	const %Float%& %tf%
%$$
It specifies the final time; i.e. $latex t_f$$.

$head yf$$
The return value has prototype
$codei%
	CppAD::vector<%Float%>& %yf%
%$$
and size two and contains the solution of the ODE; i.e.,
$codei%
	%yf%[0]%$$ $latex = y_0 ( t_f )$$
$codei%
	%yf%[1]%$$ $latex = y_1 ( t_f )$$.

$head case_number$$
This argument has prototype
$codei%
	size_t %case_number%
%$$

$subhead One$$
The case where $latex b_1 = 0$$ and $latex b_2 = 0$$ we denote by
$codei%
	%case_number% == 1
%$$
In this case we return the solution
$latex \[
\begin{array}{rcl}
	y_0 ( t_f ) & = & y_0 (0) \exp( b_0 t )
	\\
	y_1 ( t_f ) & = & y_1 (0) \exp( b_3 t )
\end{array}
\]$$

$subhead Two$$
The case where $latex b_1 \neq 0$$ and $latex b_2 = 0$$.
We denote by
$codei%
	%case_number% == 2
%$$
In this case, we switch the order of the rows and columns in
$latex B$$ and $icode yi$$,
compute the solution using
$cref/case three/eigen_ode2/Method/Case Three/$$
and then switch the order of the result.

$subhead Three$$
The case where $latex b_1 = 0$$, $latex b_2 \neq 0$$.
We denote this case by
$codei%
	%case_number% == 3
%$$
In this case, we compute the solution use the method for
$cref/case three/eigen_ode2/Method/Case Three/$$ below.

$subhead Four$$
The case where $latex b_1 \neq 0 $$ and $latex b_2 \neq 0$$.
$codei%
	%case_number% == 4
%$$
In this case, we compute the solution use the method for
$cref/case four/eigen_ode2/Method/Case Four/$$ below.

$head Method$$
The solution for case one is presented above.
The solution for case two is to convert it to case three.

$subhead Case Three$$
For this case $latex b_1 = 0$$ and $latex b_2 \neq 0$$.
It follows that
$latex \[
\begin{array}{rcl}
y_0 ( t )   & = & y_0 ( 0 ) \exp ( b_0 t )
\\
y_1 ' ( t ) & = & b_3 y_1 (t) + b_2 y_0 ( t )
\\
y_1 (t)     & = & y_1 ( 0 ) \exp ( b_3 t ) +
	\int_0^t \exp [ b_3 ( t - s ) ] b_2 y_0 (s) \B{d} s
\\
y_1 (t)     & = & y_1 ( 0 ) \exp ( b_3 t ) +
	b_2 y_0 ( 0 ) \int_0^t \exp [ b_3 ( t - s ) + b_0 s  ] \B{d} s
\\
y_1 (t)     & = & y_1 ( 0 ) \exp ( b_3 t ) +
	b_2 y_0 ( 0 ) \exp ( b_3 t ) \int_0^t \exp [ ( b_0 - b_3 ) s ] \B{d} s
\end{array}
\] $$

$subhead Case Four$$
In this case
$latex \[
	(b_0 - b_3)^2 + b_1 b_2 > 0
\] $$
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
[ 1 \; , \; ( \lambda - b_0 ) / b_2  ] \; B
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
	u_\pm & = & ( \lambda_\pm - b_0 ) / b_2
	\\
	z_\pm (t) & = & y_0 (t) + u_\pm y_1 (t)
\end{array}
\] $$
It follows that
$latex \[
\begin{array}{rcl}
z_\pm (t) & = &  (1, u_\pm ) y(t)
\\
z_\pm ' (t)
& = &  (1, u_\pm ) y'(t) =  (1, u_\pm ) B y (t) = \lambda_\pm z(t)
\\
z_\pm (t) & = & z_\pm (0) \exp( \lambda_\pm t )
\end{array}
\] $$
Which enables us to compute $latex z_\pm (t)$$.
Furthermore
$latex \[
\begin{array}{rcl}
y_1 (t) & = & [ z_+ (t) - z_- (t) ] / ( u_+ - u_- )
\\
& = & [ z_+ (t) - z_- (t) ] b_2 / ( \lambda_+ - \lambda_- )
\\
& = & [ z_+ (t) - z_- (t) ] b_2 / \sqrt{ (b_0 - b_3)^2 + 4 b_1 b_2 }
\\
y_0(t) & = & z_+ (t) - u_+ y_1 (t)
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
# include <cppad/cppad.hpp>
# include <dismod_at/eigen_ode2.hpp>
# include <dismod_at/a2_double.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

namespace {
	// ignore the double argument case
	void PrintFor(
			double pos, const char* before, double var, const char* after
	)
	{	return; }
}

template <class Float>
CppAD::vector<Float> eigen_ode2(
	size_t                       case_number ,
	const CppAD::vector<Float>&  b           ,
	const CppAD::vector<Float>&  yi          ,
	const Float&                 tf          )
{	using CppAD::abs;
	using CppAD::exp;
	using CppAD::sqrt;
	//
	assert( b.size() == 4 );
	assert( yi.size() == 2 );
	assert( 1 <= case_number && case_number <= 4 );
	CppAD::vector<Float> yf(2);

	// solution corresponding to b_1 = b_2 = 0
	if( case_number == 1 )
	{	yf[0] = yi[0] * exp( b[0] * tf );
		yf[1] = yi[1] * exp( b[3] * tf );
		return yf;
	}

	// case for which we switch the order of the rows and columns
	if( case_number == 2 )
	{	CppAD::vector<Float> b_sw(4), yi_sw(2), yf_sw(2);
		b_sw[0]  = b[3];
		b_sw[1]  = b[2];
		b_sw[2]  = b[1];
		b_sw[3]  = b[0];
		yi_sw[0] = yi[1];
		yi_sw[1] = yi[0];
		size_t case_number_sw = 3;
		yf_sw = eigen_ode2(case_number_sw, b_sw, yi_sw, tf);
		yf[0] = yf_sw[1];
		yf[1] = yf_sw[0];
		//
		return yf;
	}
	//
	if( case_number == 3 )
	{	// y_0 ( tf )
		yf[0] = yi[0] * exp( b[0] * tf );
		// y_1 ( tf )
		Float term = (exp( (b[0] - b[3]) * tf ) - 1.0 ) / (b[0] - b[3] );
		//
		Float eps = CppAD::numeric_limits<Float>::epsilon();
		Float pos = abs(b[0] - b[3]) - 10. * eps * (abs(b[0]) + abs(b[1]));
		PrintFor(pos, "\neigen: b[0] - b[3] = ", b[0] - b[3], "\n");
		//
		yf[1] = exp( b[3] * tf ) * ( yi[1] + b[2] * yi[0] * term );
		//
		return yf;
	}
	//
	assert( case_number == 4 );

	// discriminant in the quadratic equation for eigen-values
	Float disc = (b[0] - b[3])*(b[0] - b[3]) + 4.0*b[1]*b[2];
	Float root_disc = Float(sqrt( disc ));

	// use eigen vectors
	Float lambda_p  = (b[0] + b[3] + root_disc) / 2.0;
	Float lambda_m  = (b[0] + b[3] - root_disc) / 2.0;
	//
	Float u_p       = (lambda_p - b[0]) / b[2];
	Float u_m       = (lambda_m - b[0]) / b[2];
	//
	Float zi_p      = yi[0] + u_p * yi[1];
	Float zi_m      = yi[0] + u_m * yi[1];
	//
	Float zf_p      = zi_p * exp( lambda_p * tf );
	Float zf_m      = zi_m * exp( lambda_m * tf );
	//
	yf[1]           = (zf_p - zf_m) * b[2] / root_disc;
	yf[0]           = zf_p - u_p * yf[1];
	//
	return yf;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_EIGEN_ODE2(Float)       \
	template CppAD::vector<Float> eigen_ode2<Float>(   \
		size_t                       case_number ,     \
		const CppAD::vector<Float>&  b           ,     \
		const CppAD::vector<Float>&  yi          ,     \
		const Float&                 tf                \
	);

// instantiations
DISMOD_AT_INSTANTIATE_EIGEN_ODE2(double)
DISMOD_AT_INSTANTIATE_EIGEN_ODE2( CppAD::AD<double> )
DISMOD_AT_INSTANTIATE_EIGEN_ODE2( a2_double )

} // END DISMOD_AT_NAMESPACE
