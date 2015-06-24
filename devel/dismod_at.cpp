// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
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
	%n_age_ode% %n_time_ode% %ode_step_size% %tolerance% %max_num_iter%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$subhead fit_arg_table$$
A new $cref fit_arg_table$$ table is created with the arguments to
this fit command; i.e., $icode file_name$$, ... , $icode max_num_iter$$.

$subhead fit_result_table$$
A new $cref fit_result_table$$ is created with the results of this
fit command.

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

$end
*/
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
		"max_num_iter"
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
		exit(1);
	}
	size_t i_arg = 0;
	const string command_arg       = argv[++i_arg];
	if( command_arg != "fit" )
	{	cerr << usage << endl;
		exit(1);
	}
	const string file_name_arg      = argv[++i_arg];
	const string parent_node_id_arg = argv[++i_arg];
	const string n_age_ode_arg      = argv[++i_arg];
	const string n_time_ode_arg     = argv[++i_arg];
	const string ode_step_size_arg  = argv[++i_arg];
	const string tolerance_arg      = argv[++i_arg];
	const string max_num_iter_arg   = argv[++i_arg];
	// --------------- get the input tables -----------------------------
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
			weight_id, db_input.weight_grid_table
		);
	}
	// s_info_vec
	vector<dismod_at::smooth_info> s_info_vec(n_smooth);
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	s_info_vec[smooth_id] = dismod_at::smooth_info(
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
	// ----------------- fit_result_table ----------------------------------
	sql_cmd = "drop table if exists fit_result";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	sql_cmd = "create table fit_result("
		" fit_result_id integer primary key,"
		" fit_result_value real,"
		" fit_result_name  text"
	")";
	dismod_at::exec_sql_cmd(db, sql_cmd);
	table_name = "fit_result";
	//
	col_name_vec[0]   = "fit_result_id";
	col_name_vec[1]   = "fit_result_value";
	col_name_vec[2]   = "fit_result_name";
	//
	for(size_t index = 0; index < solution.size(); index++)
	{	row_val_vec[0] = dismod_at::to_string( index );
		row_val_vec[1] = dismod_at::to_string( solution[index] );
		row_val_vec[2] = pack_object.variable_name(
			index,
			parent_node_id,
			db_input.age_table,
			db_input.time_table,
			db_input.node_table,
			db_input.smooth_table,
			s_info_vec,
			child_object
		);
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	// ---------------------------------------------------------------------
	sqlite3_close(db);
	return 0;
}
