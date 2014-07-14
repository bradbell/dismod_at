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
$begin get_db_input$$
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
$icode%input% = get_db_input(%db%)%$$

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
The return value has type $code db_input_struct$$
which is defined by
$code
$verbatim%dismod_at/include/get_db_input.hpp%0%// BEGIN STRUCT%// END STRUCT%$$
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

void get_db_input(sqlite3* db, db_input_struct& db_input)
{	using CppAD::vector;
	
	db_input.age_table         = get_age_table(db);
	db_input.time_table        = get_time_table(db);
	db_input.rate_table        = get_rate_table(db);
	db_input.density_table     = get_density_table(db);
	db_input.integrand_table   = get_integrand_table(db);
	db_input.weight_table      = get_weight_table(db);
	db_input.smooth_table      = get_smooth_table(db);
	db_input.run_table         = get_run_table(db);
	db_input.covariate_table   = get_covariate_table(db);
	db_input.node_table        = get_node_table(db);
	db_input.like_table        = get_like_table(db);
	db_input.weight_grid_table = get_weight_grid(db);
	db_input.smooth_grid_table = get_smooth_grid(db);
	db_input.rate_prior_table  = get_rate_prior(db);
	db_input.multiplier_table  = get_multiplier_table(db);
	//
	size_t n_covariate = db_input.covariate_table.size();
	db_input.data_table        = get_data_table(db, n_covariate);

	return;
}

} // END DISMOD_AT_NAMESPACE
