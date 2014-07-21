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
	struct
	sg
	const
	dismod
	CppAD
$$

$section Coefficients for Interpolation from Smoothing to Ode Grid$$

$head Syntax$$
$icode%coefficient% = dismod_at::smooth2ode(
%sg%, %age_table%, %time_table%, %n_age_ode%, %n_time_ode%, %ode_step_size%
)%$$

$head sg$$
This argument has prototype
$codei%
	const dismod_at::smooth_grid& %sg%
%$$
and is the weight grid. We use the following notation below:
$codei%
	%n_age_sg%       = %sg%.age_size()
	%n_time_sg%      = %sg%.time_size()
%$$

$head age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %age_table%
%$$
and is the age values corresponding to the weight grid.
We use the following notation below:
For $icode%i_sg% = 0 , %...%, %n_age_sg%-1%$$,
$codei%
	%a%(%i_sg%) = %age_table% [ %sg%.age_id(%i_sg%) ]
%$$

$head time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %time_table%
%$$
and is the time values corresponding to the weight grid.
We use the following notation below:
For $icode%j_sg% = 0 , %...%, %n_time_sg%-1%$$,
$codei%
	%t%(%j_sg%) = %time_table% [ %sg%.time_id(%j_sg%) ]
%$$

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

$head coefficient$$
The return value has prototype
$codei%
	CppAD::vector<smooth2ode_struct>& coefficient
%$$
It size is equal to the number of ode grid points; i.e.
$codei%
	%coefficient%.size() == %n_age_ode% * %n_time_ode%
%$$
For $icode%i% = 0 , %...%, %n_age_ode%-1%$$,
and $icode%j% = 0 , %...%, %n_time_ode%-1%$$,
$codei%
	%coefficient%[ %i% * %n_time_ode% + %j% ]
%$$
is the interpolation coefficients for ode age
$cref/a_i/glossary/Ode Grid/Age, a_i/$$ and time
$cref/t_j/glossary/Ode Grid/Time, t_j/$$.
This is an $code smooth2ode_struct$$ with the following fields:

$subhead i_sg$$
This field has type $code size_t$$ and 
contains the $icode sg$$ age index for interpolating
from the smoothing grid to the ode grid.

$subhead j_sg$$
This field has type $code size_t$$ and 
contains the $icode sg$$ time index for interpolating
from the smoothing grid to the ode grid.

$subhead c_00$$
This file has type $code double$$ and contains
the coefficient for the smoothing grid index
$codei%(%i_sg%, %j_sg%)%$$.

$subhead c_10$$
This file has type $code double$$ and contains
the coefficient for the smoothing grid index
$codei%(%i_sg%+1, %j_sg%)%$$.

$subhead c_01$$
This file has type $code double$$ and contains
the coefficient for the smoothing grid index
$codei%(%i_sg%, %j_sg%+1)%$$.

$subhead c_11$$
This file has type $code double$$ and contains
the coefficient for the smoothing grid index
$codei%(%i_sg%+1, %j_sg%+1)%$$.


$head Interpolation$$
Suppose that 
for $icode%i_sg% = 0 , %...%, %n_age_sg%-1%$$,
and $icode%j_sg% = 0 , %...%, %n_time_sg%-1%$$,
$icode%v%[%i_sg%, %j_sg%]%$$ is the value of a variable
at the corresponding smoothing grid points.
The value of this variable at the $icode%(%i%, %j%)%$$ ode grid point is
$codei%
	%c_00% * %v%(%i_sg%,   %j_sg%  )   +
	%c_10% * %v%(%i_sg%+1, %j_sg%  )   +
	%c_01% * %v%(%i_sg%,   %j_sg%+1)   +
	%c_11% * %v%(%i_sg%+1, %j_sg%+1) 
%$$
where 
$icode c_00$$, 
$icode c_10$$, 
$icode c_01$$, 
$icode c_11$$, 
$icode i_sg$$,
$icode j_sg$$ are the field values corresponding to 
$codei%
	%coefficient%[ %i% * %n_time_ode% + %j% ]
%$$


$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<smooth2ode_struct> smooth2ode(
	const smooth_grid&                          sg            ,
	const CppAD::vector<double>&                age_table     ,
	const CppAD::vector<double>&                time_table    ,
	size_t                                      n_age_ode     ,
	size_t                                      n_time_ode    ,
	double                                      ode_step_size )
{	size_t i, j;	
	//
# ifndef NDEBUG
	double age_min = age_table[0];
	double age_max = age_table[ age_table.size() - 1];
	i = 1;
	while( age_min + (i-1) * ode_step_size < age_max )
		i++;
	assert( i == n_age_ode );
	double time_min = time_table[0];
	double time_max = time_table[ time_table.size() - 1];
	j = 1;
	while( time_min + (j-1) * ode_step_size < time_max )
		i++;
	assert( j == n_time_ode );
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
	CppAD::vector<smooth2ode_struct> coefficient( n_age_ode * n_time_ode );
	for(i = 0; i < n_age_ode; i++)
	{	double age   = i * ode_step_size;
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
			double time  = j * ode_step_size;
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
			coefficient[index].i_sg = i_sg;
			coefficient[index].j_sg = j_sg;

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
			{	coefficient[index].c_00 = ca0 * ct0;
				coefficient[index].c_10 = ca1 * ct0;
				coefficient[index].c_01 = ca0 * ct1;
				coefficient[index].c_11 = ca1 * ct1;
			}
			else if( two_age )
			{	coefficient[index].c_00 = ca0;
				coefficient[index].c_10 = ca1;
				coefficient[index].c_01 = 0.0;
				coefficient[index].c_11 = 0.0;
			}
			else if( two_time )
			{	coefficient[index].c_00 = ct0;
				coefficient[index].c_10 = 0.0;
				coefficient[index].c_01 = ct1;
				coefficient[index].c_11 = 0.0;
			}
			else
			{	coefficient[index].c_00 = 1.0;
				coefficient[index].c_10 = 0.0;
				coefficient[index].c_01 = 0.0;
				coefficient[index].c_11 = 0.0;
			}
		}
	}
	


}

} // END DISMOD_AT_NAMESPACE
