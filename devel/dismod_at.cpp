// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

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
# include <dismod_at/get_simulate_table.hpp>
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
# include <dismod_at/random_limits.hpp>

# define DISMOD_AT_TRACE 0

namespace { // BEGIN_EMPTY_NAMESPACE
	using CppAD::vector;
# if DISMOD_AT_TRACE
	std::time_t trace(const char* message, std::time_t previous_time = 0)
	{	std::time_t current_time = std::time( DISMOD_AT_NULL_PTR );
		if( previous_time == 0 )
			std::cout << message << std::endl;
		else
			std::cout << message << current_time - previous_time << " sec"
			<< std::endl;
		return current_time;
	}
# else
	std::time_t trace(const char* message, std::time_t previous_time = 0)
	{	std::time_t current_time = std::time( DISMOD_AT_NULL_PTR );
		return current_time;
	}
# endif
/*
-----------------------------------------------------------------------------
$begin init_command$$
$spell
	avgint
	init
	var
	dismod
$$

$section The Variable Command$$

$head Syntax$$
$codei%dismod_at %database% init%$$

$head Purpose$$
This command creates the following tables
$table
$cref var_table$$           $cnext $title var_table$$ $rnext
$cref data_subset_table$$   $cnext $title data_subset_table$$ $rnext
$cref avgint_subset_table$$ $cnext $title avgint_subset_table$$ $rnext
$tend

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head var_table$$
A new $cref var_table$$ is created with the information
that maps a $cref/var_id/var_table/var_id/$$
to its meaning in terms of the
$cref model_variables$$.

$head data_subset_table$$
A new $cref data_subset_table$$ is created.
This makes explicit exactly which rows of the data table are used.

$head avgint_subset_table$$
A new $cref avgint_subset_table$$ is created.
This makes explicit exactly which rows of the avgint table are used.

$children%example/get_started/init_command.py%$$
$head Example$$
The file $cref init_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void init_command(
	sqlite3*                                         db                  ,
	const vector<dismod_at::data_subset_struct>&     data_subset_obj     ,
	const vector<dismod_at::avgint_subset_struct>& avgint_subset_obj ,
	const dismod_at::pack_info&                      pack_object         ,
	const dismod_at::db_input_struct&                db_input            ,
	const size_t&                                    parent_node_id      ,
	const dismod_at::child_info&                     child_object        ,
	const vector<dismod_at::smooth_info>&            s_info_vec          )
{	using std::string;
	using CppAD::to_string;

	// -----------------------------------------------------------------------
	const char* drop_list[] = {
		"var",
		"data_subset",
		"avgint_subset",
	};
	size_t n_drop = sizeof( drop_list ) / sizeof( drop_list[0] );
	string sql_cmd;
	for(size_t i = 0; i < n_drop; i++)
	{	sql_cmd = "drop table if exists ";
		sql_cmd += drop_list[i];
		dismod_at::exec_sql_cmd(db, sql_cmd);
	}
	// -----------------------------------------------------------------------
	// create data_subset_table
	string table_name = "data_subset";
	size_t n_subset   = data_subset_obj.size();
	vector<string> col_name(1), col_type(1), row_value(n_subset);
	vector<bool>   col_unique(1);
	col_name[0]       = "data_id";
	col_type[0]       = "integer";
	col_unique[0]     = true;
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	int data_id    = data_subset_obj[subset_id].original_id;
		row_value[subset_id] = to_string( data_id );
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	// -----------------------------------------------------------------------
	// create avgint_subset_table
	n_subset   = avgint_subset_obj.size();
	row_value.clear();
	row_value.resize(n_subset);
	table_name     = "avgint_subset";
	n_subset       = avgint_subset_obj.size();
	col_name[0]    = "avgint_id";
	col_type[0]    = "integer";
	col_unique[0]  = true;
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	int avgint_id    = avgint_subset_obj[subset_id].original_id;
		row_value[subset_id] = to_string( avgint_id );
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	// -----------------------------------------------------------------------
	// create var table
	size_t n_row = pack_object.size();
	size_t n_col = 8;
	table_name   = "var";
	col_name.resize(n_col);
	col_type.resize(n_col);
	col_unique.resize(n_col);
	row_value.clear();
	row_value.resize(n_col * n_row);
	//
	col_name[0]   = "var_type";
	col_type[0]   = "text";
	col_unique[0] = false;
	//
	col_name[1]   = "smooth_id";
	col_type[1]   = "integer";
	col_unique[1] = false;
	//
	col_name[2]   = "age_id";
	col_type[2]   = "integer";
	col_unique[2] = false;
	//
	col_name[3]   = "time_id";
	col_type[3]   = "integer";
	col_unique[3] = false;
	//
	col_name[4]   = "node_id";
	col_type[4]   = "integer";
	col_unique[4] = false;
	//
	col_name[5]   = "rate_id";
	col_type[5]   = "integer";
	col_unique[5] = false;
	//
	col_name[6]   = "integrand_id";
	col_type[6]   = "integer";
	col_unique[6] = false;
	//
	col_name[7]   = "covariate_id";
	col_type[7]   = "integer";
	col_unique[7] = false;
	//
	// mulstd variables
	size_t n_smooth = db_input.smooth_table.size();
	size_t offset, var_id;
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	for(size_t k = 0; k < 3; k++)
		{	offset  = pack_object.mulstd_offset(smooth_id, k);
			if( offset != DISMOD_AT_NULL_SIZE_T )
			{	var_id  = offset;
# ifndef NDEBUG
				for(size_t j = 0; j < n_col; j++)
					assert( row_value[ n_col * var_id + j ] == "" );
# endif
				// var_type
				string var_type;
				switch(k)
				{	case 0:
					var_type = "mulstd_value";
					break;

					case 1:
					var_type = "mulstd_dage";
					break;

					case 2:
					var_type = "mulstd_dtime";
					break;

					default:
					assert(false);
				}
				row_value[ n_col * var_id + 0] = var_type;

				// smooth_id
				row_value[ n_col * var_id + 1] = to_string( smooth_id );
			}
		}
	}
	// rate variables
	size_t n_rate  = db_input.rate_table.size();
	size_t n_child = child_object.child_size();
	size_t smooth_id, n_var, n_time, node_id;
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	for(size_t child_id = 0; child_id <= n_child; child_id++)
		{	if( child_id == n_child )
				node_id = parent_node_id;
			else
				node_id = child_object.child_id2node_id(child_id);
			//
			dismod_at::pack_info::subvec_info info;
			info      = pack_object.rate_info(rate_id, child_id);
			smooth_id = info.smooth_id;
			if( smooth_id == DISMOD_AT_NULL_SIZE_T )
				n_var = 0;
			else
			{	offset    = info.offset;
				n_var     = info.n_var;
				n_time    = db_input.smooth_table[smooth_id].n_time;
# ifndef NDEBUG
				size_t n_age = db_input.smooth_table[smooth_id].n_age;
				assert( n_var == n_age * n_time );
# endif
			}
			for(size_t index = 0; index < n_var; index++)
			{	size_t i       = index / n_time;
				size_t j       = index % n_time;
				var_id         = offset + index;
				size_t age_id  = s_info_vec[smooth_id].age_id(i);
				size_t time_id = s_info_vec[smooth_id].time_id(j);
				//
				// variable_value
# ifndef NDEBUG
				for(size_t j = 0; j < n_col; j++)
					assert( row_value[ n_col * var_id + j ] == "" );
# endif
				row_value[n_col * var_id + 0] = "rate"; // var_type
				row_value[n_col * var_id + 1] = to_string( smooth_id );
				row_value[n_col * var_id + 2] = to_string( age_id );
				row_value[n_col * var_id + 3] = to_string( time_id );
				row_value[n_col * var_id + 4] = to_string( node_id );
				row_value[n_col * var_id + 5] = to_string( rate_id );
			}
		}
	}
	//
	// covariate multiplers
	const vector<dismod_at::mulcov_struct>&
		mulcov_table( db_input.mulcov_table );
	size_t n_mulcov        = mulcov_table.size();
	size_t n_integrand     = db_input.integrand_table.size();
	vector<size_t> count_meas_value(n_integrand), count_meas_std(n_integrand);
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	count_meas_value[integrand_id] = 0;
		count_meas_std[integrand_id]   = 0;
	}
	vector<size_t> count_rate_value(n_rate);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
		count_rate_value[rate_id] = 0;
	for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
	if( mulcov_table[mulcov_id].smooth_id != DISMOD_AT_NULL_INT )
	{	dismod_at::mulcov_type_enum mulcov_type;
		mulcov_type         = mulcov_table[mulcov_id].mulcov_type;
		size_t rate_id      = mulcov_table[mulcov_id].rate_id;
		size_t integrand_id = mulcov_table[mulcov_id].integrand_id;
		size_t covariate_id = mulcov_table[mulcov_id].covariate_id;
		size_t smooth_id    = mulcov_table[mulcov_id].smooth_id;
		//
		dismod_at::pack_info::subvec_info info;
		if( mulcov_type == dismod_at::rate_value_enum ) info =
			pack_object.mulcov_rate_value_info(
				rate_id, count_rate_value[rate_id]++
		);
		//
		else if( mulcov_type == dismod_at::meas_value_enum ) info =
			pack_object.mulcov_meas_value_info(
				integrand_id, count_meas_value[integrand_id]++
		);
		//
		else if( mulcov_type == dismod_at::meas_std_enum ) info =
			pack_object.mulcov_meas_std_info(
				integrand_id, count_meas_std[integrand_id]++
		);
		//
		else assert(false);
		//
		offset    = info.offset;
		assert( smooth_id == info.smooth_id);
		n_var     = info.n_var;
		n_time    = db_input.smooth_table[smooth_id].n_time;
# ifndef NDEBUG
		size_t n_age = db_input.smooth_table[smooth_id].n_age;
		assert( n_var == n_age * n_time );
# endif
		for(size_t index = 0; index < n_var; index++)
		{	size_t age_id   = index / n_time;
			size_t time_id  = index % n_time;
			var_id          = offset + index;
# ifndef NDEBUG
			for(size_t j = 0; j < n_col; j++)
				assert( row_value[ n_col * var_id + j ] == "" );
# endif
			if( mulcov_type == dismod_at::rate_value_enum )
			{	row_value[n_col * var_id + 0]  = "mulcov_rate_value";
				row_value[n_col * var_id + 5] = to_string( rate_id );
			}
			else if( mulcov_type == dismod_at::meas_value_enum )
			{	row_value[n_col * var_id + 0]  = "mulcov_meas_value";
				row_value[n_col * var_id + 6] = to_string( integrand_id );
			}
			else if( mulcov_type == dismod_at::meas_std_enum )
			{	row_value[n_col * var_id + 0]  = "mulcov_meas_std";
				row_value[n_col * var_id + 6] = to_string( integrand_id );
			}
			else assert(false);
			//
			row_value[n_col * var_id + 1] = to_string( smooth_id );
			row_value[n_col * var_id + 2] = to_string( age_id );
			row_value[n_col * var_id + 3] = to_string( time_id );
			row_value[n_col * var_id + 7] = to_string( covariate_id );
		}
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	// ----------------------------------------------------------------------
	return;
}
/*
-----------------------------------------------------------------------------
$begin start_command$$
$spell
	avgint
	var
	dismod
$$

$section The Start Command$$

$head Syntax$$
$codei%dismod_at %database% start %source%$$

$head Purpose$$
This command copies the values specified by $icode source$$
to the $cref start_var_table$$.

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head source$$
The start command argument $icode source$$ must be one of the following:

$subhead prior_mean$$
If $icode source$$ is $code prior_mean$$,
the mean of the priors is used as the starting values for the
next fit; see $cref prior_table$$.

$subhead fit_var$$
If $icode source$$ is $code fit_var$$,
the results of the previous fit is used as the starting values for the
next fit; see $cref prior_table$$.

$head start_var_table$$
This command creates a $cref start_var_table$$ using the
$cref model_variables$$ values specified by $icode source$$.
These values are used as the starting point for subsequent
$cref/fit_commands/fit_command/$$.

$children%example/get_started/start_command.py%$$
$head Example$$
The file $cref start_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void start_command(
	const std::string&                     source      ,
	sqlite3*                               db          ,
	const vector<dismod_at::prior_struct>& prior_table ,
	const dismod_at::pack_info&            pack_object ,
	const vector<dismod_at::smooth_info>&  s_info_vec  )
{	using std::string;
	using CppAD::to_string;
	//
	if( source != "prior_mean" && source != "fit_var" )
	{	string msg = "dismod_at start command source = ";
		msg       += source + " is not one of the following: ";
		msg       += "prior_mean, fit_var";
		dismod_at::error_exit(db, msg);
	}
	//
	string sql_cmd = "drop table if exists start_var";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	// create start_var table
	string table_name = "start_var";
	size_t n_var      = pack_object.size();
	vector<string> col_name(1), col_type(1), row_value(n_var);
	vector<bool>   col_unique(1);
	col_name[0]       = "start_var_value";
	col_type[0]       = "real";
	col_unique[0]     = false;
	//
	if( source == "prior_mean" )
	{	//
		// get prior_id in pack_info order
		vector<size_t> value_prior_id =
			pack_value_prior(pack_object, s_info_vec);
		//
		// put means in row_value
		for(size_t var_id = 0; var_id < n_var; var_id++)
		{	double start_var_value =
				prior_table[ value_prior_id[var_id] ].mean;
			row_value[var_id] = to_string(start_var_value);
		}
	}
	else
	{	assert( source == "fit_var" );
		//
		// get fit_var table information
		vector<double> variable_value;
		string table_name  = "fit_var";
		string column_name = "variable_value";
		dismod_at::get_table_column(
			db, table_name, column_name, variable_value
		);
		//
		// put it in row_value
		for(size_t var_id = 0; var_id < n_var; var_id++)
			row_value[var_id] = to_string(variable_value[var_id]);
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}

/*
-----------------------------------------------------------------------------
$begin fit_command$$
$spell
	avgint
	var
	arg
	num_iter
	dismod
$$

$section The Fit Command$$

$head Syntax$$
$codei%dismod_at %database% fit%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head simulate_table$$
If $cref/fit_simulate_index/option_table/fit_simulate_index/$$ is
not $code null$$, the fit uses the corresponding
$cref/meas_value/simulate_table/meas_value/$$ in the simulate table
in place of the
$cref/meas_value/data_table/meas_value/$$ in the data table.


$head fit_var_table$$
A new $cref fit_var_table$$ is created each time this command is run.
It contains the results of the fit in its
$cref/variable_value/fit_var_table/variable_value/$$ column.

$head fit_data_subset_table$$
A new $cref fit_data_subset_table$$ is created each time this command is run.
It is a comparison of the model and data corresponding to the fit results.

$children%example/get_started/fit_command.py%$$
$head Example$$
The file $cref fit_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void fit_command(
	sqlite3*                                     db               ,
	dismod_at::data_model&                       data_object      ,
	vector<dismod_at::data_subset_struct>&       data_subset_obj  ,
	const dismod_at::pack_info&                  pack_object      ,
	const dismod_at::db_input_struct&            db_input         ,
	const vector<dismod_at::smooth_info>&        s_info_vec       ,
	const dismod_at::prior_model&                prior_object     ,
	// effectively const
	std::map<std::string, std::string>&          option_map
)
{	using std::string;
	using CppAD::to_string;
	// -----------------------------------------------------------------------
	string fit_simulate_index = option_map["fit_simulate_index"];
	if( fit_simulate_index != "" )
	{	size_t simulate_index = std::atoi( fit_simulate_index.c_str() );
		//
		// get simulation data
		vector<dismod_at::simulate_struct> simulate_table =
				dismod_at::get_simulate_table(db);
		size_t n_subset = data_subset_obj.size();
		size_t n_sample = simulate_table.size() / n_subset;
		//
		if( simulate_index >= n_sample )
		{	string msg = "dismod_at fit command fit_simulate_index = ";
			msg += fit_simulate_index + "\nis greater than number of samples";
			msg += " in the simulate table";
			string table_name = "simulate";
			dismod_at::error_exit(db, msg, table_name);
		}
		// replace the data with the simulated values
		for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
		{	size_t simulate_id = n_subset * simulate_index + subset_id;
			data_subset_obj[subset_id].meas_value =
				simulate_table[simulate_id].meas_value;
		}
		data_object.replace_like(data_subset_obj);
	}
	// -----------------------------------------------------------------------
	// read start_var table into start_var
	vector<double> start_var;
	string table_name = "start_var";
	string column_name = "start_var_value";
	dismod_at::get_table_column(db, table_name, column_name, start_var);
	// ------------------ run fit_model ------------------------------------
	bool quasi_fixed = option_map["quasi_fixed"] == "true";
	assert( quasi_fixed || option_map["quasi_fixed"] == "false" );
	//
	// random_bound, null corresponds to infinity
	std::string tmp_str = option_map["random_bound"];
	double random_bound = std::numeric_limits<double>::infinity();
	if( tmp_str != "" )
		random_bound = std::atof( tmp_str.c_str() );
	//
	// A_info
	CppAD::mixed::sparse_mat_info A_info; // empty matrix
	//
	string fit_or_sample = "fit";
	dismod_at::fit_model fit_object(
		db                   ,
		random_bound         ,
		fit_or_sample        ,
		pack_object          ,
		start_var            ,
		db_input.prior_table ,
		s_info_vec           ,
		data_object          ,
		prior_object         ,
		quasi_fixed          ,
		A_info
	);
	fit_object.run_fit(option_map);
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
	col_name[0]       = "variable_value";
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
		// variable_value
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
	{	CppAD::vector< dismod_at::residual_struct<double> > residual;
		if( variable_type == 0 )
			residual  = prior_object.fixed(opt_value);
		else
			residual  = prior_object.random(opt_value);
		//
		for(size_t i = 0; i < residual.size(); i++)
		{	size_t id     = residual[i].id;
			size_t var_id = id / 3;
			size_t k      = 1 + id % 3;
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
	n_col           = 3;
	col_name.resize(n_col);
	col_type.resize(n_col);
	col_unique.resize(n_col);
	row_value.clear();
	row_value.resize(n_col * n_subset);
	//
	col_name[0]   = "data_subset_id";
	col_type[0]   = "integer";
	col_unique[0] = true;
	//
	col_name[1]   = "avg_integrand";
	col_type[1]   = "real";
	col_unique[1] = false;
	//
	col_name[2]   = "weighted_residual";
	col_type[2]   = "real";
	col_unique[2] = false;
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

			case dismod_at::prevalence_enum:
			case dismod_at::Tincidence_enum:
			case dismod_at::mtspecific_enum:
			case dismod_at::mtall_enum:
			case dismod_at::mtstandard_enum:
			avg = data_object.avg_yes_ode(subset_id, opt_value);
			break;

			default:
			assert(false);
		}
		// compute its residual and log likelihood
		dismod_at::residual_struct<double> residual =
			data_object.like_one(subset_id, opt_value, avg);
		//
		row_value[ subset_id * n_col + 0] = to_string( subset_id );
		row_value[ subset_id * n_col + 1] = to_string( avg );
		row_value[ subset_id * n_col + 2] = to_string( residual.wres );
	}
	dismod_at::create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}
/*
-----------------------------------------------------------------------------
$begin truth_command$$
$spell
	avgint
	var
	dismod
$$

$section The Truth Command$$

$head Syntax$$
$codei%dismod_at %database% truth%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head fit_var_table$$
In addition to the standard $cref input$$ tables,
there must be a $cref fit_var_table$$.

$head truth_var_table$$
A new $cref truth_var_table$$ is created with the information
in the fit_var table;
to be specific,
$codei%
	%truth_var_id% = %fit_var_id% = %var_id%
	%truth_var_value% = %variable_value%
%$$

$children%example/get_started/truth_command.py%$$
$head Example$$
The file $cref truth_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void truth_command(sqlite3* db)
{	using std::string;
	using CppAD::to_string;
	//
	// get fit_var table information
	vector<double> variable_value;
	string table_name  = "fit_var";
	string column_name = "variable_value";
	dismod_at::get_table_column(db, table_name, column_name, variable_value);
	//
	// create truth_var table
	string sql_cmd = "drop table if exists truth_var";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name   = "truth_var";
	size_t n_var = variable_value.size();
	vector<string> col_name(1), col_type(1), row_value(n_var);
	vector<bool>   col_unique(1);
	col_name[0]       = "truth_var_value";
	col_type[0]       = "real";
	col_unique[0]     = false;
	for(size_t fit_var_id = 0; fit_var_id < n_var; fit_var_id++)
	{	double truth_var_value  = variable_value[fit_var_id];
		row_value[fit_var_id]   = to_string( truth_var_value );
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
	avgint
	var
	dismod
	arg
	std
	covariates
$$

$head Syntax$$
$codei%dismod_at %database% simulate%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head truth_var_table$$
The $cref truth_var_table$$ is an addition input table for this command.
It specifies the true values for the
$cref model_variables$$ used during the simulation.
This table can be create by the $cref truth_command$$,
or the user can create it directly with the aid of the
$cref var_table$$ (created by the $cref init_command$$).

$head simulate_table$$
A new $cref simulate_table$$ is created.
It contains simulated measurement values that can be used in place of
the data table $cref/meas_value/data_table/meas_value/$$ column.
Only the $cref/data_id/data_subset_table/data_id/$$ that are in the
data_subset table are included.

$children%example/get_started/simulate_command.py%$$
$head Example$$
The file $cref simulate_command.py$$ contains an example and test
of using this command.

$end
*/
void simulate_command(
	sqlite3*                                            db              ,
	const vector<dismod_at::integrand_struct>&          integrand_table ,
	const vector<dismod_at::data_subset_struct>&        data_subset_obj ,
	const dismod_at::data_model&                        data_object     ,
	const size_t&                                       number_simulate
)
{
	using std::string;
	using CppAD::to_string;
	// -----------------------------------------------------------------------
	// read truth_var table into truth_var
	vector<double> truth_var;
	string table_name = "truth_var";
	string column_name = "truth_var_value";
	dismod_at::get_table_column(db, table_name, column_name, truth_var);
	// ----------------- simulate_table ----------------------------------
	//
	string sql_cmd = "drop table if exists simulate";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name      = "simulate";
	size_t n_col    = 3;
	size_t n_subset = data_subset_obj.size();
	size_t n_row    = number_simulate * n_subset;
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
	col_name[2]   = "meas_value";
	col_type[2]   = "real";
	col_unique[2] = false;
	//
	for(size_t sim_index = 0; sim_index < number_simulate; sim_index++)
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	size_t integrand_id =  data_subset_obj[subset_id].integrand_id;
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

			case dismod_at::prevalence_enum:
			case dismod_at::Tincidence_enum:
			case dismod_at::mtspecific_enum:
			case dismod_at::mtall_enum:
			case dismod_at::mtstandard_enum:
			avg = data_object.avg_yes_ode(subset_id, truth_var);
			break;

			default:
			avg = std::numeric_limits<double>::quiet_NaN();
			assert(false);
		}
		// need to simulate random noise with proper density
		dismod_at::density_enum density = dismod_at::density_enum(
			data_subset_obj[subset_id].density_id
		);
		double meas_std     = data_subset_obj[subset_id].meas_std;
		double eta          = integrand_table[integrand_id].eta;
		double meas_value   = dismod_at::sim_random(
			density, avg, meas_std, eta
		);
		size_t simulate_id = sim_index * n_subset + subset_id;
		row_value[simulate_id * n_col + 0] = to_string( sim_index );
		row_value[simulate_id * n_col + 1] = to_string( subset_id );
		row_value[simulate_id * n_col + 2] = to_string(meas_value);
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
	avgint
	dismod
	var
	arg
$$

$section The Sample Command$$

$head Syntax$$
$codei%dismod_at %database% sample %method%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head method$$
The sample command argument $icode method$$ must be one of the following:

$subhead simulate$$
If $icode method$$ is $code simulate$$,
the measurements in the $cref simulate_table$$ table will be used
to create the $cref sample_table$$.
For $cref/sample_index/sample_table/sample_index/$$ equal zero to
$cref/number_simulate/option_table/number_simulate/$$ minus one,
the corresponding measurement set has
$cref/simulate_index/simulate_table/simulate_index/$$ in the
simulate table equal to $icode sample_index$$ in the sample table.
The $cref model_variables$$ in the sample table are the optimal values
for the corresponding measurement set in the simulate table.
This requires running $icode number_simulate$$ fits of the model variables.

$subhead asymptotic$$
If $icode method$$ is $code asymptotic$$,
The number of samples of the model variables in the simulate table is
$cref/number_simulate/option_table/number_simulate/$$.
For each value of $code sample_index$$,
the asymptotic statics of the model variables is used to generate
the corresponding in the sample table.
Sample with different values of $icode sample_index$$ are independent.

$subhead fit_var$$
If $icode method$$ is $code fit_var$$,
the values in the $cref fit_var_table$$ are copied to the $cref sample_table$$.
In this case, there is only one sample of the $cref model_variables$$
in the sample table and
$cref/sample_index/sample_table/sample_index/$$ is always zero.

$head simulate_table$$
If $icode method$$ is $code simulate$$,
this command has the extra input $cref  simulate_table$$
which was created by the previous $cref simulate_command$$.

$head fit_var_table$$
If $icode method$$ is $code asymptotic$$ or $code fit_var$$,
this command has the extra input $cref fit_var_table$$
which was created by a previous $cref fit_command$$.

$head sample_table$$
A new $cref sample_table$$ is created each time this command is run.
It contains samples of the model variables.

$children%example/get_started/sample_command.py
	%example/user/asymptotic.py
%$$
$head Example$$
The files
$cref sample_command.py$$ and $cref user_asymptotic.py$$
contain examples and tests of using this command.

$end
*/
// ----------------------------------------------------------------------------
void sample_command(
	const std::string&                          method           ,
	sqlite3*                                    db               ,
	vector<dismod_at::data_subset_struct>&      data_subset_obj  ,
	dismod_at::data_model&                      data_object      ,
	const dismod_at::pack_info&                 pack_object      ,
	const dismod_at::db_input_struct&           db_input         ,
	const vector<dismod_at::smooth_info>&       s_info_vec       ,
	const dismod_at::prior_model&               prior_object     ,
	// effectively const
	std::map<std::string, std::string>&         option_map
)
{	using std::string;
	using CppAD::to_string;
	// -------------------------------------------------------------------
	if( method != "simulate" && method != "fit_var" && method != "asymptotic" )
	{	string msg = "dismod_at sample command method = ";
		msg       += method + " is not one of the following: ";
		msg       += "simulate, fit_var, asymptotic";
		dismod_at::error_exit(db, msg);
	}
	// -----------------------------------------------------------------------
	// create new sample table and prepare to write into it
	size_t n_sample = 1;
	if( method != "fit_var" )
		n_sample = std::atoi( option_map["number_simulate"].c_str() );
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
	if( method == "fit_var" )
	{	// get fit_var table information
		vector<double> variable_value;
		string table_name  = "fit_var";
		string column_name = "variable_value";
		dismod_at::get_table_column(
			db, table_name, column_name, variable_value
		);
		//
		size_t sample_index     = 0;
		string sample_index_str = to_string( sample_index );
		assert( variable_value.size() == n_var );
		for(size_t var_id = 0; var_id < n_var; var_id++)
		{	size_t sample_id = sample_index * n_var + var_id;
			row_value[n_col * sample_id + 0] = sample_index_str;
			row_value[n_col * sample_id + 1] = to_string( var_id );
			row_value[n_col * sample_id + 2] =
				to_string( variable_value[var_id] );
		}
		table_name = "sample";
		dismod_at::create_table(
			db, table_name, col_name, col_type, col_unique, row_value
		);
		return;
	}
	// ----------------------------------------------------------------------
	// quasi_fixed
	bool quasi_fixed = option_map["quasi_fixed"] == "true";
	assert( quasi_fixed || option_map["quasi_fixed"] == "false" );
	//
	// random_bound, null corresponds to infinity
	std::string tmp_str = option_map["random_bound"];
	double random_bound = std::numeric_limits<double>::infinity();
	if( tmp_str != "" )
		random_bound = std::atof( tmp_str.c_str() );
	//
	// A_info = empty matrix
	CppAD::mixed::sparse_mat_info A_info;
	// -----------------------------------------------------------------------
	if( method == "simulate" )
	{
		//
		// fit_var.variable_value
		vector<double> truth_var_value;
		string table_name  = "truth_var";
		string column_name = "truth_var_value";
		dismod_at::get_table_column(
			db, table_name, column_name, truth_var_value
		);
		// get simulated data
		vector<dismod_at::simulate_struct> simulate_table =
				dismod_at::get_simulate_table(db);
		//
		size_t n_subset = data_subset_obj.size();
		assert( simulate_table.size() % n_subset == 0 );
		assert( n_sample == simulate_table.size() / n_subset );
		//
		assert( n_sample * n_subset == simulate_table.size() );
		for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
		{	// set the measurement values for corresponding simulation
			size_t offset = n_subset * sample_index;
			for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
			{	size_t simulate_id = offset + subset_id;
				size_t sample_check =
					size_t(simulate_table[simulate_id].simulate_index);
				size_t subset_check =
					size_t(simulate_table[simulate_id].data_subset_id);
				if( sample_check != sample_index || subset_check != subset_id )
				{	string msg = "dismod_at database sample simulate\n";
					msg  += "size of simulate table does not make sense\n";
					msg +=  "restart with init command";
					table_name = "simulate";
					dismod_at::error_exit(db, msg, table_name, simulate_id);
				}
				data_subset_obj[subset_id].meas_value =
					simulate_table[simulate_id].meas_value;
			}
			data_object.replace_like(data_subset_obj);

			// fit_model
			string fit_or_sample = "fit";
			dismod_at::fit_model fit_object(
				db                   ,
				random_bound         ,
				fit_or_sample        ,
				pack_object          ,
				truth_var_value      ,
				db_input.prior_table ,
				s_info_vec           ,
				data_object          ,
				prior_object         ,
				quasi_fixed          ,
				A_info
			);
			fit_object.run_fit(option_map);
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
	// fit_var.variable_value
	vector<double> variable_value;
	string table_name  = "fit_var";
	string column_name = "variable_value";
	dismod_at::get_table_column(
		db, table_name, column_name, variable_value
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
	// random_opt
	size_t n_random = number_random(pack_object);
	vector<double> random_opt(n_random);
	unpack_random(pack_object, variable_value, random_opt);
	//
	// fit_object
	string fit_or_sample = "sample";
	dismod_at::fit_model fit_object(
		db                   ,
		random_bound         ,
		fit_or_sample        ,
		pack_object          ,
		variable_value       ,
		db_input.prior_table ,
		s_info_vec           ,
		data_object          ,
		prior_object         ,
		quasi_fixed          ,
		A_info
	);
	//
	// sample
	vector<double> sample(n_sample * n_var);
	fit_object.sample_posterior(
		sample               ,
		variable_value       ,
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
	avgint
	dismod
	var
	arg
$$

$section The Predict Command$$

$head Syntax$$
$codei%dismod_at %database% predict%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head sample_table$$
This command has the extra input $cref sample_table$$
which was created by a previous $cref sample_command$$.

$head predict_table$$
A new $cref predict_table$$ is created each time this command is run.
It contains the
$cref/average integrand/avg_integrand/Average Integrand, A_i/$$
values for each
$cref/sample_index/sample_table/sample_index/$$ in the sample table
and each
$cref/avgint_subset_id/avgint_subset_table/avgint_subset_id/$$
in the avgint_subset table.

$children%example/get_started/predict_command.py%$$
$head Example$$
The files $cref predict_command.py$$ and
$cref user_predict_fit.py$$ contain examples and tests
using this command.

$end
*/

// ----------------------------------------------------------------------------
void predict_command(
	sqlite3*                                          db                  ,
	const dismod_at::db_input_struct&                 db_input            ,
	size_t                                            n_var               ,
	dismod_at::data_model&                            avgint_object     ,
	const vector<dismod_at::avgint_subset_struct>&  avgint_subset_obj
)
{
	using std::string;
	using CppAD::to_string;

	// sample table
	vector<dismod_at::sample_struct> sample_table =
		dismod_at::get_sample_table(db);

	// -----------------------------------------------------------------------
	// create a new predict table
	string sql_cmd = "drop table if exists predict";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	string table_name = "predict";
	size_t n_col      = 3;
	size_t n_sample   = sample_table.size() / n_var;
	size_t n_subset   = avgint_subset_obj.size();
	size_t n_row      = n_sample * n_subset;
	vector<string> col_name(n_col), col_type(n_col), row_value(n_col * n_row);
	vector<bool>   col_unique(n_col);
	//
	col_name[0]   = "sample_index";
	col_type[0]   = "integer";
	col_unique[0] = false;
	//
	col_name[1]   = "avgint_subset_id";
	col_type[1]   = "integer";
	col_unique[1] = false;
	//
	col_name[2]   = "avg_integrand";
	col_type[2]   = "real";
	col_unique[2] = false;
	//
	// pack_vec
	CppAD::vector<double> pack_vec(n_var);
	//
	size_t sample_id = 0;
	for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
	{	// copy the variable values for this sample index into pack_vec
		for(size_t var_id = 0; var_id < n_var; var_id++)
		{	size_t sample_check =
				size_t( sample_table[sample_id].sample_index);
			size_t var_check =
				size_t( sample_table[sample_id].var_id);
			if( sample_check != sample_index || var_check != var_id )
			{	string msg = "database modified, restart with init command";
				table_name = "sample";
				dismod_at::error_exit(db, msg, table_name, sample_id);
			}
			pack_vec[var_id] = sample_table[sample_id++].var_value;
		}
		for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
		{	int integrand_id = avgint_subset_obj[subset_id].integrand_id;
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
				case dismod_at::prevalence_enum:
				case dismod_at::Tincidence_enum:
				case dismod_at::mtspecific_enum:
				case dismod_at::mtall_enum:
				case dismod_at::mtstandard_enum:
				avg = avgint_object.avg_yes_ode(subset_id, pack_vec);
				break;
				//
				default:
				assert(false);
			}
			size_t predict_id = sample_index * n_subset + subset_id;
			row_value[n_col * predict_id + 0] = to_string( sample_index );
			row_value[n_col * predict_id + 1] = to_string( subset_id );
			row_value[n_col * predict_id + 2] = to_string( avg );
		}
	}
	assert( n_sample * n_var == sample_table.size() );
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
		{"start",     4},
		{"truth",     3},
		{"fit",       3},
		{"simulate",  3},
		{"sample",    4},
		{"predict",   3}
	};
	size_t n_command = sizeof( command_info ) / sizeof( command_info[0] );
	//
	string program = "dismod_at-";
	program       += DISMOD_AT_VERSION;
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
	const string command_arg    = argv[2];
	for(size_t i = 0; i < n_command; i++)
	{	if( command_arg == command_info[i].name )
		{	if( n_arg != command_info[i].n_arg )
			{	cerr << program << endl
				<< "expected " << command_info[i].n_arg - 3
				<< " arguments to follow " << command_info[i].name << "\n";
				std::exit(1);
			}
		}
	}
	string message;
	// --------------- open connection to datbase ---------------------------
	bool new_file = false;
	sqlite3* db   = dismod_at::open_connection(database_arg, new_file);
	// --------------- log start of this command -----------------------------
	message = "begin " + command_arg;
	std::time_t unix_time = dismod_at::log_message(db, "command", message);
	// --------------- get the input tables ---------------------------------
	std::time_t current_time = trace("Reading database");
	dismod_at::db_input_struct db_input;
	get_db_input(db, db_input);
	trace("elapsed time = ", current_time);
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
	assert( age_max  <= age_min  + (n_age_ode - 1) * ode_step_size );
	// ---------------------------------------------------------------------
	// n_time_ode
	double time_min   = dismod_at::min_vector( db_input.time_table );
	double time_max   = dismod_at::max_vector( db_input.time_table );
	size_t n_time_ode = size_t( (time_max - time_min) / ode_step_size + 2.0 );
	assert( time_max <= time_min  + (n_time_ode - 1) * ode_step_size );
	// ---------------------------------------------------------------------
	// child_data, child_avgint, and some more size_t values
	size_t parent_node_id = std::atoi(
		option_map["parent_node_id"].c_str()
	);
	dismod_at::child_info child_data(
		parent_node_id          ,
		db_input.node_table     ,
		db_input.data_table
	);
	dismod_at::child_info child_avgint(
		parent_node_id          ,
		db_input.node_table     ,
		db_input.avgint_table
	);
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
			db                         ,
			db_input.age_table         ,
			db_input.time_table        ,
			smooth_id                  ,
			db_input.smooth_table      ,
			db_input.smooth_grid_table
		);
	}
	// pack_object
	dismod_at::pack_info pack_object(
		db                    ,
		n_integrand           ,
		n_child               ,
		db_input.smooth_table ,
		db_input.mulcov_table ,
		db_input.rate_table
	);
	if( command_arg == "start" )
	{	start_command(
			argv[3]              , // source
			db                   ,
			db_input.prior_table ,
			pack_object          ,
			s_info_vec
		);
	}
	else if( command_arg == "truth" )
	{	truth_command(db);
	}
	else
	{	// data_subset_obj
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
		// avgint_subset_obj
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
		// -------------------------------------------------------------------
		if( command_arg == "init" )
		{	init_command(
				db,
				data_subset_obj,
				avgint_subset_obj,
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
				db_input.age_table    ,
				db_input.time_table   ,
				db_input.prior_table  ,
				s_info_vec
			);
			// data_object
			dismod_at::data_model data_object(
				parent_node_id           ,
				n_covariate              ,
				n_age_ode                ,
				n_time_ode               ,
				ode_step_size            ,
				db_input.age_table       ,
				db_input.time_table      ,
				db_input.integrand_table ,
				db_input.node_table      ,
				data_subset_obj          ,
				data_subset_cov_value    ,
				w_info_vec               ,
				s_info_vec               ,
				pack_object              ,
				child_data
			);
			string rate_case = option_map["rate_case"];
			data_object.set_eigen_ode2_case_number(rate_case);
			data_object.replace_like( data_subset_obj );
			//
			// avgint_object
			dismod_at::data_model avgint_object(
				parent_node_id           ,
				n_covariate              ,
				n_age_ode                ,
				n_time_ode               ,
				ode_step_size            ,
				db_input.age_table       ,
				db_input.time_table      ,
				db_input.integrand_table ,
				db_input.node_table      ,
				avgint_subset_obj        ,
				avgint_subset_cov_value  ,
				w_info_vec               ,
				s_info_vec               ,
				pack_object              ,
				child_avgint
			);
			avgint_object.set_eigen_ode2_case_number(rate_case);
			// ------------------------------------------------------------------
			if( command_arg == "fit" )
			{	fit_command(
					db               ,
					data_object      ,
					data_subset_obj  ,
					pack_object      ,
					db_input         ,
					s_info_vec       ,
					prior_object     ,
					option_map
				);
			}
			else if( command_arg == "simulate" )
			{	size_t number_simulate = std::atoi(
					option_map["number_simulate"].c_str()
				);
				simulate_command(
					db                       ,
					db_input.integrand_table ,
					data_subset_obj          ,
					data_object              ,
					number_simulate
				);
			}
			else if( command_arg == "sample" )
			{
				sample_command(
					argv[3]          , // method
					db               ,
					data_subset_obj  ,
					data_object      ,
					pack_object      ,
					db_input         ,
					s_info_vec       ,
					prior_object     ,
					option_map
				);
			}
			else if( command_arg == "predict" )
			{	size_t n_var = pack_object.size();
				predict_command(
					db                   ,
					db_input             ,
					n_var                ,
					avgint_object      ,
					avgint_subset_obj
				);
			}
			else
				assert(false);
		}
	}
	// ---------------------------------------------------------------------
	message = "end " + command_arg;
	dismod_at::log_message(db, "command", message);
	sqlite3_close(db);
	return 0;
}
