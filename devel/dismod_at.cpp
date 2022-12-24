// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <cppad/mixed/exception.hpp>
# include <dismod_at/depend_command.hpp>
# include <dismod_at/fit_command.hpp>
# include <dismod_at/init_command.hpp>
# include <dismod_at/old2new_command.hpp>
# include <dismod_at/predict_command.hpp>
# include <dismod_at/sample_command.hpp>
# include <dismod_at/set_command.hpp>
# include <dismod_at/simulate_command.hpp>
# include <dismod_at/hold_out_command.hpp>
# include <dismod_at/bnd_mulcov_command.hpp>
# include <dismod_at/data_density_command.hpp>

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
# include <dismod_at/get_option_table.hpp>
# include <dismod_at/get_sample_table.hpp>
# include <dismod_at/get_data_sim_table.hpp>
# include <dismod_at/get_data_subset.hpp>
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
# include <dismod_at/child_data_in_fit.hpp>
# include <dismod_at/get_bnd_mulcov_table.hpp>
# include <dismod_at/map_node_cov.hpp>

# define DISMOD_AT_TRACE 0


int main(int n_arg, const char** argv)
{  // ---------------- using statements ----------------------------------
   using std::cerr;
   using std::endl;
   using std::string;
   using CppAD::vector;
   // ---------------- command line arguments ---------------------------
   // command_info
   // BEGIN_SORT_THIS_LINE_PLUS_2
   struct { const char* name; int n_arg; } command_info[] = {
      {"bnd_mulcov",   4},
      {"bnd_mulcov",   5},
      {"data_density", 3},
      {"data_density", 7},
      {"depend",       3},
      {"fit",          4},
      {"fit",          5},
      {"fit",          6},
      {"hold_out",     5},
      {"hold_out",     8},
      {"init",         3},
      {"old2new",      3},
      {"predict",      4},
      {"sample",       6},
      {"sample",       7},
      {"set",          5},
      {"set",          6},
      {"simulate",     4}
   };
   // END_SORT_THIS_LINE_MINUS_2
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
   {  cerr << program << endl
      << "usage:    dismod_at database command [arguments]\n"
      << "database: sqlite database\n"
      << "command:  " << command_info[0].name;
      size_t column = 10 + std::strlen( command_info[0].name );
      for(size_t i = 1; i < n_command; i++)
      {  string name = command_info[i].name;
         if( name != command_info[i-1].name )
         {  column += 2 + name.size();
            if( column < 80 )
               cerr << ", ";
            else
            {  cerr << "\n          ";
               column = 10 + name.size();
            }
            cerr << name;
         }
      }
      cerr << "\n"
      << "arguments: optional arguments depending on particular command\n";
      std::exit(1);
   }
   // check if comamnd matches one of the cases in command_info
   const string database_arg  = argv[1];
   const string command_arg   = argv[2];
   vector<size_t> command_match;
   bool match = false;
   for(size_t i = 0; i < n_command; i++)
   {  if( command_arg == command_info[i].name )
      {  command_match.push_back( command_info[i].n_arg );
         match |= n_arg == command_info[i].n_arg;
      }
   }
   if( command_match.size() == 0 )
   {  // commands that no longer exist
      if( command_arg == "start" )
      {  cerr <<
         "dismod_at database start source\n"
         "\thas been changed to\n"
         "dismod_at database set start_var source\n"
         "Furthermore, the init command now creates a start_var table\n";
         std::exit(1);
      }
      if( command_arg == "truth" )
      {  cerr <<
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
   {  cerr << program << endl << command_arg << " command expected "
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
   {  message += " ";
      message += argv[i_arg];
   }
   std::time_t unix_time =
      dismod_at::log_message(db, DISMOD_AT_NULL_PTR, "command", message);
   // ----------------------------------------------------------------------
   // old2new command must fix database before get_db_input can be run
   if( command_arg == "old2new" )
   {  dismod_at::old2new_command(db);
      message = "end " + command_arg;
      dismod_at::log_message(db, DISMOD_AT_NULL_PTR, "command", message);
      sqlite3_close(db);
      return 0;
   }
   // ----------------------------------------------------------------------
   // The "set option" comands must be done before get_db_input can be run
   // because an option might affect if input is correct; e.g., rate_case
   if( command_arg == "set" && strcmp(argv[3], "option") == 0 )
   {  if( n_arg != 6 )
      {  cerr << "expected name and value to follow "
         "dismod_at database set option\n";
         std::exit(1);
      }
      CppAD::vector<dismod_at::option_struct> option_table =
         dismod_at::get_option_table(db);
      std::string name  = argv[4];
      std::string value = argv[5];
      dismod_at::set_option_command(db, option_table, name, value);
      //
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
   {  string name  = db_input.option_table[id].option_name;
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
   // ------------------------------------------------------------------------
   // check for init_command output tables
   const char* init_table_name[] = {
      "var", "data_subset", "start_var", "scale_var", "bnd_mulcov"
   };
   size_t n_init_table = sizeof(init_table_name) / sizeof(init_table_name[0]);
   if( command_arg != "init" ) for(size_t i = 0; i < n_init_table; ++i)
   {  string sql_cmd = "select count(*) from sqlite_master ";
      sql_cmd       += "where type='table' and name='";
      sql_cmd       += init_table_name[i];
      sql_cmd       += "';";
      char sep       = ',';
      string result  = dismod_at::exec_sql_cmd(db, sql_cmd, sep);
      assert( result == "0\n" || result == "1\n" );
      if( result == "0\n" && command_arg != "init" && command_arg != "set" )
      {  message = init_table_name[i];
         message += " table is missing and this is not init or set command";
         dismod_at::error_exit(message);
      }
   }
   // ---------------------------------------------------------------------
   // n_covariate
   size_t n_covariate = db_input.covariate_table.size();
   //
   // n_node
   size_t n_node = db_input.node_table.size();
   // ---------------------------------------------------------------------
   // parent_node_id
   size_t parent_node_id   = db_input.node_table.size();
   string parent_node_name = option_map["parent_node_name"];
   string table_name       = "option";
   if( option_map["parent_node_id"] != "" )
   {  parent_node_id   = std::atoi( option_map["parent_node_id"].c_str() );
      if( parent_node_name != "" )
      {  string node_name = db_input.node_table[parent_node_id].node_name;
         if( parent_node_name != node_name )
         {  message = "parent_node_id and parent_node_name"
            " specify different nodes";
            dismod_at::error_exit(message, table_name);
         }
      }
   }
   else if( parent_node_name != "" )
   {  for(size_t node_id = 0; node_id < n_node; node_id++)
      {  if( db_input.node_table[node_id].node_name == parent_node_name )
            parent_node_id = node_id;
      }
      if( parent_node_id == n_node )
      {  message = "cannot find parent_node_name in node table";
         dismod_at::error_exit(message, table_name);
      }
   }
   else
   {  message = "neither parent_node_id nor parent_node_name is present";
      dismod_at::error_exit(message, table_name);
   }
   assert( parent_node_id < db_input.node_table.size() );
   // -----------------------------------------------------------------------
   // bound_random
   double bound_random = 0.0;
   bool only_fixed =
      command_arg == "fit" && std::strcmp(argv[3], "fixed") == 0;
   only_fixed  |=
      command_arg == "sample" && std::strcmp(argv[4], "fixed") == 0;
   if( ! only_fixed  )
   {  // null corresponds to infinity
      std::string tmp_str = option_map["bound_random"];
      if( tmp_str == "" )
         bound_random = std::numeric_limits<double>::infinity();
      else
         bound_random = std::atof( tmp_str.c_str() );
   }
   // ------------------------------------------------------------------------
   // child_info4data
   dismod_at::child_info child_info4data(
      parent_node_id          ,
      db_input.node_table     ,
      db_input.data_table
   );
   // child_info4avgint
   dismod_at::child_info child_info4avgint(
      parent_node_id          ,
      db_input.node_table     ,
      db_input.avgint_table
   );
   // n_child, n_integrand, n_weight, n_smooth
   size_t n_child     = child_info4data.child_size();
   size_t n_integrand = db_input.integrand_table.size();
   size_t n_weight    = db_input.weight_table.size();
   size_t n_smooth    = db_input.smooth_table.size();
   // ---------------------------------------------------------------------
   // w_info_vec
   vector<dismod_at::weight_info> w_info_vec(n_weight + 1);
   for(size_t weight_id = 0; weight_id < n_weight; weight_id++)
   {  w_info_vec[weight_id] = dismod_at::weight_info(
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
   {  s_info_vec[smooth_id] = dismod_at::smooth_info(
         smooth_id                  ,
         db_input.age_table         ,
         db_input.time_table        ,
         db_input.prior_table       ,
         db_input.smooth_table      ,
         db_input.smooth_grid_table
      );
   }
   // child_id2node_id
   vector<size_t> child_id2node_id(n_child);
   for(size_t child_id = 0; child_id < n_child; child_id++)
   {  size_t node_id = child_info4data.child_id2node_id(child_id);
      assert( node_id == child_info4avgint.child_id2node_id(child_id) );
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
   // prior_mean
   vector<double> prior_mean;
   {  vector<size_t> one(n_child);
      for(size_t child = 0; child < n_child; ++child)
         one[child] = 1;
      dismod_at::pack_prior var2prior_temp(
         bound_random,
         one,
         db_input.prior_table,
         pack_object,
         s_info_vec
      );
      prior_mean  = get_prior_mean(
         db_input.prior_table, var2prior_temp
      );
   }
   //
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
   {  // do not execute this during a set command because it might
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
   // fit_simulated_data
   bool fit_simulated_data = false;
   if( command_arg == "fit" )
   {  if( n_arg == 5 )
         fit_simulated_data = string(argv[4]) != "warm_start";
      if( n_arg == 6 )
         fit_simulated_data = true;
   }
   if( command_arg == "sample" )
   {  if( std::strcmp(argv[3], "simulate") == 0 )
         fit_simulated_data = true;
      if( std::strcmp(argv[3], "asymptotic") == 0 && n_arg == 7 )
         fit_simulated_data = true;
   }
   // node_cov_map
   vector< vector<size_t> > node_cov_map = dismod_at::map_node_cov(
      db_input.node_cov_table, n_covariate, n_node
   );
   // =======================================================================
# ifdef NDEBUG
   try { // BEGIN_TRY_BLOCK (when not debugging)
# endif
   // =======================================================================
   if( command_arg == "set" )
   {  // The set option commands should have been completed before
      // calling get_db_input.
      assert( std::strcmp(argv[3], "option") != 0 );
      //
      if( std::strcmp(argv[3], "avgint") == 0 )
      {  if( n_arg != 5 )
         {  cerr << "expected data to follow "
            "dismod_at database set avgint\n";
            std::exit(1);
         }
         dismod_at::set_avgint_command(db);
      }
      else
      {  std::string table_out     = argv[3];
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
   else if( command_arg == "init" )
   {  dismod_at::init_command(
         db,
         prior_mean,
         pack_object,
         db_input,
         parent_node_id,
         child_info4data,     // could also use child_info4avgint
         s_info_vec
      );
   }
   else if( command_arg == "hold_out" )
   {  string integrand_name  = argv[3];
      string max_fit_str     = argv[4];
      string cov_name        = "";
      string cov_value_1_str = "";
      string cov_value_2_str = "";
      if( n_arg == 8 )
      {  cov_name        = argv[5];
         cov_value_1_str = argv[6];
         cov_value_2_str = argv[7];
      }
      dismod_at::hold_out_command(
         db,
         integrand_name,
         max_fit_str,
         cov_name,
         cov_value_1_str,
         cov_value_2_str,
         child_info4data,
         db_input.integrand_table,
         db_input.covariate_table,
         db_input.data_table,
         db_input.data_cov_value
      );
   }
   else if( command_arg == "bnd_mulcov" )
   {  string max_abs_effect = argv[3];
      string covariate_name = "";
      if( n_arg == 5 )
         covariate_name = argv[4];
      vector<dismod_at::data_subset_struct> data_subset_table =
         dismod_at::get_data_subset(db);
      dismod_at::bnd_mulcov_command(
         db,
         max_abs_effect,
         covariate_name,
         db_input.covariate_table,
         db_input.mulcov_table
      );
   }
   else if( command_arg == "data_density" )
   {  string integrand_name  = "";
      string density_name    = "";
      string eta_str         = "";
      string nu_str          = "";
      if( n_arg == 7 )
      {  integrand_name = argv[3];
         density_name   = argv[4];
         eta_str        = argv[5];
         nu_str         = argv[6];
      }
      dismod_at::data_density_command(
         db,
         integrand_name,
         density_name,
         eta_str,
         nu_str,
         db_input.integrand_table,
         db_input.density_table,
         db_input.data_table
      );
   }
   else if( command_arg == "predict" )
   {  // var2prior
      vector<dismod_at::data_subset_struct> data_subset_table =
         dismod_at::get_data_subset(db);
      vector<size_t> n_child_data_in_fit = child_data_in_fit(
         option_map,
         data_subset_table,
         db_input.integrand_table,
         db_input.data_table,
         child_info4data
      );
      dismod_at::pack_prior var2prior(
         bound_random,
         n_child_data_in_fit,
         db_input.prior_table,
         pack_object,
         s_info_vec
      );
      // avgint_subset_obj
      vector<dismod_at::avgint_subset_struct> avgint_subset_obj;
      vector<double> avgint_subset_cov_value;
      avgint_subset(
            db_input.integrand_table,
            db_input.avgint_table,
            db_input.avgint_cov_value,
            db_input.covariate_table,
            child_info4avgint,
            avgint_subset_obj,
            avgint_subset_cov_value
      );
      //
      // avgint_object
      dismod_at::data_model avgint_object(
         node_cov_map             ,
         n_covariate              ,
         n_node                   ,
         fit_simulated_data       ,
         meas_noise_effect        ,
         rate_case                ,
         bound_random             ,
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
         child_info4avgint
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
   {  // -------------------------------------------------------------------
      // data_subset_table
      vector<dismod_at::data_subset_struct> data_subset_table =
         dismod_at::get_data_subset(db);
      //
      // var2pior
      vector<size_t> n_child_data_in_fit = child_data_in_fit(
         option_map,
         data_subset_table,
         db_input.integrand_table,
         db_input.data_table,
         child_info4data
      );
      dismod_at::pack_prior var2prior(
         bound_random,
         n_child_data_in_fit,
         db_input.prior_table,
         pack_object,
         s_info_vec
      );
      vector<dismod_at::bnd_mulcov_struct> bnd_mulcov_table =
         dismod_at::get_bnd_mulcov_table(db);
      var2prior.set_bnd_mulcov(bnd_mulcov_table);
      //
      // subset_data_obj
      vector<dismod_at::subset_data_struct> subset_data_obj;
      vector<double> subset_data_cov_value;
      subset_data(
         option_map,
         data_subset_table,
         db_input.integrand_table,
         db_input.density_table,
         db_input.data_table,
         db_input.data_cov_value,
         db_input.covariate_table,
         child_info4data,
         subset_data_obj,
         subset_data_cov_value
      );
      // prior_object
      dismod_at::prior_model prior_object(
         pack_object           ,
         var2prior             ,
         db_input.age_table    ,
         db_input.time_table   ,
         db_input.prior_table  ,
         db_input.density_table
      );
      // data_object
      dismod_at::data_model data_object(
         node_cov_map             ,
         n_covariate              ,
         n_node                   ,
         fit_simulated_data       ,
         meas_noise_effect        ,
         rate_case                ,
         bound_random             ,
         ode_step_size            ,
         age_avg_grid             ,
         db_input.age_table       ,
         db_input.time_table      ,
         db_input.subgroup_table  ,
         db_input.integrand_table ,
         db_input.mulcov_table    ,
         db_input.prior_table     ,
         subset_data_obj          ,
         subset_data_cov_value    ,
         w_info_vec               ,
         s_info_vec               ,
         pack_object              ,
         child_info4data
      );
      //
      if( command_arg == "depend" )
      {  depend_command(
            db               ,
            prior_mean       ,
            data_object      ,
            subset_data_obj  ,
            prior_object
         );
      }
      else if( command_arg == "fit" )
      {  string variables      = argv[3];
         string simulate_index = "";
         bool   use_warm_start = false;
         if( n_arg == 5 )
         {  if( string( argv[4] ) == "warm_start" )
               use_warm_start = true;
            else
               simulate_index = argv[4];
         }
         if( n_arg == 6 )
         {  simulate_index = argv[4];
            use_warm_start = string( argv[5] ) == "warm_start";
            if( ! use_warm_start )
            {  message = "dismod_at fit command syntax error";
               dismod_at::error_exit(message);
            }
         }
         fit_command(
            use_warm_start   ,
            variables        ,
            simulate_index   ,
            db               ,
            subset_data_obj  ,
            data_object      , // not  const
            prior_object     , // not  const
            pack_object      ,
            var2prior        ,
            db_input         ,
            option_map
         );
      }
      else if( command_arg == "simulate" )
      {  // replace_like
         data_object.replace_like(subset_data_obj );
         simulate_command(
            argv[3]                  , // number_simulate
            meas_noise_effect        ,
            db                       ,
            subset_data_obj          ,
            data_object              ,
            var2prior                ,
            pack_object              ,
            db_input                 ,
            option_map
         );
      }
      else if( command_arg == "sample" )
      {  string method         = argv[3];
         string variables      = argv[4];
         string number_sample  = argv[5];
         string simulate_index = "";
         if( n_arg == 7 )
            simulate_index = argv[6];
         sample_command(
            method               , // const
            variables            , // ..
            number_sample        , // ..
            simulate_index       , // ..
            db                   , // not const
            subset_data_obj      , // ...
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
   // =======================================================================
# ifdef NDEBUG
   } // END_TRY_BLOCK (when not debugging)
   catch(const std::exception& e)
   {  message = "dismod_at ";
      message += database_arg + " " + command_arg + "\nstd::excpetion: ";
      message += e.what();
      dismod_at::error_exit(message);
   }
   catch(const CppAD::mixed::exception& e)
   {  string catcher("dismod_at");
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
