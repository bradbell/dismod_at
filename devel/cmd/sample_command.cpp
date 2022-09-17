// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <dismod_at/sample_command.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_prior_sim_table.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/get_var_limits.hpp>
# include <dismod_at/remove_const.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/get_str_map.hpp>


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-------------------------------------------------------------------------------
$begin sample_command$$
$spell
   hes
   sim
   avgint
   dismod
   var
   arg
$$

$section The Sample Command$$

$head Syntax$$
$codei%dismod_at %database% sample %method% %variables% %number_sample%
%$$
$codei%dismod_at %database% sample %method% %variables% %number_sample% %simulate_index%
%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head Purpose$$
This command simulates samples of the $cref model_variables$$
from the posterior distribution; see
$cref/simulation/posterior/Simulation/$$ for the posterior distribution.

$head method$$
The sample command argument $icode method$$ must be
$code simulate$$ or $code asymptotic$$; see discussion below:

$head variables$$
The sample command argument $icode variables$$ must be
$code fixed$$ or $code both$$.
This corresponds to the asymptotic statistics for
$cref/fit fixed/fit_command/variables/fixed/$$ and
$cref/fit both/fit_command/variables/both/$$ respectively.

$head number_sample$$
Is the number of samples. Each sample contains a complete
set of model variables.
If $icode method$$ is $code simulate$$,
$icode number_sample$$ must be equal to
$cref/number_simulate/simulate_command/number_simulate/$$
in the previous simulate command.

$head simulate_index$$
If this argument is present, $icode method$$ must be $code asymptotic$$
and $icode simulate_index$$ must be the same as in the corresponding
$cref/fit command/fit_command/simulate_index/$$.

$head simulate$$
A complete set of $cref model_variables$$,
corresponding to each $cref/sample_index/sample_table/sample_index/$$,
is written to the sample table.
They correspond to fitting the data in the data_sim table with
$cref/simulate_index/data_sim_table/simulate_index/$$ equal to
$icode sample_index$$.
The fixed effects correspond to the optimal fit of both the fixed and random
effects with the prior for the fixed effects replaced by the corresponding
values in the $cref prior_sim_table$$.
This value is used for the fixed effects and the value for the
random effects is obtained by optimizing just the random
effects with the prior for the random effects replaced by the corresponding
values in the $cref prior_sim_table$$.
This requires running $icode number_sample$$ fits of the model variables
(fitting just the random effects is faster compared to fitting both).
See $cref/simulation/posterior/Simulation/$$ in the discussion of the
posterior distribution of maximum likelihood estimates.

$head asymptotic$$
If $icode method$$ is $code asymptotic$$,
the $cref fit_var_table$$ is an additional input in this case
and it assumed to correspond to a
fit $cref/both/fit_command/variables/both/$$.
If the previous fit did (did not) have a
$cref/simulate_index/fit_command/simulate_index/$$ it
must (must not) be included in the sample_command.
The asymptotic statistics of the model variables is used to generate
$icode number_sample$$ samples of the model variables
The samples with different values of $icode sample_index$$ are independent.
All of the Laplace density terms are ignored by the asymptotic statistics.
The constraints are also ignored, except the constraints were
the lower and upper limits for a variable are equal.

$subhead Fixed Effects Distribution$$
The asymptotic distribution used to simulate the fixed effects is a normal
with mean equal to the value of the fixed effects in the $cref fit_var_table$$
and covariance equal to the inverse of the
Hessian of the fixed effect objective
$cref/hes_fixed_table/sample_command/Output Tables/hes_fixed_table/$$.
If a fixed effect is scaled
(see $cref/scaling fixed effects/prior_table/eta/Scaling Fixed Effects/$$)
the scaled version of the fixed effect has the asymptotic distribution.
If the lower and upper limits are equal, the corresponding variable
is simulated as having that constant value.

