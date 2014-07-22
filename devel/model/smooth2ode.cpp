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
	sg
	const
	dismod
	CppAD
	var
$$

$section Interpolation from Smoothing to Ode Grid$$

$head Syntax$$
$codei%dismod_at::smooth2ode %sg2ode%(
%sg%, %age_table%, %time_table%, %n_age_ode%, %n_time_ode%, %ode_step_size%
)
%$$
$icode%var_ode% = %sg2ode%.interpolate( %var_sg% )%$$

$head sg$$
This argument has prototype
$codei%
	const dismod_at::smooth_grid& %sg%
%$$
and is the smoothing grid. We use the following notation below:
$codei%
	%n_age_sg%       = %sg%.age_size()
	%n_time_sg%      = %sg%.time_size()
%$$
The only other $icode sg$$ functions that are used are used by 
$code smooth2ode$$ are: $icode%sg%.age_id%$$ and $icode%sg%.time_id%$$,

$head age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %age_table%
%$$
and is the age values corresponding to the $icode age_id$$ values.

$head time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %time_table%
%$$
and is the time values corresponding to the $icode time_id$$ values.

$head n_age_ode$$
This argument has prototype
$codei%
	size_t %n_age_ode%
%$$
It is the number of points in the
$cref/ode age grid/glossary/Ode Grid/Age, a_i/$$.

$head n_time_ode$$
This argument has prototype
$codei%
	size_t %n_time_ode%
%$$
It is the number of points in the
$cref/ode time grid/glossary/Ode Grid/Time, t_j/$$.

$head ode_step_size$$
This argument has prototype
$codei%
	double %ode_step_size%
%$$
and is the value of $cref/ode_step_size/run_table/ode_step_size/$$
in the run table.

$head var_sg$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %var_sg%
%$$
and its size is $icode%n_age_sg%*%n_time_sg%$$.
For $icode%i_sg% = 0 , %...%, %n_age_sg%-1%$$,
$icode%j_sg% = 0 , %...%, %n_time_sg%-1%$$,
$codei%
	%var_sg%[ %i_sg% * %n_time_sg% + %j_sg% ]
%$$
is the value of the variable (being interpolated) at
$codei%
	%age%  = %age_table%[  %sg%.age_id(%i_sg%) ] 
	%time% = %time_table%[ %sg%.time_id(%j_sg%) ] 
%$$

$head var_ode$$
This return value has prototype
$codei%
	const CppAD::vector<%Float%>& %var_ode%
%$$
and its size is $icode%n_age_ode%*%n_time_ode%$$.
For $icode%i% = 0 , %...%, %n_age_ode%-1%$$,
$icode%j% = 0 , %...%, %n_time_ode%-1%$$,
$codei%
	%var_sg%[ %i_sg% * %n_time_sg% + %j_sg% ]
%$$
is the value of the interpolated value for the variable at
$codei%
	%age%  = %age_table%[0] + %i% * %ode_step_size%
	%time% = %time_table%[0] + %j% * %ode_step_size%
%$$

$children%example/devel/model/smooth2ode_xam.cpp
%$$
$head Example$$
The file $cref smooth2ode_xam.cpp$$ contains an example and test
of using this routine.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
smooth2ode<Float>::smooth2ode(
	const smooth_grid&                          sg            ,
	const CppAD::vector<double>&                age_table     ,
	const CppAD::vector<double>&                time_table    ,
	size_t                                      n_age_ode     ,
	size_t                                      n_time_ode    ,
	double                                      ode_step_size )
