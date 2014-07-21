// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

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
	      CppAD::vector<Float>&  S         ,
	      CppAD::vector<Float>&  C         )
{	assert( iota.size()  == n_age * n_time );
	assert( rho.size()   == n_age * n_time );
	assert( chi.size()   == n_age * n_time );
	assert( omega.size() == n_age * n_time );
	assert( p_zero.size() == n_time );
	size_t i, j, ij, k, ell;

	// set S and C at initial age for all times
	i = 0;
	for(j = 0; j < n_time; j++)
	{	assert( p_zero[j] <= 1.0 );
		ij    = i * n_time + j;
		C[ij] = p_zero[j];
		S[ij] = 1.0 - p_zero[j];
	}

	// set S and C at initial time and all ages
	CppAD::vector<Float> a_previous(4), a(4), a_next(4), yi(2), yf(2);
	a_previous[0] = - ( iota[0] + omega[0] );
	a_previous[1] = + rho[0];
	a_previous[2] = + iota[0];
	a_previous[3] = - ( rho[0] + chi[0] + omega[0] );
	yi[0]         = S[0];
	yi[1]         = C[0];
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
		S[ij]      = yf[0];
		C[ij]      = yf[1];
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
		yi[0]         = S[ij];
		yi[1]         = C[ij];
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
			S[ij]      = yf[0];
			C[ij]      = yf[1];
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
		yi[0]         = S[ij];
		yi[1]         = C[ij];
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
			S[ij]      = yf[0];
			C[ij]      = yf[1];
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