$subhead Random Effects Distribution$$
If the lower and upper limits for a random effect are equal,
the random effect is simulated as having that constant value.
If the lower and upper limits are not equal and
$icode variables$$ is $code fixed$$,
the random effect is simulated with value zero.
Otherwise,
the asymptotic distribution used to simulate a random effect is a normal
with mean equal to the value of the random effect in the $cref fit_var_table$$
This is the optimal value given the fixed effects; see
$cref/fit_var_table/sample_command/Extra Input Tables/fit_var_table/$$ below.
The covariance of the random effects is equal to the inverse of the
Hessian of the random effect objective
$cref/hes_fixed_table/sample_command/Output Tables/hes_fixed_table/$$.

$head Extra Input Tables$$

$subhead data_sim_table$$
If $icode method$$ is $code simulate$$,
this command has the extra input $cref data_sim_table$$
which was created by the previous $cref simulate_command$$.

$subhead prior_sim_table$$
If $icode method$$ is $code simulate$$,
this command has the extra input $cref prior_sim_table$$
which was created by the previous $cref simulate_command$$.

$subhead fit_var_table$$
If $icode method$$ is $code asymptotic$$,
this command has the extra input $cref fit_var_table$$
which was created by a previous fit command which
must have included $cref/both/fit_command/variables/both/$$
fixed and random effects.

$head Output Tables$$

$subhead sample_table$$
A new $cref sample_table$$ is created each time this command is run.
It contains samples of the model variables.
Hence the number of rows in this table is $icode number_sample$$
times the number of rows in the $cref var_table$$.
If the $code asymptotic$$ command fails because the
fixed effects information matrix is not positive definite,
this command will terminate with an error and the sample table will not exist.
The corresponding fixed effects information matrix will be in the
$cref/hes_fixed_table/sample_command/Output Tables/hes_fixed_table/$$.

$subhead No Sample Table$$
In the special case where $icode method$$ is $code asymptotic$$
and the Hessian of the fixed objective is not positive definite,
the sample table is not created; i.e.,
there is be no sample table in the database after this command.
In addition, if $icode variables$$ is $code both$$ and the Hessian
of the random effects objective is not positive definite,
the sample table is not created.

$subhead hes_fixed_table$$
A new $cref hes_fixed_table$$ is created each time this command is run
with $icode method$$ equal to $code asymptotic$$.
The Hessian of the fixed effects objective is written in this table.
If $icode simulate_index$$ is present (is not present) the Hessian corresponds
to the simulated measurements in the $cref data_sim_table$$
(measurements in the $cref data_table$$).

$subhead hes_random_table$$
A new $cref hes_random_table$$ is created each time this command is run
with $icode method$$ equal to $code asymptotic$$ and
$icode variables$$ equal to $code both$$.
The Hessian of the random effects objective is written in this table.
If $icode simulate_index$$ is present (is not present) the Hessian corresponds
to the simulated measurements in the $cref data_sim_table$$
(measurements in the $cref data_table$$).

$head Bounds$$
If you use the $code simulate$$ method,
the samples are all within the specified bounds, including the bounds
on the random effects specified by
$cref/bound_random/option_table/Optimize Random Only/bound_random/$$.
If you use the $code asymptotic$$ method,
the only bounds that are enforced are where the upper and lower limits
are equal.

$children%example/get_started/sample_command.py
%$$
$head Example$$
The files
$cref sample_command.py$$, $cref user_sample_asy.py$$
contain examples and tests using this command.

