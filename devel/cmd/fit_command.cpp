// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <dismod_at/fit_command.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/get_prior_sim_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/get_var_limits.hpp>
# include <dismod_at/remove_const.hpp>
# include <dismod_at/blob_table.hpp>
# include <dismod_at/pack_warm_start.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
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
    Ipopt
$$

$section The Fit Command$$

$head Syntax$$
$codei%dismod_at %database% fit %variables%
%$$
$codei%dismod_at %database% fit %variables% %simulate_index%
%$$
$codei%dismod_at %database% fit %variables% warm_start
%$$
$codei%dismod_at %database% fit %variables% %simulate_index% warm_start
%$$

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

$head warm_start$$
If $code warm_start$$ is at the end of the command,
the ipopt_info table, written by the previous fit,
is used to start the optimization where the previous fit left off.
In this case, the other options must be the same as for the previous fit.

$subhead ipopt_info$$
The fixed effect are optimized when
$icode variables$$ is equal to $code both$$ or $code fixed$$.
In the case a new ipopt_info table, corresponding to the final
fit for the fixed effects, is written to the ipopt_info table.
This can be used to continue the fit when it is terminated due to reaching
$cref/max_num_iter_fixed/option_table/Fixed and Random/max_num_iter/$$.
The contents of this table are unspecified; i.e., not part of the
dismod_at API and my change.

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

