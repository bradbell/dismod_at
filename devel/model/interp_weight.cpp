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
$begin interp_weight$$
$spell
	wg
	const
	CppAD
	interp
	dismod
$$

$index interpolate, weight$$
$index weight, interpolate$$
$index interp_weight$$

$section Interpolate Weight Grid$$

$head Syntax$$
$codei%interp_weight(
	%age%, %time%, %wg%, %age_table%, %time_table%, %i_wg%, %j_wg%
)%$$

$head age$$
This argument has prototype
$codei%
	double %age%
%$$
and is the age at which we are interpolating the weight grid values.

$head time$$
This argument has prototype
$codei%
	double %time%
%$$
and is the time at which we are interpolating the weight grid values.

$head wg$$
This argument has prototype
$codei%
	const dismod_at::weight_grid& %wg%
%$$
and is the weight grid. We use the following notation below:
$codei%
	%n_age%       = %wg%.age_size()
	%n_time%      = %wg%.time_size()
%$$

$head age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %age_table%
%$$
and is the age values corresponding to the weight grid.
We use the following notation below:
For $icode%i_wg% = 0 , %...%, %n_age%-1%$$,
$codei%
	%a%(%i_wg%) = %age_table% [ %wg%.age_id(%i%) ]
%$$

$head time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %time_table%
%$$
and is the time values corresponding to the weight grid.
We use the following notation below:
For $icode%j_wg% = 0 , %...%, %n_time%-1%$$,
$codei%
	%t%(%j_wg%) = %time_table% [ %wg%.time_id(%j%) ]
%$$

$head i_wg$$
This argument has prototype
$codei%
	size_t& %i_wg%
%$$
and is between $code 0$$ and $icode%n_age%-1%$$ inclusive.
Its input value is a hint as to where to start searching for
$icode%
	%a%(%i_wg%) <= %age% < %a%(%i_wg%+1)
%$$.
Its output value satisfies the condition above,
or is as close as possible under the limits for $icode i_wg$$.

$head j_wg$$
This argument has prototype
$codei%
	size_t& %j_wg%
%$$
and is between $code 0$$ and $icode%n_time%-1%$$ inclusive.
Its input value is a hint as to where to start searching for
$icode%
	%t%(%j_wg%) <= %time% < %t%(%j_wg%+1)
%$$.
Its output value satisfies the condition above,
or is as close as possible under the limits for $icode j_wg$$.

$end
------------------------------------------------------------------------------
*/

# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

double interp_weight(
	double                      age        ,
	double                      time       ,
	const weight_grid&          wg         ,
	const CppAD::vector<double> age_table  ,
	const CppAD::vector<double> time_table ,
	size_t&                     i_wg       ,
	size_t&                     j_wg       )
{
	size_t n_age   = wg.age_size();
	size_t n_time  = wg.time_size();

	assert( wg.age_id(n_age-1)   < age_table.size()  );
	assert( wg.time_id(n_time-1) < time_table.size() );

	if( n_age == 1 && n_time == 1 )
		return wg.weight(0, 0);

	double age_min  = age_table[ wg.age_id(0) ];
	double age_max  = age_table[ wg.age_id(n_age-1) ];
	//
	if( age <= age_min )
		i_wg = 0;
	else if( age_max <= age )
		i_wg = n_age - 1;
	else
	{	assert( n_age > 1 );
		while( age < age_table[ wg.age_id(i_wg) ] )
		{	assert( 0 < i_wg ); 
			i_wg--;
		}
		while( age_table[ wg.age_id(i_wg+1) ] < age )
		{	i_wg++;
			assert( i_wg+1 < n_age );
		}
	}

	double time_min  = time_table[ wg.time_id(0) ];
	double time_max  = time_table[ wg.time_id(n_time-1) ];
	//
	if( time <= time_min )
		j_wg = 0;
	else if( time_max <= time )
		j_wg = n_time - 1;
	else
	{	assert( n_time > 1 );
		while( time < time_table[ wg.time_id(j_wg) ] )
			j_wg--;
		while( time_table[ wg.time_id(j_wg+1) ] < time )
			j_wg++;
	}

	bool two_age  = (age_min  < age)  & (age  < age_max);
	bool two_time = (time_min < time) & (time < time_max);

	double a0, t0, w00;
	a0  = age_table[  wg.age_id(i_wg) ];
	t0  = time_table[ wg.time_id(j_wg) ];
	w00 = wg.weight(i_wg, j_wg);

	double a1, w10, da;
	if( two_age )
	{	a1  = age_table[  wg.age_id(i_wg+1) ];
		w10 = wg.weight(i_wg+1, j_wg);
		da  = a1 - a0;
	}
	double t1, w01, dt;
	if( two_time )
	{	t1  = time_table[  wg.time_id(j_wg+1) ];
		w01 = wg.weight(i_wg, j_wg+1);
		dt  = t1 - t0;
	}
	if( two_age & two_time )
	{	double w11 = wg.weight(i_wg+1, j_wg+1);
		w00  = (a1 - age) * (t1 - time) * w00 / (da * dt);
		w10  = (age - a0) * (t1 - time) * w10 / (da * dt);
		w01  = (a1 - age) * (time - t0) * w01 / (da * dt);
		w11  = (age - a0) * (time - t0) * w11 / (da * dt);

		return w00 + w10 + w01 + w11;
	}
	if( two_age )
	{	w00 = (a1 - age) * w00 / da;
		w10 = (age - a1) * w10 / da;
		return w00 + w10;
	}
	if( two_time )
	{	w00 = (t1 - time) * w00 / dt;
		w10 = (time - t1) * w10 / dt;
		return w00 + w10;
	}
	return w00;
}

} // END_DISMOD_AT_NAMESPACE

