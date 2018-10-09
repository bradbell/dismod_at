/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin cohort_ode$$
$spell
	pini
$$

$section Solve The ODE on a Cohort Line$$

$head Under Construction$$

$head Syntax$$
$codei%cohort_ode(
%rate_case%, %age%, %pini%, %iota%, %rho%, %chi%, %omega%, %s_out%, %c_out%
)%$$

$head Prototype$$
$srcfile%devel/utility/cohort_ode.cpp%
	0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1
%$$

$head Purpose$$
This routine approximates the functions
$latex S(a)$$ and $latex C(a)$$ that solve the
$cref/ordinary differential equation
	/avg_integrand
	/Ordinary Differential Equation
/$$.
The functions $latex S(a, t)$$, $latex C(a, t)$$
satisfy this equation along cohort lines were $latex t - a$$ is constant.

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head n_cohort$$
We use the notation $icode n_cohort$$ for the number of grid
points along this cohort at which the approximate solution is returned.

$head rate_case$$
This is the value of
$cref/rate_case/option_table/rate_case/$$ in the option table.

$head age$$
This vector has size $icode n_cohort$$ and
specifies the age values for this cohort.
For $icode%k% = 1 , %...%, %n_cohort%-1%$$, the rates are approximated
as constant over the $th k$$ age interval
from $icode%age%[%k%-1]%$$ to $icode%age%[%k%]%$$.

$head pini$$
This is the prevalence corresponding to $icode%age%[0]%$$; see
$cref/pini/rate_table/rate_name/pini/$$.
At this age $latex a_0$$,
$latex C( a_0 )$$ is equal to $icode pini$$ and
$latex S( a_0 ) = 1 - C( a_0 )$$.

$head rate$$
For $icode rate$$ equal to
iota, rho, chi, and omega,
we use $icode rate$$ for the corresponding argument.
It is vector with size $icode n_cohort$$ and
$icode%rate5[%k%]%$$
specifies the corresponding rate at age $icode%age%[%k%]%$$.

$icode s_out$$
This vector has size $icode n_cohort$$.
The input value of its elements does not matter.
Upon return, $icode%s_out%[%k%]%$$ is the approximation solution
for $latex S(a, t)$$ at the corresponding age and time.

$icode c_out$$
This vector has size $icode n_cohort$$.
The input value of its elements does not matter.
Upon return, $icode%c_out%[%k%]%$$ is the approximation solution
for $latex C(a, t)$$ at the corresponding age and time.

$end
*/
# include <dismod_at/cohort_ode.hpp>
# include <dismod_at/eigen_ode2.hpp>
# include <dismod_at/a1_double.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
template <class Float>
void cohort_ode(
	const std::string&           rate_case ,
	const CppAD::vector<double>& age       ,
	const Float&                 pini      ,
	const CppAD::vector<Float>&  iota      ,
	const CppAD::vector<Float>&  rho       ,
	const CppAD::vector<Float>&  chi       ,
	const CppAD::vector<Float>&  omega     ,
	CppAD::vector<Float>&        s_out     ,
	CppAD::vector<Float>&        c_out     )
// END_PROTOTYPE
{	size_t n_cohort = age.size();
	assert( n_cohort == iota.size() );
	assert( n_cohort == rho.size() );
	assert( n_cohort == chi.size() );
	assert( n_cohort == omega.size() );
	assert( n_cohort == s_out.size() );
	assert( n_cohort == c_out.size() );
	/*
	-------------------------------------------------------------------------
	case_number
	-------------------------------------------------------------------------
	b[0] = - ( iota + omega )
	b[1] = + rho
	b[2] = + iota
	b[3] = - ( rho + chi + omega );
	*/
	size_t case_number = 0;
	if( rate_case == "iota_zero_rho_zero" )
	{	// b[1] = 0, b[2] = 0
		case_number = 1;
	}
	else if( rate_case == "iota_zero_rho_pos" )
	{	// b[1] != 0, b[2] = 0
		case_number = 2;
	}
	else if( rate_case == "iota_pos_rho_zero" )
	{	// b[1] = 0, b[2] != 0
		case_number = 3;
	}
	else if( rate_case == "iota_pos_rho_pos" )
	{	// b[1] != 0, b[2] != 0
		case_number = 4;
	}
	assert( case_number != 0 );
	// ----------------------------------------------------------------------
	// initialize for first interval
	c_out[0] = pini;
	s_out[0] = Float(1) - pini;
	//
	CppAD::vector<Float> b(4), yi(2), yf(2);
	Float tf;
	for(size_t k = 1; k < n_cohort; ++k)
	{	// integrate from age[k-1] to age[k]
		//
		// rates at the midpoint
		Float iota_m   = (iota[k-1]   + iota[k])  / Float(2);
		Float rho_m    = (rho[k-1]    + rho[k])   / Float(2);
		Float chi_m    = (chi[k-1]    + chi[k])   / Float(2);
		Float omega_m  = (omega[k-1]  + omega[k]) / Float(2);
		//
		// arguments to eigen_ode2
		b[0]  = - (iota_m + omega_m);
		b[1]  = + rho_m;
		b[2]  = + iota_m;
		b[3]  = - (rho_m + chi_m + omega_m);
		yi[0] = s_out[k-1];
		yi[1] = c_out[k-1];
		tf    = age[k] - age[k-1];
		//
		// call to eigen_ode2
		yf = eigen_ode2(case_number, b, yi, tf);
		//
		// copy result to output vector
		s_out[k] = yf[0];
		c_out[k] = yf[1];
	}
	return;
}

// instantiation macro
# define DISMOT_AT_INSTANTIATE_COHORT_ODE(Float)     \
	template void cohort_ode<Float>(                 \
	const std::string&           rate_case       ,   \
	const CppAD::vector<double>& age             ,   \
	const Float&                 pini            ,   \
	const CppAD::vector<Float>&  iota            ,   \
	const CppAD::vector<Float>&  rho             ,   \
	const CppAD::vector<Float>&  chi             ,   \
	const CppAD::vector<Float>&  omega           ,   \
	CppAD::vector<Float>&        s_out           ,   \
	CppAD::vector<Float>&        c_out               \
	);

// instantiations
DISMOT_AT_INSTANTIATE_COHORT_ODE( double )
DISMOT_AT_INSTANTIATE_COHORT_ODE( a1_double )

} // END DISMOD_AT_NAMESPACE
