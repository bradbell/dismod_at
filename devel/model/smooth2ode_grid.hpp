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

$section Interpolate From Smoothing Grid to ODE Grid$$

$end
*/

namespace { // BEGIN_DISMOD_AT_NAMESPACE

class smooth2ode_grid {
	using CppAD::vector;
	struct ode_weight_struct {
		// minimm age index for interpolating an ode grid point
		size_t age_index_;
		// minimum time index for interpolating an ode grid point 
		size_t time_index_;
		// weight for minimum age and time
		double weight_00_;
		// weight for minimum age and maximum time
		double weight_01_;
		// weight for maximum age and minimum time
		double weight_10_;
		// weight for maximum age and maximum time
		double weight_11_;
	}
	struct info_struct {
		// age_id in age table for this smoothing
		vector<size_t> age_id;
		// time_id in age table for this smoothing
		vector<size_t> time_id;
		// ode grid weights for this smoothing
		vector<ode_weight_struct> ode_weight;
	};
private:
	// number of ages in the ode grid
	const size_t              n_age_ode_;
	// number of times in the ode grid
	const size_t              n_time_ode_;
	// copy of the age table
	const vector<double>      age_table_;
	// copy of the time table
	const vector<double>      time_table_;

	// information for each smoothing
	vector<info_struct> info_;
public:
	smooth_info(
		const vector<double>&        age_table    ,
		const vector<double>&        time_table   ,
		const vector<smooth_struct>& smooth_table ,
		const vector<smooth_grid>&   smooth_grid
	);


} // END_DISMOD_AT_NAMESPACE
