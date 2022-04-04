// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-22 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
         GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin trap_ode2$$
$spell
	yf
	yi
	tf
$$

$section Trapezoidal Solution of ODE with Two Components$$

$head Syntax$$
$icode%yf% = trap_ode2(%case_number%, %b%, %yi%, %tf%)
%$$

$head Prototype$$
$srcthisfile%0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1%$$

$head Purpose$$
Given $latex y( 0 ) \in \B{R}^2$$,
$latex B \in \B{R}^{2 \times 2}$$,
and $latex t_f \in \B{R}_+$$,
this routine uses the trapezoidal method vectors to solve for
$latex y( t_f )$$ where
$latex \[
	y' (t) = B y(t)
\]$$

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head b$$
This vector has size four and specifies the matrix $latex B$$ in
row major order; i.e.,
$latex \[
B
=
\left( \begin{array}{cc}
	b_0  & b_1 \\
	b_2  & b_3
\end{array} \right)
\] $$

$head yi$$
This vector has size two and
specifies the vector $latex y( 0 )$$.
To be specific,
$pre
	$$ $latex y_0 ( 0 ) =$$ $icode%yi%[0]%$$
$pre
	$$ $latex y_1 ( 0 ) =$$ $icode%yi%[1]%$$.

$head tf$$
The argument specifies the final time; i.e. $latex t_f$$.

$head yf$$
The return value has size two and contains the
approximate solution of the ODE; i.e.,
$codei%
	%yf%[0]%$$ $latex \approx y_0 ( t_f )$$
$codei%
	%yf%[1]%$$ $latex \approx y_1 ( t_f )$$.

$children%
	example/devel/utility/trap_ode2_xam.cpp
%$$
$head Example$$
The file $cref trap_ode2_xam.cpp$$ contains
and example and test of $code trap_ode2$$.
It returns true for success and false for failure.

$end
---------------------------------------------------------------------------
*/
# include <cppad/cppad.hpp>
# include <dismod_at/trap_ode2.hpp>
# include <dismod_at/a1_double.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
template <class Float>
CppAD::vector<Float> trap_ode2(
	const CppAD::vector<Float>&  b           ,
	const CppAD::vector<Float>&  yi          ,
	const Float&                 tf          )
// END_PROTOTYPE
{	using CppAD::abs;
	using CppAD::exp;
	using CppAD::sqrt;
	//
	assert( b.size() == 4 );
	assert( yi.size() == 2 );
	//
	//  yip_0, yip_1 = y'(ti)
	Float yip_0 = b[0] * yi[0] + b[1] * yi[1];
	Float yip_1 = b[2] * yi[0] + b[3] * yi[1];
	//
	//  yf_0, yf_1 = y(0) + y'(ti) * tf
	Float yf_0 = yi[0] + tf * yip_0;
	Float yf_1 = yi[1] + tf * yip_1;
	//
	//  yfp_0, yfp_1 = y'(tf)
	Float yfp_0 = b[0] * yf_0 + b[1] * yf_1;
	Float yfp_1 = b[2] * yf_0 + b[3] * yf_1;
	//
	// yf
	CppAD::vector<Float> yf(2);
	yf[0] = yi[0] + tf * (yip_0 + yfp_0) / Float(2.0);
	yf[1] = yi[1] + tf * (yip_1 + yfp_1) / Float(2.0);
	//
	return yf;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_TRAP_ODE2(Float)       \
	template CppAD::vector<Float> trap_ode2<Float>(   \
		const CppAD::vector<Float>&  b           ,     \
		const CppAD::vector<Float>&  yi          ,     \
		const Float&                 tf                \
	);

// instantiations
DISMOD_AT_INSTANTIATE_TRAP_ODE2( double )
DISMOD_AT_INSTANTIATE_TRAP_ODE2( a1_double )

} // END DISMOD_AT_NAMESPACE
