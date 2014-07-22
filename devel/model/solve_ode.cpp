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
$begin solve_ode$$
$spell
	dismod
	CppAD
	const
$$

$section Solving the Dismod Ordinary Differential Equation$$.

$head Syntax$$
$codei%dismod_at::solve_ode(
	%n_age%     , 
	%n_time%    , 
	%step_size% , 
	%iota%      , 
	%rho%       , 
	%chi%       , 
	%omega%     , 
	%p_zero%    , 
	%S_out%     , 
	%C_out%
)%$$

$head Purpose$$
This routine determines the value of $latex C(a,t)$$ 
and $latex S(a, t)$$ that solve the dismod
$cref/differential equation/model_data_mean/Differential Equation/$$
on the $cref/ode grid/glossary/Ode Grid/$$.

$head Notation$$
We use $icode a_min$$ for the minimum age in $cref age_table$$
and $icode t_min$$ for the minimum time in $cref time_table$$.

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code CppAD::AD<double>$$

$head n_age$$
This argument has prototype
$codei%
	size_t %n_age%
%$$
It is the number of points in the
$cref/ode age grid/glossary/Ode Grid/Age, a_i/$$.

$head n_time$$
This argument has prototype
$codei%
	size_t %n_time%
%$$
It is the number of points in the
$cref/ode time grid/glossary/Ode Grid/Time, t_j/$$.

$head step_size$$
This argument has prototype
$codei%
	const %Float% %step_size%
%$$
and is the value of $cref/ode_step_size/run_table/ode_step_size/$$
in the run table.

$head rate$$
For $icode rate$$ equal to $icode iota$$, $icode rho$$, $icode chi$$
and $icode omega$$,
this argument has prototype
$codei%
	const CppAD::vector<%Float%>& %rate%
%$$
and size $icode%n_age%*%n_time%$$.
For $icode%i% = 0 , %...%, %n_age%-1%$$,
and $icode%j% = 0 , %...%, %n_time%-1%$$,
$codei% 
	%rate%[ %i% * %n_time% + %j% ]
%$$
is the value of the corresponding rate 
at age $icode%a_i% = %a_min% + %i%*%step_size%$$
and time $icode%t_j% = %t_min% + %j%*%step_size%$$;
see the $cref/model rate functions/model_data_mean/Model Rate Function/$$. 

$head p_zero$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %p_zero%
%$$
and size $icode n_time$$.
For $icode%j% = 0 , %...%, %n_time%-1%$$,
$codei% 
	%p_zero%[ %j% ]
%$$
is the $cref/initial prevalence/run_table/initial_prevalence/$$
at time $icode%t_j% = %t_min% + %j%*%step_size%$$.

$head S_out, C_out$$
These arguments have prototypes
$codei%
	const CppAD::vector<%Float%>& %S_out%
	const CppAD::vector<%Float%>& %C_out%
%$$
and their input sizes are zero.
Upon return they have size is $icode%n_age%*%n_time%$$ and
for $icode%i% = 0 , %...%, %n_age%-1%$$,
$icode%j% = 0 , %...%, %n_time%-1%$$,
$codei% 
	%S_out%[ %i% * %n_time% + %j% ]  ,  %C_out%[ %i% * %n_time% + %j% ]
%$$
is the value of $latex S(a,t)$$ and $latex C(a,t)$$
at age $icode%a_min% + %i%*%step_size%$$
and time $icode%t_min% + %j%*%step_size%$$.


