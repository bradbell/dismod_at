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
	avgint
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
For example, $code null$$ might appear anywhere in
$cref/dage_prior_id/smooth_grid_table/dage_prior_id/$$.

$subhead Initial Prevalence Grid$$
See $cref check_pini_n_age$$.

$subhead Check Rate Limits$$
See $cref check_rate_limit$$.

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
# include <limits>
# include <dismod_at/get_db_input.hpp>
# include <dismod_at/get_age_table.hpp>
# include <dismod_at/get_time_table.hpp>
# include <dismod_at/check_pini_n_age.hpp>
# include <dismod_at/check_rate_limit.hpp>
# include <dismod_at/check_child_prior.hpp>
# include <dismod_at/null_int.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/error_exit.hpp>


# define DISMOD_AT_CHECK_PRIMARY_ID(in_table, in_name, primary_table)\
for(size_t row_id = 0; row_id < db_input.in_table ## _table.size(); row_id++) \
{	int id_value = db_input.in_table ## _table[row_id].in_name; \
	int upper = int( db_input.primary_table ## _table.size() ) - 1; \
	bool ok   = 0 <= id_value && id_value <= upper; \
	ok       |= id_value == DISMOD_AT_NULL_INT; \
	if( ! ok ) \
	{	table_name = #in_table; \
		message    = #in_name " = "; \
		message   += to_string( id_value ) + " does not appear as "; \
		message   += #primary_table "_id in " #primary_table " table"; \
		error_exit(db, message, table_name, row_id); \
	} \
}

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void get_db_input(sqlite3* db, db_input_struct& db_input)
{	using CppAD::vector;
	using CppAD::to_string;
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
	assert( db_input.option_table.size() == 0 );
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
	db_input.option_table      = get_option_table(db);
	//
	size_t n_covariate      = db_input.covariate_table.size();
	size_t n_age            = db_input.age_table.size();
	double age_min          = db_input.age_table[0];
	double age_max          = db_input.age_table[n_age - 1];
	size_t n_time           = db_input.time_table.size();
	double time_min         = db_input.time_table[0];
	double time_max         = db_input.time_table[n_time - 1];
	get_data_table(
		db, n_covariate, age_min, age_max, time_min, time_max,
		db_input.data_table, db_input.data_cov_value
	);
	get_avgint_table(
		db, n_covariate, age_min, age_max, time_min, time_max,
		db_input.avgint_table, db_input.avgint_cov_value
	);
	//
	// -----------------------------------------------------------------------
	// check primary keys
	// -----------------------------------------------------------------------
	std::string message, table_name;
	//
	// node table
	DISMOD_AT_CHECK_PRIMARY_ID(node, parent, node);

	// prior table
	DISMOD_AT_CHECK_PRIMARY_ID(prior, density_id, density);

	// weight_grid table
	DISMOD_AT_CHECK_PRIMARY_ID(weight_grid, weight_id, weight);

	// smooth table
	DISMOD_AT_CHECK_PRIMARY_ID(smooth, mulstd_value_prior_id, prior);
	DISMOD_AT_CHECK_PRIMARY_ID(smooth, mulstd_dage_prior_id,  prior);
	DISMOD_AT_CHECK_PRIMARY_ID(smooth, mulstd_dtime_prior_id, prior);

	// smooth_grid table
	DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, smooth_id,      smooth);
	DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, value_prior_id, prior);
	DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, dage_prior_id,  prior);
	DISMOD_AT_CHECK_PRIMARY_ID(smooth_grid, dtime_prior_id, prior);

	// mulcov table
	DISMOD_AT_CHECK_PRIMARY_ID(mulcov, rate_id,      rate);
	DISMOD_AT_CHECK_PRIMARY_ID(mulcov, integrand_id, integrand);
	DISMOD_AT_CHECK_PRIMARY_ID(mulcov, covariate_id, covariate);
	DISMOD_AT_CHECK_PRIMARY_ID(mulcov, smooth_id,    smooth);

	// data table
	DISMOD_AT_CHECK_PRIMARY_ID(data, integrand_id, integrand);
	DISMOD_AT_CHECK_PRIMARY_ID(data, density_id,   density);
	DISMOD_AT_CHECK_PRIMARY_ID(data, node_id,      node);
	DISMOD_AT_CHECK_PRIMARY_ID(data, weight_id,    weight);

	// avgint table
	DISMOD_AT_CHECK_PRIMARY_ID(avgint, integrand_id, integrand);
	DISMOD_AT_CHECK_PRIMARY_ID(avgint, node_id,      node);
	DISMOD_AT_CHECK_PRIMARY_ID(avgint, weight_id,    weight);

	// rate table
	DISMOD_AT_CHECK_PRIMARY_ID(rate, parent_smooth_id, smooth);
	DISMOD_AT_CHECK_PRIMARY_ID(rate, parent_smooth_id, smooth);

	// -----------------------------------------------------------------------
	// get rate_case
	std::string rate_case;
	size_t n_option = db_input.option_table.size();
	for(size_t option_id = 0; option_id < n_option; option_id++)
		if( db_input.option_table[option_id].option_name == "rate_case" )
			rate_case = db_input.option_table[option_id].option_value;
	assert( rate_case != "" );
	// -----------------------------------------------------------------------
	// other checks
	check_pini_n_age(
		db                        ,
		db_input.rate_table       ,
		db_input.smooth_table
	);
	check_rate_limit(
		db                        ,
		rate_case                 ,
		db_input.rate_table       ,
		db_input.prior_table      ,
		db_input.smooth_grid_table
	);
	check_child_prior(
		db                         ,
		db_input.rate_table        ,
		db_input.smooth_grid_table ,
		db_input.prior_table
	);

	return;
}

} // END DISMOD_AT_NAMESPACE
