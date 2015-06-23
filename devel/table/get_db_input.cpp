// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_db_input$$
$spell
	dage
	struct
	sqlite
	enum
	cpp
	std
$$

$section C++: Get the Data Base Input Tables$$

$head Syntax$$
$codei%get_db_input(%db%, %db_input%)%$$

$head Purpose$$
Read all the input tables and return them as a C++ data structure.
In addition, preform the following checks:

$subhead Primary Key$$
Check that all occurrences of $icode%table_name%_id%$$ are with in
the limit for the corresponding table.
Note that this only checks limits, and not positional dependent limits.
For example, $code -1$$ might appear anywhere in
$cref/dage_prior_id/smooth_grid_table/dage_prior_id/$$.

$subhead Initial Prevalence Grid$$
See $cref check_pini_n_age$$.

$subhead Child Priors$$
See $cref check_child_prior$$.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head db_input$$
The return value has prototype
$codei%
	db_input_struct& %db_input%
%$$
where $code db_input_struct$$ is defined by
$code
$verbatim%include/dismod_at/get_db_input.hpp%0%// BEGIN STRUCT%// END STRUCT%$$
$$
$pre
$$
Each $icode%name%_table%$$ above is defined by the corresponding
$codei%get_%name%_table%$$ routine.
For example, $codei%age_table%$$ is the return value of
$cref get_age_table$$ routine.
All of the tables must be empty when $code get_db_input$$ is called; i.e.,
the size of the corresponding vector must be zero.
Upon return, each table will have the corresponding database $icode db$$
information.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/get_db_input.hpp>
# include <dismod_at/get_age_table.hpp>
# include <dismod_at/get_time_table.hpp>
# include <dismod_at/check_pini_n_age.hpp>
# include <dismod_at/check_child_prior.hpp>


# define DISMOD_AT_CHECK_PRIMARY_ID(in_table, in_name, primary_table, lower)\
for(size_t i = 0; i < db_input.in_table ## _table.size(); i++) \
{	int id_value = db_input.in_table ## _table[i].in_name; \
	int upper = int( db_input.primary_table ## _table.size() ) - 1; \
	bool ok   = lower <= id_value && id_value <= upper; \
	if( ! ok ) \
	{	std::cerr << #in_name << "=" << id_value << " does not appear as " \
		<< #primary_table "_id in " << #primary_table " table" << std::endl \
		<< "Detected in " << #in_table << " table at " #in_table "_id =" \
		<< i << std::endl; \
		exit(1); \
	} \
}

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void get_db_input(sqlite3* db, db_input_struct& db_input)
{	using CppAD::vector;
	//
	assert( db_input.age_table.size() == 0 );
	assert( db_input.time_table.size() == 0 );
	assert( db_input.rate_table.size() == 0 );
	assert( db_input.density_table.size() == 0 );
	assert( db_input.integrand_table.size() == 0 );
	assert( db_input.weight_table.size() == 0 );
	assert( db_input.smooth_table.size() == 0 );
	assert( db_input.covariate_table.size() == 0 );
	assert( db_input.node_table.size() == 0 );
	assert( db_input.prior_table.size() == 0 );
	assert( db_input.weight_grid_table.size() == 0 );
	assert( db_input.smooth_grid_table.size() == 0 );
	assert( db_input.mulcov_table.size() == 0 );
	//
	db_input.age_table         = get_age_table(db);
	db_input.time_table        = get_time_table(db);
	db_input.rate_table        = get_rate_table(db);
	db_input.density_table     = get_density_table(db);
	db_input.integrand_table   = get_integrand_table(db);
	db_input.weight_table      = get_weight_table(db);
	db_input.smooth_table      = get_smooth_table(db);
	db_input.covariate_table   = get_covariate_table(db);
	db_input.node_table        = get_node_table(db);
	db_input.prior_table       = get_prior_table(db);
	db_input.weight_grid_table = get_weight_grid(db);
	db_input.smooth_grid_table = get_smooth_grid(db);
	db_input.mulcov_table      = get_mulcov_table(db);
	//
	size_t n_covariate   = db_input.covariate_table.size();
	db_input.data_table  = get_data_table(db, n_covariate);
	//
	// -----------------------------------------------------------------------
	// check primary keys
	//
	// node table
	DISMOD_AT_CHECK_PRIMARY_ID(node, parent, node, -1);

	// prior table
	DISMOD_AT_CHECK_PRIMARY_ID(prior, density_id, density, 0);

	// weight_grid table
	DISMOD_AT_CHECK_PRIMARY_ID(weight_grid, weight_id, weight, 0);

	// smooth table
	DISMOD_AT_CHECK_PRIMARY_ID(smooth, mulstd_value, prior, 0);
	DISMOD_AT_CHECK_PRIMARY_ID(smooth, mulstd_dage,  prior, 0);
	DISMOD_AT_CHECK_PRIMARY_ID(smooth, mulstd_dtime, prior, 0);

	// smooth_grid table
	DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, smooth_id, smooth, 0);
	DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, value_prior_id, prior, 0);
	DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, dage_prior_id, prior, -1);
	DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, dtime_prior_id, prior, -1);

	// mulcov table
	DISMOD_AT_CHECK_PRIMARY_ID(mulcov, rate_id, rate, -1);
	DISMOD_AT_CHECK_PRIMARY_ID(mulcov, integrand_id, integrand, -1);
	DISMOD_AT_CHECK_PRIMARY_ID(mulcov, covariate_id, covariate, 0);
	DISMOD_AT_CHECK_PRIMARY_ID(mulcov, smooth_id, smooth, 0);

	// data table
	DISMOD_AT_CHECK_PRIMARY_ID(data, integrand_id, integrand, 0);
	DISMOD_AT_CHECK_PRIMARY_ID(data, density_id,   density,   0);
	DISMOD_AT_CHECK_PRIMARY_ID(data, node_id,      node,      0);
	DISMOD_AT_CHECK_PRIMARY_ID(data, weight_id,    weight,    0);

	// rate table
	DISMOD_AT_CHECK_PRIMARY_ID(rate, parent_smooth_id, smooth, 0);
	DISMOD_AT_CHECK_PRIMARY_ID(rate, parent_smooth_id, smooth, 0);

	// -----------------------------------------------------------------------
	// other checks
	check_pini_n_age(
		db_input.rate_table       ,
		db_input.smooth_table
	);
	check_child_prior(
		db_input.rate_table        ,
		db_input.smooth_grid_table ,
		db_input.prior_table
	);

	return;
}

} // END DISMOD_AT_NAMESPACE