$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
void solve_ode(
	size_t                       n_age     ,
	size_t                       n_time    ,
	const Float&                 step_size ,
	const CppAD::vector<Float>&  iota      ,
	const CppAD::vector<Float>&  rho       ,
	const CppAD::vector<Float>&  chi       ,
	const CppAD::vector<Float>&  omega     ,
	const CppAD::vector<Float>&  p_zero    ,
	      CppAD::vector<Float>&  S_out     ,
	      CppAD::vector<Float>&  C_out     )
{	assert( iota.size()  == n_age * n_time );
	assert( rho.size()   == n_age * n_time );
	assert( chi.size()   == n_age * n_time );
	assert( omega.size() == n_age * n_time );
	assert( p_zero.size() == n_time );
	assert( S_out.size() == 0 );
	assert( C_out.size() == 0 );
	S_out.resize( n_age * n_time );
	C_out.resize( n_age * n_time );
	size_t i, j, ij, k, ell;

	// set S and C at initial age for all times
	i = 0;
	for(j = 0; j < n_time; j++)
	{	assert( p_zero[j] <= 1.0 );
		ij        = i * n_time + j;
		C_out[ij] = p_zero[j];
		S_out[ij] = 1.0 - p_zero[j];
	}

	// set S and C at initial time and all ages
	CppAD::vector<Float> a_previous(4), a(4), a_next(4), yi(2), yf(2);
	a_previous[0] = - ( iota[0] + omega[0] );
	a_previous[1] = + rho[0];
	a_previous[2] = + iota[0];
	a_previous[3] = - ( rho[0] + chi[0] + omega[0] );
	yi[0]         = S_out[0];
	yi[1]         = C_out[0];
	j = 0;
	for(i = 1; i < n_age; i++) 
	{	ij        = i * n_time + j;	
		a_next[0] = - ( iota[ij] + omega[ij] );
		a_next[1] = + rho[ij];
		a_next[2] = + iota[ij];
		a_next[3] = - ( rho[ij] + chi[ij] + omega[ij] );
		//
		for(ell = 0; ell < 4; ell++)
			a[ell] = (a_previous[ell] + a_next[ell]) / 2.0;
		//
		eigen_ode2(step_size, a, yi, yf);
		//
		S_out[ij]  = yf[0];
		C_out[ij]  = yf[1];
		a_previous = a_next;
		yi         = yf;
	}

	// Cohort solutions starting at initial time
	for(k = 0; k < n_age; k++)
	{	j = 0;
		i = k;
		ij = i * n_time + j;
		a_previous[0] = - ( iota[ij] + omega[ij] );
		a_previous[1] = + rho[ij];
		a_previous[2] = + iota[ij];
		a_previous[3] = - ( rho[ij] + chi[ij] + omega[ij] );
		yi[0]         = S_out[ij];
		yi[1]         = C_out[ij];
		while( (i + 1 < n_age) & (j + 1 < n_time) )
		{	i++;
			j++;
			ij = i * n_time + j;
			a_next[0] = - ( iota[ij] + omega[ij] );
			a_next[1] = + rho[ij];
			a_next[2] = + iota[ij];
			a_next[3] = - ( rho[ij] + chi[ij] + omega[ij] );
			//
			for(ell = 0; ell < 4; ell++)
				a[ell] = (a_previous[ell] + a_next[ell]) / 2.0;
			//
			eigen_ode2(step_size, a, yi, yf);
			//
			S_out[ij]  = yf[0];
			C_out[ij]  = yf[1];
			a_previous = a_next;
			yi         = yf;
		}
	}
	// Cohort solutions starting at initial age
	for(k = 0; k < n_time; k++)
	{	i = 0;
		j = k;
		ij = i * n_time + j;
		a_previous[0] = - ( iota[ij] + omega[ij] );
		a_previous[1] = + rho[ij];
		a_previous[2] = + iota[ij];
		a_previous[3] = - ( rho[ij] + chi[ij] + omega[ij] );
		yi[0]         = S_out[ij];
		yi[1]         = C_out[ij];
		while( (i + 1 < n_age) & (j + 1 < n_time) )
		{	i++;
			j++;
			ij = i * n_time + j;
			a_next[0] = - ( iota[ij] + omega[ij] );
			a_next[1] = + rho[ij];
			a_next[2] = + iota[ij];
			a_next[3] = - ( rho[ij] + chi[ij] + omega[ij] );
			//
			for(ell = 0; ell < 4; ell++)
				a[ell] = (a_previous[ell] + a_next[ell]) / 2.0;
			//
			eigen_ode2(step_size, a, yi, yf);
			//
			S_out[ij]  = yf[0];
			C_out[ij]  = yf[1];
			a_previous = a_next;
			yi         = yf;
		}
	}
	return;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_SOLVE_ODE(Float)     \
	template void solve_ode<Float>(                \
		size_t                       n_age     ,  \
		size_t                       n_time    ,  \
		const Float&                 step_size ,  \
		const CppAD::vector<Float>&  iota      ,  \
		const CppAD::vector<Float>&  rho       ,  \
		const CppAD::vector<Float>&  chi       ,  \
		const CppAD::vector<Float>&  omega     ,  \
		const CppAD::vector<Float>&  p_zero    ,  \
		      CppAD::vector<Float>&  S         ,  \
		      CppAD::vector<Float>&  C            \
	);

// instantiations
DISMOD_AT_INSTANTIATE_SOLVE_ODE(double)
DISMOD_AT_INSTANTIATE_SOLVE_ODE( CppAD::AD<double> )

} // END DISMOD_AT_NAMESPACE
