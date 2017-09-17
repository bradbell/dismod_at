// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin interp_weight$$
$spell
	interpolant
	w_info
	wi
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
$icode%weight% = interp_weight(
	%age%, %time%, %w_info%, %age_table%, %time_table%, %i_wi%, %j_wi%
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

$head w_info$$
This argument has prototype
$codei%
	const weight_info& %w_info%
%$$
and is the weight grid. We use the following notation below:
$codei%
	%n_age%       = %w_info%.age_size()
	%n_time%      = %w_info%.time_size()
%$$

$head age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %age_table%
%$$
and is the age values corresponding to the weight grid.
We use the following notation below:
For $icode%i_wi% = 0 , %...%, %n_age%-1%$$,
$codei%
	%a%(%i_wi%) = %age_table% [ %w_info%.age_id(%i_wi%) ]
%$$

$head time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %time_table%
%$$
and is the time values corresponding to the weight grid.
We use the following notation below:
For $icode%j_wi% = 0 , %...%, %n_time%-1%$$,
$codei%
	%t%(%j_wi%) = %time_table% [ %w_info%.time_id(%j_wi%) ]
%$$

$head i_wi$$
This argument has prototype
$codei%
	size_t& %i_wi%
%$$
and is between $code 0$$ and $icode%n_age%-1%$$ inclusive.
Its input value is a hint as to where to start searching for
$codei%
	%a%(%i_wi%) <= %age% < %a%(%i_wi%+1)
%$$
Its output value satisfies the condition above,
or is as close as possible under the limits for $icode i_wi$$.

$head j_wi$$
This argument has prototype
$codei%
	size_t& %j_wi%
%$$
and is between $code 0$$ and $icode%n_time%-1%$$ inclusive.
Its input value is a hint as to where to start searching for
$codei%
	%t%(%j_wi%) <= %time% < %t%(%j_wi%+1)
%$$
Its output value satisfies the condition above,
or is as close as possible under the limits for $icode j_wi$$.

$head weight$$
The return value has prototype
$codei%
	double %weight%
%$$
It is the value of the
$cref bilinear$$ interpolant at the specified
$icode age$$ and $icode time$$.

$children%example/devel/utility/interp_weight_xam.cpp
%$$
$head Example$$
The file $cref interp_weight_xam.cpp$$ contains an example and test
of using this routine.

$end
------------------------------------------------------------------------------
*/

# include <dismod_at/interp_weight.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

double interp_weight(
	double                      age        ,
	double                      time       ,
	const weight_info&          w_info         ,
	const CppAD::vector<double> age_table  ,
	const CppAD::vector<double> time_table ,
	size_t&                     i_wi       ,
	size_t&                     j_wi       )
{
	size_t n_age   = w_info.age_size();
	size_t n_time  = w_info.time_size();

	assert( w_info.age_id(n_age-1)   < age_table.size()  );
	assert( w_info.time_id(n_time-1) < time_table.size() );

	if( n_age == 1 && n_time == 1 )
		return w_info.weight(0, 0);

	double age_min  = age_table[ w_info.age_id(0) ];
	double age_max  = age_table[ w_info.age_id(n_age-1) ];
	//
	if( age <= age_min )
		i_wi = 0;
	else if( age_max <= age )
		i_wi = n_age - 1;
	else
	{	assert( n_age > 1 );
		while( age < age_table[ w_info.age_id(i_wi) ] )
		{	assert( 0 < i_wi );
			i_wi--;
		}
		while( age_table[ w_info.age_id(i_wi+1) ] < age )
		{	i_wi++;
			assert( i_wi+1 < n_age );
		}
	}

	double time_min  = time_table[ w_info.time_id(0) ];
	double time_max  = time_table[ w_info.time_id(n_time-1) ];
	//
	if( time <= time_min )
		j_wi = 0;
	else if( time_max <= time )
		j_wi = n_time - 1;
	else
	{	assert( n_time > 1 );
		while( time < time_table[ w_info.time_id(j_wi) ] )
			j_wi--;
		while( time_table[ w_info.time_id(j_wi+1) ] < time )
			j_wi++;
	}

	bool two_age  = (age_min  < age)  & (age  < age_max);
	bool two_time = (time_min < time) & (time < time_max);

	double a0, t0, w00;
	a0  = age_table[  w_info.age_id(i_wi) ];
	t0  = time_table[ w_info.time_id(j_wi) ];
	w00 = w_info.weight(i_wi, j_wi);

	double nan = std::numeric_limits<double>::quiet_NaN();
	double a1 = nan, w10 = nan, da = nan;
	if( two_age )
	{	a1  = age_table[  w_info.age_id(i_wi+1) ];
		w10 = w_info.weight(i_wi+1, j_wi);
		da  = a1 - a0;
	}
	double t1 = nan, w01 = nan, dt = nan;
	if( two_time )
	{	t1  = time_table[  w_info.time_id(j_wi+1) ];
		w01 = w_info.weight(i_wi, j_wi+1);
		dt  = t1 - t0;
	}
	if( two_age & two_time )
	{	double w11 = w_info.weight(i_wi+1, j_wi+1);
		w00  = (a1 - age) * (t1 - time) * w00;
		w10  = (age - a0) * (t1 - time) * w10;
		w01  = (a1 - age) * (time - t0) * w01;
		w11  = (age - a0) * (time - t0) * w11;

		return (w00 + w10 + w01 + w11) / (da * dt);
	}
	if( two_age )
	{	w00 = (a1 - age) * w00;
		w10 = (age - a0) * w10;
		return (w00 + w10) / da;;
	}
	if( two_time )
	{	w00 = (t1 - time) * w00;
		w01 = (time - t0) * w01;
		return (w00 + w01) / dt;;
	}
	return w00;
}

} // END_DISMOD_AT_NAMESPACE

