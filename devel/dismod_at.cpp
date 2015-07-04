// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

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

namespace { // BEGIN_EMPTY_NAMESPACE

// --------------------------------------------------------------------------
// create command argument table
void command_arg_table
(	sqlite3*            db         ,
	const std::string&  table_name ,
	const size_t&       n_arg      ,
	const char*         arg_name[] ,
	const char**        argv
)
{
	using std::string;
	using CppAD::vector;
	vector<string> col_name_vec(2), row_val_vec(2);
	string cmd;
	//
	cmd  = "drop table if exists ";
	cmd += table_name;
	dismod_at::exec_sql_cmd(db, cmd);
	//
	cmd  = "create table ";
	cmd += table_name;
	cmd += "(";
	cmd += table_name + "_id integer primary key, ";
	cmd += table_name + "_name text unique, ";
	cmd += table_name + "_value text)";
	dismod_at::exec_sql_cmd(db, cmd);
	//
	col_name_vec[0]   = table_name + "_name";
	col_name_vec[1]   = table_name + "_value";
	//
	for(size_t id = 0; id < n_arg; id++)
	{	row_val_vec[0] = arg_name[id];
		row_val_vec[1] = argv[2 + id];
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
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
$codei%dismod_at fit %file_name% %parent_node_id% \
	%ode_step_size% %tolerance% %max_num_iter% %rate_info%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$subhead fit_arg_table$$
A new $cref fit_arg_table$$ table is created with the arguments to
this fit command; i.e., $icode file_name$$, ... , $icode rate_info$$.

$subhead variable_table$$
A new $cref variable_table$$ is created with the maximum likelihood
estimate corresponding to this fit command.
To be specific, the
$cref/fixed effects/model_variable/Fixed Effects, theta/$$
maximize the Laplace approximation
$cref/L(theta)/approx_mixed_theory/Objective/L(theta)/$$ and the
$cref/random effects/model_variable/Random Effects, u/$$
maximize the joint likelihood; see
$cref/u^(theta)/approx_mixed_theory/Objective/u^(theta)/$$.

$head parent_node_id$$
This is a non-negative integer (greater than or equal zero)
that specifies the parent $cref/node_id/node_table/node_id/$$.

$head ode_step_size$$
This is a positive floating point number (greater than zero)
that specifies numerical ode step size.
The step size is the same in age and time because it is along cohort lines.
The smaller $icode ode_step_size$$, the more accurate the solution of the
$cref/
ordinary differential equation /avg_integrand/Ordinary Differential Equation
/$$
and the more time required to compute this solution.

$head tolerance$$
This is a positive floating point number
and specifies the $href%http://www.coin-or.org/Ipopt/documentation/%Ipopt%$$
desired relative convergence tolerance $code tol$$.

$head max_num_iter$$
This is a positive integer
and specifies the
$href%http://www.coin-or.org/Ipopt/documentation/%Ipopt%$$
maximum number of iterations $code max_iter$$.

$include devel/rate_info.omh$$

$head Example$$
See the $cref get_started.py$$ example and test.

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
	const std::string&                           max_num_iter_arg ,
	const size_t&                                parent_node_id   ,
	const dismod_at::child_info&                 child_object
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
	// ----------------- variable_table ----------------------------------
	string sql_cmd = "drop table if exists variable";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table variable("
		" variable_id    integer primary key,"
		" variable_value real,"
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
	CppAD::vector<string> col_name_vec(9), row_val_vec(9);
	col_name_vec[0]   = "variable_value";
	col_name_vec[1]   = "variable_type";
	col_name_vec[2]   = "smooth_id";
	col_name_vec[3]   = "age_id";
	col_name_vec[4]   = "time_id";
	col_name_vec[5]   = "node_id";
	col_name_vec[6]   = "rate_id";
	col_name_vec[7]   = "integrand_id";
	col_name_vec[8]   = "covariate_id";
	//
	// mulstd variables
	size_t n_smooth = db_input.smooth_table.size();
	size_t offset, variable_id;
	for(size_t i = 3; i < row_val_vec.size(); i++)
		row_val_vec[i] = "null"; // these columns are null for mulstd variables
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	offset      = pack_object.mulstd_offset(smooth_id);
		for(size_t i = 0; i < 3; i++)
		{	variable_id              = offset + i;
			// variable_type
			if( i == 0 )
				row_val_vec[1] = "mulstd_value";
			else if( i == 1 )
				row_val_vec[1] = "mulstd_dage";
			else
				row_val_vec[1] = "mulstd_dtime";
			//
			// variable_value
			row_val_vec[0] = to_string( solution[variable_id] );
			// smooth_id
			row_val_vec[2] = to_string( smooth_id );
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
			n_age     = s_info_vec[smooth_id].age_size();
			n_time    = s_info_vec[smooth_id].time_size();
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
				row_val_vec[0]  = to_string( solution[variable_id] );
				row_val_vec[1]  = "rate";     // variable_type
				row_val_vec[2]  = "null";     // smooth_id
				row_val_vec[3]  = to_string( age_id );
				row_val_vec[4]  = to_string( time_id );
				row_val_vec[5]  = to_string( node_id );
				row_val_vec[6]  = to_string( rate_id );
				row_val_vec[7]  = "null";     // integrand_id
				row_val_vec[8]  = "null";     // covariate_id
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
		n_age     = s_info_vec[smooth_id].age_size();
		n_time    = s_info_vec[smooth_id].time_size();
		assert( n_var == n_age * n_time );
		for(size_t index = 0; index < n_var; index++)
		{	size_t age_id   = index % n_age;
			size_t time_id  = index / n_age;
			variable_id     = offset + index;
			//
		// variable_type
			if( mulcov_type == dismod_at::rate_mean_enum )
				row_val_vec[1]  = "mulcov_rate_mean";
			else if( mulcov_type == dismod_at::meas_value_enum )
				row_val_vec[1]  = "mulcov_meas_value";
			else if( mulcov_type == dismod_at::meas_std_enum )
				row_val_vec[1]  = "mulcov_meas_std";
			else assert(false);
			//
			// variable_value
			row_val_vec[0]  = to_string( solution[variable_id] );
			row_val_vec[2]  = "null";     // smooth_id
			row_val_vec[3]  = to_string( age_id );
			row_val_vec[4]  = to_string( time_id );
			row_val_vec[5]  = "null";     // node_id
			row_val_vec[6]  = "null";     // rate_id
			row_val_vec[7]  = to_string( integrand_id );
			row_val_vec[8]  = to_string( covariate_id );
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
// ----------------------------------------------------------------------------
/*
$begin simulate_command$$

$section The Simulate Command$$
$spell
	dismod
	arg
	std
	covariates
$$

$head Under Construction$$
This command is under construction and does not yet work.

$head Syntax$$
$codei%dismod_at simulate %file_name% %parent_node_id% \
	%ode_step_size% %rate_info% %random_seed%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$subhead variable_table$$
The data base must contain a $cref variable_table$$ that specifies the
value of the model variables that is used to simulate the data.
It may have been created by a previous $cref fit_command$$.

$subhead simulate_arg_table$$
A new $cref simulate_arg_table$$ table is created with the arguments to
this simulate command;
i.e., $icode file_name$$, ... , $icode rate_info$$.

$subhead simulate_table$$
A new $cref simulate_table$$ is created.
It contains simulated values that can be used in place of the data table
$cref/meas_value/data_table/meas_value/$$ column.
Only those entires in the data table for the following conditions
hold are simulated:
$list number$$
The $cref/node_id/data_table/node_id/$$ for the data is $icode parent_node_id$$,
or that is a descendent of $icode parent_node_id$$.
$lnext
All of the covariates satisfy the
$cref/max_difference/covariate_table/max_difference/$$ criteria.
$lend

$head parent_node_id$$
This is a non-negative integer (greater than or equal zero)
that specifies the parent $cref/node_id/node_table/node_id/$$
for this simulation (see data_table heading above).

$head ode_step_size$$
This is a positive floating point number (greater than zero)
that specifies numerical ode step size.
The step size is the same in age and time because it is along cohort lines.
The smaller $icode ode_step_size$$, the more accurate the solution of the
$cref/
ordinary differential equation /avg_integrand/Ordinary Differential Equation
/$$
and the more time required to compute this solution.

$head random_seed$$
This is a non-negative integer used to seed the random number
generator (used to generate simulated measurement noise).
If this value is zero, the clock is used to seed the random number generator.

$include devel/rate_info.omh$$

$head Example$$
2DO: add an example and test for the simulate command.

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
	// pack_vec
	vector<double> pack_vec;
	string table_name = "variable";
	string column_name = "value";
	dismod_at::get_table_column(db, table_name, column_name, pack_vec);
	// ----------------- simulate_table ----------------------------------
	table_name = "simulate";
	//
	string sql_cmd = "drop table if exists simulate";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table simulate("
		" simulate_id integer primary key, meas_value real"
	");";
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
# if 0
		// need to simulate random noise with proper density
		dismod_at::density_enum density = dismod_at::density_enum(
			subset_object[subset_id].density_id
		);
		double meas_std = subset_object[subset_id].meas_std;
# endif
		row_val_vec[0] = to_string( subset_object[subset_id].data_id );
		row_val_vec[1] = to_string(avg);
# ifdef NDEBUG
		dismod_at::put_table_row(table_name, col_name_vec, row_val_vec);
# else
		size_t simulate_id = dismod_at::put_table_row(
			db, table_name, col_name_vec, row_val_vec
		);
		assert( simulate_id == subset_id );
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
	// -----------------------------------------------------------------------
	// fit command line argument names
	const char* fit_arg_name[] = {
		"file_name",
		"parent_node_id",
		"ode_step_size",
		"tolerance",
		"max_num_iter",
		"rate_info"
	};
	size_t n_fit_arg = sizeof(fit_arg_name) / sizeof(fit_arg_name[0]);
	// -----------------------------------------------------------------------
	// simulate command line argument names
	const char* simulate_arg_name[] = {
		"file_name",
		"parent_node_id",
		"ode_step_size",
		"random_seed",
		"rate_info"
	};
	size_t n_simulate_arg =
		sizeof(simulate_arg_name) / sizeof(simulate_arg_name[0]);
	//
	// ---------------- command line arguments ---------------------------
	string program = "dismod_at-";
	program       += DISMOD_AT_VERSION;
	string fit_usage   = "dismod_at fit";
	for(size_t i = 0; i < n_fit_arg; i++)
	{	fit_usage += " ";
		fit_usage += fit_arg_name[i];
	}
	string simulate_usage   = "dismod_at simulate";
	for(size_t i = 0; i < n_simulate_arg; i++)
	{	simulate_usage += " ";
		simulate_usage += simulate_arg_name[i];
	}
	if( n_arg < 2 )
	{	cerr << program << endl;
		cerr << fit_usage << endl;
		cerr << simulate_usage << endl;
		std::exit(1);
	}
	size_t i_arg = 0;
	const string command_arg  = argv[++i_arg];
	if( command_arg == "fit" )
	{	if( size_t(n_arg) != n_fit_arg + 2 )
		{	cerr << fit_usage << endl;
			std::exit(1);
		}
	}
	else if( command_arg == "simulate" )
	{	if( size_t(n_arg) != n_simulate_arg + 2 )
		{	cerr << simulate_usage << endl;
			std::exit(1);
		}
	}
	else
	{	cerr << program << endl;
		cerr << fit_usage << endl;
		cerr << simulate_usage << endl;
		std::exit(1);
	}
	const string file_name_arg      = argv[++i_arg];
	const string parent_node_id_arg = argv[++i_arg];
	const string ode_step_size_arg  = argv[++i_arg];
	string tolerance_arg, max_num_iter_arg, random_seed_arg;
	if( command_arg == "fit" )
	{	tolerance_arg    = argv[++i_arg];
		max_num_iter_arg = argv[++i_arg];
	}
	if( command_arg == "simulate" )
	{	random_seed_arg  = argv[++i_arg];
	}
	const string rate_info_arg      = argv[++i_arg];
	// ------------- check rate_info_arg ------------------------------------
	bool ok = rate_info_arg == "chi_positive";
	ok     |= rate_info_arg == "iota_and_chi_zero";
	ok     |= rate_info_arg == "rho_and_chi_zero";
	ok     |= rate_info_arg == "iota_and_rho_zero";
	if( ! ok )
	{	if( command_arg == "fit" )
			cerr << fit_usage << endl;
		else
			cerr << simulate_usage << endl;
		cerr << "rate_info = " << rate_info_arg
		<< " is not a valid choice" << endl;
		std::exit(1);
	}
	// --------------- get the input tables ---------------------------------
	bool new_file = false;
	sqlite3* db   = dismod_at::open_connection(file_name_arg, new_file);
	dismod_at::db_input_struct db_input;
	get_db_input(db, db_input);
	// ---------------------------------------------------------------------
	// ode_step_size
	double ode_step_size  = std::atof( ode_step_size_arg.c_str() );
	if( ode_step_size <= 0.0 )
	{	if( command_arg == "fit" )
			cerr << fit_usage << endl;
		else
			cerr << simulate_usage << endl;
		cerr << "ode_step_size = " << ode_step_size_arg
		<< " is less than or equal zero " << endl;
		std::exit(1);
	}
	// ---------------------------------------------------------------------
	// initialize random number generator
	size_t random_seed = std::atoi( random_seed_arg.c_str() );
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
	size_t parent_node_id = std::atoi( parent_node_id_arg.c_str() );
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
	data_object.set_eigen_ode2_case_number(rate_info_arg);
	//
	if( command_arg == "fit" )
	{	fit_command(
			db               ,
			pack_object      ,
			db_input         ,
			s_info_vec       ,
			data_object      ,
			prior_object     ,
			tolerance_arg    ,
			max_num_iter_arg ,
			parent_node_id   ,
			child_object
		);
		string table_name = "fit_arg";
		command_arg_table(
			db           ,
			table_name   ,
			n_fit_arg    ,
			fit_arg_name ,
			argv
		);
	}
	else if( command_arg == "simulate" )
	{	simulate_command(
			db                       ,
			db_input.integrand_table ,
			subset_object            ,
			data_object              ,
			actual_seed
		);
		string table_name = "simulate_arg";
		command_arg_table(
			db                ,
			table_name        ,
			n_simulate_arg    ,
			simulate_arg_name ,
			argv
		);
		// add actual_seed to simulate_arg table
		vector<string> col_name_vec(2), row_val_vec(2);
		col_name_vec[0]   = table_name + "_name";
		col_name_vec[1]   = table_name + "_value";
		row_val_vec[0]    = "actual_seed";
		row_val_vec[1]    = dismod_at::to_string(actual_seed);
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	else
		assert(false);
	// ---------------------------------------------------------------------
	sqlite3_close(db);
	return 0;
}