: 
n_age_sg_( sg.age_size() )     ,
n_time_sg_( sg.time_size() )   ,
n_age_ode_(n_age_ode)          ,
n_time_ode_(n_time_ode)        ,
ode_step_size_(ode_step_size) 
{	size_t i, j;	
	//
# ifndef NDEBUG
	double age_min = age_table[0];
	double age_max = age_table[ age_table.size() - 1];
	assert( age_max  <= age_min + (n_age_ode-1) * ode_step_size );
	double time_min = time_table[0];
	double time_max = time_table[ time_table.size() - 1];
	assert( time_max  <= time_min + (n_time_ode-1) * ode_step_size );
# endif
	// smoothing grid information
	size_t i_sg        = 0;
	size_t j_sg        = 0;
	double age_min_sg  = age_table[ sg.age_id(i_sg) ];
	double time_min_sg = time_table[ sg.time_id(j_sg) ];
	i_sg               = sg.age_size() - 1;
	j_sg               = sg.time_size() - 1;
	double age_max_sg  = age_table[ sg.age_id(i_sg) ];
	double time_max_sg = time_table[ sg.time_id(j_sg) ];

	// compute the coefficients for each computational grid point
	coefficient_.resize( n_age_ode * n_time_ode );
	for(i = 0; i < n_age_ode; i++)
	{	double age   = i * ode_step_size + age_table[0];
		//
		if( age <= age_min_sg )
			i_sg = 0;
		else if( age_max_sg <= age )
			i_sg = sg.age_size() - 1;
		else
		{	assert( sg.age_size() > 1 );
			while( age < age_table[ sg.age_id(i_sg) ] )
			{	assert( 0 < i_sg ); 
				i_sg--;
			}
			while( age_table[ sg.age_id(i_sg+1) ] < age )
			{	i_sg++;
				assert( i_sg+1 < sg.age_size() );
			}
		}
		bool two_age   = (age_min_sg < age) & ( age < age_max_sg); 
		//
		for(j = 0; j < n_time_ode; j++)
		{	// ode grid information
			double time  = j * ode_step_size + time_table[0];
			//
			if( time <= time_min_sg )
				j_sg = 0;
			else if( time_max_sg <= time )
				j_sg = sg.time_size() - 1;
			else
			{	assert( sg.time_size() > 1 );
				while( time < time_table[ sg.time_id(j_sg) ] )
				{	assert( 0 < j_sg ); 
					j_sg--;
				}
				while( time_table[ sg.time_id(j_sg+1) ] < time )
				{	j_sg++;
					assert( j_sg+1 < sg.time_size() );
				}
			}
			bool two_time   = (time_min_sg < time) & ( time < time_max_sg); 

			// coefficient index information
			size_t index = i * n_time_ode + j;
			coefficient_[index].i_sg = i_sg;
			coefficient_[index].j_sg = j_sg;

			double a0, a1, ca0, ca1;
			a0  = age_table[  sg.age_id(i_sg) ];
			if( two_age )
			{	a1  = age_table[  sg.age_id(i_sg+1) ];
				ca0  = (a1 - age) / (a1 - a0);
				ca1  = (age - a0) / (a1 - a0);
			}

			double t0, t1, ct0, ct1;
			t0  = time_table[  sg.time_id(j_sg) ];
			if( two_time )
			{	t1  = time_table[  sg.time_id(j_sg+1) ];
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
CppAD::vector<Float> smooth2ode<Float>::interpolate(
	const CppAD::vector<Float>& var_sg )
{	size_t i, j, k;
	assert( var_sg.size() == n_age_sg_ * n_time_sg_ );
	CppAD::vector<Float> var_ode( n_age_ode_ * n_time_ode_ );
	for(i = 0; i < n_age_ode_; i++)
	{	for(j = 0; j < n_time_ode_; j++)
		{	k = i * n_time_ode_ + j;
			size_t i_sg = coefficient_[k].i_sg;
			size_t j_sg = coefficient_[k].j_sg;
			double c_00 = coefficient_[k].c_00;
			double c_10 = coefficient_[k].c_10;
			double c_01 = coefficient_[k].c_01;
			double c_11 = coefficient_[k].c_11;
			Float  sum  = 0.0;
			sum      += c_00 * var_sg[i_sg*n_time_sg_ + j_sg];
			if( c_10 != 0.0 )
				sum += c_10 * var_sg[(i_sg+1)*n_time_sg_ + j_sg];
			if( c_01 != 0.0 )
				sum += c_01 * var_sg[i_sg*n_time_sg_ + (j_sg+1)];
			if( c_11 != 0.0 )
				sum += c_11 * var_sg[(i_sg+1)*n_time_sg_ + (j_sg+1)];
			var_ode[ i * n_time_ode_ + j ] = sum;
		}
	}
	return var_ode;
}

// instantiation 
template class smooth2ode<double>;
template class smooth2ode< CppAD::AD<double> >;

} // END DISMOD_AT_NAMESPACE

