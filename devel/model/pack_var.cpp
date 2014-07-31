// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <cppad/cppad.hpp>
# include <dismod_at/dismod_at.hpp>

/*
$begin pack_var_ctor$$
$spell
	dismod
	var
	const
	integrands
$$

$section Pack Variables Constructor$$

$head Syntax$$
$codei%dismod_at::pack_var %var%(
	%n_smooth%, %n_integrand%
)
%$$
$icode%size%  = %var%.size()
%$$

$head n_smooth$$
This argument has prototype
$codei%
	size_t %n_smooth%
%$$
and is the number of smoothing; i.e., the size of
$cref/smooth_table/get_smooth_table/smooth_table/$$.

$head n_integrand$$
This argument has prototype
$codei%
	size_t %n_integrand%
%$$
and is the number of integrands; i.e., the size of
$cref/integrand_table/get_integrand_table/integrand_table/$$.

$head size$$
This function is $code const$$.
Its return value has prototype
$codei%
	size_t %size%
%$$
and is the total number of variables; i.e.,
the number of elements in the packed variable vector.

$end
*/

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

pack_var::pack_var(
	size_t      parent_node_id    ,
	size_t      n_smooth          ,
	size_t      n_integrand
) :
parent_node_id_( parent_node_id ) ,
n_smooth_( n_smooth )             ,
n_integrand_( n_integrand ) 
{	
	offset_value_mulstd_  = 0;
	offset_dage_mulstd_   = offset_value_mulstd_ + n_smooth_;
	offset_dtime_mulstd_  = offset_dage_mulstd_  + n_smooth_;

	size_                 = offset_dtime_mulstd_ + n_smooth_;
};

// size
size_t pack_var::size(void) const
{	return size_; }

/*
$begin pack_var_mulstd$$
$spell
	var
	mulstd
	dage
	dtime
	const
	dismod
$$

$section Pack Variables Standard Deviations Multipliers$$

$head Syntax$$
$icode%index% = %var%.value_mulstd(%smooth_id%)
%$$
$icode%index% = %var%.dage_mulstd(%smooth_id%)
%$$
$icode%index% = %var%.dtime_mulstd(%smooth_id%)
%$$

$head var$$
This object has prototype
$codei%
	const dismod_at::pack_var %var%
%$$.

$head smooth_id$$
This argument has prototype
$codei%
	size_t %smooth_id%
%$$
and is the 
$cref/smooth_id/smooth_table/smooth_id/$$.

$subhead index$$
The return value has prototype
$codei%
	size_t index
%$$
and is the unique index for the correspond smoothing standard
deviation multiplier in a packed variable vector.


$end

*/

// mulstd
size_t pack_var::value_mulstd(size_t smooth_id) const
{	assert( smooth_id < n_smooth_ );
	return offset_value_mulstd_ + smooth_id;
}
size_t pack_var::dage_mulstd(size_t smooth_id) const
{	assert( smooth_id < n_smooth_ );
	return offset_dage_mulstd_ + smooth_id;
}
size_t pack_var::dtime_mulstd(size_t smooth_id) const
{	assert( smooth_id < n_smooth_ );
	return offset_dtime_mulstd_ + smooth_id;
}


} // END DISMOD_AT_NAMESPACE
