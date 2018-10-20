// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <cppad/mixed/exception.hpp>
# include <dismod_at/set_command.hpp>
# include <dismod_at/depend_command.hpp>
# include <dismod_at/init_command.hpp>

# include <map>
# include <cassert>
# include <string>
# include <iostream>
# include <cppad/utility/vector.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>
# include <dismod_at/min_max_vector.hpp>
# include <dismod_at/avgint_subset.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/get_column_max.hpp>
# include <dismod_at/get_db_input.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_sample_table.hpp>
# include <dismod_at/get_data_sim_table.hpp>
# include <dismod_at/get_prior_sim_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/sim_random.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/depend.hpp>
# include <dismod_at/get_prior_mean.hpp>
# include <dismod_at/ode_age_grid.hpp>

# define DISMOD_AT_TRACE 0

namespace { // BEGIN_EMPTY_NAMESPACE
	using CppAD::vector;

/*
-----------------------------------------------------------------------------
$begin fit_command$$
$spell
	sim
	nslist
	avgint
	var
	arg
	num_iter
	dismod
	py
	std
$$

$section The Fit Command$$

$head Syntax$$
$codei%dismod_at %database% fit %variables%
%$$
$codei%dismod_at %database% fit %variables% %simulate_index%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head variables$$
This argument is $code fixed$$, $code random$$ or $code both$$.
If it is $code fixed$$ ($code random$$)
only the fixed effects (random effects) are optimized.
If it is $code both$$, both the fixed and random effects
are optimized.

$subhead fixed$$
This option optimizes the fixed effects with the
random effects corresponding to
$cref/constant child value priors
	/rate_table
	/Child Value Priors
	/Constant
/$$
at their constrained values and the other random effects
constrained to be zero.
(This is equivalent to fitting with
$cref/bound_random/option_table/Random Only/bound_random/$$ equal to zero.)
This is useful when one uses fitting with no random effects as
a starting point for fitting with random effects; see
$cref set_command$$,
$cref start_var_table$$, and
$cref/fit_fixed_both.py/user_fit_fixed_both.py/$$.
This enables one to see the different between the two
fits in the $cref log_table$$
(as apposed to changing
$cref/bound_random/option_table/Random Only/bound_random/$$
in the $code option$$ table).

$subhead random$$
This optimizes the random effects with
the fixed effects set to their starting values; see
$cref start_var_table$$.

$subhead both$$
This option fits both the
$cref/fixed/model_variables/Fixed Effects, theta/$$ and
$cref/random/model_variables/Random Effects, u/$$ effects.

$head simulate_index$$
If $icode simulate_index$$ is present, it must be less than
$cref/number_simulate/simulate_command/number_simulate/$$.

$subhead data_sim_table$$
If $icode simulate_index$$ is present, this is an extra input table.
The $cref/data_sim_value/data_sim_table/data_sim_value/$$ entries,
corresponding to $icode simulate_index$$,
are used in place of the data table
$cref/meas_value/data_table/meas_value/$$ entries.
All the rest of the data table values
are the same as when $icode simulated_index$$ is not present; e.g.,
$cref/meas_std/data_table/meas_std/$$ comes from the data table.

$subhead prior_sim_table$$
If $icode simulate_index$$ is present, this is an extra input table.
The $cref/prior_sim_value/prior_sim_table/prior_sim_value/$$ entries,
corresponding to $icode simulate_index$$,
are used in place of the prior table
$cref/mean/prior_table/mean/$$ entries.
All the rest of the prior table values
are the same as when $icode simulated_index$$ is not present; e.g.,
$cref/std/prior_table/std/$$ comes from the prior table.

$head fit_var_table$$
A new $cref fit_var_table$$ is created each time this command is run.
It contains the results of the fit in its
$cref/fit_var_value/fit_var_table/fit_var_value/$$ column.

$head fit_data_subset_table$$
A new $cref fit_data_subset_table$$ is created each time this command is run.
It is a comparison of the model and data corresponding to the fit results.

$head Random Effects$$
A model has random effects if one of the
$cref/child_smooth_id/rate_table/child_smooth_id/$$ or
$cref/child_nslist_id/rate_table/child_nslist_id/$$ is not $code null$$.
In this case, it is suggest that you
first fit with $icode variables$$ equal to $code fixed$$
and then fit with $icode variables$$ equal to $code both$$.

$head Convergence Problems$$
The derivative of the fixed effects objective,
at the prior mean for the fixed effects,
is used to scale the fixed effects objective.
If the optimal value for a fixed effect is equal to its prior mean,
its partial derivative at the prior mean is often near zero.
Near zero derivatives, at the prior mean, cause the scaling to be unstable
and result in convergence problems.
You may be able to fix this
by using a uniform distribution, and a different prior mean,
for a fixed effect that does not change.
If this does not work, the data will not determine that fixed effect
and you may have to constrain it to have the value you want.

$children%example/get_started/fit_command.py
%$$
$head Example$$
The file $cref fit_command.py$$ contains an example and test
using this command.

$end
*/

// ----------------------------------------------------------------------------
// data_subset_obj and prior_object are const when simulate_index == ""
void fit_command(
	const std::string&                           variables        ,
	const std::string&                           simulate_index   ,
	sqlite3*                                     db               ,
	vector<dismod_at::data_subset_struct>&       data_subset_obj  ,
	dismod_at::data_model&                       data_object      ,
	dismod_at::prior_model&                      prior_object     ,
	const dismod_at::pack_info&                  pack_object      ,
	const dismod_at::pack_prior&                 var2prior        ,
	const dismod_at::db_input_struct&            db_input         ,
	// effectively const
	std::map<std::string, std::string>&          option_map
)
{	using std::string;
	using CppAD::to_string;
	// -----------------------------------------------------------------------
	bool ok = variables == "fixed";
	ok     |= variables == "random";
	ok     |= variables == "both";
	if( ! ok )
	{	string msg = "dismod_at fit command variables = ";
			msg += variables + "\nis not 'fixed', 'random' or 'both'";
			dismod_at::error_exit(msg);
	}
	//
	// bound_random
	double bound_random = 0.0;
	if( variables != "fixed" )
	{	// null corresponds to infinity
		std::string tmp_str = option_map["bound_random"];
		if( tmp_str == "" )
			bound_random = std::numeric_limits<double>::infinity();
		else
			bound_random = std::atof( tmp_str.c_str() );
	}
	// random_only
	bool random_only = variables == "random";
	// -----------------------------------------------------------------------
	if( simulate_index != "" )
	{	size_t sim_index = std::atoi( simulate_index.c_str() );
		//
		// get simulation data
		vector<dismod_at::data_sim_struct> data_sim_table =
			dismod_at::get_data_sim_table(db);
		vector<dismod_at::prior_sim_struct> prior_sim_table =
			dismod_at::get_prior_sim_table(db);
		size_t n_var      = var2prior.size();
		size_t n_subset   = data_subset_obj.size();
		size_t n_simulate = prior_sim_table.size() / n_var;
		//
		if( sim_index >= n_simulate )
		{	string msg = "dismod_at fit command simulate_index = ";
			msg += simulate_index + "\nis greater than or equal ";
			msg += "number of samples in the data_sim table.";
			string table_name = "data_sim";
			dismod_at::error_exit(msg, table_name);
		}
		// vector used for replacement of prior means
		vector<double> prior_mean(n_var * 3);
		for(size_t var_id = 0; var_id < n_var; ++var_id)
		{	size_t prior_sim_id = sim_index * n_var + var_id;
			prior_mean[var_id * 3 + 0] =
				prior_sim_table[prior_sim_id].prior_sim_value;
			prior_mean[var_id * 3 + 1] =
				prior_sim_table[prior_sim_id].prior_sim_dage;
			prior_mean[var_id * 3 + 2] =
				prior_sim_table[prior_sim_id].prior_sim_dtime;
		}
		prior_object.replace_mean(prior_mean);

		// replace meas_value in data_subset_obj
		for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
		{	size_t data_sim_id = n_subset * sim_index + subset_id;
			data_subset_obj[subset_id].meas_value =
				data_sim_table[data_sim_id].data_sim_value;
		}
	}
	data_object.replace_like(data_subset_obj);
	// -----------------------------------------------------------------------
	// read start_var table into start_var
	vector<double> start_var;
	string table_name = "start_var";
	string column_name = "start_var_value";
	dismod_at::get_table_column(db, table_name, column_name, start_var);
	// -----------------------------------------------------------------------
	// read scale_var table into scale_var
	vector<double> scale_var;
	table_name = "scale_var";
	column_name = "scale_var_value";
	dismod_at::get_table_column(db, table_name, column_name, scale_var);
	// ----------------------------------------------------------------------
	// zero_sum_random
	size_t n_rate      = size_t(dismod_at::number_rate_enum);
	size_t option_size = option_map["zero_sum_random"].size();
	vector<bool> zero_sum_random(n_rate);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	string rate_name = dismod_at::get_rate_name(rate_id);
		size_t found     = option_map["zero_sum_random"].find( rate_name );
		zero_sum_random[rate_id] = found < option_size;
	}
	// ------------------ run fit_model ------------------------------------
	// quasi_fixed
	bool quasi_fixed = option_map["quasi_fixed"] == "true";
	//
	// warn_on_stderr
	bool warn_on_stderr = option_map["warn_on_stderr"] == "true";
	//
	string fit_or_sample = "fit";
	dismod_at::fit_model fit_object(
		db                   ,
		warn_on_stderr       ,
		bound_random         ,
		fit_or_sample        ,
		pack_object          ,
		var2prior            ,
		start_var            ,
		scale_var            ,
		db_input.prior_table ,
		prior_object         ,
		quasi_fixed          ,
		zero_sum_random      ,
		data_object
	);
	fit_object.run_fit(random_only, option_map);
	vector<double> opt_value, lag_value, lag_dage, lag_dtime;
	fit_object.get_solution(
		opt_value, lag_value, lag_dage, lag_dtime
	);
	// -------------------- fit_var table --------------------------------------
	string sql_cmd = "drop table if exists fit_var";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name   = "fit_var";
	size_t n_var = opt_value.size();
	size_t n_col = 7;
	vector<string> col_name(n_col), col_type(n_col), row_value(n_col * n_var);
	vector<bool>   col_unique(n_col);
	//
	col_name[0]       = "fit_var_value";
	col_type[0]       = "real";
	col_unique[0]     = false;
	//
	col_name[1]       = "residual_value";
	col_type[1]       = "real";
	col_unique[1]     = false;
	//
	//
	col_name[2]       = "residual_dage";
	col_type[2]       = "real";
	col_unique[2]     = false;
	//
	col_name[3]       = "residual_dtime";
	col_type[3]       = "real";
	col_unique[3]     = false;
	//
	col_name[4]       = "lagrange_value";
	col_type[4]       = "real";
	col_unique[4]     = false;
	//
	col_name[5]       = "lagrange_dage";
	col_type[5]       = "real";
	col_unique[5]     = false;
	//
	col_name[6]       = "lagrange_dtime";
	col_type[6]       = "real";
	col_unique[6]     = false;
	//
	for(size_t var_id = 0; var_id < n_var; var_id++)
	{
		// fit_var_value
		row_value[var_id * n_col + 0] = to_string( opt_value[var_id] );
		//
		// initialzie residuals as empty (null in database)
		for(size_t k = 1; k < 4; k++)
			row_value[var_id * n_col + k] = "";
		//
		// lagrange_value
		row_value[var_id * n_col + 4] = to_string( lag_value[var_id] );
		//
		// lagrange_dage
		row_value[var_id * n_col + 5] = to_string( lag_dage[var_id] );
		//
		// lagrange_dtime
		row_value[var_id * n_col + 6] = to_string( lag_dtime[var_id] );
	}
	// residual_value, residual_dage, residual_dtime
	for(size_t variable_type = 0; variable_type < 2; variable_type++)
	{	vector< dismod_at::residual_struct<double> > residual;
		if( variable_type == 0 )
			residual  = prior_object.fixed(opt_value);
		else
			residual  = prior_object.random(opt_value);
		//
		for(size_t i = 0; i < residual.size(); i++)
		{	size_t index  = residual[i].index;
			size_t var_id = index / 3;
			size_t k      = 1 + index % 3;
			assert( var_id < n_var );
			row_value[var_id * n_col + k] = to_string( residual[i].wres );
		}
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	// ------------------ fit_data_subset table --------------------------------
	sql_cmd = "drop table if exists fit_data_subset";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	size_t n_subset = data_subset_obj.size();
	table_name      = "fit_data_subset";
	//
	n_col           = 2;
	col_name.resize(n_col);
	col_type.resize(n_col);
	col_unique.resize(n_col);
	row_value.clear();
	row_value.resize(n_col * n_subset);
	//
	col_name[0]   = "avg_integrand";
	col_type[0]   = "real";
	col_unique[0] = false;
	//
	col_name[1]   = "weighted_residual";
	col_type[1]   = "real";
	col_unique[1] = false;
	//
	bool parent_only = false;
	CppAD::vector<double> reference_sc =
		data_object.reference_ode(opt_value, parent_only);
	//
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	// compute average integrand for this data item
		double avg;
		size_t integrand_id = data_subset_obj[subset_id].integrand_id;
		dismod_at::integrand_enum integrand =
			db_input.integrand_table[integrand_id].integrand;
		switch( integrand )
		{	case dismod_at::Sincidence_enum:
			case dismod_at::remission_enum:
			case dismod_at::mtexcess_enum:
			case dismod_at::mtother_enum:
			case dismod_at::mtwith_enum:
			case dismod_at::relrisk_enum:
			avg = data_object.avg_no_ode(subset_id, opt_value);
			break;

			case dismod_at::susceptible_enum:
			case dismod_at::withC_enum:
			case dismod_at::prevalence_enum:
			case dismod_at::Tincidence_enum:
			case dismod_at::mtspecific_enum:
			case dismod_at::mtall_enum:
			case dismod_at::mtstandard_enum:
			avg = data_object.avg_yes_ode(subset_id, opt_value, reference_sc);
			break;

			default:
			assert(false);
		}
		// compute its residual and log likelihood
		double not_used;
		dismod_at::residual_struct<double> residual =
			data_object.like_one(subset_id, opt_value, avg, not_used);
		//
		row_value[ subset_id * n_col + 0] = to_string( avg );
		row_value[ subset_id * n_col + 1] = to_string( residual.wres );
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}

/*
-----------------------------------------------------------------------------
$begin simulate_command$$

$section The Simulate Command$$
$spell
	sim
	avgint
	var
	dismod
	arg
	std
	covariates
$$

$head Syntax$$
$codei%dismod_at %database% simulate %number_simulate%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head number_simulate$$
Is the number of simulations. Each simulation contains a complete
data set.

$head meas_value$$
The $cref/meas_value/data_table/meas_value/$$ in the data
table is not used.

$head truth_var_table$$
The $cref truth_var_table$$ is an addition input table for this command.
It specifies the values for the
$cref model_variables$$ used during the simulation.
This table can be create by the
$cref/set_command/set_command/table_out/truth_var/$$,
or the user can create it directly with the aid of the
$cref var_table$$ (created by the $cref init_command$$).


$head data_sim_table$$
A new $cref data_sim_table$$ is created by this command.
It contains $icode number_simulate$$ values
for each $cref/data_id/data_subset_table/data_id/$$ in the data_subset table.
Hence the number of rows in $cref data_sim_table$$ is
$icode number_simulate$$ times the number of rows in $cref data_subset_table$$.

$head prior_sim_table$$
A new $cref prior_sim_table$$ is created by this command.
It contains $icode number_simulate$$ values
for each $cref/var_id/var_table/var_id/$$ in the var table.
Hence the number of rows in $cref data_sim_table$$ is
$icode number_simulate$$ times the number of rows in $cref var_table$$.


$children%example/get_started/simulate_command.py
%$$
$head Example$$
The file $cref simulate_command.py$$ contains an example and test
using this command.

$end
*/
void simulate_command(
	const std::string&                                  number_simulate ,
	sqlite3*                                            db              ,
	const vector<dismod_at::integrand_struct>&          integrand_table ,
	const vector<dismod_at::data_subset_struct>&        data_subset_obj ,
	const dismod_at::data_model&                        data_object     ,
	const dismod_at::pack_prior&                        var2prior       ,
	const vector<dismod_at::prior_struct>&              prior_table
)
{
	using std::string;
	using CppAD::to_string;
	string msg;
	// -----------------------------------------------------------------------
	int tmp = std::atoi( number_simulate.c_str() );
	if( tmp <= 0 )
	{	msg  = "dismod_at simulate command number_simulate = ";
		msg += number_simulate + " is not an integer greater than zero";
		dismod_at::error_exit(msg);
	}
	size_t n_simulate = size_t(tmp);
	// -----------------------------------------------------------------------
	// read truth_var table into truth_var
	vector<double> truth_var;
	string table_name = "truth_var";
	string column_name = "truth_var_value";
	dismod_at::get_table_column(db, table_name, column_name, truth_var);
	// ----------------- data_sim_table ----------------------------------
	string sql_cmd = "drop table if exists data_sim";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name      = "data_sim";
	size_t n_col    = 4;
	size_t n_subset = data_subset_obj.size();
	size_t n_row    = n_simulate * n_subset;
	vector<string> col_name(n_col), col_type(n_col), row_value(n_col * n_row);
	vector<bool>   col_unique(n_col);
	//
	col_name[0]   = "simulate_index";
	col_type[0]   = "integer";
	col_unique[0] = false;
	//
	col_name[1]   = "data_subset_id";
	col_type[1]   = "integer";
	col_unique[1] = false;
	//
	col_name[2]   = "data_sim_value";
	col_type[2]   = "real";
	col_unique[2] = false;
	//
	col_name[3]   = "data_sim_delta";
	col_type[3]   = "real";
	col_unique[3] = false;
	//
	bool parent_only = false;
	CppAD::vector<double> reference_sc =
		data_object.reference_ode(truth_var, parent_only);
	//
	// for each measurement in the data_subset table
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	//
		// compute the average integrand, avg
		size_t integrand_id =  data_subset_obj[subset_id].integrand_id;
		dismod_at::integrand_enum integrand =
			integrand_table[integrand_id].integrand;
		double avg;
		switch( integrand )
		{	case dismod_at::Sincidence_enum:
			case dismod_at::remission_enum:
			case dismod_at::mtexcess_enum:
			case dismod_at::mtother_enum:
			case dismod_at::mtwith_enum:
			case dismod_at::relrisk_enum:
			avg = data_object.avg_no_ode(subset_id, truth_var);
			break;

			case dismod_at::susceptible_enum:
			case dismod_at::withC_enum:
			case dismod_at::prevalence_enum:
			case dismod_at::Tincidence_enum:
			case dismod_at::mtspecific_enum:
			case dismod_at::mtall_enum:
			case dismod_at::mtstandard_enum:
			avg = data_object.avg_yes_ode(subset_id, truth_var, reference_sc);
			break;

			default:
			avg = std::numeric_limits<double>::quiet_NaN();
			assert(false);
		}
		//
		// compute the adjusted standard deviation corresponding
		// to the values in the data table, delta.
		double sim_delta;
		data_object.like_one(subset_id, truth_var, avg, sim_delta);
		//
		// density corresponding to this data point
		dismod_at::density_enum density =
			dismod_at::density_enum( data_subset_obj[subset_id].density_id );
		//
		double difference   = false;
		double eta          = data_subset_obj[subset_id].eta;
		double nu           = data_subset_obj[subset_id].nu;
		//
		for(size_t sim_index = 0; sim_index < n_simulate; sim_index++)
		{	// for each simulate_index
			//
			// compute the simulated measurement value
			double sim_value   = dismod_at::sim_random(
				difference, density, avg, sim_delta, eta, nu
			);
			if( density == dismod_at::log_gaussian_enum
			||  density == dismod_at::log_laplace_enum
			||  density == dismod_at::log_students_enum )
				sim_value = std::max(sim_value, 0.0);
			//
			size_t data_sim_id = sim_index * n_subset + subset_id;
			row_value[data_sim_id * n_col + 0] = to_string( sim_index );
			row_value[data_sim_id * n_col + 1] = to_string( subset_id );
			row_value[data_sim_id * n_col + 2] = to_string( sim_value );
			row_value[data_sim_id * n_col + 3] = to_string( sim_delta );
		}
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	// ----------------- prior_sim_table ----------------------------------
	sql_cmd = "drop table if exists prior_sim";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name    = "prior_sim";
	n_col         = 5;
	size_t n_var  = var2prior.size();
	n_row         = n_simulate * n_var;
	col_name.resize(n_col);
	col_type.resize(n_col);
	col_unique.resize(n_col);
	row_value.resize(n_col * n_row);
	//
	col_name[0]   = "simulate_index";
	col_type[0]   = "integer";
	col_unique[0] = false;
	//
	col_name[1]   = "var_id";
	col_type[1]   = "integer";
	col_unique[1] = false;
	//
	col_name[2]   = "prior_sim_value";
	col_name[3]   = "prior_sim_dage";
	col_name[4]   = "prior_sim_dtime";
	for(size_t k = 0; k < 3; k++)
	{	col_type[2+k]   = "real";
		col_unique[2+k] = false;
	}
	// for each measurement in the data_subset table
	for(size_t var_id = 0; var_id < n_var; ++var_id)
	{	//
		// prior id for mean of this this variable
		size_t prior_id[3];
		vector<string> sim_str(3);
		prior_id[0] = var2prior.value_prior_id(var_id);
		prior_id[1] = var2prior.dage_prior_id(var_id);
		prior_id[2] = var2prior.dtime_prior_id(var_id);
		for(size_t sim_index = 0; sim_index < n_simulate; sim_index++)
		{	for(size_t k = 0; k < 3; ++k)
			if( prior_id[k] == DISMOD_AT_NULL_SIZE_T )
				sim_str[k] = "null";
			else
			{	double lower = prior_table[ prior_id[k] ].lower;
				double upper = prior_table[ prior_id[k] ].upper;
				double mean  = prior_table[ prior_id[k] ].mean;
				double std   = prior_table[ prior_id[k] ].std;
				double eta   = prior_table[ prior_id[k] ].eta;
				double nu    = prior_table[ prior_id[k] ].nu;
				//
				// k = 0 is a value prior, others are difference priors
				bool difference = k > 0;
				//
				int    den   = prior_table[prior_id[k]].density_id;
				dismod_at::density_enum density = dismod_at::density_enum(den);
				//
				if( density == dismod_at::uniform_enum )
					sim_str[k] = "null";
				else
				{	double sim = dismod_at::sim_random(
						difference, density, mean, std, eta, nu
					);
					//
					sim = std::min(sim, upper);
					sim = std::max(sim, lower);
					//
					if( density == dismod_at::log_gaussian_enum
					||  density == dismod_at::log_laplace_enum
					||  density == dismod_at::log_students_enum )
						sim = std::max(sim, 0.0);
					//
					if( density == dismod_at::laplace_enum
					&& k == 0 && mean == 0.0 )
						sim = 0.0;
					//
					sim_str[k] = to_string( sim );
				}
			}
			//
			size_t prior_sim_id = sim_index * n_var + var_id;
			row_value[prior_sim_id * n_col + 0] = to_string( sim_index );
			row_value[prior_sim_id * n_col + 1] = to_string( var_id );
			row_value[prior_sim_id * n_col + 2] = sim_str[0];
			row_value[prior_sim_id * n_col + 3] = sim_str[1];
			row_value[prior_sim_id * n_col + 4] = sim_str[2];
		}
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}
/*
-------------------------------------------------------------------------------
$begin sample_command$$
$spell
	sim
	avgint
	dismod
	var
	arg
$$

$section The Sample Command$$

$head Syntax$$
$codei%dismod_at %database% sample %method% %number_sample%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head Purpose$$
This command simulates samples of the $cref model_variables$$
from the posterior distribution; see
$cref/simulation/posterior/Simulation/$$ for the posterior distribution.

$head number_sample$$
Is the number of samples. Each sample contains a complete
set of model variables. See the different methods below
for more discussion of $icode number_sample$$.

$head method$$
The sample command argument $icode method$$ must be one of the following:

$subhead simulate$$
If $icode method$$ is $code simulate$$,
$icode number_sample$$ must be equal to
$cref/number_simulate/simulate_command/number_simulate/$$.
The variable sample corresponding to each
$cref/sample_index/sample_table/sample_index/$$ the sample table
is the optimal estimate corresponding to the
$cref data_sim_table$$ and the $cref prior_sim_table$$ with
$cref/simulate_index/data_sim_table/simulate_index/$$ equal to
the sample index.
This requires running $icode number_sample$$ fits of the model variables.
See $cref/simulation/posterior/Simulation/$$ in the discussion of the
posterior distribution of maximum likelihood estimates.

$subhead asymptotic$$
If $icode method$$ is $code asymptotic$$,
The asymptotic statics of the model variables is used to generate
$icode number_sample$$ samples of the model variables
The samples with different values of $icode sample_index$$ are independent.
(Note that the $cref fit_var_table$$ is an additional input in this case.)

$head data_sim_table$$
If $icode method$$ is $code simulate$$,
this command has the extra input $cref data_sim_table$$
which was created by the previous $cref simulate_command$$.

$head prior_sim_table$$
If $icode method$$ is $code simulate$$,
this command has the extra input $cref prior_sim_table$$
which was created by the previous $cref simulate_command$$.

$head fit_var_table$$
If $icode method$$ is $code asymptotic$$,
this command has the extra input $cref fit_var_table$$
which was created by a previous $cref fit_command$$.

$head sample_table$$
A new $cref sample_table$$ is created each time this command is run.
It contains samples of the model variables.
Hence the number of rows in this table is $icode number_sample$$
times the number of rows in the $cref var_table$$.

$head Bounds$$
The samples are all within the specified bounds, including the bounds
on the random effects specified by
$cref/bound_random/option_table/Random Only/bound_random/$$.

$children%example/get_started/sample_command.py
%$$
$head Example$$
The files
$cref sample_command.py$$ and $cref user_asymptotic.py$$
contain examples and tests using this command.

$end
*/
// ----------------------------------------------------------------------------
void sample_command(
	const std::string&                          method           ,
	const std::string&                          number_sample    ,
	sqlite3*                                    db               ,
	vector<dismod_at::data_subset_struct>&      data_subset_obj  ,
	dismod_at::data_model&                      data_object      ,
	dismod_at::prior_model&                     prior_object     ,
	const dismod_at::pack_info&                 pack_object      ,
	const dismod_at::pack_prior&                var2prior        ,
	const dismod_at::db_input_struct&           db_input         ,
	// effectively const
	std::map<std::string, std::string>&         option_map
)
{	using std::string;
	using CppAD::to_string;
	string msg;
	// -------------------------------------------------------------------
	if( method != "simulate" && method != "asymptotic" )
	{	msg  = "dismod_at sample command method = ";
		msg += method + " is not one of the following: ";
		msg += "simulate, asymptotic";
		dismod_at::error_exit(msg);
	}
	int tmp = std::atoi( number_sample.c_str() );
	if( tmp <= 0 )
	{	msg  = "dismod_at sample command number_sample = ";
		msg += number_sample + " is not an integer greater than zero";
		dismod_at::error_exit(msg);
	}
	size_t n_sample = size_t(tmp);
	// -----------------------------------------------------------------------
	// create new sample table and prepare to write into it
	//
	string sql_cmd = "drop table if exists sample";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	size_t n_col      = 3;
	size_t n_var      = pack_object.size();
	size_t n_row      = n_sample * n_var;
	vector<string> col_name(n_col), col_type(n_col), row_value(n_col * n_row);
	vector<bool>   col_unique(n_col);
	//
	col_name[0]   = "sample_index";
	col_type[0]   = "integer";
	col_unique[0] = false;
	//
	col_name[1]   = "var_id";
	col_type[1]   = "integer";
	col_unique[1] = false;
	//
	col_name[2]   = "var_value";
	col_type[2]   = "real";
	col_unique[2] = false;
	// -----------------------------------------------------------------------
	// zero_sum_random
	size_t n_rate      = size_t(dismod_at::number_rate_enum);
	size_t option_size = option_map["zero_sum_random"].size();
	vector<bool> zero_sum_random(n_rate);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	string rate_name = dismod_at::get_rate_name(rate_id);
		size_t found     = option_map["zero_sum_random"].find( rate_name );
		zero_sum_random[rate_id] = found < option_size;
	}
	// ----------------------------------------------------------------------
	// quasi_fixed
	bool quasi_fixed = option_map["quasi_fixed"] == "true";
	//
	// warn_on_stderr
	bool warn_on_stderr = option_map["warn_on_stderr"] == "true";
	//
	// bound_random, null corresponds to infinity
	std::string tmp_str = option_map["bound_random"];
	double bound_random = std::numeric_limits<double>::infinity();
	if( tmp_str != "" )
		bound_random = std::atof( tmp_str.c_str() );
	// -----------------------------------------------------------------------
	if( method == "simulate" )
	{
		// start_var
		vector<double> start_var_value;
		string table_name  = "start_var";
		string column_name = "start_var_value";
		dismod_at::get_table_column(
			db, table_name, column_name, start_var_value
		);
		// scale_var
		vector<double> scale_var_value;
		table_name  = "scale_var";
		column_name = "scale_var_value";
		dismod_at::get_table_column(
			db, table_name, column_name, scale_var_value
		);
		// get simulated data
		vector<dismod_at::data_sim_struct> data_sim_table =
				dismod_at::get_data_sim_table(db);
		// get simulated prior
		vector<dismod_at::prior_sim_struct> prior_sim_table =
				dismod_at::get_prior_sim_table(db);
		//
		size_t n_subset = data_subset_obj.size();
		if( n_subset == 0 && data_sim_table.size() != 0  )
		{	msg  = "dismod_at sample command method = simulate and ";
			msg += "data_subset table size is zero and ";
			msg += "data_sim table size is not zero.";
			dismod_at::error_exit(msg);
		}
		if( n_subset != 0 && data_sim_table.size() % n_subset != 0  )
		{	msg  = "dismod_at sample command method = simulate and ";
			msg += "data_sim table size modulo data_subset table size not 0.";
			dismod_at::error_exit(msg);
		}
		if( n_subset != 0 && n_sample != data_sim_table.size() / n_subset )
		{	msg  = "dismod_at sample command method = simulate and ";
			msg += "data_sim table size not equal number_sample times ";
			msg += "data_subset table size.";
			dismod_at::error_exit(msg);
		}
		// vector used for replacement of prior means
		vector<double> prior_mean(n_var * 3);
		for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
		{	// replace prior means in prior_object
			for(size_t var_id = 0; var_id < n_var; ++var_id)
			{	size_t prior_sim_id = sample_index * n_var + var_id;
				prior_mean[var_id * 3 + 0] =
					prior_sim_table[prior_sim_id].prior_sim_value;
				prior_mean[var_id * 3 + 1] =
					prior_sim_table[prior_sim_id].prior_sim_dage;
				prior_mean[var_id * 3 + 2] =
					prior_sim_table[prior_sim_id].prior_sim_dtime;
			}
			prior_object.replace_mean(prior_mean);
			//
			// replace meas_value in data_subset_obj
			size_t offset = n_subset * sample_index;
			for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
			{	size_t data_sim_id = offset + subset_id;
				size_t sample_check =
					size_t(data_sim_table[data_sim_id].simulate_index);
				size_t subset_check =
					size_t(data_sim_table[data_sim_id].data_subset_id);
				if( sample_check != sample_index || subset_check != subset_id )
				{	msg  = "dismod_at database sample simulate\n";
					msg += "size of data_sim table does not make sense\n";
					msg +=  "restart with init command";
					table_name = "data_sim";
					dismod_at::error_exit(msg, table_name, data_sim_id);
				}
				data_subset_obj[subset_id].meas_value =
					data_sim_table[data_sim_id].data_sim_value;
			}
			// replace_like
			data_object.replace_like(data_subset_obj);
			//
			// fit_model
			string fit_or_sample = "fit";
			bool   random_only   = false;
			dismod_at::fit_model fit_object(
				db                   ,
				warn_on_stderr       ,
				bound_random         ,
				fit_or_sample        ,
				pack_object          ,
				var2prior            ,
				start_var_value      ,
				scale_var_value      ,
				db_input.prior_table ,
				prior_object         ,
				quasi_fixed          ,
				zero_sum_random      ,
				data_object
			);
			fit_object.run_fit(random_only, option_map);
			vector<double> opt_value, lag_value, lag_dage, lag_dtime;
			fit_object.get_solution(
				opt_value, lag_value, lag_dage, lag_dtime
			);
			assert( opt_value.size() == n_var );
			//
			// put solution for this sample_index in row_value
			string sample_index_str = to_string( sample_index );
			for(size_t var_id = 0; var_id < n_var; var_id++)
			{	size_t sample_id = sample_index * n_var + var_id;
				row_value[n_col * sample_id + 0] = sample_index_str;
				row_value[n_col * sample_id + 1] = to_string( var_id );
				row_value[n_col * sample_id + 2] =
					to_string(opt_value[var_id] );
			}
		}
		table_name = "sample";
		dismod_at::create_table(
			db, table_name, col_name, col_type, col_unique, row_value
		);
		return;
	}
	// ----------------------------------------------------------------------
	assert( method == "asymptotic" );
	//
	// replace_like
	data_object.replace_like(data_subset_obj);
	//
	// fit_var.fit_var_value
	vector<double> fit_var_value;
	string table_name  = "fit_var";
	string column_name = "fit_var_value";
	dismod_at::get_table_column(
		db, table_name, column_name, fit_var_value
	);
	//
	// fit_var.lagrange_value
	vector<double> lagrange_value;
	column_name = "lagrange_value";
	dismod_at::get_table_column(
		db, table_name, column_name, lagrange_value
	);
	//
	// fit_var.lagrange_dage
	vector<double> lagrange_dage;
	column_name = "lagrange_dage";
	dismod_at::get_table_column(
		db, table_name, column_name, lagrange_dage
	);
	//
	// fit_var.lagrange_dtime
	vector<double> lagrange_dtime;
	column_name = "lagrange_dtime";
	dismod_at::get_table_column(
		db, table_name, column_name, lagrange_dtime
	);
	//
	// fit_object
	string fit_or_sample = "sample";
	dismod_at::fit_model fit_object(
		db                   ,
		warn_on_stderr       ,
		bound_random         ,
		fit_or_sample        ,
		pack_object          ,
		var2prior            ,
		fit_var_value        ,
		fit_var_value        ,
		db_input.prior_table ,
		prior_object         ,
		quasi_fixed          ,
		zero_sum_random      ,
		data_object
	);
	//
	// sample
	vector<double> sample(n_sample * n_var);
	fit_object.sample_posterior(
		sample               ,
		fit_var_value        ,
		lagrange_value       ,
		lagrange_dage        ,
		lagrange_dtime       ,
		option_map
	);
	//
	for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
	{	string sample_index_str = to_string( sample_index );
		for(size_t var_id = 0; var_id < n_var; var_id++)
		{	size_t sample_id = sample_index * n_var + var_id;
			row_value[n_col * sample_id + 0] = sample_index_str;
			row_value[n_col * sample_id + 1] = to_string( var_id );
			row_value[n_col * sample_id + 2] =
					to_string( sample[ sample_index * n_var + var_id] );
		}
	}
	table_name = "sample";
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}
/*
-------------------------------------------------------------------------------
$begin predict_command$$
$spell
	sim
	avgint
	dismod
	var
	arg
$$

$section The Predict Command$$

$head Syntax$$
$codei%dismod_at %database% predict %source%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head source$$
This argument specifies where model variable values to use
for the predictions. The possible values are listed below:

$subhead sample$$
If $icode source$$ is $code sample$$,
the values in the $cref sample_table$$ are used for the predictions.
In this case there are
$cref/number_simulate/simulate_command/number_simulate/$$ sets
of model variables that predictions are computed for.

$subhead fit_var$$
If $icode source$$ is $code fit_var$$,
the values in the $cref fit_var_table$$ are used for the predictions.
In this case there is only one set of model variables that the
predictions are computed for and
$cref/sample_index/predict_table/sample_index/$$ is always zero.

$subhead truth_var$$
If $icode source$$ is $code truth_var$$,
the values in the $cref truth_var_table$$ are used for the predictions.
In this case there is only one set of model variables that the
predictions are computed for and
$cref/sample_index/predict_table/sample_index/$$ is always zero.

$head predict_table$$
A new $cref predict_table$$ is created each time this command is run.
It contains the
$cref/average integrand/avg_integrand/Average Integrand, A_i/$$
values for set of model variables
and each
$cref/avgint_id/predict_table/avgint_id/$$
in the
$cref/avgint subset/predict_table/Avgint Subset/$$.

$children%example/get_started/predict_command.py
%$$
$head Example$$
The files $cref predict_command.py$$ and
$cref user_predict_fit.py$$ contain examples and tests
using this command.

$end
*/

// ----------------------------------------------------------------------------
void predict_command(
	const std::string&                                source              ,
	sqlite3*                                          db                  ,
	const dismod_at::db_input_struct&                 db_input            ,
	size_t                                            n_var               ,
	dismod_at::data_model&                            avgint_object     ,
	const vector<dismod_at::avgint_subset_struct>&  avgint_subset_obj
)
{
	using std::string;
	using CppAD::to_string;
	//
	if( source != "sample"
	&&  source != "fit_var"
	&&  source != "truth_var" )
	{	string msg  = "dismod_at predict command source = ";
		msg        += source + " is not one of the following: ";
		msg        += "sample, fit_var, truth_var";
		dismod_at::error_exit(msg);
	}
	// ------------------------------------------------------------------------
	// variable_value
	vector<double> variable_value;
	string table_name = source;
	string column_name;
	if( source == "sample" )
		column_name = "var_value";
	else if( source == "fit_var" )
		column_name = "fit_var_value";
	else
		column_name = "truth_var_value";
	dismod_at::get_table_column(
		db, table_name, column_name, variable_value
	);
	size_t n_sample = variable_value.size() / n_var;
	assert( n_sample * n_var == variable_value.size() );
# ifndef NDEBUG
	// ------------------------------------------------------------------------
	// check sample table
	if( source == "sample" )
	{	// sample table
		vector<dismod_at::sample_struct> sample_table =
			dismod_at::get_sample_table(db);
		size_t sample_id = 0;
		for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
		{	for(size_t var_id = 0; var_id < n_var; var_id++)
			{	size_t sample_check =
					size_t( sample_table[sample_id].sample_index);
				size_t var_check =
					size_t( sample_table[sample_id].var_id);
				if( sample_check != sample_index || var_check != var_id )
				{	string msg = "database modified, restart with init command";
					table_name = "sample";
					dismod_at::error_exit(msg, table_name, sample_id);
				}
				++sample_id;
			}
		}
	}
# endif
	// -----------------------------------------------------------------------
	// create a new predict table
	string sql_cmd = "drop table if exists predict";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name = "predict";
	size_t n_col      = 3;
	size_t n_subset   = avgint_subset_obj.size();
	size_t n_row      = n_sample * n_subset;
	vector<string> col_name(n_col), col_type(n_col), row_value(n_col * n_row);
	vector<bool>   col_unique(n_col);
	//
	col_name[0]   = "sample_index";
	col_type[0]   = "integer";
	col_unique[0] = false;
	//
	col_name[1]   = "avgint_id";
	col_type[1]   = "integer";
	col_unique[1] = false;
	//
	col_name[2]   = "avg_integrand";
	col_type[2]   = "real";
	col_unique[2] = false;
	//
	// pack_vec
	vector<double> pack_vec(n_var);
	//
	size_t sample_id = 0;
	for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
	{	// copy the variable values for this sample index into pack_vec
		for(size_t var_id = 0; var_id < n_var; var_id++)
			pack_vec[var_id] = variable_value[sample_id++];
		//
		bool parent_only = false;
		CppAD::vector<double> reference_sc =
			avgint_object.reference_ode(pack_vec, parent_only);
		//
		for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
		{	int integrand_id = avgint_subset_obj[subset_id].integrand_id;
			int avgint_id    = avgint_subset_obj[subset_id].original_id;
			double avg = 0.0;
			dismod_at::integrand_enum integrand =
				db_input.integrand_table[integrand_id].integrand;
			switch( integrand )
			{
				case dismod_at::Sincidence_enum:
				case dismod_at::remission_enum:
				case dismod_at::mtexcess_enum:
				case dismod_at::mtother_enum:
				case dismod_at::mtwith_enum:
				case dismod_at::relrisk_enum:
				avg = avgint_object.avg_no_ode(subset_id, pack_vec);
				break;
				//
				case dismod_at::susceptible_enum:
				case dismod_at::withC_enum:
				case dismod_at::prevalence_enum:
				case dismod_at::Tincidence_enum:
				case dismod_at::mtspecific_enum:
				case dismod_at::mtall_enum:
				case dismod_at::mtstandard_enum:
				avg = avgint_object.avg_yes_ode(
					subset_id, pack_vec, reference_sc
				);
				break;
				//
				default:
				assert(false);
			}
			size_t predict_id = sample_index * n_subset + subset_id;
			if( source == "sample" )
				row_value[n_col * predict_id + 0] = to_string( sample_index );
			else
				row_value[n_col * predict_id + 0] = "";
			row_value[n_col * predict_id + 1] = to_string( avgint_id );
			row_value[n_col * predict_id + 2] = to_string( avg );
		}
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}
// ---------------------------------------------------------------------------
} // END_EMPTY_NAMESPACE

int main(int n_arg, const char** argv)
{	// ---------------- using statements ----------------------------------
	using std::cerr;
	using std::endl;
	using std::string;
	// ---------------- command line arguments ---------------------------
	struct { const char* name; int n_arg; } command_info[] = {
		{"init",      3},
		{"set",       5},
		{"set",       6},
		{"depend",    3},
		{"fit",       4},
		{"fit",       5},
		{"simulate",  4},
		{"sample",    5},
		{"predict",   4}
	};
	size_t n_command = sizeof( command_info ) / sizeof( command_info[0] );
	//
	string program = "dismod_at-";
	program       += DISMOD_AT_VERSION;
# ifndef NDEBUG
	program       += " debug build";
# else
	program       += " release build";
# endif
	if( n_arg < 3 )
	{	cerr << program << endl
		<< "usage:    dismod_at database command [arguments]\n"
		<< "database: sqlite database\n"
		<< "command:  " << command_info[0].name;
		for(size_t i = 1; i < n_command; i++)
			cerr << ", " << command_info[i].name;
		cerr << endl
		<< "arguments: optional arguments depending on particular command\n";
		std::exit(1);
	}
	const string database_arg  = argv[1];
	const string command_arg   = argv[2];
	vector<size_t> command_match;
	bool match = false;
	for(size_t i = 0; i < n_command; i++)
	{	if( command_arg == command_info[i].name )
		{	command_match.push_back( command_info[i].n_arg );
			match |= n_arg == command_info[i].n_arg;
		}
	}
	if( command_match.size() == 0 )
	{	// commands that no longer exist
		if( command_arg == "start" )
		{	cerr <<
			"dismod_at database start source\n"
			"\thas been changed to\n"
			"dismod_at database set start_var source\n"
			"Furthermore, the init command now creates a start_var table\n";
			std::exit(1);
		}
		if( command_arg == "truth" )
		{	cerr <<
			"dismod_at database truth\n"
			"\thas been changed to\n"
			"dismod_at database set truth_var fit_var\n";
			std::exit(1);
		}
		// commands that never existed
		cerr << program << endl;
		cerr << command_arg << " is not a valid command" << endl;
		std::exit(1);
	}
	if( ! match )
	{	cerr << program << endl << command_arg << " command expected "
			<< command_match[0] - 3;
		if( command_match.size() == 2 )
			cerr << " or " << command_match[1] - 3;
		cerr << " arguments to follow " << command_arg << endl;
		std::exit(1);
	}
	string message;
	// --------------- open connection to datbase ---------------------------
	bool new_file = false;
	sqlite3* db   = dismod_at::open_connection(database_arg, new_file);
	//
	// set error_exit database so it can log fatal errors
	assert( db != DISMOD_AT_NULL_PTR );
	dismod_at::error_exit(db);
	// --------------- log start of this command -----------------------------
	message = "begin";
	for(int i_arg = 2; i_arg < n_arg; i_arg++)
	{	message += " ";
		message += argv[i_arg];
	}
	std::time_t unix_time =
		dismod_at::log_message(db, DISMOD_AT_NULL_PTR, "command", message);
	// --------------- get the input tables ---------------------------------
	dismod_at::db_input_struct db_input;
	get_db_input(db, db_input);
	// ----------------------------------------------------------------------
	// option_map
	std::map<string, string> option_map;
	size_t n_option = db_input.option_table.size();
	for(size_t id = 0; id < n_option; id++)
	{	string name  = db_input.option_table[id].option_name;
		string value = db_input.option_table[id].option_value;
		option_map[name] = value;
	}
	// ---------------------------------------------------------------------
	// ode_step_size
	double ode_step_size  = std::atof( option_map["ode_step_size"].c_str() );
	assert( ode_step_size > 0.0 );
	// ---------------------------------------------------------------------
	// initialize random number generator
	size_t random_seed = std::atoi( option_map["random_seed"].c_str() );
	if( random_seed == 0 )
	{
# ifndef NDEBUG
		size_t actual_seed = CppAD::mixed::new_gsl_rng( size_t(unix_time) );
		assert( std::time_t( actual_seed ) == unix_time );
# else
		CppAD::mixed::new_gsl_rng( size_t(unix_time) );
# endif
	}
	else
	{
# ifndef NDEBUG
		size_t actual_seed = CppAD::mixed::new_gsl_rng(random_seed);
		assert( actual_seed == random_seed );
# else
		CppAD::mixed::new_gsl_rng(random_seed);
# endif
	}
	// ---------------------------------------------------------------------
	// n_covariate
	size_t n_covariate = db_input.covariate_table.size();
	// ---------------------------------------------------------------------
	// n_age_ode
	double age_min    = dismod_at::min_vector( db_input.age_table );
	double age_max    = dismod_at::max_vector( db_input.age_table );
	size_t n_age_ode  = size_t( (age_max - age_min) / ode_step_size + 2.0 );
	assert( age_max  <= age_min  + double(n_age_ode - 1) * ode_step_size );
	// ---------------------------------------------------------------------
	// n_time_ode
	double time_min   = dismod_at::min_vector( db_input.time_table );
	double time_max   = dismod_at::max_vector( db_input.time_table );
	size_t n_time_ode = size_t( (time_max - time_min) / ode_step_size + 2.0 );
	assert( time_max <= time_min  + double(n_time_ode - 1) * ode_step_size );
	// ---------------------------------------------------------------------
	// parent_node_id
	size_t parent_node_id   = db_input.node_table.size();
	string parent_node_name = option_map["parent_node_name"];
	string table_name       = "option";
	if( option_map["parent_node_id"] != "" )
	{	parent_node_id   = std::atoi( option_map["parent_node_id"].c_str() );
		if( parent_node_name != "" )
		{	string node_name = db_input.node_table[parent_node_id].node_name;
			if( parent_node_name != node_name )
			{	message = "parent_node_id and parent_node_name"
				" specify different nodes";
				dismod_at::error_exit(message, table_name);
			}
		}
	}
	else if( parent_node_name != "" )
	{	size_t n_node  = db_input.node_table.size();
		for(size_t node_id = 0; node_id < n_node; node_id++)
		{	if( db_input.node_table[node_id].node_name == parent_node_name )
				parent_node_id = node_id;
		}
		if( parent_node_id == n_node )
		{	message = "cannot find parent_node_name in node table";
			dismod_at::error_exit(message, table_name);
		}
	}
	else
	{	message = "neither parent_node_id nor parent_node_name is present";
		dismod_at::error_exit(message, table_name);
	}
	assert( parent_node_id < db_input.node_table.size() );
	// ------------------------------------------------------------------------

	// child_data
	dismod_at::child_info child_data(
		parent_node_id          ,
		db_input.node_table     ,
		db_input.data_table
	);
	// child_avgint
	dismod_at::child_info child_avgint(
		parent_node_id          ,
		db_input.node_table     ,
		db_input.avgint_table
	);
	// n_child, n_integrand, n_weight, n_smooth
	size_t n_child     = child_data.child_size();
	size_t n_integrand = db_input.integrand_table.size();
	size_t n_weight    = db_input.weight_table.size();
	size_t n_smooth    = db_input.smooth_table.size();
	// ---------------------------------------------------------------------
	// w_info_vec
	vector<dismod_at::weight_info> w_info_vec(n_weight);
	for(size_t weight_id = 0; weight_id < n_weight; weight_id++)
	{	w_info_vec[weight_id] = dismod_at::weight_info(
			db_input.age_table,
			db_input.time_table,
			weight_id,
			db_input.weight_table,
			db_input.weight_grid_table
		);
	}
	// s_info_vec
	vector<dismod_at::smooth_info> s_info_vec(n_smooth);
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	s_info_vec[smooth_id] = dismod_at::smooth_info(
			db_input.age_table         ,
			db_input.time_table        ,
			smooth_id                  ,
			db_input.smooth_table      ,
			db_input.smooth_grid_table
		);
	}
	// child_id2node_id
	vector<size_t> child_id2node_id(n_child);
	for(size_t child_id = 0; child_id < n_child; child_id++)
	{	size_t node_id = child_data.child_id2node_id(child_id);
		assert( node_id == child_avgint.child_id2node_id(child_id) );
		child_id2node_id[child_id] = node_id;
	}
	// pack_object
	dismod_at::pack_info pack_object(
		n_integrand                 ,
		child_id2node_id            ,
		db_input.smooth_table       ,
		db_input.mulcov_table       ,
		db_input.rate_table         ,
		db_input.nslist_pair_table
	);
	//
	// var2prior
	dismod_at::pack_prior var2prior(pack_object, s_info_vec);
	//
	// prior_mean
	vector<double> prior_mean = get_prior_mean(
		db_input.prior_table, var2prior
	);
	// rate_case
	string rate_case = option_map["rate_case"];
	//
	// ode_age_split
	string ode_age_split = option_map["ode_age_split"];
	//
	// ode_age_grid
	vector<double> ode_age_grid = dismod_at::ode_age_grid(
		ode_step_size, ode_age_split, db_input.age_table
	);
	//
	// bound_random
	double bound_random = 0.0;
	if( command_arg != "fit" || std::strcmp(argv[3], "fixed") != 0 )
	{	// null corresponds to infinity
		std::string tmp_str = option_map["bound_random"];
		if( tmp_str == "" )
			bound_random = std::numeric_limits<double>::infinity();
		else
			bound_random = std::atof( tmp_str.c_str() );
	}
	// =======================================================================
# ifdef NDEBUG
	try { // BEGIN_TRY_BLOCK (when not debugging)
# endif
	// =======================================================================
	if( command_arg == "set" )
	{	if( std::strcmp(argv[3], "option") == 0 )
		{	if( n_arg != 6 )
			{	cerr << "expected name and value to follow "
				"dismod_at database set option\n";
				std::exit(1);
			}
			std::string name  = argv[4];
			std::string value = argv[5];
			dismod_at::set_option_command(
				db, db_input.option_table, name, value);
		}
		else
		{	std::string table_out     = argv[3];
			std::string source        = argv[4];
			std::string sample_index  = "";
			if( n_arg == 6 )
				sample_index = argv[5];
			dismod_at::set_command(
				table_out       ,
				source          ,
				sample_index    ,
				db              ,
				prior_mean
			);
		}
	}
	else if( command_arg == "predict" )
	{	// avgint_subset_obj
		vector<dismod_at::avgint_subset_struct> avgint_subset_obj;
		vector<double> avgint_subset_cov_value;
		avgint_subset(
				db_input.avgint_table,
				db_input.avgint_cov_value,
				db_input.covariate_table,
				child_avgint,
				avgint_subset_obj,
				avgint_subset_cov_value
		);
		//
		// avgint_object
		dismod_at::data_model avgint_object(
			rate_case                ,
			bound_random             ,
			n_covariate              ,
			n_age_ode                ,
			n_time_ode               ,
			ode_step_size            ,
			ode_age_grid             ,
			db_input.age_table       ,
			db_input.time_table      ,
			db_input.integrand_table ,
			db_input.prior_table     ,
			avgint_subset_obj        ,
			avgint_subset_cov_value  ,
			w_info_vec               ,
			s_info_vec               ,
			pack_object              ,
			child_avgint
		);
		size_t n_var = pack_object.size();
		std::string source = argv[3];
		predict_command(
			source               ,
			db                   ,
			db_input             ,
			n_var                ,
			avgint_object      ,
			avgint_subset_obj
		);
	}
	else
	{	// -------------------------------------------------------------------
		// data_subset_obj
		vector<dismod_at::data_subset_struct> data_subset_obj;
		vector<double> data_subset_cov_value;
		data_subset(
			db_input.data_table,
			db_input.data_cov_value,
			db_input.covariate_table,
			child_data,
			data_subset_obj,
			data_subset_cov_value
		);
		if( command_arg == "init" )
		{	init_command(
				db,
				prior_mean,
				data_subset_obj,
				pack_object,
				db_input,
				parent_node_id,
				child_data,     // could also use child_avgint
				s_info_vec
			);
		}
		else
		{	// ---------------------------------------------------------------
			// prior_object
			dismod_at::prior_model prior_object(
				pack_object           ,
				var2prior             ,
				db_input.age_table    ,
				db_input.time_table   ,
				db_input.prior_table
			);
			dismod_at::data_model data_object(
				rate_case                ,
				bound_random             ,
				n_covariate              ,
				n_age_ode                ,
				n_time_ode               ,
				ode_step_size            ,
				ode_age_grid             ,
				db_input.age_table       ,
				db_input.time_table      ,
				db_input.integrand_table ,
				db_input.prior_table     ,
				data_subset_obj          ,
				data_subset_cov_value    ,
				w_info_vec               ,
				s_info_vec               ,
				pack_object              ,
				child_data
			);
			//
			if( command_arg == "depend" )
			{	depend_command(
					db               ,
					prior_mean       ,
					data_object      ,
					data_subset_obj  ,
					prior_object
				);
			}
			else if( command_arg == "fit" )
			{	string variables      = argv[3];
				string simulate_index = "";
				if( n_arg == 5 )
					simulate_index = argv[4];
				fit_command(
					variables        ,
					simulate_index   ,
					db               ,
					data_subset_obj  ,
					data_object      , // not  const
					prior_object     , // not  const
					pack_object      ,
					var2prior        ,
					db_input         ,
					option_map
				);
			}
			else if( command_arg == "simulate" )
			{	// replace_like
				data_object.replace_like(data_subset_obj );
				simulate_command(
					argv[3]                  , // number_simulate
					db                       ,
					db_input.integrand_table ,
					data_subset_obj          ,
					data_object              ,
					var2prior                ,
					db_input.prior_table
				);
			}
			else if( command_arg == "sample" )
			{	sample_command(
					argv[3]          , // method
					argv[4]          , // number_sample
					db               ,
					data_subset_obj  ,
					data_object      , // not const
					prior_object     , // not const
					pack_object      ,
					var2prior        ,
					db_input         ,
					option_map
				);
			}
			else
				assert(false);
		}
	}
	// =======================================================================
# ifdef NDEBUG
	} // END_TRY_BLOCK (when not debugging)
	catch(const CppAD::mixed::exception& e)
	{	string catcher("dismod_at");
		catcher += " " + database_arg + " " + command_arg;
		message  = e.message(catcher);
		dismod_at::error_exit(message);
	}
# endif
	// =======================================================================
	// ---------------------------------------------------------------------
	message = "end " + command_arg;
	dismod_at::log_message(db, DISMOD_AT_NULL_PTR, "command", message);
	sqlite3_close(db);
	return 0;
}
