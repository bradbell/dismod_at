// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

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

/*
$begin fit_command$$
$spell
	tol
	arg
	Dismod
	Ipopt
	num_iter
	mtspecific
	mtall
	mtstandard
$$

$section The Fit Command$$

$head Syntax$$
$codei%dismod_at fit %file_name% %parent_node_id% \
	%n_age_ode% %n_time_ode% %ode_step_size% %tolerance% %max_num_iter%$$ \
	[ rho_zero ] [chi_zero]

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$subhead fit_arg_table$$
A new $cref fit_arg_table$$ table is created with the arguments to
this fit command; i.e., $icode file_name$$, ... , $icode max_num_iter$$.

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

$head n_age_ode$$
This is a positive integer (greater than zero)
specifying the number of points in the
$cref/ode age grid/glossary/Ode Grid/Age, a_i/$$.

$head n_time_ode$$
This is a positive integer
specifying the number of points in the
$cref/ode time grid/glossary/Ode Grid/Time, t_j/$$.

$head ode_step_size$$
This is a positive floating point number
that specifies numerical ode step size.
The step size is the same in age and time because it is along cohort lines.

$head tolerance$$
This is a positive floating point number
and specifies the $href%http://www.coin-or.org/Ipopt/documentation/%Ipopt%$$
desired relative convergence tolerance $code tol$$.

$head max_num_iter$$
This is a positive integer
and specifies the
$href%http://www.coin-or.org/Ipopt/documentation/%Ipopt%$$
maximum number of iterations $code max_iter$$.

$head rate_info$$
This argument only matters when one of the following
$cref/integrand names/integrand_table/integrand_name/$$ is used:
$code prevalence$$,
$code mtspecific$$,
$code mtall$$,
$code mtstandard$$.
In this case it is necessary to solve the dismod_ode
$cref/
ordinary differential equation /avg_integrand/Ordinary Differential Equation
/$$
The possible values for this argument are listed below:

$subhead chi_positive$$
If $icode%rate_info% = "chi_positive"%$$,
the $cref/parent smoothing/rate_table/parent_smooth_id/$$ for
$icode chi$$ always has a lower limit greater than zero.

$subhead iota_and_chi_zero$$
If $icode%rate_info% = "iota_and_chi_zero"%$$,
the $cref/parent smoothing/rate_table/parent_smooth_id/$$ for both
$icode iota$$ and $icode chi$$ have lower and upper limit zero.
In this case
the $cref/parent smoothing/rate_table/parent_smooth_id/$$ for
$icode rho$$ must always has a lower limit greater than zero.

$subhead rho_and_chi_zero$$
If $icode%rate_info% = "rho_and_chi_zero"%$$,
the $cref/parent smoothing/rate_table/parent_smooth_id/$$ for both
$icode rho$$ and $icode chi$$ have lower and upper limit zero.
In this case
the $cref/parent smoothing/rate_table/parent_smooth_id/$$ for
$icode iota$$ must always has a lower limit greater than zero.

$subhead iota_and_rho_zero$$
If $icode%rate_info% = "iota_and_rho_zero"%$$,
the $cref/parent smoothing/rate_table/parent_smooth_id/$$ for
both $icode iota$$ and $icode rho$$ have lower and upper limit zero.

$end
*/
namespace { // BEGIN_EMPTY_NAMESPACE

void fit_command(
	sqlite3*                                     db               ,
	const dismod_at::pack_info&                  pack_object      ,
	const dismod_at::db_input_struct&            db_input         ,
	const CppAD::vector<dismod_at::smooth_info>& s_info_vec       ,
	const dismod_at::data_model&                 data_object      ,
	const dismod_at::prior_model&                prior_object     ,
	const std::string&                           tolerance_arg    ,
	const std::string&                           max_num_iter_arg ,
	const size_t&                                n_fit_arg        ,
	const char*                                  fit_arg_name[]   ,
	const char**                                 argv             ,
	const size_t&                                parent_node_id   ,
	const dismod_at::child_info&                 child_object
)
{	using CppAD::vector;
	using std::string;

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
	// ----------------- fit_arg_table ----------------------------------
	const char* sql_cmd;
	CppAD::vector<string> col_name_vec, row_val_vec;
	string table_name;
	//
	sql_cmd = "drop table if exists fit_arg";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table fit_arg("
		"fit_arg_id integer primary key, fit_arg_name text, fit_arg_value text"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	table_name = "fit_arg";
	//
	col_name_vec.resize(3);
	col_name_vec[0]   = "fit_arg_id";
	col_name_vec[1]   = "fit_arg_name";
	col_name_vec[2]   = "fit_arg_value";
	//
	row_val_vec.resize(3);
	for(size_t id = 0; id < n_fit_arg; id++)
	{	row_val_vec[0] = dismod_at::to_string(id);
		row_val_vec[1] = fit_arg_name[id];
		row_val_vec[2] = argv[2 + id];
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	// ----------------- variable_table ----------------------------------
	sql_cmd = "drop table if exists variable";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table variable("
		" variable_id integer primary key,"
		" variable_value real,"
		" variable_name  text"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	table_name = "variable";
	//
	col_name_vec[0]   = "variable_id";
	col_name_vec[1]   = "variable_value";
	col_name_vec[2]   = "variable_name";
	//
	for(size_t index = 0; index < solution.size(); index++)
	{	row_val_vec[0] = dismod_at::to_string( index );
		row_val_vec[1] = dismod_at::to_string( solution[index] );
		row_val_vec[2] = pack_object.variable_name(
			index,
			parent_node_id,
			db_input.age_table,
			db_input.covariate_table,
			db_input.integrand_table,
			db_input.mulcov_table,
			db_input.node_table,
			db_input.smooth_table,
			db_input.time_table,
			s_info_vec,
			child_object
		);
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
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
		"n_age_ode",
		"n_time_ode",
		"ode_step_size",
		"tolerance",
		"max_num_iter",
		"rate_info"
	};
	size_t n_fit_arg = sizeof(fit_arg_name) / sizeof(fit_arg_name[0]);
	//
	// ---------------- command line arguments ---------------------------
	string usage = "dismod_at-";
	usage       += DISMOD_AT_VERSION;
	usage       += "\ndismod_at fit";
	for(size_t i = 0; i < n_fit_arg; i++)
	{	usage += " ";
		usage += fit_arg_name[i];
	}
	if( size_t(n_arg) != n_fit_arg + 2 )
	{	cerr << usage << endl;
		std::exit(1);
	}
	size_t i_arg = 0;
	const string command_arg       = argv[++i_arg];
	if( command_arg != "fit" )
	{	cerr << usage << endl;
		std::exit(1);
	}
	const string file_name_arg      = argv[++i_arg];
	const string parent_node_id_arg = argv[++i_arg];
	const string n_age_ode_arg      = argv[++i_arg];
	const string n_time_ode_arg     = argv[++i_arg];
	const string ode_step_size_arg  = argv[++i_arg];
	const string tolerance_arg      = argv[++i_arg];
	const string max_num_iter_arg   = argv[++i_arg];
	const string rate_info_arg      = argv[++i_arg];
	// ------------- check rate_info_arg ------------------------------------
	bool ok = rate_info_arg == "chi_positive";
	ok     |= rate_info_arg == "iota_and_chi_zero";
	ok     |= rate_info_arg == "rho_and_chi_zero";
	ok     |= rate_info_arg == "iota_and_rho_zero";
	if( ! ok )
	{	cerr << usage << endl
		<< "rate_info = " << rate_info_arg
		<< " is not a valid choice" << endl;
		std::exit(1);
	}
	// --------------- get the input tables ---------------------------------
	bool new_file = false;
	sqlite3* db   = dismod_at::open_connection(file_name_arg, new_file);
	dismod_at::db_input_struct db_input;
	get_db_input(db, db_input);
	// ---------------------------------------------------------------------
	size_t parent_node_id = std::atoi( parent_node_id_arg.c_str() );
	size_t n_age_ode      = std::atoi( n_age_ode_arg.c_str() );
	size_t n_time_ode     = std::atoi( n_time_ode_arg.c_str() );
	double ode_step_size  = std::atof( ode_step_size_arg.c_str() );
	dismod_at::child_info child_object(
		parent_node_id ,
		db_input.node_table ,
		db_input.data_table
	);
	size_t n_child     = child_object.child_size();
	size_t n_integrand = db_input.integrand_table.size();
	size_t n_weight    = db_input.weight_table.size();
	size_t n_smooth    = db_input.smooth_table.size();
	// data_sample
	vector<dismod_at::data_subset_struct> data_sample = data_subset(
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
		data_sample              ,
		w_info_vec               ,
		s_info_vec               ,
		pack_object              ,
		child_object
	);
	data_object.set_eigen_ode2_case_number(rate_info_arg);
	//
	fit_command(
		db               ,
		pack_object      ,
		db_input         ,
		s_info_vec       ,
		data_object      ,
		prior_object     ,
		tolerance_arg    ,
		max_num_iter_arg ,
		n_fit_arg        ,
		fit_arg_name     ,
		argv             ,
		parent_node_id   ,
		child_object
	);
	// ---------------------------------------------------------------------
	sqlite3_close(db);
	return 0;
}
