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
$codei%dismod_at %command% %file_name% %fit_id%$$

$head command$$
The only valid value (so far) for $icode command$$
is $code fit$$.
Other commands, such as sample from the
posterior distribution, are planned for the future.

$head file_name$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ data base containing the
$code dismod_at$$ $cref input$$ and $code output$$ tables.
The input tables are not modified and result are written to the
output tables.

$head fit_id$$
This is a non-negative integer and specifies which row of the
$cref/fit_table/fit_table/fit_id/$$ will be used for this fit.

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
	const char* usage = "dismod_at command file_name fit_id";
	if( n_arg != 4 )
	{	cerr << usage << endl <<
		"Expected 3 command line argument but found " << n_arg - 1 << endl;
		exit(1);
	}
	size_t i_arg = 0;
	const string command_arg       = argv[++i_arg];
	const string file_name_arg     = argv[++i_arg];
	const string fit_id_arg        = argv[++i_arg];
	// ------------------------------------------------------------------
	if( command_arg != "fit" )
	{	cerr << usage << endl <<
		"Only the 'fit' command is supported (so far)" << endl;
		exit(1);
	}
	// --------------- get the input tables -----------------------------
	bool new_file = false;
	sqlite3* db   = dismod_at::open_connection(file_name_arg, new_file);
	dismod_at::db_input_struct db_input;
	get_db_input(db, db_input);
	// ----------------- fit_id -----------------------------------------
	size_t fit_id = static_cast<size_t>( std::atoi( fit_id_arg.c_str() ) );
	if( fit_id >= db_input.fit_table.size() )
	{	cerr << usage << endl <<
		"Invalid value for fit_id (not in fit table)" << endl;
		exit(1);
	}
	// ---------------------------------------------------------------------
	size_t parent_node_id = size_t(db_input.fit_table[fit_id].parent_node_id);
	size_t n_age_ode      = size_t(db_input.fit_table[fit_id].n_age_ode);
	size_t n_time_ode     = size_t(db_input.fit_table[fit_id].n_time_ode);
	double ode_step_size  = db_input.fit_table[fit_id].ode_step_size;
	// 2DO: remove need to conver these two values to strings
	string tolerance_str  = dismod_at::to_string(
		db_input.fit_table[fit_id].tolerance
	);
	string max_num_iter_str = dismod_at::to_string(
		db_input.fit_table[fit_id].max_num_iter
	);
	dismod_at::child_data child_object(
		parent_node_id ,
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
	fit_object.run_fit(tolerance_str, max_num_iter_str);
	vector<double> solution = fit_object.get_solution();
	// ----------------- variable_table ----------------------------------
	CppAD::vector<string> col_name_vec(4), row_val_vec(4);
	string table_name = "variable";
	col_name_vec[0]   = "fit_id";
	col_name_vec[1]   = "sample";
	col_name_vec[2]   = "offset";
	col_name_vec[3]   = "value";
	//
	// maximum likelihood sample
	size_t sample = 0;
	//
	row_val_vec[0]  = dismod_at::to_string( fit_id );
	row_val_vec[1]  = dismod_at::to_string( sample );
	for(size_t offset = 0; offset < solution.size(); offset++)
	{	row_val_vec[2] = dismod_at::to_string( offset );
		row_val_vec[3] = dismod_at::to_string( solution[offset] );
		dismod_at::put_table_row(db, table_name, col_name_vec, row_val_vec);
	}
	// ---------------------------------------------------------------------
	return 0;
}
