// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <dismod_at/fit_command.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/get_prior_sim_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/get_var_limits.hpp>
# include <dismod_at/remove_const.hpp>
# include <dismod_at/blob_table.hpp>
# include <dismod_at/pack_warm_start.hpp>
# include <dismod_at/get_str_map.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
{xrst_begin fit_command}
{xrst_spell
   apposed
   iter
}

The Fit Command
###############

Syntax
******

| ``dismod_at`` *database* ``fit`` *variables*
| ``dismod_at`` *database* ``fit`` *variables* *simulate_index*
| ``dismod_at`` *database* ``fit`` *variables* ``warm_start``
| ``dismod_at`` *database* ``fit`` *variables* *simulate_index* ``warm_start``

database
********
Is an
http://www.sqlite.org/sqlite/ database containing the
``dismod_at`` :ref:`input-name` tables which are not modified.

variables
*********
This argument is ``fixed`` , ``random`` or ``both`` .
If it is ``fixed`` (``random`` )
only the fixed effects (random effects) are optimized.
If it is ``both`` , both the fixed and random effects
are optimized.

fixed
=====
This option optimizes the fixed effects with the
random effects corresponding to
:ref:`constant child value priors<rate_table@Child Value Priors@Constant>`
at their constrained values and the other random effects
constrained to be zero.
(This is equivalent to fitting with
:ref:`option_table@Optimize Random Only@bound_random` equal to zero.)
This is useful when one uses fitting with no random effects as
a starting point for fitting with random effects; see
:ref:`set_command-name` ,
:ref:`start_var_table-name` , and
:ref:`fit_fixed_both.py<user_fit_fixed_both.py-name>` .
This enables one to see the different between the two
fits in the :ref:`log_table-name`
(as apposed to changing
:ref:`option_table@Optimize Random Only@bound_random`
in the ``option`` table).

random
======
This optimizes the random effects with
the fixed effects set to their starting values; see
:ref:`start_var_table-name` .

both
====
This option fits both the
:ref:`fixed<model_variables@Fixed Effects, theta>` and
:ref:`random<model_variables@Random Effects, u>` effects.

simulate_index
**************
If *simulate_index* is present, it must be less than
:ref:`simulate_command@number_simulate` .

data_sim_table
==============
If *simulate_index* is present, this is an extra input table.
The :ref:`data_sim_table@data_sim_value` entries,
corresponding to *simulate_index* ,
are used in place of the data table
:ref:`data_table@meas_value` entries.
All the rest of the data table values
are the same as when *simulated_index* is not present; e.g.,
:ref:`data_table@meas_std` comes from the data table.

prior_sim_table
===============
If *simulate_index* is present, this is an extra input table.
The :ref:`prior_sim_table@prior_sim_value` entries,
corresponding to *simulate_index* ,
are used in place of the prior table
:ref:`prior_table@mean` entries.
All the rest of the prior table values
are the same as when *simulated_index* is not present; e.g.,
:ref:`prior_table@std` comes from the prior table.

warm_start
**********
If ``warm_start`` is at the end of the command, the
:ref:`fit_command@Output Tables@ipopt_info_table`
written by the previous fit,
is used to start the optimization of the fixed effects
where the previous fit left off.
This is intended to be used in the following cases:

#. Termination of previous fit is due to reaching
   :ref:`max_num_iter_fixed<option_table@Optimize Fixed and Random@max_num_iter>` .
#. The
   :ref:`option_table@Optimize Fixed and Random@tolerance` for the
   fixed or random effects been changed.

Other options besides those listed above,
should be the same as for the previous fit.

data_subset_table
*****************
Only the data table rows with :ref:`data_table@data_id`
that also appear in the :ref:`data_subset table<data_subset_table@data_id>`
are included in the fit and residuals.

hold_out
********
A data table :ref:`data_table@meas_value`
is held out from the fit, but included in the residuals,
if any of the following conditions hold:

