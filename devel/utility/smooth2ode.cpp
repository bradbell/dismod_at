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
$begin smooth2ode$$
$spell
	interpolant
	struct
	s_info
	const
	dismod
	CppAD
	var
$$

$section Interpolation from Smoothing to Ode Grid$$

$head Syntax$$
$codei%smooth2ode %si2ode%(
	%n_age_ode%, %n_time_ode%, %ode_step_size% , %age_table%, %time_table%, 
	%s_info%
)%$$
$codei%
%var_ode% = %si2ode%.interpolate( %var_si%, %ode_index% )%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code CppAD::AD<double>$$

$head smooth2ode$$
This constructs an object that interpolates from 
the specified smoothing grid to the ode grid.

$subhead n_age_ode$$
This argument has prototype
$codei%
	size_t %n_age_ode%
%$$
It is the number of points in the
$cref/ode age grid/glossary/Ode Grid/Age, a_i/$$.

$subhead n_time_ode$$
This argument has prototype
$codei%
	size_t %n_time_ode%
%$$
It is the number of points in the
$cref/ode time grid/glossary/Ode Grid/Time, t_j/$$.

$subhead ode_step_size$$
This argument has prototype
$codei%
	double %ode_step_size%
%$$
and is the value of $cref/ode_step_size/run_table/ode_step_size/$$
in the run table.

$subhead age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %age_table%
%$$
and is the age values corresponding to the $icode age_id$$ values.

$subhead time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %time_table%
%$$
and is the time values corresponding to the $icode time_id$$ values.

$subhead s_info$$
This argument has prototype
$codei%
	const smooth_info& %s_info%
%$$
and is the smoothing grid. We use the following notation below:
$codei%
	%n_age_si%       = %s_info%.age_size()
	%n_time_si%      = %s_info%.time_size()
%$$
The only other $icode s_info$$ functions that are used are used by 
$code smooth2ode$$ are: $icode%s_info%.age_id%$$ and $icode%s_info%.time_id%$$,

$head interpolate$$
This is a $code const$$ function 
that interpolates from the smoothing grid to the ode grid.

$subhead var_si$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %var_si%
%$$
and its size is $icode%n_age_si%*%n_time_si%$$.
For $icode%i_si% = 0 , %...%, %n_age_si%-1%$$,
$icode%j_si% = 0 , %...%, %n_time_si%-1%$$,
$codei%
	%var_si%[ %i_si% * %n_time_si% + %j_si% ]
%$$
is the value of the variable (being interpolated) at
$codei%
	%age%  = %age_table%[  %s_info%.age_id(%i_si%) ] 
	%time% = %time_table%[ %s_info%.time_id(%j_si%) ] 
%$$

$subhead ode_index$$
This argument has prototype
$codei%
	const CppAD::vector<size_t>& %ode_index%
%$$
For $icode%k% = 0 , %...%, %ode_index%.size()%$$
the corresponding ode grid point age
$cref/a_i/glossary/Ode Grid/Age, a_i/$$ is defined by
$codei%
	%i%(%k%) = int( %ode_index%[%k%] / %n_time_ode% )
%$$
and the ode grid point time
$cref/t_i/glossary/Ode Grid/Time, t_j/$$ is defined by
$codei%
	%j%(%k%) = mod( %ode_index%[%k%], %n_time_ode% )
%$$

$subhead var_ode$$
This return value has prototype
$codei%
	const CppAD::vector<%Float%>& %var_ode%
%$$
and its size is $icode%ode_index%.size()%$$.
For $icode%k% = 0 , %...%, %ode_index%.size()-1%$$,
$codei%
	%var_ode%[ %k% ]
%$$
is the value of the interpolated value for the variable at
$codei%
	%age%  = %a_i% =  %age_table%[0] + %i%(%k%) * %ode_step_size%
	%time% = %t_j% = %time_table%[0] + %j%(%k%) * %ode_step_size%
%$$

$children%example/devel/utility/smooth2ode_xam.cpp
%$$
$head Example$$
The file $cref smooth2ode_xam.cpp$$ contains an example and test
of using this routine.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/include/smooth2ode.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

smooth2ode::smooth2ode(
	size_t                                      n_age_ode     ,
	size_t                                      n_time_ode    ,
	double                                      ode_step_size ,
	const CppAD::vector<double>&                age_table     ,
	const CppAD::vector<double>&                time_table    ,
	const smooth_info&                          s_info        )
