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
$begin get_input_database$$
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
$codei%# include <get_input_database>
%$$
$icode%get_input_database(%db%)%$$

$head Purpose$$
To read the $cref database$$ and return it as a C++ data structure.

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

void get_input_database(sqlite3* db, input_struct& input_database)
{	using CppAD::vector;
	using std::string;
	
	input_database.age_table         = get_age_table(db);
	input_database.time_table        = get_time_table(db);
	input_database.rate_table        = get_rate_table(db);
	input_database.density_table     = get_density_table(db);
	input_database.integrand_table   = get_integrand_table(db);
	input_database.weight_table      = get_weight_table(db);
	input_database.smooth_table      = get_smooth_table(db);
	input_database.covariate_table   = get_covariate_table(db);
	input_database.node_table        = get_node_table(db);
	input_database.like_table        = get_like_table(db);
	input_database.weight_grid_table = get_weight_grid_table(db);
	input_database.smooth_grid_table = get_smooth_grid_table(db);
	input_database.rate_prior_table  = get_rate_prior_table(db);
	input_database.multiplier_table  = get_multiplier_table(db);
	//
	size_t n_covariate = input_database.covariate_table.size();
	input_database.data_table        = get_data_table(db);

	return age;
}

} // END DISMOD_AT_NAMESPACE
