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
	struct
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
$icode%input% = get_input_table(%db%)%$$

$head Purpose$$
To read all the input tables and return it as a C++ data structure
and check for consistency between the tables..

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head Return$$
The return value has type $code input_table_struct$$
which is defined by
$code
$verbatim%dismod_at/include/get_input_table.hpp%0%// BEGIN STRUCT%// END STRUCT%$$
$$
$pre
$$
Each of the tables above is defined by the corresponding get routine. 
For example, the age table is the return value of
$cref get_age_table$$.

$end
-----------------------------------------------------------------------------
*/



# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void get_input_table(sqlite3* db, input_table_struct& input_table)
{	using CppAD::vector;
	
	input_table.age_table         = get_age_table(db);
	input_table.time_table        = get_time_table(db);
	input_table.rate_table        = get_rate_table(db);
	input_table.density_table     = get_density_table(db);
	input_table.integrand_table   = get_integrand_table(db);
	input_table.weight_table      = get_weight_table(db);
	input_table.smooth_table      = get_smooth_table(db);
	input_table.run_table         = get_run_table(db);
	input_table.covariate_table   = get_covariate_table(db);
	input_table.node_table        = get_node_table(db);
	input_table.like_table        = get_like_table(db);
	input_table.weight_grid_table = get_weight_grid(db);
	input_table.smooth_grid_table = get_smooth_grid(db);
	input_table.rate_prior_table  = get_rate_prior(db);
	input_table.multiplier_table  = get_multiplier_table(db);
	//
	size_t n_covariate = input_table.covariate_table.size();
	input_table.data_table        = get_data_table(db, n_covariate);

	return;
}

} // END DISMOD_AT_NAMESPACE
