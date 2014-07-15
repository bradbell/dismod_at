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
$begin weight$$

$section Interpolate From Smoothing Grid to ODE Grid$$

$begin Syntax$$
$codei%weight_grid %wg%(
	%weight_id%,
	%age_table%,
	%time_table%,
	%weight_grid_table%
)%$$
$icode%wg%.weight_id()
%$$
$icode%wg%.age_grid()
%$$
$icode%wg%.time_grid()
%$$
$icode%wg%.weight_grid()
%$$

$head Purpose$$
Extracts the information for one weighting from
the database input tables.


$end
*/

namespace { // BEGIN_DISMOD_AT_NAMESPACE

class weight_grid {
	using CppAD::vector;
private:
	// id value for this weighting
	size_t         weight_id_;
	// grid of age values for this weighting
	vector<double> age_grid_;
	// grid of time values for this weighting
	vector<double> time_grid_;
	// vector of weights for each age, time pair
	vector<double> weight_grid_;
public:
	weight_grid(
	size_t                            weight_id         ,
	const vector<double>&             age_table         ,
	const vector<double>&             time_table        ,
	const vector<weight_grid_struct>& weight_grid_table i
	);

	size_t weight_id(void) const
	{	return weight_id_; }

	const vector<double>& age_grid(void) const
	{	return age_grid_; }

	const vector<double>& time_grid(void) const
	{	return time_grid_; }

	const vector<double>& weight_grid(void) const
	{	return weight_grid_; }


} // END_DISMOD_AT_NAMESPACE