$end
*/
// ----------------------------------------------------------------------------
void sample_command(
   const std::string&                                 method           ,
   const std::string&                                 variables        ,
   const std::string&                                 number_sample    ,
   const std::string&                                 simulate_index   ,
   sqlite3*                                           db               ,
   CppAD::vector<dismod_at::subset_data_struct>&      subset_data_obj  ,
   dismod_at::data_model&                             data_object      ,
   dismod_at::prior_model&                            prior_object     ,
   const CppAD::vector<dismod_at::prior_struct>&      prior_table      ,
   const dismod_at::pack_info&                        pack_object      ,
   const dismod_at::pack_prior&                       var2prior        ,
   const dismod_at::db_input_struct&                  db_input         ,
   // effectively const
   const std::map<std::string, std::string>&          option_map
)
{  using std::string;
   using CppAD::to_string;
   using CppAD::vector;
   string msg;
   // -------------------------------------------------------------------
   if( method != "simulate" && method != "asymptotic" )
   {  msg  = "dismod_at sample command method = ";
      msg += method + " is not one of the following: ";
      msg += "simulate, asymptotic";
      dismod_at::error_exit(msg);
   }
   if( variables != "fixed" && variables != "both" )
   {  msg  = "dismod_at sample command variables = ";
      msg += variables + " is not one of the following: ";
      msg += "fixed, both";
      dismod_at::error_exit(msg);
   }
   int tmp = std::atoi( number_sample.c_str() );
   if( tmp <= 0 )
   {  msg  = "dismod_at sample command number_sample = ";
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
   // zero_sum_child_rate
   size_t n_rate      = size_t(dismod_at::number_rate_enum);
   size_t option_size = get_str_map(option_map, "zero_sum_child_rate").size();
   vector<bool> zero_sum_child_rate(n_rate);
   for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
   {  string rate_name = dismod_at::get_rate_name(rate_id);
      size_t found     =
         get_str_map(option_map, "zero_sum_child_rate").find( rate_name );
      zero_sum_child_rate[rate_id] = found < option_size;
   }
   // ----------------------------------------------------------------------
   // zero_sum_mulcov_group
   size_t n_group = pack_object.group_size();
   option_size    = get_str_map(option_map, "zero_sum_mulcov_group").size();
   vector<bool> zero_sum_mulcov_group(n_group);
   for(size_t group_id = 0; group_id < n_group; group_id++)
   {  const vector<subgroup_struct>&
         subgroup_table( db_input.subgroup_table );
      size_t first_subgroup_id = pack_object.first_subgroup_id(group_id);
      string group_name = subgroup_table[first_subgroup_id].group_name;
      size_t found =
         get_str_map(option_map, "zero_sum_mulcov_group").find(group_name);
      zero_sum_mulcov_group[group_id] = found < option_size;
   }
   // ----------------------------------------------------------------------
   // quasi_fixed
   bool quasi_fixed = get_str_map(option_map, "quasi_fixed") == "true";
   //
   // trace_init
   bool trace_init =
      get_str_map(option_map, "trace_init_fit_model") == "true";
   //
   //
   // warn_on_stderr
   bool warn_on_stderr = get_str_map(option_map, "warn_on_stderr") == "true";
   //
   // bound_random, null corresponds to infinity
   double bound_random = 0.0;
   if( variables != "fixed" )
   {  // null corresponds to infinity
      std::string tmp_str = get_str_map(option_map, "bound_random");
      if( tmp_str == "" )
         bound_random = std::numeric_limits<double>::infinity();
      else
         bound_random = std::atof( tmp_str.c_str() );
   }
   //
   // random_const
   size_t n_random = pack_object.random_size();
   CppAD::mixed::d_vector var_lower(n_var), var_upper(n_var);
   get_var_limits(
      var_lower, var_upper, var2prior, db_input.prior_table
   );
   CppAD::mixed::d_vector random_lower(n_random);
   CppAD::mixed::d_vector random_upper(n_random);
   unpack_random(pack_object, var_lower, random_lower);
   unpack_random(pack_object, var_upper, random_upper);
   remove_const random_const(random_lower, random_upper);
   //
   // data_sim_table, n_subset
   vector<data_sim_struct> data_sim_table;
   size_t n_subset = data_sim_table.size();
   assert( n_subset == 0 );
   if( method == "simulate" || simulate_index != "" )
   {  data_sim_table = get_data_sim_table(db);
      // n_subset
      n_subset = subset_data_obj.size();
      //
      // check n_sample * n_subset == data_sim_table.size()
      if( n_subset == 0 && data_sim_table.size() != 0  )
      {  msg  = "dismod_at sample command method = simulate and ";
         msg += "data_subset table size is zero and ";
         msg += "data_sim table size is not zero.";
         dismod_at::error_exit(msg);
      }
      if( n_subset != 0 && data_sim_table.size() % n_subset != 0  )
      {  msg  = "dismod_at sample command method = simulate and ";
         msg += "data_sim table size modulo data_subset table size not 0.";
         dismod_at::error_exit(msg);
      }
   }
   //
   // -----------------------------------------------------------------------
   if( method == "simulate" )
   {  // check n_sample
      if( n_subset != 0 && n_sample != data_sim_table.size() / n_subset )
      {  msg  = "dismod_at sample command method = simulate and ";
         msg += "data_sim table size not equal number_sample times ";
         msg += "data_subset table size.";
         dismod_at::error_exit(msg);
      }
      //
      // start_var
      vector<double> start_var_value;
      string table_name  = "start_var";
      string column_name = "start_var_value";
      dismod_at::get_table_column(
         db, table_name, column_name, start_var_value
      );
      // scale_var
      vector<double> scale_var_value;
      table_name  = "scale_var";
      column_name = "scale_var_value";
      dismod_at::get_table_column(
         db, table_name, column_name, scale_var_value
      );
      // prior_sim_table
      vector<prior_sim_struct> prior_sim_table = get_prior_sim_table(db);
      //
      // n_child
      size_t n_child = pack_object.child_size();
      //
      // is_random_effect
      vector<bool> is_random_effect(n_var);
      for(size_t var_id = 0; var_id < n_var; ++var_id)
         is_random_effect[var_id] = false;
      for(size_t rate_id = 0; rate_id < n_rate; ++rate_id)
      for(size_t child_id = 0; child_id < n_child; ++child_id)
      {  // packing information for this rate, child
         dismod_at::pack_info::subvec_info info =
            pack_object.node_rate_value_info(rate_id, child_id);
         if( info.smooth_id != DISMOD_AT_NULL_SIZE_T )
         {  size_t offset = info.offset;
            size_t n_grid = info.n_var;
            // grid points for this rate, child
            for(size_t i_grid = 0; i_grid < n_grid; ++i_grid)
            {  size_t var_id = offset + i_grid;
               is_random_effect[var_id] = true;
            }
         }
      }
      // Vector used to replacement of the prior means:
      // wor each variable it has a mean for value, dage and  dtime.
      vector<double> prior_mean(n_var * 3);
      //
      // for each simulated data set
      for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
      {  // --------------------------------------------------------------
         // estimate fixed effects for this sample_index
         // --------------------------------------------------------------
         //
         // replace data_sim_value in subset_data_obj
         size_t offset = n_subset * sample_index;
         for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
         {  size_t data_sim_id = offset + subset_id;
            size_t sample_check =
               size_t(data_sim_table[data_sim_id].simulate_index);
            size_t subset_check =
               size_t(data_sim_table[data_sim_id].data_subset_id);
            if( sample_check != sample_index || subset_check != subset_id )
            {  msg  = "dismod_at database sample simulate\n";
               msg += "size of data_sim table does not make sense\n";
               msg +=  "restart with init command";
               table_name = "data_sim";
               dismod_at::error_exit(msg, table_name, data_sim_id);
            }
# ifndef NDEBUG
            double old_value = subset_data_obj[subset_id].data_sim_value;
# endif
            double new_value =data_sim_table[data_sim_id].data_sim_value;
            assert(   std::isnan(old_value) || sample_index > 0 );
            assert( ! std::isnan(new_value) );
            subset_data_obj[subset_id].data_sim_value = new_value;
         }
         // replace_like
         data_object.replace_like(subset_data_obj);
         //
         // replace prior means for fixed effects
         for(size_t var_id = 0; var_id < n_var; ++var_id)
         if( ! is_random_effect[var_id] )
         {  // This is a fixed effect so use prior_sim table means
            size_t prior_sim_id = sample_index * n_var + var_id;
            // value
            prior_mean[var_id * 3 + 0] =
               prior_sim_table[prior_sim_id].prior_sim_value;
            // dage
            prior_mean[var_id * 3 + 1] =
               prior_sim_table[prior_sim_id].prior_sim_dage;
            // dtime
            prior_mean[var_id * 3 + 2] =
               prior_sim_table[prior_sim_id].prior_sim_dtime;
         }
         else
         {  // This is a random effect so use the prior table means
            //
            // value
            double const_value = var2prior.const_value(var_id);
            size_t prior_id    = var2prior.value_prior_id(var_id);
            if( ! CppAD::isnan( const_value ) )
            {  assert( prior_id == DISMOD_AT_NULL_SIZE_T );
               prior_mean[var_id * 3 + 0] = const_value;
            }
            else
            {  assert( prior_id != DISMOD_AT_NULL_SIZE_T );
               prior_mean[var_id * 3 + 0] = prior_table[prior_id].mean;
            }
            // dage
            prior_id = var2prior.dage_prior_id(var_id);
            if( prior_id != DISMOD_AT_NULL_SIZE_T )
               prior_mean[var_id * 3 + 1] = prior_table[prior_id].mean;
            else
            {  // default is uniform on [-inf, +inf]
               prior_mean[var_id * 3 + 1] = 0.0;
            }
            // dtime
            prior_id = var2prior.dtime_prior_id(var_id);
            if( prior_id != DISMOD_AT_NULL_SIZE_T )
               prior_mean[var_id * 3 + 2] = prior_table[prior_id].mean;
            else
            {  // default is uniform on [-inf, +inf]
               prior_mean[var_id * 3 + 2] = 0.0;
            }
         }
         prior_object.replace_mean(prior_mean);
         //
         // fit both fixed and random effects
         bool random_only   = false;
         int  sim_index_int = int(sample_index);
         dismod_at::fit_model fit_object_both(
            db                   ,
            sim_index_int        ,
            warn_on_stderr       ,
            bound_random         ,
            pack_object          ,
            var2prior            ,
            start_var_value      ,
            scale_var_value      ,
            db_input.prior_table ,
            prior_object         ,
            random_const         ,
            quasi_fixed          ,
            zero_sum_child_rate  ,
            zero_sum_mulcov_group,
            data_object          ,
            trace_init
         );
         // input empty warm_start information
         CppAD::mixed::warm_start_struct warm_start_1;
         fit_object_both.run_fit(random_only, option_map, warm_start_1);
         //
         // ignore resulting warm_start information
         vector<double> opt_value, lag_value, lag_dage, lag_dtime;
         vector<CppAD::mixed::trace_struct> trace_vec;
         fit_object_both.get_solution(
         opt_value, lag_value, lag_dage, lag_dtime, trace_vec, warm_start_1
         );
         assert( opt_value.size() == n_var );
         //
         // solution for fixed effects and this sample_index -> row_value
         string sample_index_str = to_string( sample_index );
         for(size_t var_id = 0; var_id < n_var; var_id++)
         if( ! is_random_effect[var_id] )
         {  size_t sample_id = sample_index * n_var + var_id;
            row_value[n_col * sample_id + 0] = sample_index_str;
            row_value[n_col * sample_id + 1] = to_string( var_id );
            row_value[n_col * sample_id + 2] =
               to_string(opt_value[var_id] );
         }
         // --------------------------------------------------------------
         // estimate random effects for this sample_index
         // --------------------------------------------------------------
         //
         // Replace prior means for random effects. Prior means for
         // fixed effects do not matter when only fitting random effects.
         for(size_t var_id = 0; var_id < n_var; ++var_id)
         if( is_random_effect[var_id] )
         {  // This is a fixed effect so use prior_sim table means
            size_t prior_sim_id = sample_index * n_var + var_id;
            // value
            prior_mean[var_id * 3 + 0] =
               prior_sim_table[prior_sim_id].prior_sim_value;
            // dage
            prior_mean[var_id * 3 + 1] =
               prior_sim_table[prior_sim_id].prior_sim_dage;
            // dtime
            prior_mean[var_id * 3 + 2] =
               prior_sim_table[prior_sim_id].prior_sim_dtime;
         }
         prior_object.replace_mean(prior_mean);
         //
         // Only fit random effects.
         // 2DO: add replacement of prior_object and start_var to fit_model
         // and then use the same fit_model object for random effects.
         random_only = true;
         dismod_at::fit_model fit_object_random(
            db                   ,
            sim_index_int        ,
            warn_on_stderr       ,
            bound_random         ,
            pack_object          ,
            var2prior            ,
            opt_value            , // use optimal value for fixed effects
            scale_var_value      ,
            db_input.prior_table ,
            prior_object         ,
            random_const         ,
            quasi_fixed          ,
            zero_sum_child_rate  ,
            zero_sum_mulcov_group,
            data_object          ,
            trace_init
         );
         // empty warm_start information
         CppAD::mixed::warm_start_struct warm_start_2;
         fit_object_random.run_fit(random_only, option_map, warm_start_2);
         //
         // ignore resulting warm_start information
         fit_object_random.get_solution(
         opt_value, lag_value, lag_dage, lag_dtime, trace_vec, warm_start_2
         );
         //
         // solution for random effects and this sample_index -> row_value
         for(size_t var_id = 0; var_id < n_var; var_id++)
         if( is_random_effect[var_id] )
         {  size_t sample_id = sample_index * n_var + var_id;
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
   sql_cmd = "drop table if exists hes_fixed";
   dismod_at::exec_sql_cmd(db, sql_cmd);
   //
   sql_cmd = "drop table if exists hes_random";
   dismod_at::exec_sql_cmd(db, sql_cmd);
   //
   // simulation_index
   int sim_index_int = -1; // corresponds to fitting data table values
   if( simulate_index != "" )
   {  // corresponds to fitting simulated data table values
      sim_index_int           = std::atoi( simulate_index.c_str() );
      size_t sim_index_size_t = size_t( sim_index_int );
      //
      // number of simulated data sets
      assert( data_sim_table.size() % n_subset == 0 );
      size_t n_simulate = data_sim_table.size() / n_subset;
      if( sim_index_size_t >= n_simulate )
      {  msg  = "dismod_at sample command: simulate_index = ";
         msg += simulate_index + "\nis greater than or equal ";
         msg += "number of samples in data_sim_table.";
         string table_name = "data_sim";
         dismod_at::error_exit(msg, table_name);
      }
      //
      // replace meas_value in subset_data_obj
      for(size_t subset_id = 0; subset_id < n_subset; ++subset_id)
      {  size_t data_sim_id = n_subset * sim_index_size_t + subset_id;
# ifndef NDEBUG
         double old_value = subset_data_obj[subset_id].data_sim_value;
# endif
         double new_value =data_sim_table[data_sim_id].data_sim_value;
         assert(   std::isnan(old_value) );
         assert( ! std::isnan(new_value) );
         subset_data_obj[subset_id].data_sim_value = new_value;
      }
   }
   //
   // replace_like
   data_object.replace_like(subset_data_obj);
   //
   // fit_var_value
   vector<double> fit_var_value;
   string table_name  = "fit_var";
   string column_name = "fit_var_value";
   dismod_at::get_table_column(
      db, table_name, column_name, fit_var_value
   );
   //
   // fit_object
   dismod_at::fit_model fit_object(
      db                   ,
      sim_index_int        ,
      warn_on_stderr       ,
      bound_random         ,
      pack_object          ,
      var2prior            ,
      fit_var_value        ,
      fit_var_value        ,
      db_input.prior_table ,
      prior_object         ,
      random_const         ,
      quasi_fixed          ,
      zero_sum_child_rate  ,
      zero_sum_mulcov_group,
      data_object          ,
      trace_init
   );
   //
   // hes_fixed_obj_out, hes_random_obj_out, sample_out
   CppAD::mixed::d_sparse_rcv hes_fixed_obj_out, hes_random_obj_out;
   vector<double> sample_out;
   fit_object.sample_posterior(
      hes_fixed_obj_out    ,
      hes_random_obj_out   ,
      n_sample             ,
      sample_out           ,
      fit_var_value        ,
      option_map
   );
   // ----------------------------------------------------------------------
   // Create sample table first so we can use create_table settings above.
   // If sample_out.size() is zero, we will report the error at the end.
   if( sample_out.size() != 0 )
   {  assert( sample_out.size() == n_sample * n_var );
      for(size_t sample_index = 0; sample_index < n_sample; sample_index++)
      {  string sample_index_str = to_string( sample_index );
         for(size_t var_id = 0; var_id < n_var; var_id++)
         {  size_t sample_id = sample_index * n_var + var_id;
            row_value[n_col * sample_id + 0] = sample_index_str;
            row_value[n_col * sample_id + 1] = to_string( var_id );
            //
            double var_value = sample_out[ sample_index * n_var + var_id];
            row_value[n_col * sample_id + 2] = to_string(var_value);
         }
      }
      table_name = "sample";
      dismod_at::create_table(
         db, table_name, col_name, col_type, col_unique, row_value
      );
   }
   // ----------------------------------------------------------------------
   // create hes_fixed table
   n_col         = 3;
   n_row         = hes_fixed_obj_out.nnz();
   col_name.resize(n_col);
   col_type.resize(n_col);
   row_value.resize(n_col * n_row);
   //
   col_name[0]   = "row_var_id";
   col_type[0]   = "integer";
   col_unique[0] = false;
   //
   col_name[1]   = "col_var_id";
   col_type[1]   = "integer";
   col_unique[1] = false;
   //
   col_name[2]   = "hes_fixed_value";
   col_type[2]   = "integer";
   col_unique[2] = false;
   //
   CppAD::mixed::s_vector row_major = hes_fixed_obj_out.row_major();
   for(size_t k = 0; k < n_row; ++k)
   {  size_t ell             = row_major[k];
      size_t row_var_id      = hes_fixed_obj_out.row()[ell];
      size_t col_var_id      = hes_fixed_obj_out.col()[ell];
      double hes_fixed_value = hes_fixed_obj_out.val()[ell];
      row_value[n_col * k + 0] = to_string(row_var_id);
      row_value[n_col * k + 1] = to_string(col_var_id);
      row_value[n_col * k + 2] = to_string(hes_fixed_value);
   }
   table_name = "hes_fixed";
   dismod_at::create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   // ----------------------------------------------------------------------
   // create hes_random table
   n_col         = 3;
   n_row         = hes_random_obj_out.nnz();
   col_name.resize(n_col);
   col_type.resize(n_col);
   row_value.resize(n_col * n_row);
   //
   col_name[0]   = "row_var_id";
   col_type[0]   = "integer";
   col_unique[0] = false;
   //
   col_name[1]   = "col_var_id";
   col_type[1]   = "integer";
   col_unique[1] = false;
   //
   col_name[2]   = "hes_random_value";
   col_type[2]   = "integer";
   col_unique[2] = false;
   //
   // re-size to zero to avoid error on assignment
   row_major.resize(0);
   row_major = hes_random_obj_out.row_major();
   for(size_t k = 0; k < n_row; ++k)
   {  size_t ell              = row_major[k];
      size_t row_var_id       = hes_random_obj_out.row()[ell];
      size_t col_var_id       = hes_random_obj_out.col()[ell];
      double hes_random_value = hes_random_obj_out.val()[ell];
      row_value[n_col * k + 0] = to_string(row_var_id);
      row_value[n_col * k + 1] = to_string(col_var_id);
      row_value[n_col * k + 2] = to_string(hes_random_value);
   }
   table_name = "hes_random";
   dismod_at::create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   // ----------------------------------------------------------------------
   if( sample_out.size() == 0 )
   {  msg = "sample_command: sample table was not created";
      dismod_at::error_exit(msg);
   }
   return;
}
} // END_DISMOD_AT_NAMESPACE
