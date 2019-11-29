// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <cppad/mixed/exception.hpp>
# include <dismod_at/depend_command.hpp>
# include <dismod_at/fit_command.hpp>
# include <dismod_at/init_command.hpp>
# include <dismod_at/old2new_command.hpp>
# include <dismod_at/predict_command.hpp>
# include <dismod_at/sample_command.hpp>
# include <dismod_at/set_command.hpp>
# include <dismod_at/simulate_command.hpp>

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
# include <dismod_at/get_data_sim_table.hpp>
# include <dismod_at/get_prior_sim_table.hpp>
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
# include <dismod_at/get_prior_mean.hpp>
# include <dismod_at/age_avg_grid.hpp>

# define DISMOD_AT_TRACE 0


int main(int n_arg, const char** argv)
{	// ---------------- using statements ----------------------------------
	using std::cerr;
	using std::endl;
	using std::string;
	using CppAD::vector;
	// ---------------- command line arguments ---------------------------
	struct { const char* name; int n_arg; } command_info[] = {
		{"old2new",   3},
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
# ifndef NDEBUG
	program       += " debug build";
# else
	program       += " release build";
# endif
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
	// ----------------------------------------------------------------------
	// old2new command must fix database before get_db_input can be run
	if( command_arg == "old2new" )
	{	dismod_at::old2new_command(db);
		message = "end " + command_arg;
		dismod_at::log_message(db, DISMOD_AT_NULL_PTR, "command", message);
		sqlite3_close(db);
		return 0;
	}
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
	vector<dismod_at::weight_info> w_info_vec(n_weight + 1);
	for(size_t weight_id = 0; weight_id < n_weight; weight_id++)
	{	w_info_vec[weight_id] = dismod_at::weight_info(
			db_input.age_table,
			db_input.time_table,
			weight_id,
			db_input.weight_table,
			db_input.weight_grid_table
		);
	}
	// The constant weighting is placed at the end of w_info_vec
	w_info_vec[n_weight] = dismod_at::weight_info();
	//
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
		db_input.subgroup_table     ,
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
		db_input.prior_table, var2prior
	);
	// meas_noise_effect
	string meas_noise_effect = option_map["meas_noise_effect"];
	//
	// rate_case
	string rate_case = option_map["rate_case"];
	//
	// age_avg_split
	string age_avg_split = option_map["age_avg_split"];
	//
	// age_avg_grid and age_avg table
	vector<double> age_avg_grid;
	if( command_arg != "set" )
	{	// do not execute this during a set command because it might
		// exit with an error that the user is trying to fix
		age_avg_grid = dismod_at::age_avg_grid(
			ode_step_size, age_avg_split, db_input.age_table
		);
		size_t n_age_avg = age_avg_grid.size();
		//
		// output age_avg table
		string sql_cmd = "drop table if exists age_avg";
		dismod_at::exec_sql_cmd(db, sql_cmd);
		//
		table_name = "age_avg";
		vector<string> col_name(1), col_type(1), row_value(n_age_avg);
		vector<bool> col_unique(1);
		col_name[0]   = "age";
		col_type[0]   = "real";
		col_unique[0] = true;
		for(size_t i = 0; i < n_age_avg; ++i)
			row_value[i] = CppAD::to_string( age_avg_grid[i] );
		dismod_at::create_table(
			db, table_name, col_name, col_type, col_unique, row_value
		);
	}
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
	// fit_simulated_data
	bool fit_simulated_data = false;
	if( command_arg == "fit" && n_arg >= 5 )
		fit_simulated_data = true;
	if( command_arg == "sample" )
		fit_simulated_data = true;
	// =======================================================================
# ifdef NDEBUG
	try { // BEGIN_TRY_BLOCK (when not debugging)
# endif
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
			dismod_at::set_option_command(
				db, db_input.option_table, name, value);
		}
		else if( std::strcmp(argv[3], "avgint") == 0 )
		{	if( n_arg != 5 )
			{	cerr << "expected data to follow "
				"dismod_at database set avgint\n";
				std::exit(1);
			}
			dismod_at::set_avgint_command(db);
		}
		else
		{	std::string table_out     = argv[3];
			std::string source        = argv[4];
			std::string sample_index  = "";
			if( n_arg == 6 )
				sample_index = argv[5];
			dismod_at::set_command(
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
			fit_simulated_data       ,
			meas_noise_effect        ,
			rate_case                ,
			bound_random             ,
			n_covariate              ,
			ode_step_size            ,
			age_avg_grid             ,
			db_input.age_table       ,
			db_input.time_table      ,
			db_input.subgroup_table  ,
			db_input.integrand_table ,
			db_input.mulcov_table    ,
			db_input.prior_table     ,
			avgint_subset_obj        ,
			avgint_subset_cov_value  ,
			w_info_vec               ,
			s_info_vec               ,
			pack_object              ,
			child_avgint
		);
		size_t n_var = pack_object.size();
		std::string source = argv[3];
		dismod_at::predict_command(
			source               ,
			db                   ,
			db_input             ,
			n_var                ,
			avgint_object        ,
			avgint_subset_obj    ,
			var2prior
		);
	}
	else
	{	// -------------------------------------------------------------------
		// data_subset_obj
		vector<dismod_at::data_subset_struct> data_subset_obj;
		vector<double> data_subset_cov_value;
		data_subset(
			db_input.density_table,
			db_input.data_table,
			db_input.data_cov_value,
			db_input.covariate_table,
			child_data,
			data_subset_obj,
			data_subset_cov_value
		);
		if( command_arg == "init" )
		{	dismod_at::init_command(
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
			//
			dismod_at::prior_model prior_object(
				pack_object           ,
				var2prior             ,
				db_input.age_table    ,
				db_input.time_table   ,
				db_input.prior_table  ,
				db_input.density_table
			);
			dismod_at::data_model data_object(
				fit_simulated_data       ,
				meas_noise_effect        ,
				rate_case                ,
				bound_random             ,
				n_covariate              ,
				ode_step_size            ,
				age_avg_grid             ,
				db_input.age_table       ,
				db_input.time_table      ,
				db_input.subgroup_table  ,
				db_input.integrand_table ,
				db_input.mulcov_table    ,
				db_input.prior_table     ,
				data_subset_obj          ,
				data_subset_cov_value    ,
				w_info_vec               ,
				s_info_vec               ,
				pack_object              ,
				child_data
			);
			//
			if( command_arg == "depend" )
			{	depend_command(
					db               ,
					prior_mean       ,
					data_object      ,
					data_subset_obj  ,
					prior_object
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
					data_subset_obj  ,
					data_object      , // not  const
					prior_object     , // not  const
					pack_object      ,
					var2prior        ,
					db_input         ,
					option_map
				);
			}
			else if( command_arg == "simulate" )
			{	// replace_like
				data_object.replace_like(data_subset_obj );
				simulate_command(
					argv[3]                  , // number_simulate
					meas_noise_effect        ,
					db                       ,
					data_subset_obj          ,
					data_object              ,
					var2prior                ,
					pack_object              ,
					db_input                 ,
					option_map
				);
			}
			else if( command_arg == "sample" )
			{	sample_command(
					argv[3]              , // const method
					argv[4]              , // const number_sample
					db                   , // not const
					data_subset_obj      , // ...
					data_object          , // ...
					prior_object         , // ...
					db_input.prior_table , // const
					pack_object          , // ...
					var2prior            , // ...
					db_input             , // ...
					option_map             // effectively const
				);
			}
			else
				assert(false);
		}
	}
	// =======================================================================
# ifdef NDEBUG
	} // END_TRY_BLOCK (when not debugging)
	catch(const CppAD::mixed::exception& e)
	{	string catcher("dismod_at");
		catcher += " " + database_arg + " " + command_arg;
		message  = e.message(catcher);
		dismod_at::error_exit(message);
	}
# endif
	// =======================================================================
	// ---------------------------------------------------------------------
	message = "end " + command_arg;
	dismod_at::log_message(db, DISMOD_AT_NULL_PTR, "command", message);
	sqlite3_close(db);
	return 0;
}
