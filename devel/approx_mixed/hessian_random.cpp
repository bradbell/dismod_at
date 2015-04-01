// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/approx_mixed.hpp>
# include <cppad/ipopt/solve.hpp>
/*
$begin approx_mixed_hessian_random$$
$spell
	vec
	const
	Cpp
	xam
$$

$section Hessian With Respect to Random Effects$$

$head Syntax$$
$icode%approx_object%.hessian_random(
	%fixed_vec%, %random_vec%, %row_out%, %col_out%, %val_out%
)%$$

$head Purpose$$
This routine computes the Hessian of the negative log of the joint density
$cref/f(theta, u)/approx_mixed_theory/f(theta, u)/$$
with respect to the random effects vector $latex u$$; i.e.
$latex \[
	f_{uu}^{(2)} ( \theta, u )
\] $$

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$ 
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$.

$head row_out$$
This argument has prototype
$codei%
	CppAD::vector<size_t>& %row_out%
%$$
It contains the row indices for values in the Hessian
that are possibly non-zero.

$head col_out$$
This argument has prototype
$codei%
	CppAD::vector<size_t>& %col_out%
%$$
It contains the column indices for values in the Hessian
that are possibly non-zero.

$head val_out$$
This argument has prototype
$codei%
	CppAD::vector<size_t>& %val_out%
%$$
It contains the values in the Hessian that are possibly non-zero.

$comment%
	example/devel/approx_mixed/hessian_random_xam.cpp
%$$
$head Example$$
The file $code hessian_random_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/