#. The data table :ref:`data_table@hold_out` is non-zero.
#. The corresponding data_subset table
   :ref:`data_subset_table@hold_out` is non-zero;
   see :ref:`hold_out_command-name` .
#. The data table :ref:`integrand<data_table@integrand_id>`
   corresponds to an :ref:`integrand_table@integrand_name`
   that is in the option table
   :ref:`option_table@hold_out_integrand` list

After the optimal variable values are found,
the hold out residuals are computed so one can check the
predictive validity for hold out data.
The residuals for hold out data (other data) are computed once (many times)
for each fit.

Output Tables
*************

fit_var_table
=============
A new :ref:`fit_var_table-name` is created each time this command is run.
It contains the results of the fit in its
:ref:`fit_var_table@fit_var_value` column.

fit_data_subset_table
=====================
A new :ref:`fit_data_subset_table-name` is created each time this command is run.
It is a comparison of the model and data corresponding to the fit results.

trace_fixed
===========
A new :ref:`trace_fixed_table-name` is created each time a
``fit fixed`` or ``fit both`` command is run.
It contains a trace of the corresponding optimization.

hes_random_table
================
A new :ref:`hes_random_table-name` is created each time this command is run
with *variables* equal to ``random`` or ``both`` .
The Hessian of the random effects objective is written in this table.
If *simulate_index* is present (is not present) the Hessian corresponds
to the simulated measurements in the :ref:`data_sim_table-name`
(measurements in the :ref:`data_table-name` ).

mixed_info
==========
A new :ref:`mixed_info_table-name` table is created each time this command is run.

ipopt_info_table
================
The fixed effect are optimized when
*variables* is equal to ``both`` or ``fixed`` .
In the case a new ``ipopt_info`` table, corresponding to the final
fit for the fixed effects, is written to the ipopt_info table.
The contents of this table are unspecified; i.e., not part of the
dismod_at API and my change.

Random Effects
**************
A model has random effects if one of the
:ref:`rate_table@child_smooth_id` or
:ref:`rate_table@child_nslist_id` is not ``null`` .
In some cases it helps to
first fit with *variables* equal to ``fixed``
and then fit with *variables* equal to ``both`` .

Convergence Problems
********************
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

Ipopt Options
*************
Some of the options in the :ref:`option_table-name` are for controlling
`Ipopt <https://coin-or.github.io/Ipopt/OPTIONS.html>`_.
You can override these options, or set other options, using a file called
``ipopt.opt`` in the directory where a fit command is run.
Doing so not supported because ``dismod_at``
expects certain options to be set a certain way.
The set of such options that is not specified and may change with time.
{xrst_toc_hidden
   example/get_started/fit_command.py
}
Example
*******
The file :ref:`fit_command.py-name` contains an example and test
using this command.

{xrst_end fit_command}
*/

