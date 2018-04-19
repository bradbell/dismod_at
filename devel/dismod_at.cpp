// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
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
# include <dismod_at/depend.hpp>

# define DISMOD_AT_TRACE 0

namespace { // BEGIN_EMPTY_NAMESPACE
	using CppAD::vector;
	// -----------------------------------------------------------------------
	// trace
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
	// -----------------------------------------------------------------------
	// get_prior_mean
	vector<double> get_prior_mean(
		const vector<dismod_at::prior_struct>& prior_table ,
		const dismod_at::pack_info&            pack_object ,
		const dismod_at::pack_prior&           var2prior   )
	{
		// put means in return value
		size_t n_var = pack_object.size();
		vector<double> result(n_var);
		for(size_t var_id = 0; var_id < n_var; var_id++)
		{	double var_value = var2prior.const_value(var_id);
			size_t prior_id  = var2prior.value_prior_id(var_id);
			if( prior_id != DISMOD_AT_NULL_SIZE_T )
				var_value = prior_table[prior_id].mean;
			result[var_id] = var_value;
		}
		return result;
	}
/*
-----------------------------------------------------------------------------
$begin set_command$$
$spell
	var
	dismod
$$

$section Directly Setting Table Values$$

$head Syntax$$
$codei%dismod_at %database% set option %name% %value%
%$$
$codei%dismod_at %database% set %table_out% %source%
%$$
$codei%dismod_at %database% set %table_out% %source% %sample_index%
%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head option$$

$subhead name$$
Is the name of the option we are setting.
It must be a valid
$cref/option_name/option_table/Conventions/option_name/$$.

$subhead value$$
Is the new
$cref/option_value/option_table/Conventions/option_value/$$
for the specified option name.

$subhead Warning$$
If $icode option_name$$ is $code parent_node_id$$,
the set of $cref model_variables$$ will change and you must
re-run the $cref init_command$$.

$head table_out$$
The $icode table_out$$ syntax sets the values in $icode table_out$$
using the values specified by $icode source$$.
If this table exists before the command,
the values originally in the table are lost.

$subhead start_var$$
If $icode table_out$$ is $code start_var$$,
the $cref/start_var/start_var_table/$$ table is created.
Note that this table may also be created directly by the user
(with the aid of the $cref var_table$$).

$subhead scale_var$$
If $icode table_out$$ is $code scale_var$$,
the $cref/scale_var/scale_var_table/$$ table is created.
Note that this table may also be created directly by the user
(with the aid of the $cref var_table$$).

$subhead truth_var$$
If $icode table_out$$ is $code truth_var$$,
the $cref/truth_var/truth_var_table/$$ table is created.
Note that this table may also be created directly by the user
(with the aid of the $cref var_table$$).

$head source$$
The set command $icode source$$ must be one of the following
(and not be the same as $icode table_out$$):

$subhead prior_mean$$
If $icode source$$ is $code prior_mean$$,
the mean of the priors is used for the values in $icode table_out$$.
In this case $icode sample_index$$ is not present.

$subhead fit_var$$
If $icode source$$ is $code fit_var$$,
the results of the previous fit is used for the values in $icode table_out$$.
In this case $icode sample_index$$ is not present.

$subhead start_var$$
If $icode source$$ is $code start_var$$,
the contents of this table are copied to $icode table_out$$.
This is useful when the user creates the $code start_var$$ table
and wants the $code scale_var$$ table to have the same values.
In this case, $icode table_out$$ would be $code scale_var$$.

$subhead sample$$
If $icode source$$ is $code sample$$,
$icode sample_index$$ must be present.
In this case the $cref model_variables$$ in the sample table,
and corresponding to the specified sample index,
are used for the values in $icode table_out$$.

$children%example/get_started/set_command.py
%$$
$head Example$$
The files
$cref set_command.py$$ and $cref set_command.py$$
contains examples and tests using this command.

$end
*/

// ----------------------------------------------------------------------------
void set_option_command(
	sqlite3*                                        db           ,
	const CppAD::vector<dismod_at::option_struct>&  option_table ,
	const std::string&                              name         ,
	const std::string&                              value        )
{	using std::string;
	// option_table contains all the valid option names with default values
	// for the names that are not in the option table, check name is valid
	bool found = false;
	for(size_t i = 0; i < option_table.size(); ++i)
		found |= option_table[i].option_name == name;
	if( ! found )
	{	string msg  = "set option name value: ";
		msg += "name = " + name + " is not a valid option name\n";
		dismod_at::error_exit(msg);
	}
	string table_name  = "option";
	string column_name = "option_name";
	CppAD::vector<string> option_name;
	dismod_at::get_table_column(db, table_name, column_name, option_name);
	found = false;
	for(size_t i = 0; i < option_name.size(); i++)
		found |= option_name[i] == name;
	string sql_cmd;
	if( found )
	{	sql_cmd  = "update option set option_value = '" + value + "' ";
		sql_cmd += "where option_name = '" + name + "'";
		dismod_at::exec_sql_cmd(db, sql_cmd);
	}
	else
	{	string option_id = CppAD::to_string( option_name.size() );
		sql_cmd  = "insert into option ";
		sql_cmd += "values ('" + option_id + "','";
		sql_cmd += name + "','" + value + "')";
		dismod_at::exec_sql_cmd(db, sql_cmd);
	}
	//
	return;
}
void set_command(
	const std::string&                     table_out    ,
	const std::string&                     source       ,
	const std::string&                     sample_index ,
	sqlite3*                               db           ,
	const vector<double>&                  prior_mean   )
{	using std::string;
	using CppAD::to_string;
	//
	// string used for error messaging
	string msg;
	//
	if( table_out != "start_var"
	&&  table_out != "scale_var"
	&&  table_out != "truth_var" )
	{	msg  = "dismod_at set command table_out = ";
		msg += table_out + " is not one of the following: ";
		msg += "start_var, scale_var, truth_var";
		dismod_at::error_exit(msg);
	}
	if( source != "prior_mean"
	&&  source != "fit_var"
	&&  source != "start_var"
	&&  source != "sample" )
	{	msg  = "dismod_at set command source = ";
		msg += source + " is not one of the following: ";
		msg += "prior_mean, fit_var, start_var, sample";
		dismod_at::error_exit(msg);
	}
	if( source == "sample" && sample_index == "" )
	{	msg  = "dismod_at set command source = sample";
		msg += " sample_index is missing";
		dismod_at::error_exit(msg);
	}
	if( source == table_out )
	{	msg  = "dismod_at set command table_out and source are the same";
		dismod_at::error_exit(msg);
	}
	//
	string sql_cmd = "drop table if exists ";
	sql_cmd       += table_out;
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	// create table_out
	string table_name_out = table_out;
	size_t n_var          = prior_mean.size();
	vector<string> col_name(1), col_type(1), row_value(n_var);
	vector<bool>   col_unique(1);
	col_name[0]       = table_out + "_value";
	col_type[0]       = "real";
	col_unique[0]     = false;
	//
	if( source == "prior_mean" )
	{	for(size_t var_id = 0; var_id < n_var; var_id++)
			row_value[var_id] = CppAD::to_string( prior_mean[var_id] );
	}
	else if( source == "fit_var" )
	{	// fit_var_value
		vector<double> fit_var_value;
		string table_name_in  = "fit_var";
		string column_name    = "fit_var_value";
		dismod_at::get_table_column(
			db, table_name_in, column_name, fit_var_value
		);
		//
		// put it in row_value
		for(size_t var_id = 0; var_id < n_var; var_id++)
			row_value[var_id] = to_string(fit_var_value[var_id]);
	}
	else if( source == "start_var" )
	{	// start_var_value
		vector<double> start_var_value;
		string table_name_in  = "start_var";
		string column_name    = "start_var_value";
		dismod_at::get_table_column(
			db, table_name_in, column_name, start_var_value
		);
		//
		// put it in row_value
		for(size_t var_id = 0; var_id < n_var; var_id++)
			row_value[var_id] = to_string(start_var_value[var_id]);
	}
	else
	{	assert( source == "sample" );
		//
		// index
		size_t index = size_t ( std::atoi( sample_index.c_str() ) );
		//
		// var_value
		vector<double> var_value;
		string table_name_in  = "sample";
		string column_name    = "var_value";
		dismod_at::get_table_column(
			db, table_name_in, column_name, var_value
		);
		// n_sample
		if( var_value.size() % n_var != 0 )
		{	msg  = "sample table size not a multiple of number of variables";
			dismod_at::error_exit(msg);
		}
		size_t n_sample = var_value.size() / n_var;
		if( n_sample <= index )
		{	msg  = "dismod_at set command: sample_index >= number of samples";
			dismod_at::error_exit(msg);
		}
		//
		// put in row_value
		for(size_t var_id = 0; var_id < n_var; var_id++)
		{	size_t sample_id = index * n_var + var_id;
				row_value[var_id] = to_string(var_value[sample_id]);
		}
	}
	dismod_at::create_table(
		db, table_name_out, col_name, col_type, col_unique, row_value
	);
	return;
}
/*
-----------------------------------------------------------------------------
$begin init_command$$
$spell
	avgint
	init
	var
	dismod
	initializes
$$

$section The Variable Command$$

$head Syntax$$
$codei%dismod_at %database% init%$$

$head Purpose$$
This command initializes the data flow.
To be specific, it begins by deleting any existing
$cref/output tables/data_flow/Output Tables/$$,
except for the $cref log_table$$,
and then creates new versions of the following tables:
$table
$cref var_table$$           $cnext $title var_table$$         $rnext
$cref data_subset_table$$   $cnext $title data_subset_table$$ $rnext
$cref start_var_table$$     $cnext $title start_var_table$$   $rnext
$cref scale_var_table$$     $cnext $title scale_var_table$$
$tend

$head Changing Values$$
You must re-run this command when ever any of the
$cref input$$ tables is changed.
The option table is an exception to this rule; see
$cref/changing values/option_table/Conventions/Changing Values/$$.

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

$head start_var_table$$
A new $cref start_var_table$$ is created using the
means of the priors for the model variables.

$head scale_var_table$$
A new $cref scale_var_table$$ is created using the
means of the priors for the model variables.

$head Deleted Tables$$
The following tables are deleted (if they exist) when the
$code init$$ command is run:
$cref/depend_var/depend_var_table/$$,
$cref/fit_var/fit_var_table/$$,
$cref/truth_var/truth_var_table/$$,
$cref/fit_data_subset/fit_data_subset_table/$$,
$cref/simulate/simulate_table/$$,
$cref/sample/sample_table/$$,
$cref/predict/predict_table/$$.

$children%example/get_started/init_command.py
%$$
$head Example$$
The file $cref init_command.py$$ contains an example and test
using this command.

$end
*/

// ----------------------------------------------------------------------------
void init_command(
	sqlite3*                                         db                  ,
	const vector<double>&                            prior_mean          ,
	const vector<dismod_at::data_subset_struct>&     data_subset_obj     ,
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
		"depend_var",
		"fit_var",
		"scale_var",
		"start_var",
		"truth_var",
		//
		"data_subset",
		"fit_data_subset",
		//
		"simulate",
		"sample",
		"predict"
	};
	size_t n_drop = sizeof( drop_list ) / sizeof( drop_list[0] );
	string sql_cmd;
	for(size_t i = 0; i < n_drop; i++)
	{	sql_cmd = "drop table if exists ";
		sql_cmd += drop_list[i];
		dismod_at::exec_sql_cmd(db, sql_cmd);
	}
	// -----------------------------------------------------------------------
	// start_var table
	string table_out    = "start_var";
	string source       = "prior_mean";
	string sample_index = "";
	set_command(table_out, source, sample_index, db, prior_mean);
	// -----------------------------------------------------------------------
	// scale_var table
	table_out    = "scale_var";
	source       = "prior_mean";
	sample_index = "";
	set_command(table_out, source, sample_index, db, prior_mean);
	// -----------------------------------------------------------------------
	// data_subset table
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
	size_t offset = DISMOD_AT_NULL_SIZE_T, var_id = DISMOD_AT_NULL_SIZE_T;
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
	size_t n_var, n_time, node_id;
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	for(size_t child_id = 0; child_id <= n_child; child_id++)
		{	if( child_id == n_child )
				node_id = parent_node_id;
			else
				node_id = child_object.child_id2node_id(child_id);
			//
			dismod_at::pack_info::subvec_info info;
			info      = pack_object.rate_info(rate_id, child_id);
			size_t smooth_id = info.smooth_id;
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
				// fit_var_value
# ifndef NDEBUG
				for(size_t k = 0; k < n_col; k++)
					assert( row_value[ n_col * var_id + k ] == "" );
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
$begin depend_command$$
$spell
	dismod
	var
$$

$section The Depend Command$$

$head Syntax$$
$codei%dismod_at %database% depend%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head depend_var_table$$
A new $cref depend_var_table$$ is created with the information
that maps a $cref/var_id/var_table/var_id/$$
to its meaning in terms of the
$cref model_variables$$.

$children%example/get_started/depend_command.py
%$$
$head Example$$
The file $cref depend_command.py$$ contains an example and test
using this command.

$end
*/
void depend_command(
	sqlite3*                                      db               ,
	const vector<double>&                         prior_mean       ,
	dismod_at::data_model&                        data_object      ,
	const vector<dismod_at::data_subset_struct>&  data_subset_obj  ,
	const dismod_at::prior_model&                 prior_object     ,
	// effectively const
	std::map<std::string, std::string>&           option_map       )
{	using std::string;
	string table_name = "depend_var";
	//
	// minimum_meas_cv
	double minimum_meas_cv = std::atof(
		option_map["minimum_meas_cv"].c_str()
	);
	data_object.replace_like(minimum_meas_cv, data_subset_obj);
	//
	// compute the dependencies
	vector<bool> data_depend  =
		dismod_at::data_depend(prior_mean, data_object);
	vector<bool> prior_depend =
		dismod_at::prior_depend(prior_mean, prior_object);
	//
	// delete old version of depend_var table
	string sql_cmd = "drop table if exists ";
	sql_cmd       += table_name;
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	// create new depend_var table
	size_t n_var      = prior_mean.size();
	vector<string> col_name(2), col_type(2), row_value(2 * n_var);
	vector<bool>   col_unique(2);
	col_name[0]       = "data_depend";
	col_type[0]       = "integer";
	col_unique[0]     = false;
	col_name[1]       = "prior_depend";
	col_type[1]       = "integer";
	col_unique[1]     = false;
	for(size_t i = 0; i < n_var; i++)
	{	// values in i-th row
		//
		// data_depend
		if( data_depend[i]  )
			row_value[i * 2 + 0] = "1";
		else
			row_value[i * 2 + 0] = "0";
		//
		// prior_depend
		if( prior_depend[i]  )
			row_value[i * 2 + 1] = "1";
		else
			row_value[i * 2 + 1] = "0";
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
$cref/fit_fixed.py/user_fit_fixed.py/$$.
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
In this case the corresponding simulate table
$cref/simulate_value/simulate_table/simulate_value/$$ entries
are used in place of the data table
$cref/meas_value/data_table/meas_value/$$ entries.
All the rest of the inputs are the same as when $icode simulated_index$$
is not present; e.g.,
$cref/meas_std/data_table/meas_std/$$ comes from the data table.

$head simulate_table$$
If $icode simulate_index$$ is present,
the corresponding $icode meas_value$$ entries the simulate table
are used as inputs.

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
void fit_command(
	const std::string&                           variables        ,
	const std::string&                           simulate_index   ,
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
	bool ok = variables == "fixed";
	ok     |= variables == "random";
	ok     |= variables == "both";
	if( ! ok )
	{	string msg = "dismod_at fit command variables = ";
			msg += variables + "\nis not 'fixed', 'random' or 'both'";
			string table_name = "simulate";
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
	// minimum_meas_cv
	double minimum_meas_cv = std::atof(
		option_map["minimum_meas_cv"].c_str()
	);
	// -----------------------------------------------------------------------
	if( simulate_index != "" )
	{	size_t sim_index = std::atoi( simulate_index.c_str() );
		//
		// get simulation data
		vector<dismod_at::simulate_struct> simulate_table =
				dismod_at::get_simulate_table(db);
		size_t n_subset   = data_subset_obj.size();
		size_t n_simulate = simulate_table.size() / n_subset;
		//
		if( sim_index >= n_simulate )
		{	string msg = "dismod_at fit command simulate_index = ";
			msg += simulate_index + "\nis greater than or equal ";
			msg += "number of samples in the simulate table.";
			string table_name = "simulate";
			dismod_at::error_exit(msg, table_name);
		}
		// replace meas_value in data_subset_obj
		for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
		{	size_t simulate_id = n_subset * sim_index + subset_id;
			data_subset_obj[subset_id].meas_value =
				simulate_table[simulate_id].simulate_value;
		}
	}
	data_object.replace_like(minimum_meas_cv, data_subset_obj);
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
		start_var            ,
		scale_var            ,
		db_input.prior_table ,
		s_info_vec           ,
		data_object          ,
		prior_object         ,
		quasi_fixed          ,
		zero_sum_random
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
	bool parent_only = false;
	CppAD::vector<double> reference_sc =
		data_object.reference_ode(opt_value, parent_only);
	//
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	// compute average integrand for this data item
		double avg;
		size_t integrand_id = data_subset_obj[subset_id].integrand_id;
		dismod_at::integrand_enum integrand =
			db_input.integrand_table[integrand_id];
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

$head simulate_table$$
A new $cref simulate_table$$ is created by this command.
It contains simulated measurements and
adjusted standard deviations.
Only the $cref/data_id/data_subset_table/data_id/$$ that are in the
data_subset table are included in the simulated measurements.
In addition, $icode number_simulate$$ values are simulated
for such $icode data_id$$.
Hence the number of rows in $cref simulate_table$$ is
$icode number_simulate$$ times the number of rows in $cref data_subset_table$$.

$subhead simulate_value$$
This value is different for each $icode subset_data_id$$
and each $icode simulate_index$$.
In the $cref/linear/simulate_command/y/Linear/$$ case,
this column contains the value $icode y$$.
In the $cref/log-transformed/simulate_command/y/Log-Transformed/$$ case,
this column contains the value $codei%max(%y%, 0)%$$.

$subhead simulate_delta$$
This column contains the value of
$cref/delta/simulate_command/delta/$$
and is different for each $icode subset_data_id$$.

$head d$$
We use $latex d$$ to denote the
$cref/density_id/data_table/density_id/$$ and
$cref/eta/data_table/eta/$$
corresponding to this $icode data_id$$.

$head delta$$
We use $latex \delta$$ to denote the
$cref/adjusted standard deviation
	/data_like
	/Adjusted Standard Deviation, delta_i
/$$
corresponding to this $icode data_id$$.

$head A$$
We use $latex A$$ denote the
$cref/average integrand/avg_integrand/Average Integrand, A_i/$$
corresponding to the value for the model variables and
this $icode data_id$$.

$head sigma$$
We use $latex \sigma$$ to denote the
$cref/log-transformed standard deviation
	/statistic
	/Log-Transformed Standard Deviation, sigma
/$$
$latex \[
	\sigma = \log( A + \eta + \delta ) - \log(A + \eta)
\] $$

$head y$$

$subhead Linear$$
If $latex d$$ is Gaussian, Laplace, or Student's-t,
$latex \[
	y = A + e
\]$$
where $latex e$$ is simulated from the corresponding distribution
with mean zero and standard deviation $latex \delta$$.
The corresponding
$cref/weighted residual
	/statistic
	/Weighted Residual Function, R
	/Gaussian, Laplace, Student's-t
/$$ is
$latex \[
	R = e / \delta
\]$$

$subhead Log-Transformed$$
If $latex d$$ is Log-Gaussian, Log-Laplace, or Log-Student's-t,
$latex \[
	e = \log( y + \eta ) - \log( A + \eta )
\] $$
$latex e$$ is simulated from the corresponding linear distribution
with mean zero and standard deviation $latex \sigma$$.
It follows that
$latex \[
	y = \exp(e) * ( A + \eta )
\] $$
The corresponding
$cref/weighted residual
	/statistic
	/Weighted Residual Function, R
	/Gaussian, Laplace, Student's-t
/$$ is
$latex \[
	R = e / \sigma
\]$$


$children%example/get_started/simulate_command.py
%$$
$head Example$$
The file $cref simulate_command.py$$ contains an example and test
using this command.

$end
*/
void simulate_command(
	const std::string&                                  number_simulate ,
	double                                              minimum_meas_cv ,
	sqlite3*                                            db              ,
	const vector<dismod_at::integrand_enum>&            integrand_table ,
	const vector<dismod_at::data_subset_struct>&        data_subset_obj ,
	const dismod_at::data_model&                        data_object
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
	// ----------------- simulate_table ----------------------------------
	//
	string sql_cmd = "drop table if exists simulate";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name      = "simulate";
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
	col_name[2]   = "simulate_value";
	col_type[2]   = "real";
	col_unique[2] = false;
	//
	col_name[3]   = "simulate_delta";
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
		dismod_at::integrand_enum integrand = integrand_table[integrand_id];
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
		double eta          = data_subset_obj[subset_id].eta;
		double nu           = data_subset_obj[subset_id].nu;
		//
		for(size_t sim_index = 0; sim_index < n_simulate; sim_index++)
		{	// for each simulate_index
			//
			// compute the simulated measurement value
			double sim_value   = dismod_at::sim_random(
				density, avg, sim_delta, eta, nu
			);
			if( density == dismod_at::log_gaussian_enum
			||  density == dismod_at::log_laplace_enum
			||  density == dismod_at::log_students_enum )
				sim_value = std::max(sim_value, 0.0);
			//
			size_t simulate_id = sim_index * n_subset + subset_id;
			row_value[simulate_id * n_col + 0] = to_string( sim_index );
			row_value[simulate_id * n_col + 1] = to_string( subset_id );
			row_value[simulate_id * n_col + 2] = to_string( sim_value );
			row_value[simulate_id * n_col + 3] = to_string( sim_delta );
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
is the optimal estimate corresponding to data in the simulate table with
$cref/simulate_index/simulate_table/simulate_index/$$ equal to
the sample index.
This requires running $icode number_sample$$ fits of the model variables.

$subhead asymptotic$$
If $icode method$$ is $code asymptotic$$,
The asymptotic statics of the model variables is used to generate
$icode number_sample$$ samples of the model variables
The samples with different values of $icode sample_index$$ are independent.
(Note that the $cref fit_var_table$$ is an additional input in this case.)

$head simulate_table$$
If $icode method$$ is $code simulate$$,
this command has the extra input $cref  simulate_table$$
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
	const dismod_at::pack_info&                 pack_object      ,
	const dismod_at::db_input_struct&           db_input         ,
	const vector<dismod_at::smooth_info>&       s_info_vec       ,
	const dismod_at::prior_model&               prior_object     ,
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
	// minimum_meas_cv
	double minimum_meas_cv = std::atof(
		option_map["minimum_meas_cv"].c_str()
	);
	// -----------------------------------------------------------------------
	if( method == "simulate" )
	{
		// truth_var
		vector<double> truth_var_value;
		string table_name  = "truth_var";
		string column_name = "truth_var_value";
		dismod_at::get_table_column(
			db, table_name, column_name, truth_var_value
		);
		// scale_var
		vector<double> scale_var_value;
		table_name  = "scale_var";
		column_name = "scale_var_value";
		dismod_at::get_table_column(
			db, table_name, column_name, scale_var_value
		);
		// get simulated data
		vector<dismod_at::simulate_struct> simulate_table =
				dismod_at::get_simulate_table(db);
		//
		size_t n_subset = data_subset_obj.size();
		if( simulate_table.size() % n_subset != 0  )
		{	msg  = "dismod_at sample command method = simulate and ";
			msg += "sample table size modulo data_subset table size not zero.";
			dismod_at::error_exit(msg);
		}
		if( n_sample != simulate_table.size() / n_subset )
		{	msg  = "dismod_at sample command method = simulate and ";
			msg += "sample table size not equal number_sample times ";
			msg += "data_subset table size.";
			dismod_at::error_exit(msg);
		}
		for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
		{	// replace meas_value in data_subset_obj
			size_t offset = n_subset * sample_index;
			for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
			{	size_t simulate_id = offset + subset_id;
				size_t sample_check =
					size_t(simulate_table[simulate_id].simulate_index);
				size_t subset_check =
					size_t(simulate_table[simulate_id].data_subset_id);
				if( sample_check != sample_index || subset_check != subset_id )
				{	msg  = "dismod_at database sample simulate\n";
					msg += "size of simulate table does not make sense\n";
					msg +=  "restart with init command";
					table_name = "simulate";
					dismod_at::error_exit(msg, table_name, simulate_id);
				}
				data_subset_obj[subset_id].meas_value =
					simulate_table[simulate_id].simulate_value;
			}
			// replace_like
			data_object.replace_like(minimum_meas_cv, data_subset_obj);
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
				truth_var_value      ,
				scale_var_value      ,
				db_input.prior_table ,
				s_info_vec           ,
				data_object          ,
				prior_object         ,
				quasi_fixed          ,
				zero_sum_random
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
	data_object.replace_like(minimum_meas_cv, data_subset_obj);
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
		fit_var_value        ,
		fit_var_value        ,
		db_input.prior_table ,
		s_info_vec           ,
		data_object          ,
		prior_object         ,
		quasi_fixed          ,
		zero_sum_random
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
				db_input.integrand_table[integrand_id];
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
			row_value[n_col * predict_id + 0] = to_string( sample_index );
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
		db_input.prior_table,
		pack_object,
		var2prior
	);
	// minimum_meas_cv
	double minimum_meas_cv = std::atof(
		option_map["minimum_meas_cv"].c_str()
	);
	// rate_case
	string rate_case = option_map["rate_case"];
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
	if( command_arg == "set" )
	{	if( std::strcmp(argv[3], "option") == 0 )
		{	if( n_arg != 6 )
			{	cerr << "expected name and value to follow "
				"dismod_at database set option\n";
				std::exit(1);
			}
			std::string name  = argv[4];
			std::string value = argv[5];
			set_option_command(db, db_input.option_table, name, value);
		}
		else
		{	std::string table_out     = argv[3];
			std::string source        = argv[4];
			std::string sample_index  = "";
			if( n_arg == 6 )
				sample_index = argv[5];
			set_command(
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
			bound_random             ,
			parent_node_id           ,
			n_covariate              ,
			n_age_ode                ,
			n_time_ode               ,
			ode_step_size            ,
			db_input.age_table       ,
			db_input.time_table      ,
			db_input.integrand_table ,
			db_input.node_table      ,
			db_input.prior_table     ,
			avgint_subset_obj        ,
			avgint_subset_cov_value  ,
			w_info_vec               ,
			s_info_vec               ,
			pack_object              ,
			child_avgint
		);
		avgint_object.set_eigen_ode2_case_number(rate_case);
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
			// data_object
			dismod_at::data_model data_object(
				bound_random             ,
				parent_node_id           ,
				n_covariate              ,
				n_age_ode                ,
				n_time_ode               ,
				ode_step_size            ,
				db_input.age_table       ,
				db_input.time_table      ,
				db_input.integrand_table ,
				db_input.node_table      ,
				db_input.prior_table     ,
				data_subset_obj          ,
				data_subset_cov_value    ,
				w_info_vec               ,
				s_info_vec               ,
				pack_object              ,
				child_data
			);
			data_object.set_eigen_ode2_case_number(rate_case);
			//
			if( command_arg == "depend" )
			{	depend_command(
					db               ,
					prior_mean       ,
					data_object      ,
					data_subset_obj  ,
					prior_object     ,
					option_map
				);
			}
			else if( command_arg == "sample" )
			{	sample_command(
					argv[3]          , // method
					argv[4]          , // number_sample
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
			else if( command_arg == "fit" )
			{	string variables      = argv[3];
				string simulate_index = "";
				if( n_arg == 5 )
					simulate_index = argv[4];
				fit_command(
					variables        ,
					simulate_index   ,
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
			{	// replace_like
				data_object.replace_like(minimum_meas_cv, data_subset_obj );
				simulate_command(
					argv[3]                  , // number_simulate
					minimum_meas_cv          ,
					db                       ,
					db_input.integrand_table ,
					data_subset_obj          ,
					data_object
				);
			}
			else
				assert(false);
		}
	}
	// ---------------------------------------------------------------------
	message = "end " + command_arg;
	dismod_at::log_message(db, DISMOD_AT_NULL_PTR, "command", message);
	sqlite3_close(db);
	return 0;
}