$head Ipopt Options$$
Some of the options in the $cref option_table$$ are for controlling
$href%https://coin-or.github.io/Ipopt/OPTIONS.html%Ipopt%$$.
You can override these options, or set other options, using a file called
$code ipopt.opt$$ in the directory where a fit command is run.
Doing so not supported because $code dismod_at$$
expects certain options to be set a certain way.
The set of such options that is not specified and may change with time.

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
	bool                                          use_warm_start   ,
	const std::string&                            variables        ,
	const std::string&                            simulate_index   ,
	sqlite3*                                      db               ,
	CppAD::vector<dismod_at::data_subset_struct>& data_subset_obj  ,
	dismod_at::data_model&                        data_object      ,
	dismod_at::prior_model&                       prior_object     ,
	const dismod_at::pack_info&                   pack_object      ,
	const dismod_at::pack_prior&                  var2prior        ,
	const dismod_at::db_input_struct&             db_input         ,
	// effectively const
	std::map<std::string, std::string>&           option_map
)
{	using std::string;
	using CppAD::to_string;
	using CppAD::vector;
	// -----------------------------------------------------------------------
	bool ok = variables == "fixed";
	ok     |= variables == "random";
	ok     |= variables == "both";
	if( ! ok )
	{	string msg = "dismod_at fit command variables = ";
		msg += variables + "\nis not 'fixed', 'random' or 'both'";
		dismod_at::error_exit(msg);
	}
	if( use_warm_start && variables == "random" )
	{	string msg = "dismod_at fit command: cannot warm start when ";
		msg       += "only optimizing random effects";
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
	// simulation index corresponding to data
	int simulation_index = -1;
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
		{	string msg = "dismod_at fit command: simulate_index = ";
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
# ifndef NDEBUG
			double old_value = data_subset_obj[subset_id].data_sim_value;
# endif
			double new_value =data_sim_table[data_sim_id].data_sim_value;
			assert(   std::isnan(old_value) );
			assert( ! std::isnan(new_value) );
			data_subset_obj[subset_id].data_sim_value = new_value;
		}

		// simulation index
		simulation_index = int(sim_index);
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
	// zero_sum_child_rate
	size_t n_rate      = size_t(dismod_at::number_rate_enum);
	size_t option_size = option_map["zero_sum_child_rate"].size();
	vector<bool> zero_sum_child_rate(n_rate);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	string rate_name = dismod_at::get_rate_name(rate_id);
		size_t found     = option_map["zero_sum_child_rate"].find( rate_name );
		zero_sum_child_rate[rate_id] = found < option_size;
	}
	// ----------------------------------------------------------------------
	// zero_sum_mulcov_group
	size_t n_group = pack_object.group_size();
	option_size    = option_map["zero_sum_mulcov_group"].size();
	vector<bool> zero_sum_mulcov_group(n_group);
	for(size_t group_id = 0; group_id < n_group; group_id++)
	{	const vector<subgroup_struct>&
			subgroup_table( db_input.subgroup_table );
		size_t first_subgroup_id = pack_object.first_subgroup_id(group_id);
		string group_name = subgroup_table[first_subgroup_id].group_name;
		size_t found = option_map["zero_sum_mulcov_group"].find(group_name);
		zero_sum_mulcov_group[group_id] = found < option_size;
	}
	// ----------------------------------------------------------------------
	// random_const
	size_t n_var    = pack_object.size();
	size_t n_random = pack_object.random_size();
	CppAD::mixed::d_vector var_lower(n_var), var_upper(n_var);
	get_var_limits(
		var_lower, var_upper, bound_random, var2prior, db_input.prior_table
	);
	CppAD::mixed::d_vector random_lower(n_random);
	CppAD::mixed::d_vector random_upper(n_random);
	unpack_random(pack_object, var_lower, random_lower);
	unpack_random(pack_object, var_upper, random_upper);
	remove_const random_const(random_lower, random_upper);
	//
	// warm_start_in
	CppAD::mixed::warm_start_struct warm_start_in;
	if( use_warm_start )
	{	// get sizeof_data
		table_name         = "ipopt_info";
		string col_name    = "warm_start";
		size_t sizeof_data = 0;
		void* data         = nullptr;
		read_blob_table(db, table_name, col_name, sizeof_data, data);
		//
		// read the data
		assert( sizeof_data % sizeof(double) == 0 );
		vector<double> vec( sizeof_data / sizeof(double) );
		data = reinterpret_cast<void*>( vec.data() );
		read_blob_table(db, table_name, col_name, sizeof_data, data);
		warm_start_in = unpack_warm_start(vec);
	}
	// ------------------ run fit_model ------------------------------------
	// quasi_fixed
	bool quasi_fixed = option_map["quasi_fixed"] == "true";
	//
	// warn_on_stderr
	bool warn_on_stderr = option_map["warn_on_stderr"] == "true";
	//
	bool no_scaling = false;
	dismod_at::fit_model fit_object(
		db                   ,
		simulation_index     ,
		warn_on_stderr       ,
		bound_random         ,
		no_scaling           ,
		pack_object          ,
		var2prior            ,
		start_var            ,
		scale_var            ,
		db_input.prior_table ,
		prior_object         ,
		random_const         ,
		quasi_fixed          ,
		zero_sum_child_rate  ,
		zero_sum_mulcov_group,
		data_object
	);
	fit_object.run_fit(random_only, option_map, warm_start_in);
	vector<double> opt_value, lag_value, lag_dage, lag_dtime;
	vector<CppAD::mixed::trace_struct> trace_vec;
	CppAD::mixed::warm_start_struct warm_start_out;
	fit_object.get_solution(
		opt_value, lag_value, lag_dage, lag_dtime, trace_vec, warm_start_out
	);
	// ------------------ ipopt_info table -----------------------------------
	if( ! random_only )
	{	string sql_cmd = "drop table if exists ipopt_info";
		dismod_at::exec_sql_cmd(db, sql_cmd);
		//
		// pack the warm start information in a vector
		vector<double> vec = pack_warm_start(warm_start_out);
		//
		table_name         = "ipopt_info";
		string col_name    = "warm_start";
		size_t sizeof_data = vec.size() * sizeof(double);
		void* data         = reinterpret_cast<void*>( vec.data() );
		write_blob_table(db, table_name, col_name, sizeof_data, data);
	}
	// -------------------- fit_var table --------------------------------------
	string sql_cmd = "drop table if exists fit_var";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name   = "fit_var";
	assert( n_var == opt_value.size() );
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
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	// compute average integrand for this data item
		double avg = data_object.average(subset_id, opt_value);
		assert( ! CppAD::isnan(avg) );

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
	if( ! random_only )
	{	// -------------------- fixed_trace table -----------------------------
		sql_cmd = "drop table if exists fixed_trace";
		dismod_at::exec_sql_cmd(db, sql_cmd);
		//
		table_name     = "fixed_trace";
		size_t n_trace = trace_vec.size();
		n_col = 10;
		col_name.resize(n_col);
		col_unique.resize(n_col);
		col_type.resize(n_col);
		row_value.resize(n_col * n_trace);
		const char* col_name_lst[] = {
			"iter",
			"obj_value",
			"inf_pr",
			"inf_du",
			"mu",
			"d_norm",
			"regularization_size",
			"alpha_du",
			"alpha_pr",
			"ls_trials"
		};
		for(size_t j = 0; j < n_col; ++j)
		{	col_name[j]   = col_name_lst[j];
			col_unique[j] = false;
			if( col_name[j] == "iter" || col_name[j] == "ls_trials" )
				col_type[j] = "integer";
			else
				col_type[j] = "real";
		}
		for(size_t id = 0; id < n_trace; ++id)
		{
			row_value[ id * n_col + 0] = to_string( trace_vec[id].iter );
			row_value[ id * n_col + 1] = to_string( trace_vec[id].obj_value );
			row_value[ id * n_col + 2] = to_string( trace_vec[id].inf_pr );
			row_value[ id * n_col + 3] = to_string( trace_vec[id].inf_du );
			row_value[ id * n_col + 4] = to_string( trace_vec[id].mu );
			row_value[ id * n_col + 5] = to_string( trace_vec[id].d_norm );
			row_value[ id * n_col + 6] =
				to_string( trace_vec[id].regularization_size );
			row_value[ id * n_col + 7] = to_string( trace_vec[id].alpha_du );
			row_value[ id * n_col + 8] = to_string( trace_vec[id].alpha_pr );
			row_value[ id * n_col + 9] = to_string( trace_vec[id].ls_trials );
		}
		dismod_at::create_table(
			db, table_name, col_name, col_type, col_unique, row_value
		);
	}

	return;
}

} // END_DISMOD_AT_NAMESPACE
