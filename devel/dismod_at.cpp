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
$begin dismod_at_exe$$
$escape $$
$spell
	Dismod
	Ipopt
	num_iter
$$

$section The Dismod Age Time Executable$$

$head Syntax$$
$codei%dismod_at %file_name%$$
$icode%parent_node% %ode_step_size% %tolerance% %max_num_iter%$$

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ and $code output$$ tables.
The input tables are not modified and result are written to the
output tables.

$head parent_node$$
If all of the characters in $icode parent_node$$ are decimal digits,
then  it specifies the parent
$cref/node_id/node_table/node_id/$$ for this fit.
Otherwise it specifies the parent
$cref/node_name/node_table/node_name/$$.

$head n_age_ode$$
is a positive integer specifying the number of points in the
$cref/ode age grid/glossary/Ode Grid/Age, a_i/$$.

$subhead n_time_ode$$
is a positive integer specifying the number of points in the
$cref/ode time grid/glossary/Ode Grid/Time, t_j/$$.

$head ode_step_size$$
is a floating point number specifying the step size used to integrate the
ordinary differential equation.
The step size is the same in age and time because it is along cohort lines.

$head tolerance$$
is a floating point number specifying the desired relative convergence
tolerance (requested of Ipopt).

$head max_num_iter$$
is a positive integer specifying the maximum number of iterations.
The algorithm terminates with an error message if the number of
iterations exceeded this number.

$end
*/
# include <string>
# include <iostream>
# include <cppad/vector.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/get_db_input.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/child_data.hpp>
# include <dismod_at/put_table_row.hpp>
# include <dismod_at/to_string.hpp>
# include <dismod_at/get_column_max.hpp>

int main(int n_arg, const char** argv)
{	// ---------------- using statements ----------------------------------
	using std::cerr;
	using std::endl;
	using CppAD::vector;
	using std::string;

	// ---------------- command line arguments ---------------------------
	const char* usage =
	"dismod_at \\\n"
	"\tfile_name      \\\n"
	"\tparent_node    \\\n"
	"\tn_age_ode      \\\n"
	"\tn_time_ode     \\\n"
	"\tode_step_size  \\\n"
	"\ttolerance      \\\n"
	"\tmax_num_iter\n";
	if( n_arg != 8 )
	{	cerr << usage << endl <<
		"Expected 7 command line argument but found " << n_arg - 1 << endl;
		exit(1);
	}
	size_t i_arg = 0;
	const string file_name_arg     = argv[++i_arg];
	const string parent_node_arg   = argv[++i_arg];
	const string n_age_ode_arg     = argv[++i_arg];
	const string n_time_ode_arg    = argv[++i_arg];
	const string ode_step_size_arg = argv[++i_arg];
	const string tolerance_arg     = argv[++i_arg];
	const string max_num_iter_arg  = argv[++i_arg];
	//
	// --------------- get the input tables -----------------------------
	bool new_file = false;
	sqlite3* db   = dismod_at::open_connection(file_name_arg, new_file);
	dismod_at::db_input_struct db_input;
	get_db_input(db, db_input);
	//
	// --------------- parent_node_id -----------------------------------
	// parent_node_id
	size_t parent_node_id;
	bool is_id = true;
	for(size_t i = 0; i < parent_node_arg.size(); i++)
		is_id &= std::isdigit( parent_node_arg[i] );
	if( is_id )
		parent_node_id = std::atoi( parent_node_arg.c_str() );
	else
	{	parent_node_id = db_input.node_table.size();
		for(size_t id = 0; id < db_input.node_table.size(); id++)
			if( db_input.node_table[id].node_name == parent_node_arg )
				parent_node_id = id;
		if( parent_node_id == db_input.node_table.size() )
		{	cerr << "dismod_at: cannot find node_name " << parent_node_arg
			<< endl << "in " << file_name_arg << " node table" << endl;
		}
	}
	// ---------------------------------------------------------------------
	size_t n_age_ode     = std::atoi( n_age_ode_arg.c_str() );
	size_t n_time_ode    = std::atoi( n_time_ode_arg.c_str() );
	double ode_step_size = std::atof( ode_step_size_arg.c_str() );
	dismod_at::child_data child_object(
		parent_node_id      ,
		db_input.node_table ,
		db_input.data_table
	);
	size_t n_child     = child_object.child_size();
	size_t n_integrand = db_input.integrand_table.size();
	size_t n_weight    = db_input.weight_table.size();
	size_t n_smooth    = db_input.smooth_table.size();
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
		db_input.data_table      ,
		w_info_vec               ,
		s_info_vec               ,
		pack_object
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
	// ------------------- fit_table ------------------------------------
	vector<string> col_name_vec(6), row_val_vec(6);
	string table_name = "fit";
	col_name_vec[0]   = "parent_node_id";
	col_name_vec[1]   = "n_age_ode";
	col_name_vec[2]   = "n_time_ode";
	col_name_vec[3]   = "ode_step_size";
	col_name_vec[4]   = "tolerance";
	col_name_vec[5]   = "max_num_iter";
	//
	row_val_vec[0]    = dismod_at::to_string( parent_node_id );
	row_val_vec[1]    = n_age_ode_arg;
	row_val_vec[2]    = n_time_ode_arg;
	row_val_vec[3]    = ode_step_size_arg;
	row_val_vec[4]    = tolerance_arg;
	row_val_vec[5]    = max_num_iter_arg;
	size_t fit_id     = dismod_at::put_table_row(
		db, table_name, col_name_vec, row_val_vec
	);
	// ----------------- variable_table ----------------------------------
	table_name      = "variable";
	col_name_vec.resize(4);
	col_name_vec[0] = "fit_id";
	col_name_vec[1] = "sample";
	col_name_vec[2] = "offset";
	col_name_vec[3] = "value";
	//
	// determine the maximum value of sample (so far)
	string max_str = dismod_at::get_column_max(
		db, table_name, col_name_vec[1]
	);
	int sample = -1;
	if( max_str.size() > 0 )
		sample = std::atoi( max_str.c_str() );
	//
	row_val_vec.resize(4);
	row_val_vec[0]  = dismod_at::to_string( fit_id );
	row_val_vec[1]  = dismod_at::to_string( sample + 1 );
	for(size_t offset = 0; offset < solution.size(); offset++)
	{	row_val_vec[2] = dismod_at::to_string( offset );
		row_val_vec[3] = dismod_at::to_string( solution[offset] );
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	// ---------------------------------------------------------------------
	return 0;
}