// ----------------------------------------------------------------------------
// subset_data_obj and prior_object are const when simulate_index == ""
void fit_command(
   bool                                          use_warm_start   ,
   const std::string&                            variables        ,
   const std::string&                            simulate_index   ,
   sqlite3*                                      db               ,
   CppAD::vector<dismod_at::subset_data_struct>& subset_data_obj  ,
   dismod_at::data_model&                        data_object      ,
   dismod_at::prior_model&                       prior_object     ,
   const dismod_at::pack_info&                   pack_object      ,
   const dismod_at::pack_prior&                  var2prior        ,
   const dismod_at::db_input_struct&             db_input         ,
   // effectively const
   const std::map<std::string, std::string>&           option_map
)
{  using std::string;
   using CppAD::to_string;
   using CppAD::vector;
   // -----------------------------------------------------------------------
   bool ok = variables == "fixed";
   ok     |= variables == "random";
   ok     |= variables == "both";
   if( ! ok )
   {  string msg = "dismod_at fit command variables = ";
      msg += variables + "\nis not 'fixed', 'random' or 'both'";
      dismod_at::error_exit(msg);
   }
   if( use_warm_start && variables == "random" )
   {  string msg = "dismod_at fit command: cannot warm start when ";
      msg       += "only optimizing random effects";
      dismod_at::error_exit(msg);
   }
   //
   // bound_random
   double bound_random = 0.0;
   if( variables != "fixed" )
   {  // null corresponds to infinity
      std::string tmp_str = get_str_map(option_map, "bound_random");
      if( tmp_str == "" )
         bound_random = std::numeric_limits<double>::infinity();
      else
         bound_random = std::atof( tmp_str.c_str() );
   }
   // random_only
   bool random_only = variables == "random";
   // simulation index corresponding to data
   int simulation_index = -1;
   // -----------------------------------------------------------------------
   if( simulate_index != "" )
   {  size_t sim_index = std::atoi( simulate_index.c_str() );
      //
      // get simulation data
      vector<dismod_at::data_sim_struct> data_sim_table =
         dismod_at::get_data_sim_table(db);
      vector<dismod_at::prior_sim_struct> prior_sim_table =
         dismod_at::get_prior_sim_table(db);
      size_t n_var      = var2prior.size();
      size_t n_subset   = subset_data_obj.size();
      size_t n_simulate = prior_sim_table.size() / n_var;
      //
      if( sim_index >= n_simulate )
      {  string msg = "dismod_at fit command: simulate_index = ";
         msg += simulate_index + "\nis greater than or equal ";
         msg += "number of samples in the data_sim table.";
         string table_name = "data_sim";
         dismod_at::error_exit(msg, table_name);
      }
      // vector used for replacement of prior means
      vector<double> prior_mean(n_var * 3);
      for(size_t var_id = 0; var_id < n_var; ++var_id)
      {  size_t prior_sim_id = sim_index * n_var + var_id;
         prior_mean[var_id * 3 + 0] =
            prior_sim_table[prior_sim_id].prior_sim_value;
         prior_mean[var_id * 3 + 1] =
            prior_sim_table[prior_sim_id].prior_sim_dage;
         prior_mean[var_id * 3 + 2] =
            prior_sim_table[prior_sim_id].prior_sim_dtime;
      }
      prior_object.replace_mean(prior_mean);

      // replace meas_value in subset_data_obj
      for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
      {  size_t data_sim_id = n_subset * sim_index + subset_id;
# ifndef NDEBUG
         double old_value = subset_data_obj[subset_id].data_sim_value;
# endif
         double new_value =data_sim_table[data_sim_id].data_sim_value;
         assert(   std::isnan(old_value) );
         assert( ! std::isnan(new_value) );
         subset_data_obj[subset_id].data_sim_value = new_value;
      }

      // simulation index
      simulation_index = int(sim_index);
   }
   data_object.replace_like(subset_data_obj);
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
   // random_const
   size_t n_var    = pack_object.size();
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
   // warm_start_in
   CppAD::mixed::warm_start_struct warm_start_in;
   if( use_warm_start )
   {  // get sizeof_data
      table_name         = "ipopt_info";
      string col_name    = "warm_start";
      size_t sizeof_data = 0;
      void* data         = nullptr;
      read_blob_table(db, table_name, col_name, sizeof_data, data);
      //
      // read the data
      assert( sizeof_data % sizeof(double) == 0 );
      vector<double> vec( sizeof_data / sizeof(double) );
      data = reinterpret_cast<void*>( vec.data() );
      read_blob_table(db, table_name, col_name, sizeof_data, data);
      warm_start_in = unpack_warm_start(vec);
   }
   // ------------------ run fit_model ------------------------------------
   // quasi_fixed
   bool quasi_fixed = get_str_map(option_map, "quasi_fixed") == "true";
   //
   // trace_init_fit_model
   bool trace_init =
      get_str_map(option_map, "trace_init_fit_model") == "true";
   //
   // warn_on_stderr
   bool warn_on_stderr = get_str_map(option_map, "warn_on_stderr") == "true";
   //
   dismod_at::fit_model fit_object(
      db                   ,
      simulation_index     ,
      warn_on_stderr       ,
      bound_random         ,
      pack_object          ,
      var2prior            ,
      start_var            ,
      scale_var            ,
      db_input.prior_table ,
      prior_object         ,
      random_const         ,
      quasi_fixed          ,
      zero_sum_child_rate  ,
      zero_sum_mulcov_group,
      data_object          ,
      trace_init
   );
   fit_object.run_fit(random_only, option_map, warm_start_in);
   vector<double> opt_value, lag_value, lag_dage, lag_dtime;
   vector<CppAD::mixed::trace_struct> trace_vec;
   CppAD::mixed::warm_start_struct warm_start_out;
   fit_object.get_solution(
      opt_value, lag_value, lag_dage, lag_dtime, trace_vec, warm_start_out
   );
   // ------------------ hes_random table ----------------------------------
   if( variables != "fixed" )
   {  //
      // random_hes_rcv
      CppAD::mixed::d_sparse_rcv random_hes_rcv =
         fit_object.random_obj_hes(opt_value);
      //
      // drop previous verison of this table
      string sql_cmd = "drop table if exists hes_random";
      dismod_at::exec_sql_cmd(db, sql_cmd);
      //
      // create hes_random table
      size_t n_col         = 3;
      size_t n_row         = random_hes_rcv.nnz();
      vector<string> col_name(n_col), col_type(n_col), row_value(n_col*n_row);
      vector<bool>   col_unique(n_col);
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
      CppAD::mixed::s_vector row_major = random_hes_rcv.row_major();
      for(size_t k = 0; k < n_row; ++k)
      {  size_t ell              = row_major[k];
         size_t row_var_id       = random_hes_rcv.row()[ell];
         size_t col_var_id       = random_hes_rcv.col()[ell];
         double hes_random_value = random_hes_rcv.val()[ell];
         row_value[n_col * k + 0] = to_string(row_var_id);
         row_value[n_col * k + 1] = to_string(col_var_id);
         row_value[n_col * k + 2] = to_string(hes_random_value);
      }
      table_name = "hes_random";
      dismod_at::create_table(
         db, table_name, col_name, col_type, col_unique, row_value
      );
   }
   // ------------------ mixed_info table ----------------------------
   {  //
      // drop previous verison of this table
      string sql_cmd = "drop table if exists mixed_info";
      dismod_at::exec_sql_cmd(db, sql_cmd);
      //
      // copy of map
      std::map<std::string, size_t> info = fit_object.cppad_mixed_info();
      //
      // iterator for elements of map
      std::map<std::string, size_t>::iterator itr;
      //
      // count number of elements in map
      size_t n_info = 0;
      for(itr = info.begin(); itr != info.end(); ++itr)
         ++n_info;
      //
      table_name   = "mixed_info";
      size_t n_col = 2;
      vector<string> col_name(n_col), col_type(n_col);
      vector<string> row_value(n_col * n_info);
      vector<bool>   col_unique(n_col);
      //
      col_name[0]   = "mixed_name";
      col_type[0]   = "text";
      col_unique[0] = true;
      //
      col_name[1]   = "mixed_value";
      col_type[1]   = "integer";
      col_unique[1] = false;
      //
      size_t i_info = 0;
      for(itr = info.begin(); itr != info.end(); ++itr)
      {  row_value[i_info * n_col + 0] = itr->first;
         row_value[i_info * n_col + 1] = to_string( itr->second );
         ++i_info;
      }
      dismod_at::create_table(
         db, table_name, col_name, col_type, col_unique, row_value
      );
   }
   // ------------------ ipopt_info table -----------------------------------
   if( ! random_only )
   {  string sql_cmd = "drop table if exists ipopt_info";
      dismod_at::exec_sql_cmd(db, sql_cmd);
      //
      // pack the warm start information in a vector
      vector<double> vec = pack_warm_start(warm_start_out);
      //
      table_name         = "ipopt_info";
      string col_name    = "warm_start";
      size_t sizeof_data = vec.size() * sizeof(double);
      void* data         = reinterpret_cast<void*>( vec.data() );
      write_blob_table(db, table_name, col_name, sizeof_data, data);
   }
   // -------------------- fit_var table --------------------------------------
   string sql_cmd = "drop table if exists fit_var";
   dismod_at::exec_sql_cmd(db, sql_cmd);
   //
   table_name   = "fit_var";
   assert( n_var == opt_value.size() );
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
   {  vector< dismod_at::residual_struct<double> > residual;
      if( variable_type == 0 )
         residual  = prior_object.fixed(opt_value);
      else
         residual  = prior_object.random(opt_value);
      //
      for(size_t i = 0; i < residual.size(); i++)
      {  size_t index  = residual[i].index;
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
   size_t n_subset = subset_data_obj.size();
   table_name      = "fit_data_subset";
   //
   n_col           = 2;
   col_name.resize(n_col);
   col_type.resize(n_col);
   col_unique.resize(n_col);
   row_value.clear();
   row_value.resize(n_col * n_subset);
   //
   col_name[0]   = "avg_integrand";
   col_type[0]   = "real";
   col_unique[0] = false;
   //
   col_name[1]   = "weighted_residual";
   col_type[1]   = "real";
   col_unique[1] = false;
   //
   for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
   {  // compute average integrand for this data item
      double avg = data_object.average(subset_id, opt_value);
      assert( ! CppAD::isnan(avg) );

      // compute its residual and log likelihood
      double not_used;
      dismod_at::residual_struct<double> residual =
         data_object.like_one(subset_id, opt_value, avg, not_used);
      //
      row_value[ subset_id * n_col + 0] = to_string( avg );
      row_value[ subset_id * n_col + 1] = to_string( residual.wres );
   }
   dismod_at::create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   if( ! random_only )
   {  // -------------------- trace_fixed table -----------------------------
      sql_cmd = "drop table if exists trace_fixed";
      dismod_at::exec_sql_cmd(db, sql_cmd);
      //
      table_name     = "trace_fixed";
      size_t n_trace = trace_vec.size();
      n_col = 11;
      col_name.resize(n_col);
      col_unique.resize(n_col);
      col_type.resize(n_col);
      row_value.resize(n_col * n_trace);
      const char* col_name_lst[] = {
         "iter",
         "obj_value",
         "inf_pr",
         "inf_du",
         "mu",
         "d_norm",
         "regularization_size",
         "alpha_du",
         "alpha_pr",
         "ls_trials",
         "restoration"
      };
      for(size_t j = 0; j < n_col; ++j)
      {  col_name[j]   = col_name_lst[j];
         col_unique[j] = false;
         bool integer = col_name[j] == "iter";
         integer     |= col_name[j] == "ls_trials";
         integer     |= col_name[j] == "restoration";
         if( integer )
            col_type[j] = "integer";
         else
            col_type[j] = "real";
      }
      for(size_t id = 0; id < n_trace; ++id)
      {
         row_value[ id * n_col + 0] = to_string( trace_vec[id].iter );
         row_value[ id * n_col + 1] = to_string( trace_vec[id].obj_value );
         row_value[ id * n_col + 2] = to_string( trace_vec[id].inf_pr );
         row_value[ id * n_col + 3] = to_string( trace_vec[id].inf_du );
         row_value[ id * n_col + 4] = to_string( trace_vec[id].mu );
         row_value[ id * n_col + 5] = to_string( trace_vec[id].d_norm );
         row_value[ id * n_col + 6] =
            to_string( trace_vec[id].regularization_size );
         row_value[ id * n_col + 7] = to_string( trace_vec[id].alpha_du );
         row_value[ id * n_col + 8] = to_string( trace_vec[id].alpha_pr );
         row_value[ id * n_col + 9] = to_string( trace_vec[id].ls_trials );
         row_value[ id * n_col + 10] =
            to_string( int( trace_vec[id].restoration ) );
      }
      dismod_at::create_table(
         db, table_name, col_name, col_type, col_unique, row_value
      );
   }

   return;
}

} // END_DISMOD_AT_NAMESPACE
