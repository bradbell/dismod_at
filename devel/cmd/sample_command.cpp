/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
         GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <dismod_at/sample_command.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_prior_sim_table.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/create_table.hpp>


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
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
	const std::string&                                 method           ,
	const std::string&                                 number_sample    ,
	sqlite3*                                           db               ,
	CppAD::vector<dismod_at::data_subset_struct>&      data_subset_obj  ,
	dismod_at::data_model&                             data_object      ,
	dismod_at::prior_model&                            prior_object     ,
	const dismod_at::pack_info&                        pack_object      ,
	const dismod_at::pack_prior&                       var2prior        ,
	const dismod_at::db_input_struct&                  db_input         ,
	// effectively const
	std::map<std::string, std::string>&                option_map
)
{	using std::string;
	using CppAD::to_string;
	using CppAD::vector;
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
				if( log_density( data_subset_obj[subset_id].density ) )
				{	// simulated data is fit with no mimumum_meas_cv
					// so meas_std is same as meas_stdcv
					data_subset_obj[subset_id].meas_std =
						data_sim_table[data_sim_id].data_sim_stdcv;
				}
			}
			// replace_like
			data_object.replace_like(data_subset_obj);
			//
			// fit_model
			bool no_scaling      = false;
			bool   random_only   = false;
			int    simulate_index = int(sample_index);
			dismod_at::fit_model fit_object(
				db                   ,
				simulate_index       ,
				warn_on_stderr       ,
				bound_random         ,
				no_scaling           ,
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
	bool no_scaling     = true;
	int  simulate_index = -1; // used actual data in fit
	dismod_at::fit_model fit_object(
		db                   ,
		simulate_index       ,
		warn_on_stderr       ,
		bound_random         ,
		no_scaling           ,
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
} // END_DISMOD_AT_NAMESPACE