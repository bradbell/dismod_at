// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <map>
# include <cassert>
# include <string>
# include <iostream>
# include <cppad/vector.hpp>
# include <dismod_at/avg_case_subset.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/get_column_max.hpp>
# include <dismod_at/get_db_input.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_sample_table.hpp>
# include <dismod_at/get_simulate_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/manage_gsl_rng.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/put_table_row.hpp>
# include <dismod_at/sim_random.hpp>
# include <dismod_at/to_string.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/pack_prior.hpp>

namespace { // BEGIN_EMPTY_NAMESPACE
	using CppAD::vector;
/*
-----------------------------------------------------------------------------
$begin init_command$$
$spell
	init
	var
	dismod
$$

$section The Variable Command$$

$head Syntax$$
$codei%dismod_at init %file_name%$$

$head Purpose$$
This command should be executed whenever any of the
$cref input$$ tables change.

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head var_table$$
A new $cref var_table$$ is created with the information
that maps a $cref/var_id/var_table/var_id/$$
to its meaning in terms of the
$cref/model variables/model_variable/$$.

$head data_subset_table$$
A new $cref data_subset_table$$ is created.
This makes explicit exactly which rows of the data table are used.

$head avg_case_subset_table$$
A new $cref avg_case_subset_table$$ is created.
This makes explicit exactly which rows of the avg_case table are used.

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
	const vector<dismod_at::avg_case_subset_struct>& avg_case_subset_obj ,
	const dismod_at::pack_info&                      pack_object         ,
	const dismod_at::db_input_struct&                db_input            ,
	const size_t&                                    parent_node_id      ,
	const dismod_at::child_info&                     child_object        ,
	const vector<dismod_at::smooth_info>&            s_info_vec          )
{	using std::string;
	using dismod_at::to_string;

	// -----------------------------------------------------------------------
	const char* drop_list[] = {
		"var",
		"avg_data_subset",
		"data_subset"
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
	sql_cmd = "create table data_subset("
		" data_subset_id  integer primary key,"
		" data_id         integer"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	string table_name = "data_subset";
	size_t n_subset   = data_subset_obj.size();
	vector<string> col_name_vec(1), row_val_vec(1);
	col_name_vec[0] = "data_id";
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	int data_id    = data_subset_obj[subset_id].original_id;
		row_val_vec[0] = to_string( data_id );
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	// -----------------------------------------------------------------------
	// create avg_case_subset_table
	sql_cmd = "create table avg_case_subset("
		" avg_case_subset_id  integer primary key,"
		" avg_case_id         integer"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name = "avg_case_subset";
	n_subset   = avg_case_subset_obj.size();
	col_name_vec[0] = "avg_case_id";
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	int avg_case_id    = avg_case_subset_obj[subset_id].original_id;
		row_val_vec[0] = to_string( avg_case_id );
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	// -----------------------------------------------------------------------
	sql_cmd = "create table var("
		" var_id         integer primary key,"
		" var_type       text,"
		" smooth_id      integer,"
		" age_id         integer,"
		" time_id        integer,"
		" node_id        integer,"
		" rate_id        integer,"
		" integrand_id   integer,"
		" covariate_id   integer"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	table_name = "var";
	//
	col_name_vec.resize(8);
	row_val_vec.resize(8);
	col_name_vec[0]   = "var_type";
	col_name_vec[1]   = "smooth_id";
	col_name_vec[2]   = "age_id";
	col_name_vec[3]   = "time_id";
	col_name_vec[4]   = "node_id";
	col_name_vec[5]   = "rate_id";
	col_name_vec[6]   = "integrand_id";
	col_name_vec[7]   = "covariate_id";
	//
	// mulstd variables
	size_t n_smooth = db_input.smooth_table.size();
	size_t offset, var_id;
	for(size_t i = 2; i < row_val_vec.size(); i++)
		row_val_vec[i] = "null"; // these columns are null for mulstd variables
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	offset      = pack_object.mulstd_offset(smooth_id);
		for(size_t i = 0; i < 3; i++)
		{	var_id                   = offset + i;
			// var_type
			if( i == 0 )
				row_val_vec[0] = "mulstd_value";
			else if( i == 1 )
				row_val_vec[0] = "mulstd_dage";
			else
				row_val_vec[0] = "mulstd_dtime";
			//
			// smooth_id
			row_val_vec[1] = to_string( smooth_id );
			//
			dismod_at::put_table_row(
				db,
				table_name,
				col_name_vec,
				row_val_vec,
				var_id
			);
		}
	}
	//
	// rate variables
	size_t n_rate  = db_input.rate_table.size();
	size_t n_child = child_object.child_size();
	size_t smooth_id, n_var, n_age, n_time, node_id;
	dismod_at::pack_info::subvec_info info;
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	for(size_t child_id = 0; child_id <= n_child; child_id++)
		{	info      = pack_object.rate_info(rate_id, child_id);
			offset    = info.offset;
			smooth_id = info.smooth_id;
			n_var     = info.n_var;
			n_age     = db_input.smooth_table[smooth_id].n_age;
			n_time    = db_input.smooth_table[smooth_id].n_time;
			if( child_id == n_child )
				node_id = parent_node_id;
			else
				node_id = child_object.child_id2node_id(child_id);
			assert( n_var == n_age * n_time );
			for(size_t index = 0; index < n_var; index++)
			{	size_t i       = index % n_age;
				size_t j       = index / n_age;
				var_id         = offset + index;
				size_t age_id  = s_info_vec[smooth_id].age_id(i);
				size_t time_id = s_info_vec[smooth_id].time_id(j);
				//
				// variable_value
				row_val_vec[0]  = "rate";     // var_type
				row_val_vec[1]  = "null";     // smooth_id
				row_val_vec[2]  = to_string( age_id );
				row_val_vec[3]  = to_string( time_id );
				row_val_vec[4]  = to_string( node_id );
				row_val_vec[5]  = to_string( rate_id );
				row_val_vec[6]  = "null";     // integrand_id
				row_val_vec[7]  = "null";     // covariate_id
				dismod_at::put_table_row(
					db,
					table_name,
					col_name_vec,
					row_val_vec,
					var_id
				);
			}
		}
	}
	//
	// covariate multiplers
	const vector<dismod_at::mulcov_struct>&
		mulcov_table( db_input.mulcov_table );
	size_t n_mulcov        = mulcov_table.size();
	size_t count_rate_mean  = 0;
	size_t count_meas_value = 0;
	size_t count_meas_std   = 0;
	for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
	{	dismod_at::mulcov_type_enum mulcov_type;
		mulcov_type     = mulcov_table[mulcov_id].mulcov_type;
		size_t rate_id  = mulcov_table[mulcov_id].rate_id;
		size_t integrand_id = mulcov_table[mulcov_id].integrand_id;
		size_t covariate_id = mulcov_table[mulcov_id].covariate_id;
		size_t smooth_id    = mulcov_table[mulcov_id].smooth_id;
		//
		if( mulcov_type == dismod_at::rate_mean_enum ) info =
		pack_object.mulcov_rate_mean_info(rate_id, count_rate_mean++);
		//
		else if( mulcov_type == dismod_at::meas_value_enum ) info =
		pack_object.mulcov_meas_value_info(integrand_id, count_meas_value++);
		//
		else if( mulcov_type == dismod_at::meas_std_enum ) info =
		pack_object.mulcov_meas_std_info(integrand_id, count_meas_std++);
		//
		else assert(false);
		//
		offset    = info.offset;
		assert( smooth_id == info.smooth_id);
		n_var     = info.n_var;
		n_age     = db_input.smooth_table[smooth_id].n_age;
		n_time    = db_input.smooth_table[smooth_id].n_time;
		assert( n_var == n_age * n_time );
		for(size_t index = 0; index < n_var; index++)
		{	size_t age_id   = index % n_age;
			size_t time_id  = index / n_age;
			var_id          = offset + index;
			//
		// var_type
			if( mulcov_type == dismod_at::rate_mean_enum )
				row_val_vec[0]  = "mulcov_rate_mean";
			else if( mulcov_type == dismod_at::meas_value_enum )
				row_val_vec[0]  = "mulcov_meas_value";
			else if( mulcov_type == dismod_at::meas_std_enum )
				row_val_vec[0]  = "mulcov_meas_std";
			else assert(false);
			//
			row_val_vec[1]  = "null";     // smooth_id
			row_val_vec[2]  = to_string( age_id );
			row_val_vec[3]  = to_string( time_id );
			row_val_vec[4]  = "null";     // node_id
			row_val_vec[5]  = "null";     // rate_id
			row_val_vec[6]  = to_string( integrand_id );
			row_val_vec[7]  = to_string( covariate_id );
			dismod_at::put_table_row(
				db,
				table_name,
				col_name_vec,
				row_val_vec,
				var_id
			);
		}
	}
	return;
}
/*
-----------------------------------------------------------------------------
$begin start_command$$
$spell
	var
	dismod
$$

$section The Start Command$$

$head Syntax$$
$codei%dismod_at start %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head start_var_table$$
The start table is required for the $cref fit_command$$.
This command creates a $cref start_var_table$$ using the means
for the prior for each $cref model_variable$$.

$children%example/get_started/start_command.py%$$
$head Example$$
The file $cref start_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void start_command(
	sqlite3*          db        ,
	const vector<dismod_at::prior_struct>& prior_table ,
	const dismod_at::pack_info&            pack_object ,
	const vector<dismod_at::smooth_info>&  s_info_vec  )
{	using std::string;
	using dismod_at::to_string;
	//
	// Value prior_id in pack_info order
	vector<size_t> value_prior_id = pack_value_prior(pack_object, s_info_vec);
	//
	// number of variables
	size_t n_var = pack_object.size();
	//
	// create start_var table
	string sql_cmd = "drop table if exists start_var";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table start_var("
		" start_var_id       integer primary key,"
		" start_var_value    real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	string table_name = "start_var";
	vector<string> col_name_vec(1), row_val_vec(1);
	col_name_vec[0]   = "start_var_value";
	for(size_t var_id = 0; var_id < n_var; var_id++)
	{	double start_var_value    = prior_table[ value_prior_id[var_id] ].mean;
		row_val_vec[0]     = to_string(start_var_value);
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	return;
}

/*
-----------------------------------------------------------------------------
$begin fit_command$$
$spell
	var
	arg
	num_iter
	dismod
$$

$section The Fit Command$$

$head Syntax$$
$codei%dismod_at fit %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head fit_var_table$$
A new $cref fit_var_table$$ is created each time this command is run.
It contains the results of the fit in its
$cref/fit_var_value/fit_var_table/fit_var_value/$$ column.

$head fit_residual_table$$
A new $cref fit_residual_table$$ is created each time this command is run.
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
	const vector<dismod_at::data_subset_struct>& data_subset_obj  ,
	const dismod_at::pack_info&                  pack_object      ,
	const dismod_at::db_input_struct&            db_input         ,
	const vector<dismod_at::smooth_info>&        s_info_vec       ,
	const dismod_at::prior_model&                prior_object     ,
	// effectively const
	std::map<std::string, std::string>&          option_map
)
{	using std::string;
	using dismod_at::to_string;
	// -----------------------------------------------------------------------
	// read start_var table into start_var
	vector<double> start_var;
	string table_name = "start_var";
	string column_name = "start_var_value";
	dismod_at::get_table_column(db, table_name, column_name, start_var);
	// ------------------ run fit_model ------------------------------------
	dismod_at::fit_model fit_object(
		db                   ,
		pack_object          ,
		start_var            ,
		db_input.prior_table ,
		s_info_vec           ,
		data_object          ,
		prior_object
	);
	fit_object.run_fit(option_map);
	vector<double> solution = fit_object.get_solution();
	// -------------------- fit_var table --------------------------------------
	string sql_cmd = "drop table if exists fit_var";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table fit_var("
		" fit_var_id   integer primary key,"
		" fit_var_value    real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name = "fit_var";
	vector<string> col_name_vec(1), row_val_vec(1);
	col_name_vec[0]   = "fit_var_value";
	for(size_t fit_var_id = 0; fit_var_id < solution.size(); fit_var_id++)
	{	double fit_var_value   = solution[fit_var_id];
		row_val_vec[0] = to_string( fit_var_value );
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	// ------------------ fit_residual table --------------------------------
	sql_cmd = "drop table if exists fit_residual";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table    fit_residual("
		" fit_residual_id      integer primary key,"
		" data_subset_id       integer,"
		" avg_integrand        real,"
		" weighted_residual    real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name = "fit_residual";
	col_name_vec.resize(3);
	row_val_vec.resize(3);
	col_name_vec[0] = "data_subset_id";
	col_name_vec[1] = "avg_integrand";
	col_name_vec[2] = "weighted_residual";
	//
	size_t n_subset = data_subset_obj.size();
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
			avg = data_object.avg_no_ode(subset_id, solution);
			break;

			case dismod_at::prevalence_enum:
			case dismod_at::Tincidence_enum:
			case dismod_at::mtspecific_enum:
			case dismod_at::mtall_enum:
			case dismod_at::mtstandard_enum:
			avg = data_object.avg_yes_ode(subset_id, solution);
			break;

			default:
			assert(false);
		}
		// compute its residual and log likelihood
		dismod_at::residual_struct<double> residual =
			data_object.like_one(subset_id, solution, avg);
		//
		row_val_vec[0] = to_string( subset_id );
		row_val_vec[1] = to_string( avg );
		row_val_vec[2] = to_string( residual.wres );
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	return;
}
/*
-----------------------------------------------------------------------------
$begin truth_command$$
$spell
	var
	dismod
$$

$section The Truth Command$$

$head Syntax$$
$codei%dismod_at truth %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
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
	%truth_var_value% = %fit_var_value%
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
	using dismod_at::to_string;
	//
	// get fit_var table information
	vector<double> fit_var_value;
	string table_name  = "fit_var";
	string column_name = "fit_var_value";
	dismod_at::get_table_column(db, table_name, column_name, fit_var_value);
	//
	// create truth_var table
	string sql_cmd = "drop table if exists truth_var";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table truth_var("
		" truth_var_id   integer primary key,"
		" truth_var_value    real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name = "truth_var";
	vector<string> col_name_vec(1), row_val_vec(1);
	col_name_vec[0]   = "truth_var_value";
	for(size_t fit_var_id = 0; fit_var_id < fit_var_value.size(); fit_var_id++)
	{	string truth_var_value = to_string( fit_var_value[fit_var_id] );
		row_val_vec[0]     = truth_var_value;
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	return;
}
/*
-----------------------------------------------------------------------------
$begin simulate_command$$

$section The Simulate Command$$
$spell
	var
	dismod
	arg
	std
	covariates
$$

$head Syntax$$
$codei%dismod_at simulate %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head truth_var_table$$
The $cref truth_var_table$$ is an addition input table for this command.
It specifies the true values for the
$cref/model_variables/model_variable/$$ used during the simulation.
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
	const size_t&                                       number_sample
)
{
	using std::string;
	using dismod_at::to_string;
	// -----------------------------------------------------------------------
	// read truth_var table into truth_var
	vector<double> truth_var;
	string table_name = "truth_var";
	string column_name = "truth_var_value";
	dismod_at::get_table_column(db, table_name, column_name, truth_var);
	// ----------------- simulate_table ----------------------------------
	table_name = "simulate";
	//
	string sql_cmd = "drop table if exists simulate";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	sql_cmd = "create table simulate("
		" simulate_id     integer primary key,"
		" sample_index    integer,"
		" data_subset_id  integer,"
		" meas_value      real"
	");";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	vector<string> col_name_vec(3), row_val_vec(3);
	col_name_vec[0]   = "sample_index";
	col_name_vec[1]   = "data_subset_id";
	col_name_vec[2]   = "meas_value";
	//
	size_t n_subset = data_subset_obj.size();
	for(size_t sample_index = 0; sample_index < number_sample; sample_index++)
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
		row_val_vec[0] = to_string( sample_index );
		row_val_vec[1] = to_string( subset_id );
		row_val_vec[2] = to_string(meas_value);
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}

	return;
}
/*
-------------------------------------------------------------------------------
$begin sample_command$$
$spell
	dismod
	var
	arg
$$

$section The Sample Command$$

$head Syntax$$
$codei%dismod_at sample %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head simulate_table$$
This command has the extra input $cref  simulate_table$$
which was created by a previous $cref simulate_command$$.

$head sample_table$$
A new $cref sample_table$$ is created each time this command is run.
It contains the optimal $cref model_variable$$ values
for each simulated $cref/sample_index/simulate_table/sample_index/$$.

$children%example/get_started/sample_command.py%$$
$head Example$$
The file $cref sample_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void sample_command(
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
{
	using std::string;
	using dismod_at::to_string;
	// -----------------------------------------------------------------------
	// read truth_var table into truth_var
	vector<double> truth_var;
	string table_name = "truth_var";
	string column_name = "truth_var_value";
	dismod_at::get_table_column(db, table_name, column_name, truth_var);
	// -----------------------------------------------------------------------
	// get simulation data
	vector<dismod_at::simulate_struct> simulate_table =
			dismod_at::get_simulate_table(db);
	// -----------------------------------------------------------------------
	// create a new sample table
	string sql_cmd = "drop table if exists sample";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table sample("
		" sample_id        integer primary key,"
		" sample_index     integer,"
		" var_id           integer,"
		" var_value        real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	table_name = "sample";
	vector<string> col_name_vec(3), row_val_vec(3);
	col_name_vec[0]   = "sample_index";
	col_name_vec[1]   = "var_id";
	col_name_vec[2]   = "var_value";
	// -----------------------------------------------------------------------

	// n_subset
	size_t n_subset = data_subset_obj.size();

	// n_var, n_sample
	size_t n_var    = pack_object.size();
	size_t n_sample = simulate_table.size() / n_subset;
	assert( simulate_table.size() == n_sample * n_subset );
	for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
	{	// set the measurement values for this simulation subset
		size_t offset = n_subset * sample_index;
		for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
		{	size_t simulate_id = offset + subset_id;
			size_t sample_check =
				size_t(simulate_table[simulate_id].sample_index);
			size_t subset_check =
				size_t(simulate_table[simulate_id].data_subset_id);
			if( sample_check != sample_index || subset_check != subset_id )
			{	string msg = "database modified, restart with init command";
				table_name = "simulate";
				dismod_at::error_exit(db, msg, table_name, simulate_id);
			}
			data_subset_obj[subset_id].meas_value =
				simulate_table[simulate_id].meas_value;
		}
		data_object.replace_like(data_subset_obj);

		// fit_model
		dismod_at::fit_model fit_object(
			db                   ,
			pack_object          ,
			truth_var            ,
			db_input.prior_table ,
			s_info_vec           ,
			data_object          ,
			prior_object
		);
		fit_object.run_fit(option_map);
		vector<double> solution = fit_object.get_solution();
		assert( solution.size() == n_var );
		//
		// write out solution for this sample_index
		row_val_vec[0] = to_string( sample_index );
		for(size_t var_id = 0; var_id < n_var; var_id++)
		{	row_val_vec[1] = to_string( var_id );
			row_val_vec[2] = to_string( solution[var_id] );
			dismod_at::put_table_row(
				db, table_name, col_name_vec, row_val_vec
			);
		}
	}
	return;
}
/*
-------------------------------------------------------------------------------
$begin predict_command$$
$spell
	dismod
	var
	arg
$$

$section The Predict Command$$

$head Syntax$$
$codei%dismod_at predict %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
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
$cref/avg_case_subset_id/avg_case_subset_table/avg_case_subset_id/$$
in the avg_case_subset table.

$children%example/get_started/predict_command.py%$$
$head Example$$
The file $cref predict_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void predict_command(
	sqlite3*                                          db                  ,
	const dismod_at::db_input_struct&                 db_input            ,
	size_t                                            n_var               ,
	dismod_at::data_model&                            avg_case_object     ,
	const vector<dismod_at::avg_case_subset_struct>&  avg_case_subset_obj
)
{
	using std::string;
	using dismod_at::to_string;

	// sample table
	vector<dismod_at::sample_struct> sample_table =
		dismod_at::get_sample_table(db);

	// create a new sample table
	string sql_cmd = "drop table if exists predict";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table predict("
		" predict_id          integer primary key,"
		" sample_index       integer,"
		" avg_case_subset_id integer,"
		" avg_integrand      real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	string table_name = "predict";
	vector<string> col_name_vec(3), row_val_vec(3);
	col_name_vec[0]   = "sample_index";
	col_name_vec[1]   = "avg_case_subset_id";
	col_name_vec[2]   = "avg_integrand";

	// n_sample
	size_t n_sample  = sample_table.size() / n_var;
	assert( sample_table.size() == n_sample * n_var );
	//
	// n_subset
	size_t n_subset  = avg_case_subset_obj.size();
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
		{	int integrand_id = avg_case_subset_obj[subset_id].integrand_id;
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
				avg = avg_case_object.avg_no_ode(subset_id, pack_vec);
				break;
				//
				case dismod_at::prevalence_enum:
				case dismod_at::Tincidence_enum:
				case dismod_at::mtspecific_enum:
				case dismod_at::mtall_enum:
				case dismod_at::mtstandard_enum:
				avg = avg_case_object.avg_yes_ode(subset_id, pack_vec);
				break;
				//
				default:
				assert(false);
			}
			row_val_vec[0] = to_string( sample_index );
			row_val_vec[1] = to_string( subset_id );
			row_val_vec[2] = to_string( avg );
			dismod_at::put_table_row(
				db, table_name, col_name_vec, row_val_vec
			);
		}
	}
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
	string program = "dismod_at-";
	program       += DISMOD_AT_VERSION;
	if( n_arg != 3 )
	{	cerr << program << endl;
		cerr << "usage: dismod_at command file_name" << endl;
		std::exit(1);
	}
	size_t i_arg = 0;
	const string command_arg    = argv[++i_arg];
	const string file_name_arg  = argv[++i_arg];
	bool ok = false;
	ok     |= command_arg == "init";
	ok     |= command_arg == "start";
	ok     |= command_arg == "truth";
	ok     |= command_arg == "fit";
	ok     |= command_arg == "simulate";
	ok     |= command_arg == "sample";
	ok     |= command_arg == "predict";
	string message;
	if( ! ok )
	{	message =  "dismod_at: command not one the following:\n";
		message += "\tinit, start, fit, truth, simulate, sample, predict";
		cerr << message << endl;
		std::exit(1);
	}
	// --------------- open connection to datbase ---------------------------
	bool new_file = false;
	sqlite3* db   = dismod_at::open_connection(file_name_arg, new_file);
	// --------------- log start of this command -----------------------------
	message = command_arg + " start";
	std::time_t unix_time = dismod_at::log_message(db, "command", message);
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
	{	size_t actual_seed = dismod_at::new_gsl_rng( size_t(unix_time) );
		assert( std::time_t( actual_seed ) == unix_time );
	}
	else
	{	size_t actual_seed = dismod_at::new_gsl_rng(random_seed);
		assert( actual_seed == random_seed );
	}
	// ---------------------------------------------------------------------
	// n_age_ode
	double age_min    = db_input.age_table[0];
	double age_max    = db_input.age_table[ db_input.age_table.size() - 1 ];
	size_t n_age_ode  = size_t( (age_max - age_min) / ode_step_size + 2.0 );
	assert( age_max  <= age_min  + (n_age_ode - 1) * ode_step_size );
	// ---------------------------------------------------------------------
	// n_time_ode
	double time_min   = db_input.time_table[0];
	double time_max   = db_input.time_table[ db_input.time_table.size() - 1 ];
	size_t n_time_ode = size_t( (time_max - time_min) / ode_step_size + 2.0 );
	assert( time_max <= time_min  + (n_time_ode - 1) * ode_step_size );
	// ---------------------------------------------------------------------
	// child_data, child_avg_case, and some more size_t values
	size_t parent_node_id = std::atoi(
		option_map["parent_node_id"].c_str()
	);
	dismod_at::child_info child_data(
		parent_node_id          ,
		db_input.node_table     ,
		db_input.data_table
	);
	dismod_at::child_info child_avg_case(
		parent_node_id          ,
		db_input.node_table     ,
		db_input.avg_case_table
	);
	size_t n_child     = child_data.child_size();
	size_t n_integrand = db_input.integrand_table.size();
	size_t n_weight    = db_input.weight_table.size();
	size_t n_smooth    = db_input.smooth_table.size();
	// ---------------------------------------------------------------------
	// data_subset_obj
	vector<dismod_at::data_subset_struct> data_subset_obj = data_subset(
		db_input.data_table,
		db_input.covariate_table,
		child_data
	);
	// ---------------------------------------------------------------------
	// avg_case_subset_obj
	vector<dismod_at::avg_case_subset_struct> avg_case_subset_obj =
		avg_case_subset(
			db_input.avg_case_table,
			db_input.covariate_table,
			child_avg_case
	);
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
	// ---------------------------------------------------------------------
	if( command_arg == "init" )
	{	init_command(
			db,
			data_subset_obj,
			avg_case_subset_obj,
			pack_object,
			db_input,
			parent_node_id,
			child_data,     // could also use child_avg_case
			s_info_vec
		);
	}
	else if( command_arg == "start" )
	{	start_command(
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
	{	// ------------------------------------------------------------------
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
			n_age_ode                ,
			n_time_ode               ,
			ode_step_size            ,
			db_input.age_table       ,
			db_input.time_table      ,
			db_input.integrand_table ,
			db_input.node_table      ,
			data_subset_obj          ,
			w_info_vec               ,
			s_info_vec               ,
			pack_object              ,
			child_data
		);
		string rate_info = option_map["rate_info"];
		data_object.set_eigen_ode2_case_number(rate_info);
		data_object.replace_like( data_subset_obj );
		//
		// avg_case_object
		dismod_at::data_model avg_case_object(
			parent_node_id           ,
			n_age_ode                ,
			n_time_ode               ,
			ode_step_size            ,
			db_input.age_table       ,
			db_input.time_table      ,
			db_input.integrand_table ,
			db_input.node_table      ,
			avg_case_subset_obj          ,
			w_info_vec               ,
			s_info_vec               ,
			pack_object              ,
			child_avg_case
		);
		avg_case_object.set_eigen_ode2_case_number(rate_info);
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
		{	size_t number_sample = std::atoi(
				option_map["number_sample"].c_str()
			);
			simulate_command(
				db                       ,
				db_input.integrand_table ,
				data_subset_obj          ,
				data_object              ,
				number_sample
			);
		}
		else if( command_arg == "sample" )
		{
			sample_command(
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
				avg_case_object      ,
				avg_case_subset_obj
			);
		}
		else
			assert(false);
	}
	// ---------------------------------------------------------------------
	message = command_arg + " done";
	dismod_at::log_message(db, "command", message);
	sqlite3_close(db);
	return 0;
}
