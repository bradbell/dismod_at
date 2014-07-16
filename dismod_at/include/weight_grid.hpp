// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_WEIGHT_GRID_HPP
# define DISMOD_AT_WEIGHT_GRID_HPP
/*
$begin weight$$

$section Extract Information for One Weighting Function$$

$begin Syntax$$
$codei%weight_grid %wg%(
	%weight_id%,
	%weight_grid_table%
)%$$
$icode%n_age%  = %wg%.age_size()
%$$
$icode%n_time% = %wg%.time_size()
%$$
$icode%a_id%   = %wg%.age_id(%i%)
%$$
$icode%t_id%   = %wg%.time_id(%j%)
%$$
$icode%w%      = %wg%.weight(%i%, %j%)
%$$

$head Purpose$$
Extracts the information for one weighting from
the $cref weight_grid_table$$.

$head Constructor$$

$subhead weight_id$$
This argument has prototype
$codei%
	size_t %weight_id%
%$$
and is the $cref/weight_id/weight_grid_table/weight_id/$$ for the
weighting that $icode wg$$ corresponds to.

$subhead weight_grid_table$$
This argument has prototype
$codei%
	const CppAD::vector<weight_grid_struct>& %weight_grid_table%
%$$
an is the $cref weight_grid_table$$.

$subhead wg$$
This result has type $code weight_grid$$.
All of the funcition calls below are $code const$$; i.e.,
they do not modify $icode wg$$.

$head n_age$$
This result has prototype
$codei%
	size_t %n_age%
%$$
and is the number of age values for this weighting.

$head n_time$$
This result has prototype
$codei%
	size_t %n_time%
%$$
and is the number of time values for this weighting.

$head i$$
The argument $icode i$$ has prototype
$codei%
	size_t %i%
%$$
and is the age index; $icode%i% < %n_age%$$.

$head j$$
The argument $icode j$$ has prototype
$codei%
	size_t %j%
%$$
and is the time index; $icode%j% < %n_time%$$.

$head a_id$$
This return value has prototype
$codei%
	size_t %a_id%
%$$
and is the $th i$$ $cref/age_id/weight_grid_table/age_id/$$ 
for this weighting and increases with $icode i$$; i.e.,
for $icode%i% < %n_age%-2$$
$codei%
	%wg%.age_id(%i%) < %wg%.age_id(%i%+1)
%$$.

$head t_id$$
This return value has prototype
$codei%
	size_t %t_id%
%$$
and is the $th j$$ $cref/time_id/weight_grid_table/time_id/$$ 
for this weighting and increases with $icode j$$; i.e.,
for $icode%j% < %n_time%-2$$
$codei%
	%wg%.time_id(%j%) < %wg%.time_id(%j%+1)
%$$.

$head w$$
This return value has prototype
$codei%
	double %w%
%$$
and is the weighting for the corresponding age and time indices.


$end
*/
# include <cppad/cppad.hpp>
# include <dismod_at/include/get_weight_grid.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class weight_grid {
private:
	// grid of age values for this weighting
	CppAD::vector<size_t> age_id_;
	// grid of time values for this weighting
	CppAD::vector<size_t> time_id_;
	// vector of weights for each age, time pair
	CppAD::vector<double> weight_;
public:
	weight_grid(
	size_t                                   weight_id         ,
	const CppAD::vector<weight_grid_struct>& weight_grid_table
	);
	//
	size_t age_size(void) const
	{	return age_id_.size(); }
	size_t time_size(void) const
	{	return time_id_.size(); }
	//
	size_t age_id(size_t i) const
	{	return age_id_[i]; }

	size_t time_id(size_t j) const
	{	return time_id_[j]; }
	//
	const double weight(size_t i, size_t j) const
	{	assert( i < age_id_.size() );
		assert( j < time_id_.size() );
		return weight_[ i * time_id_.size() + j]; 
	}

};

} // END_DISMOD_AT_NAMESPACE

# endif
