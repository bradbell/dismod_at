// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin solve_ode$$
$spell
	pini
	dismod
	CppAD
	const
$$

$section Solving the Dismod Ordinary Differential Equation$$.

$head Syntax$$
$codei%solve_ode(
	%i_max%     ,
	%j_max%     ,
	%step_size% ,
	%pini%      ,
	%iota%      ,
	%rho%       ,
	%chi%       ,
	%omega%     ,
	%S_out%     ,
	%C_out%
)%$$

$head 2DO$$
Remove $icode i_max$$,  $code j_max$$ from the arguments to
$code solve_ode$$ because age and time do not explicitly appear in the
ODE.

$head Purpose$$
This routine determines the value of $latex C(a,t)$$
and $latex S(a, t)$$ that solve the dismod
$cref/differential equation/avg_integrand/Differential Equation/$$
for a subset of the $cref ode_grid$$ corresponding
to a cohort (constant time minus age).

$head Notation$$
We use $icode a_min$$ for the minimum age in $cref age_table$$
and $icode t_min$$ for the minimum time in $cref time_table$$.

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head i_max$$
This argument has prototype
$codei%
	size_t %i_max%
%$$
It is the maximum
$cref/ode age grid/ode_grid/Age, a_i/$$ index for this cohort.

$head j_max$$
This argument has prototype
$codei%
	size_t %j_max%
%$$
It is the maximum
$cref/ode time grid/ode_grid/Time, t_j/$$ index for this cohort.

$head step_size$$
This argument has prototype
$codei%
	const %Float%& %step_size%
%$$
and is the value of $cref/ode_step_size/option_table/ode_step_size/$$
in the fit command.

$head pini$$
This argument has prototype
$codei%
	const %Float%& %pini%
%$$
and is the initial prevalence for this cohort; see
$cref/pini/avg_integrand/Rate Functions/pini, p_i0(t)/$$.

$head rate$$
For $icode rate$$ equal to
$icode iota$$, $icode rho$$, $icode chi$$ and $icode omega$$,
this argument has prototype
$codei%
	const CppAD::vector<%Float%>& %rate%
%$$
and size $icode%i_max%+1%$$.
For $icode%k% = 0 , %...%, %i_max%$$,
$codei%
	%rate%[ %k% ]
%$$
is the value of the corresponding rate at age
$codei%
	%a% = %a_min% + %k% * %step_size%
%$$
and at time
$codei%
	%t% = %t_min% + (%j_max% - %i_max% + %k%) * %step_size%
%$$

$head S_out, C_out$$
These arguments have prototypes
$codei%
	CppAD::vector<%Float%>& %S_out%
	CppAD::vector<%Float%>& %C_out%
%$$
and their input sizes are zero.
Upon return they have size is $icode%i_max% + 1%$$ and
for $icode%k% = 0 , %...%, %i_max%$$,
$codei%
	%S_out%[ %k% ]  ,  %C_out%[ %k% ]
%$$
is the value of $latex S(a,t)$$ and $latex C(a,t)$$
$codei%
	%a% = %a_min% + %k% * %step_size%
%$$
and at
$codei%
	%t% = %t_min% + (%j_max% - %i_max% + %k%) * %step_size%
%$$


$children%
	example/devel/utility/solve_ode_xam.cpp
%$$
$head Example$$
The file $cref solve_ode_xam.cpp$$ contains
and example and test of $code solve_ode$$.
It returns true for success and false for failure.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/solve_ode.hpp>
# include <dismod_at/eigen_ode2.hpp>
# include <dismod_at/a1_double.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
void solve_ode(
	size_t                       eigen_ode2_case_number ,
	size_t                       i_max     ,
	size_t                       j_max     ,
	const Float&                 step_size ,
	const Float&                 pini      ,
	const CppAD::vector<Float>&  iota      ,
	const CppAD::vector<Float>&  rho       ,
	const CppAD::vector<Float>&  chi       ,
	const CppAD::vector<Float>&  omega     ,
	      CppAD::vector<Float>&  S_out     ,
	      CppAD::vector<Float>&  C_out     )
{	assert( pini <= 1.0 );
	assert( iota.size()  == i_max+1 );
	assert( rho.size()   == i_max+1 );
	assert( chi.size()   == i_max+1 );
	assert( omega.size() == i_max+1 );
	assert( S_out.size() == 0 );
	assert( C_out.size() == 0 );
	S_out.resize( i_max+1 );
	C_out.resize( i_max+1 );

	// set S and C at initial age
	C_out[0] = pini;
	S_out[0] = 1.0 - pini;

	// Cohort solutions starting at initial time
	CppAD::vector<Float> b_previous(4), b_avg(4), b_next(4), yi(2), yf(2);
	b_previous[0] = - ( iota[0] + omega[0] );
	b_previous[1] = + rho[0];
	b_previous[2] = + iota[0];
	b_previous[3] = - ( rho[0] + chi[0] + omega[0] );
	for(size_t k = 0; k < i_max; k++)
	{
		yi[0]         = S_out[k];
		yi[1]         = C_out[k];
		//
		b_next[0] = - ( iota[k+1] + omega[k+1] );
		b_next[1] = + rho[k+1];
		b_next[2] = + iota[k+1];
		b_next[3] = - ( rho[k+1] + chi[k+1] + omega[k+1] );
		//
		for(size_t j = 0; j < 4; j++)
			b_avg[j] = (b_previous[j] + b_next[j]) / 2.0;
		//
		// |b[2]| > |b[1]| and b[0] != b[3]
		yf = eigen_ode2(eigen_ode2_case_number, b_avg, yi, step_size);
		//
		S_out[k+1]  = yf[0];
		C_out[k+1]  = yf[1];
		//
		b_previous = b_next;
	}
	return;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_SOLVE_ODE(Float)     \
	template void solve_ode<Float>(                \
		size_t                       eigen_ode2_case_number , \
		size_t                       i_max     ,  \
		size_t                       j_max     ,  \
		const Float&                 step_size ,  \
		const Float&                 pini      ,  \
		const CppAD::vector<Float>&  iota      ,  \
		const CppAD::vector<Float>&  rho       ,  \
		const CppAD::vector<Float>&  chi       ,  \
		const CppAD::vector<Float>&  omega     ,  \
		      CppAD::vector<Float>&  S         ,  \
		      CppAD::vector<Float>&  C            \
	);

// instantiations
DISMOD_AT_INSTANTIATE_SOLVE_ODE( double )
DISMOD_AT_INSTANTIATE_SOLVE_ODE( a1_double )

} // END DISMOD_AT_NAMESPACE
