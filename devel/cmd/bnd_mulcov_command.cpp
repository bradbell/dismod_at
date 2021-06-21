/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
         GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <gsl/gsl_randist.h>
# include <cppad/utility/to_string.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/get_mulcov_table.hpp>
# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/get_bnd_mulcov_table.hpp>
# include <dismod_at/bnd_mulcov_command.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
$begin bnd_mulcov_command$$
$spell
	Covariate
	dismod
	bnd_mulcov
	covariates
	mul
	cov
	py
	diff
	inf
$$

$section Bound The Covariate Multiplier Absolute Data Effect$$

$head Syntax$$
$codei%dismod_at %database% bnd_mulcov %max_abs_effect%$$

$head Purpose$$
This command is used to set the maximum absolute effect,
in the model for the data values,
for all the covariate multipliers.
This is done by changing the lower and upper bounds
for the covariate multipliers (ignoring bounds in the corresponding priors).
The $cref/meas_noise/mulcov_table/mulcov_type/meas_noise/$$
covariates are not included.

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head max_abs_effect$$
This is a non-negative value.
A covariate multiplier is defined by a row of the $cref mulcov_table$$.
We use the notation $icode mul_value$$ for a value of the multiplier.
There is a corresponding covariate for each multiplier.
We use the notation $icode cov_value$$ for a value of the
$cref/covariate/data_table/Covariates/$$ in the data table.
We use the notation $icode cov_ref$$ for the
$cref/reference/covariate_table/reference/$$ for the covariate.
The maximum effect condition is
$codei%
	| %mul_value% * (%cov_value% - %cov_ref%) | <= %max_abs_effect%
%$$
Note that the limits on the covariate multiplier in its prior have units
and the $icode max_abs_effect$$ does not have units.

$head bnd_mulcov_table$$
The table $cref bnd_mulcov_table$$ is and input and output for this command.
Its $cref/max_cov_diff/bnd_mulcov_table/max_cov_diff/$$ column is not changed.
The $cref/max_mulcov/bnd_mulcov_table/max_mulcov/$$ column is set so the
inequality above is true for all the data that is modeled using this
covariate multiplier and that is included in the fit.
To be specific, for each covariate multiplier
$codei%
	%max_mulcov% = %max_abs_effect% / %max_cov_diff%
%$$

$head Infinite Case$$
The case where $icode max_abs_effect$$ is $code inf$$ or
$icode max_cov_diff$$ is zero,
$icode max_mulcov$$ is set to null (which corresponds to plus infinity).

$comment 2DO: create the user_bnd_mulcov.py example$$
$head Example$$
The file $code user_bnd_mulcov.py$$ contains an example and test
using this command.

$end
*/
void bnd_mulcov_command(
	sqlite3*                                      db                ,
	std::string&                                  max_abs_effect    ,
	const CppAD::vector<mulcov_struct>&           mulcov_table      )
{	using std::string;
	using CppAD::vector;
	using CppAD::to_string;
	double inf = std::numeric_limits<double>::infinity();
	//
	// max_abs_effect
	double max_effect = inf;
	if( max_abs_effect != "inf" )
		max_effect = std::atoi( max_abs_effect.c_str() );
	//
	// n_mulcov
	size_t n_mulcov    = mulcov_table.size();
	//
	// get the current bnd_mulcov table
	vector<bnd_mulcov_struct> bnd_mulcov_table = get_bnd_mulcov_table(db);
	//
	// drop old bnd_mulcov table
	string sql_cmd    = "drop table bnd_mulcov";
	exec_sql_cmd(db, sql_cmd);
	//
	// write new data_subset table
	string table_name = "bnd_mulcov";
	size_t n_col      = 2;
	vector<string> col_name(n_col), col_type(n_col);
	vector<string> row_value(n_col * n_mulcov);
	vector<bool>   col_unique(n_col);
	//
	col_name[0]       = "max_mulcov";
	col_type[0]       = "real";
	col_unique[0]     = false;
	//
	col_name[1]       = "max_cov_diff";
	col_type[1]       = "real";
	col_unique[1]     = false;
	//
	for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
	{	double max_cov_diff = bnd_mulcov_table[mulcov_id].max_cov_diff;
		if( max_cov_diff == 0.0 || max_effect == inf )
			row_value[n_col * mulcov_id + 0] = "";
		else
		{	double max_mulcov = max_effect / max_cov_diff;
			row_value[n_col * mulcov_id + 0] = to_string( max_mulcov );
		}
		row_value[n_col * mulcov_id + 1] = to_string( max_cov_diff );
	}
	create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}
} // END_DISMOD_AT_NAMESPACE