: 
n_age_ode_       (n_age_ode)             ,
n_time_ode_      (n_time_ode)            ,
ode_step_size_   (ode_step_size)         ,
n_age_si_        ( s_info.age_size() )   ,
n_time_si_       ( s_info.time_size() )
{	size_t i, j;	
	//
# ifndef NDEBUG
	double age_min = age_table[0];
	double age_max = age_table[ age_table.size() - 1];
	assert( age_max  <= age_min + (n_age_ode-1) * ode_step_size );
	//
	double time_min = time_table[0];
	double time_max = time_table[ time_table.size() - 1];
	assert( time_max  <= time_min + (n_time_ode-1) * ode_step_size );
# endif
	// smoothing grid information
	size_t i_si        = 0;
	size_t j_si        = 0;
	double age_min_si  = age_table[ s_info.age_id(i_si) ];
	double time_min_si = time_table[ s_info.time_id(j_si) ];
	i_si               = s_info.age_size() - 1;
	j_si               = s_info.time_size() - 1;
	double age_max_si  = age_table[ s_info.age_id(i_si) ];
	double time_max_si = time_table[ s_info.time_id(j_si) ];

	// compute the coefficients for each computational grid point
	coefficient_.resize( n_age_ode * n_time_ode );
	for(i = 0; i < n_age_ode; i++)
	{	double age   = i * ode_step_size + age_table[0];
		//
		if( age <= age_min_si )
			i_si = 0;
		else if( age_max_si <= age )
			i_si = s_info.age_size() - 1;
		else
		{	assert( s_info.age_size() > 1 );
			while( age < age_table[ s_info.age_id(i_si) ] )
			{	assert( 0 < i_si ); 
				i_si--;
			}
			while( age_table[ s_info.age_id(i_si+1) ] < age )
			{	i_si++;
				assert( i_si+1 < s_info.age_size() );
			}
		}
		bool two_age   = (age_min_si < age) & ( age < age_max_si); 
		//
		for(j = 0; j < n_time_ode; j++)
		{	// ode grid information
			double time  = j * ode_step_size + time_table[0];
			//
			if( time <= time_min_si )
				j_si = 0;
			else if( time_max_si <= time )
				j_si = s_info.time_size() - 1;
			else
			{	assert( s_info.time_size() > 1 );
				while( time < time_table[ s_info.time_id(j_si) ] )
				{	assert( 0 < j_si ); 
					j_si--;
				}
				while( time_table[ s_info.time_id(j_si+1) ] < time )
				{	j_si++;
					assert( j_si+1 < s_info.time_size() );
				}
			}
			bool two_time   = (time_min_si < time) & ( time < time_max_si); 

			// coefficient index information
			size_t index = i * n_time_ode + j;
			coefficient_[index].i_si = i_si;
			coefficient_[index].j_si = j_si;

			double a0, a1, ca0, ca1;
			a0  = age_table[  s_info.age_id(i_si) ];
			if( two_age )
			{	a1  = age_table[  s_info.age_id(i_si+1) ];
				ca0  = (a1 - age) / (a1 - a0);
				ca1  = (age - a0) / (a1 - a0);
			}

			double t0, t1, ct0, ct1;
			t0  = time_table[  s_info.time_id(j_si) ];
			if( two_time )
			{	t1  = time_table[  s_info.time_id(j_si+1) ];
				ct0  = (t1 - time) / (t1 - t0);
				ct1  = (time - t0) / (t1 - t0);
			}

			if( two_age & two_time )
			{	coefficient_[index].c_00 = ca0 * ct0;
				coefficient_[index].c_10 = ca1 * ct0;
				coefficient_[index].c_01 = ca0 * ct1;
				coefficient_[index].c_11 = ca1 * ct1;
			}
			else if( two_age )
			{	coefficient_[index].c_00 = ca0;
				coefficient_[index].c_10 = ca1;
				coefficient_[index].c_01 = 0.0;
				coefficient_[index].c_11 = 0.0;
			}
			else if( two_time )
			{	coefficient_[index].c_00 = ct0;
				coefficient_[index].c_10 = 0.0;
				coefficient_[index].c_01 = ct1;
				coefficient_[index].c_11 = 0.0;
			}
			else
			{	coefficient_[index].c_00 = 1.0;
				coefficient_[index].c_10 = 0.0;
				coefficient_[index].c_01 = 0.0;
				coefficient_[index].c_11 = 0.0;
			}
		}
	}
}

template <class Float>
CppAD::vector<Float> smooth2ode::interpolate(
	const CppAD::vector<Float>&  var_si    , 
	const CppAD::vector<size_t>& ode_index ) const
{	assert( var_si.size() == n_age_si_ * n_time_si_ );
	//
	CppAD::vector<Float> var_ode( ode_index.size() );
	for(size_t k = 0; k < ode_index.size(); k ++)
	{	size_t index = ode_index[k];
		size_t i_si  = coefficient_[index].i_si;
		size_t j_si  = coefficient_[index].j_si;
		double c_00  = coefficient_[index].c_00;
		double c_10  = coefficient_[index].c_10;
		double c_01  = coefficient_[index].c_01;
		double c_11  = coefficient_[index].c_11;
		Float  sum   = 0.0;
		sum      += c_00 * var_si[i_si*n_time_si_ + j_si];
		if( c_10 != 0.0 )
			sum += c_10 * var_si[(i_si+1)*n_time_si_ + j_si];
		if( c_01 != 0.0 )
			sum += c_01 * var_si[i_si*n_time_si_ + (j_si+1)];
		if( c_11 != 0.0 )
			sum += c_11 * var_si[(i_si+1)*n_time_si_ + (j_si+1)];
		var_ode[k] = sum;
	}
	return var_ode;
}

// instantiation 
# define DISMOD_AT_INSTANTIATE_SMOOTH2ODE_IMPLEMENT(Float)  \
template CppAD::vector<Float> smooth2ode::interpolate<Float>( \
	const CppAD::vector<Float>&  var_si    ,                  \
	const CppAD::vector<size_t>& ode_index                    \
) const;

DISMOD_AT_INSTANTIATE_SMOOTH2ODE_IMPLEMENT( double )
DISMOD_AT_INSTANTIATE_SMOOTH2ODE_IMPLEMENT( CppAD::AD<double> )

} // END DISMOD_AT_NAMESPACE

