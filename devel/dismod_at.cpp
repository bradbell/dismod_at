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
# include <dismod_at/configure.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/get_db_input.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/put_table_row.hpp>
# include <dismod_at/to_string.hpp>
# include <dismod_at/get_column_max.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/to_string.hpp>
# include <dismod_at/manage_gsl_rng.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/sim_random.hpp>

namespace { // BEGIN_EMPTY_NAMESPACE
/*
$begin variable_command$$
$spell
	dismod
$$

$section The Variable Command$$

$head Syntax$$
$codei%dismod_at variable %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$subhead variable_table$$
A new variable table is created with the information
that maps a $cref/variable_id/variable_table/variable_id/$$
to its meaning in terms of the
$cref/model variables/model_variable/$$.
You can use this information to interpret a $cref fit_table$$
created by the $cref fit_command$$,
or to build a fit table for use as input to the $cref simulate_command$$.

$children%example/get_started/variable_command.py%$$
$head Example$$
The file $cref variable_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void variable_command(
	sqlite3*                                     db               ,
	const dismod_at::pack_info&                  pack_object      ,
	const dismod_at::db_input_struct&            db_input         ,
	const size_t&                                parent_node_id   ,
	const dismod_at::child_info&                 child_object
)
{	using CppAD::vector;
	using std::string;
	using dismod_at::to_string;

	string sql_cmd = "drop table if exists variable";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table variable("
		" variable_id    integer primary key,"
		" variable_type  text,"
		" smooth_id      integer,"
		" age_id         integer,"
		" time_id        integer,"
		" node_id        integer,"
		" rate_id        integer,"
		" integrand_id   integer,"
		" covariate_id   integer"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	string table_name = "variable";
	//
	CppAD::vector<string> col_name_vec(8), row_val_vec(8);
	col_name_vec[0]   = "variable_type";
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
	size_t offset, variable_id;
	for(size_t i = 2; i < row_val_vec.size(); i++)
		row_val_vec[i] = "null"; // these columns are null for mulstd variables
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	offset      = pack_object.mulstd_offset(smooth_id);
		for(size_t i = 0; i < 3; i++)
		{	variable_id              = offset + i;
			// variable_type
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
				variable_id
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
			{	size_t age_id   = index % n_age;
				size_t time_id  = index / n_age;
				variable_id     = offset + index;
				//
				// variable_value
				row_val_vec[0]  = "rate";     // variable_type
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
					variable_id
				);
			}
		}
	}
	//
	// covariate multiplers
	const CppAD::vector<dismod_at::mulcov_struct>&
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
			variable_id     = offset + index;
			//
		// variable_type
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
				variable_id
			);
		}
	}
	return;
}

/*
$begin fit_command$$
$spell
	tol
	arg
	Dismod
	Ipopt
	num_iter
$$

$section The Fit Command$$

$head Syntax$$
$codei%dismod_at fit %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$subhead fit_table$$
A new $cref fit_table$$ is created each time this command is run.
It contains the results of the fit in its
$cref/fit_value/fit_table/fit_value/$$ column.

$children%example/get_started/fit_command.py%$$
$head Example$$
The file $cref fit_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void fit_command(
	sqlite3*                                     db               ,
	const dismod_at::pack_info&                  pack_object      ,
	const dismod_at::db_input_struct&            db_input         ,
	const CppAD::vector<dismod_at::smooth_info>& s_info_vec       ,
	const dismod_at::data_model&                 data_object      ,
	const dismod_at::prior_model&                prior_object     ,
	const std::string&                           tolerance_arg    ,
	const std::string&                           max_num_iter_arg
)
{	using CppAD::vector;
	using std::string;
	using dismod_at::to_string;

	// ------------------ run fit_model ------------------------------------
	dismod_at::fit_model fit_object(
		pack_object          ,
		db_input.prior_table ,
		s_info_vec           ,
		data_object          ,
		prior_object
	);
	fit_object.run_fit(tolerance_arg, max_num_iter_arg);
	vector<double> solution = fit_object.get_solution();
	// -------------------- fit table --------------------------------------
	string sql_cmd = "drop table if exists fit";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table fit("
		" fit_id       integer primary key,"
		" fit_value    real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	string table_name = "fit";
	//
	CppAD::vector<string> col_name_vec(1), row_val_vec(1);
	col_name_vec[0]   = "fit_value";
	for(size_t fit_id = 0; fit_id < solution.size(); fit_id++)
	{	double fit_value   = solution[fit_id];
		row_val_vec[0] = to_string( fit_value );
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	return;
}
// ----------------------------------------------------------------------------
/*
$begin truth_command$$
$spell
	dismod
$$

$section The Truth Command$$

$head Syntax$$
$codei%dismod_at variable %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$subhead fit_table$$
In addition to the standard $cref input$$ tables,
there must be a $cref fit_table$$.

$subhead truth_table$$
A new $cref truth_table$$ is created with the information in the fit table;
to be specific,
$codei%
	%truth_id% = %fit_id% = %variable_id%
	%truth_value% = %fit_value%
%$$

$children%example/get_started/truth_command.py%$$
$head Example$$
The file $cref truth_command.py$$ contains an example and test
of using this command.

$end
*/

// ----------------------------------------------------------------------------
void truth_command(sqlite3* db)
{	using CppAD::vector;
	using std::string;
	//
	// get fit table information
	CppAD::vector<double> fit_value;
	string table_name  = "fit";
	string column_name = "fit_value";
	dismod_at::get_table_column(db, table_name, column_name, fit_value);
	//
	// create fit table
	string sql_cmd = "drop table if exists truth";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table truth("
		" truth_id       integer primary key,"
		" truth_value    real"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	table_name = "truth";
	CppAD::vector<string> col_name_vec(1), row_val_vec(1);
	col_name_vec[0]   = "truth_value";
	for(size_t fit_id = 0; fit_id < fit_value.size(); fit_id++)
	{	string truth_value = dismod_at::to_string( fit_value[fit_id] );
		row_val_vec[0]     = truth_value;
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	return;
}
// ----------------------------------------------------------------------------
/*
$begin simulate_command$$

$section The Simulate Command$$
$spell
	dismod
	arg
	std
	sim
	covariates
$$

$head Syntax$$
$codei%dismod_at simulate %file_name%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$subhead truth_table$$
The $cref truth_table$$ is an addition input table for this command.
It specifies the true values for the
$cref/model_variables/model_variable/$$ used during the simulation.
This table can be create by the $cref truth_command$$,
or the user can create it directly with the aid of the
$cref variable_table$$ (created by the $cref variable_command$$).

$subhead sim_meas_table$$
A new $cref sim_meas_table$$ is created.
It contains simulated measurement values that can be used in place of
the data table $cref/meas_value/data_table/meas_value/$$ column.
Only those entires in the data table for the following conditions
hold are simulated:
$list number$$
The $cref/node_id/data_table/node_id/$$ for the data is $icode parent_node_id$$,
or that is a descendent of $icode parent_node_id$$.
$lnext
All of the covariates satisfy the
$cref/max_difference/covariate_table/max_difference/$$ criteria.
$lend

$children%example/get_started/simulate_command.py%$$
$head Example$$
The file $cref simulate_command.py$$ contains an example and test
of using this command.

$end
*/
void simulate_command
(	sqlite3*                                            db              ,
	const CppAD::vector<dismod_at::integrand_struct>&   integrand_table ,
	const CppAD::vector<dismod_at::data_subset_struct>& subset_object   ,
	const dismod_at::data_model&                        data_object     ,
	const size_t&                                       actual_seed
)
{	using std::cerr;
	using std::endl;
	using std::string;
	using CppAD::vector;
	using dismod_at::to_string;
	// -----------------------------------------------------------------------
	// read truth table into pack_vec
	vector<double> pack_vec;
	string table_name = "truth";
	string column_name = "truth_value";
	dismod_at::get_table_column(db, table_name, column_name, pack_vec);
	// ----------------- sim_meas_table ----------------------------------
	table_name = "sim_meas";
	//
	string sql_cmd = "drop table if exists sim_meas";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	sql_cmd = "create table sim_meas("
		" sim_meas_id integer primary key,"
		" data_id     integer,"
		" meas_value  real"
	");";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	//
	vector<string> col_name_vec(2), row_val_vec(2);
	col_name_vec[0]   = "data_id";
	col_name_vec[1]   = "meas_value";
	//
	size_t n_sample = subset_object.size();
	for(size_t subset_id = 0; subset_id < n_sample; subset_id++)
	{	size_t integrand_id =  subset_object[subset_id].integrand_id;
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
			avg = data_object.avg_no_ode(subset_id, pack_vec);
			break;

			case dismod_at::prevalence_enum:
			case dismod_at::Tincidence_enum:
			case dismod_at::mtspecific_enum:
			case dismod_at::mtall_enum:
			case dismod_at::mtstandard_enum:
			avg = data_object.avg_yes_ode(subset_id, pack_vec);
			break;

			default:
			assert(false);
		}
		// need to simulate random noise with proper density
		dismod_at::density_enum density = dismod_at::density_enum(
			subset_object[subset_id].density_id
		);
		double meas_std     = subset_object[subset_id].meas_std;
		double eta          = integrand_table[integrand_id].eta;
		double meas_value   = dismod_at::sim_random(
			density, avg, meas_std, eta
		);
		row_val_vec[0] = to_string( subset_object[subset_id].data_id );
		row_val_vec[1] = to_string(meas_value);
# ifdef NDEBUG
		dismod_at::put_table_row(table_name, col_name_vec, row_val_vec);
# else
		size_t sim_meas_id = dismod_at::put_table_row(
			db, table_name, col_name_vec, row_val_vec
		);
		assert( sim_meas_id == subset_id );
# endif
	}

	return;
}
} // END_EMPTY_NAMESPACE

int main(int n_arg, const char** argv)
{	// ---------------- using statements ----------------------------------
	using std::cerr;
	using std::endl;
	using CppAD::vector;
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
	ok     |= command_arg == "variable";
	ok     |= command_arg == "fit";
	ok     |= command_arg == "truth";
	ok     |= command_arg == "simulate";
	if( ! ok )
	{	cerr << "dismod_at: command is not one of the following:" << endl
		<< "\tvariable, fit, truth, simulate" << endl;
		std::exit(1);
	}
	// --------------- get the input tables ---------------------------------
	bool new_file = false;
	sqlite3* db   = dismod_at::open_connection(file_name_arg, new_file);
	dismod_at::db_input_struct db_input;
	get_db_input(db, db_input);
	// ----------------------------------------------------------------------
	// argument_map
	std::map<string, string> argument_map;
	size_t n_argument = db_input.argument_table.size();
	for(size_t id = 0; id < n_argument; id++)
	{	string name  = db_input.argument_table[id].argument_name;
		string value = db_input.argument_table[id].argument_value;
		argument_map[name] = value;
	}
	// ---------------------------------------------------------------------
	// ode_step_size
	double ode_step_size  = std::atof( argument_map["ode_step_size"].c_str() );
	assert( ode_step_size > 0.0 );
	// ---------------------------------------------------------------------
	// initialize random number generator
	// 2DO: need to put actual seed in some output table or std::cout
	size_t random_seed = std::atoi( argument_map["random_seed"].c_str() );
	size_t actual_seed = dismod_at::new_gsl_rng(random_seed);
	// ---------------------------------------------------------------------
	// n_age_ode
	double age_min    = db_input.age_table[0];
	double age_max    = db_input.age_table[ db_input.age_table.size() - 1 ];
	size_t n_age_ode  = size_t( (age_max - age_min) / ode_step_size + 1.0 );
	assert( age_max  <= age_min  + n_age_ode * ode_step_size );
	// ---------------------------------------------------------------------
	// n_time_ode
	double time_min   = db_input.time_table[0];
	double time_max   = db_input.time_table[ db_input.time_table.size() - 1 ];
	size_t n_time_ode = size_t( (time_max - time_min) / ode_step_size + 1.0 );
	assert( time_max <= time_min  + n_time_ode * ode_step_size );
	// ---------------------------------------------------------------------
	// child_object and some more size_t values
	size_t parent_node_id = std::atoi(
		argument_map["parent_node_id"].c_str()
	);
	dismod_at::child_info child_object(
		parent_node_id ,
		db_input.node_table ,
		db_input.data_table
	);
	size_t n_child     = child_object.child_size();
	size_t n_integrand = db_input.integrand_table.size();
	size_t n_weight    = db_input.weight_table.size();
	size_t n_smooth    = db_input.smooth_table.size();
	// ---------------------------------------------------------------------
	// subset_object
	vector<dismod_at::data_subset_struct> subset_object = data_subset(
		db_input.data_table,
		db_input.covariate_table,
		child_object
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
			db_input.age_table         ,
			db_input.time_table        ,
			smooth_id                  ,
			db_input.smooth_table      ,
			db_input.smooth_grid_table
		);
	}
	// pack_object
	dismod_at::pack_info pack_object(
		n_integrand           ,
		n_child               ,
		db_input.smooth_table ,
		db_input.mulcov_table ,
		db_input.rate_table
	);
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
		subset_object            ,
		w_info_vec               ,
		s_info_vec               ,
		pack_object              ,
		child_object
	);
	string rate_info = argument_map["rate_info"];
	data_object.set_eigen_ode2_case_number(rate_info);
	// ---------------------------------------------------------------------
	if( command_arg == "variable" )
	{	variable_command(
			db,
			pack_object,
			db_input,
			parent_node_id,
			child_object
		);
	}
	else if( command_arg == "fit" )
	{	string tolerance    = argument_map["tolerance"];
		string max_num_iter = argument_map["max_num_iter"];
		fit_command(
			db               ,
			pack_object      ,
			db_input         ,
			s_info_vec       ,
			data_object      ,
			prior_object     ,
			tolerance        ,
			max_num_iter
		);
	}
	else if( command_arg == "truth" )
	{	truth_command(db);
	}
	else if( command_arg == "simulate" )
	{	simulate_command(
			db                       ,
			db_input.integrand_table ,
			subset_object            ,
			data_object              ,
			actual_seed
		);
	}
	else
		assert(false);
	// ---------------------------------------------------------------------
	sqlite3_close(db);
	return 0;
}
