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
$begin get_input_table$$
$spell
	sqlite
	enum
	cpp
	std
$$

$section C++: Get the Data Base Input Information$$
$index get, input tables$$
$index table, get input$$
$index input, get tables$$

$head Under Construction$$
This routine is under construction and not yet used.

$head Syntax$$
$icode%get_input_table(%db%)%$$

$head Purpose$$
To read all the input tables and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$end
-----------------------------------------------------------------------------
*/



# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void get_input_table(sqlite3* db, input_table_struct& input_table)
{	using CppAD::vector;
	
	input_table.age         = get_age_table(db);
	input_table.time        = get_time_table(db);
	input_table.rate        = get_rate_table(db);
	input_table.density     = get_density_table(db);
	input_table.integrand   = get_integrand_table(db);
	input_table.weight      = get_weight_table(db);
	input_table.smooth      = get_smooth_table(db);
	input_table.run         = get_run_table(db);
	input_table.covariate   = get_covariate_table(db);
	input_table.node        = get_node_table(db);
	input_table.like        = get_like_table(db);
	input_table.weight_grid = get_weight_grid(db);
	input_table.smooth_grid = get_smooth_grid(db);
	input_table.rate_prior  = get_rate_prior(db);
	input_table.multiplier  = get_multiplier_table(db);
	//
	size_t n_covariate = input_table.covariate.size();
	input_table.data        = get_data_table(db, n_covariate);

	return;
}

} // END DISMOD_AT_NAMESPACE
