# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin db2csv_command}
# {xrst_spell
#     diff
#     dir
#     ij
#     res
#     stdcv
# }
# {xrst_comment_ch #}
#
# Create Csv Files that Summarize The Database
# ############################################
#
# Syntax
# ******
#
# As Program
# ==========
# ``dismod-at`` *database* ``db2csv``
#
# As Python Function
# ==================
# ``dismod_at.db2csv_command`` ( *database*  )
#
# Convention
# **********
# The ``null`` value in the database corresponds
# to an empty string in the csv files.
#
# database
# ********
# is the path from the currently directory to the database.
# This must be a :ref:`dismod_at<database-name>` and
# the :ref:`init_command-name` must have been run on the database.
#
# dir
# ***
# We use the notation *dir* for the directory where *database*
# is located.
#
# fit_var, fit_data_subset
# ************************
# The :ref:`log_table-name` is used to determine if the previous
# fit command had a :ref:`fit_command@simulate_index` .
# If so, the :ref:`fit_var_table-name` and :ref:`fit_data_subset_table-name`
# corresponds to simulated data.
# Otherwise, if they exist, the correspond to the measured data.
#
# simulate_index
# **************
# If the previous fit command had a
# :ref:`fit_command@simulate_index`
# that value is used for *simulate_index* below.
# Otherwise, zero is used for *simulate_index* below.
#
# option.csv
# **********
# The file *dir* / ``option.csv`` is written by this command.
# It is a CSV file with one row for each possible row in the
# :ref:`option_table-name` .
# The columns in ``option.csv`` are
# :ref:`option_table@Table Format@option_name` and
# :ref:`option_table@Table Format@option_value` .
# If a row does not appear in the option table, the corresponding
# default value is written to ``option.csv`` .
# If the :ref:`option_table@Parent Node@parent_node_id` appears
# in the option table, the *parent_node_name* row of ``option.csv``
# is filled in with the corresponding node name.
#
# log.csv
# *******
# The file *dir* / ``log.csv`` is written by this command.
# It is a CSV file with one row for each message in the :ref:`log_table-name` .
# The columns in this table are
# :ref:`log_table@message_type` ,
# :ref:`log_table@table_name` ,
# :ref:`log_table@row_id` ,
# :ref:`log_table@unix_time` , and
# :ref:`log_table@message` .
#
# age_avg.csv
# ***********
# The file *dir* / ``age_avg.csv`` is written by this command.
# It is a CSV file with the contents of the age_avg table.
# The only column in this table is :ref:`age_avg_table@age` .
# Note that a :ref:`set_command-name` may change the value of
# :ref:`option_table@Age Average Grid@ode_step_size` or
# :ref:`option_table@Age Average Grid@age_avg_split` but it will not
# write out the new age_avg table.
#
# hes_fixed.csv
# *************
# If the :ref:`sample_command@asymptotic`
# sample command was executed,
# the contents of the :ref:`hes_fixed_table-name` are written to
# the CSV file *dir* / ``hes_fixed.csv`` .
# The columns in this table are
# :ref:`hes_fixed_table@row_var_id` ,
# :ref:`hes_fixed_table@col_var_id` ,
# :ref:`hes_fixed_table@hes_fixed_value` .
#
# hes_random.csv
# **************
# If a :ref:`fit both<fit_command@variables@both>` ,
# :ref:`fit random<fit_command@variables@random>` ,
# or :ref:`sample asymptotic<sample_command@asymptotic>`
# command was executed,
# the contents of the :ref:`hes_random_table-name` are written to
# the CSV file *dir* / ``hes_random.csv`` .
# The columns in this table are
# :ref:`hes_random_table@row_var_id` ,
# :ref:`hes_random_table@col_var_id` ,
# :ref:`hes_random_table@hes_random_value` .
#
# trace_fixed.csv
# ***************
# If the :ref:`fit fixed<fit_command@variables@fixed>` or
# :ref:`fit both<fit_command@variables@both>` command has completed,
# the contents of the :ref:`trace_fixed_table-name` are written to
# the CSV file *dir* / ``trace_fixed.csv`` .
# The columns in this table have the same name as in the corresponding table
# with the exception that the column
# :ref:`trace_fixed_table@regularization_size`
# is called *reg_size* .
#
# mixed_info.csv
# **************
# If the :ref:`fit_command-name` completed
# the contents of the :ref:`mixed_info_table-name` are written to
# the CSV file *dir* / ``mixed_info.csv`` .
#
# {xrst_comment -------------------------------------------------------}
# variable.csv
# ************
# {xrst_comment -------------------------------------------------------}
# The file *dir* / ``variable.csv`` is written by this command.
# It is a CSV file with one row for each of the :ref:`model_variables-name`
# and has the following columns:
#
# var_id
# ======
# is the :ref:`var_table@var_id` .
#
# var_type
# ========
# is the :ref:`var_table@var_type` .
#
# s_id
# ====
# is the :ref:`smooth_table@smooth_id` for this variable.
# If the variable is a
# :ref:`smoothing standard deviation multiplier<model_variables@Fixed Effects, theta@Smoothing Standard Deviation Multipliers, lambda>`
# this is the smoothing that this multiplier effects.
# Otherwise, it is the smoothing where the prior for this variable
# comes from.
#
# m_id
# ====
# If this variable is a covariate multiplier, this is the corresponding
# :ref:`mulcov_table@mulcov_id` .
#
# m_diff
# ======
# If this variable is a covariate multiplier, this is the corresponding
# :ref:`bnd_mulcov_table@max_cov_diff` .
#
# bound
# =====
# If the upper and lower value limits in the value prior for this variable
# are not equal,
# this is a bound for the absolute value of this variable; see
# :ref:`bnd_mulcov_table@max_mulcov` and
# :ref:`option_table@Optimize Random Only@bound_random` .
#
# age
# ===
# is the :ref:`age_table@age` .
#
# time
# ====
# is the :ref:`time_table@time` .
#
# rate
# ====
# is the :ref:`rate_table@rate_name` .
#
# integrand
# =========
# is the
# :ref:`integrand_table@integrand_name` .
#
# covariate
# =========
# is the
# :ref:`covariate_table@covariate_name` .
#
# node
# ====
# is the
# :ref:`node_table@node_name` .
#
# group
# =====
# This field is non-empty for
# :ref:`model_variables@Fixed Effects, theta@Group Covariate Multipliers` .
#
# subgroup
# ========
# This field is non-empty for
# :ref:`model_variables@Random Effects, u@Subgroup Covariate Multipliers` .
#
# fixed
# =====
# is ``true`` if this variable is a
# :ref:`fixed effect<model_variables@Fixed Effects, theta>` ,
# otherwise it is ``false`` .
#
# depend
# ======
# If the :ref:`depend_var_table-name` exists, this has one of the following:
# ``none`` if neither the data nor the prior depends on this variable,
# ``data`` if only the data depends on this variable,
# ``prior`` if only the prior depends on this variable,
# ``both`` if both the data and the prior depend on this variable.
#
# fit_value
# =========
# If the :ref:`fit_command-name` has been run, this is the
# :ref:`fit_var_table@fit_var_value` .
#
# start
# =====
# is the
# :ref:`start_var_table@start_var_value`
# for this variable.
#
# scale
# =====
# is the
# :ref:`scale_var_table@scale_var_value`
# for this variable.
#
# truth
# =====
# If the truth_var table exists, this is the
# :ref:`truth_var_table@truth_var_value`
# for this variable.
#
# sam_avg
# =======
# If the sample table exists,
# for each :ref:`sample_table@var_id`
# this is the average with respect to
# with respect to :ref:`sample_table@sample_index`
# of the :ref:`sample_table@var_value` corresponding to
# this *var_id* .
#
# sam_std
# =======
# If the sample table exists,
# for each fixed :ref:`sample_table@var_id`
# this is the estimated standard deviation with respect to
# with respect to :ref:`sample_table@sample_index`
# of the # :ref:`sample_table@var_value` corresponding to
# this *var_id* .
# If there is only one *sample_index* in the sample table,
# this column is empty because the standard deviation cannot be estimated
# from one sample.
#
# res_value
# =========
# If the :ref:`fit_command-name` has been run, this is the
# :ref:`fit_var_table@residual_value` .
#
# res_dage
# ========
# If the :ref:`fit_command-name` has been run, this is the
# :ref:`fit_var_table@residual_dage` ; see
# :ref:`fit_var<db2csv_command@fit_var, fit_data_subset>` above.
#
# res_dtime
# =========
# If the :ref:`fit_command-name` has been run, this is the
# :ref:`fit_var_table@residual_dtime` ; see
# :ref:`fit_var<db2csv_command@fit_var, fit_data_subset>` above.
#
# lag_value
# =========
# If the :ref:`fit_command-name` has been run, this is the
# :ref:`fit_var_table@lagrange_value` ; see
# :ref:`fit_var<db2csv_command@fit_var, fit_data_subset>` above.
#
# lag_dage
# ========
# If the :ref:`fit_command-name` has been run, this is the
# :ref:`fit_var_table@lagrange_dage` ; see
# :ref:`fit_var<db2csv_command@fit_var, fit_data_subset>` above.
#
# lag_dtime
# =========
# If the :ref:`fit_command-name` has been run, this is the
# :ref:`fit_var_table@lagrange_dtime` ; see
# :ref:`fit_var<db2csv_command@fit_var, fit_data_subset>` above.
#
# sim_v, sim_a, sim_t
# ===================
# If the :ref:`simulate_command-name` has been run,
# these are the values of
# :ref:`prior_sim_table@prior_sim_value` ,
# :ref:`prior_sim_table@prior_sim_dage` , and
# :ref:`prior_sim_table@prior_sim_dtime` ,
# for the
# :ref:`db2csv_command@simulate_index` .
#
# prior_info
# ==========
# There is a column named
#
#     *field* _ *character*
#
# for *character* equal to ``v`` , ``a`` and ``t``
# and for *field* equal to
# :ref:`prior_table@mean` ,
# :ref:`prior_table@lower` ,
# :ref:`prior_table@upper` ,
# :ref:`prior_table@std` ,
# :ref:`prior_table@eta` ,
# :ref:`prior_table@nu` and
# :ref:`density<prior_table@density_id>` .
#
# #. The character ``v`` denotes this is the prior information for a value,
#    ``a`` the prior information for an age difference, and
#    ``t`` the prior information for a time difference.
# #. The density has been mapped to the corresponding
#    :ref:`density_table@density_name` .
# #. If the corresponding :ref:`smooth_grid_table@value_prior_id`
#    is ``null`` ,
#    the :ref:`smooth_grid_table@const_value` prior is displayed.
# #. If is ``null`` , or has no affect, it is displayed as empty.
#    Note that the fields *eta_v* are always displayed for fixed
#    effects because they have a
#    :ref:`scaling<prior_table@eta@Scaling Fixed Effects>` affect.
#
# {xrst_comment -------------------------------------------------------}
# data.csv
# ********
# {xrst_comment -------------------------------------------------------}
# The file *dir* / ``data.csv`` is written by this command.
# It is a CSV file with one row for each row in the :ref:`data_subset_table-name`
# and has the following columns:
#
# data_id
# =======
# is the data table
# :ref:`data_table@data_id` .
#
# data_extra_columns
# ==================
# Each column specified by the
# :ref:`option_table@Extra Columns@data_extra_columns`
# option is included in the ``data.csv`` file.
#
# child
# =====
# If this data row is associated with a child,
# this is the name of the child. Otherwise, this data is associated
# with the :ref:`option_table@Parent Node` .
#
# node
# ====
# is the
# :ref:`node_table@node_name` for this data row.
# This will correspond directly to the data table
# :ref:`data_table@node_id` .
#
# group
# =====
# is the :ref:`subgroup_table@group_name` corresponding
# to the subgroup for this data row.
#
# subgroup
# ========
# is the
# :ref:`subgroup_table@subgroup_name` for this data row.
# This will correspond directly to the data table
# :ref:`data_table@subgroup_id` .
#
# integrand
# =========
# is the integrand table
# :ref:`integrand_table@integrand_name` .
#
# weight
# ======
# is the
# :ref:`weight_table@weight_name` .
#
# age_lo
# ======
# is the lower age used in the fits; i.e., the data table
# :ref:`data_table@age_lower` modified by the
# age compression interval in the
# :ref:`option_table@compress_interval` option.
#
# age_up
# ======
# is the upper age used in the fits; i.e., the data table
# :ref:`data_table@age_upper` modified by the
# age compression interval.
#
# time_lo
# =======
# is the lower time used in the fits; i.e., the data table
# :ref:`data_table@time_lower` modified by the
# time compression interval.
#
# time_up
# =======
# is the upper time used in the fits; i.e., the data table
# :ref:`data_table@time_upper` modified by the
# time compression interval.
#
# d_out
# =====
# is the value of
# :ref:`data_table@hold_out` in the data table.
#
# s_out
# =====
# is the value of
# :ref:`data_subset_table@hold_out` in the
# data_subset table.
#
# density
# =======
# is the
# :ref:`density_table@density_name` for data_subset table
# :ref:`data_subset_table@density_id` for this row.
#
# eta
# ===
# is the data_subset table
# :ref:`data_subset_table@eta` for this row.
#
# nu
# ==
# is the data_subset table
# :ref:`data_subset_table@nu` for this row.
#
# ss
# ==
# is the data_subset table
# :ref:`data_subset_table@sample_size` for this row.
#
# meas_std
# ========
# is the data table
# :ref:`data_table@meas_std` .
# Except in the binomial case, where it is an approximation for the
# standard deviation of the binomial counts divided by the sample size.
#
# meas_stdcv
# ==========
# is the minimum cv standard deviation used to define the likelihood; see
# :ref:`Delta<data_like@Notation@Minimum CV Standard Deviation, Delta_i>` .
# In the binomial case it is equal to *meas_std*.
#
# meas_sigma
# ==========
# If the previous fit command had a
# :ref:`db2csv_command@simulate_index` ,
# this column is empty.
# We use *sigma* to denote the adjusted standard deviation
# :ref:`sigma<data_like@Adjusted Standard Deviation, sigma_i(theta)>`
# for this row.
#
# The transformed standard deviation
# :ref:`delta<data_like@Transformed Standard Deviation, delta_i(theta)>`
# is computed by dividing by the residual.
# This results in plus infinity and not valid when the residual is zero.
# If this calculation for *delta*
# is greater than the maximum python ``float`` value,
# *meas_sigma* is reported as empty .
# Otherwise the transformation is inverted to get the value of *sigma* .
#
# meas_value
# ==========
# is the data table
# :ref:`data_table@meas_value` .
#
# avgint
# ======
# If the :ref:`fit_command-name` has been run, this is the
# :ref:`fit_data_subset_table@avg_integrand` for this row.
#
# residual
# ========
# If the :ref:`fit_command-name` has been run, this is the
# :ref:`fit_data_subset_table@weighted_residual`
# for this row; see
# :ref:`fit_data_subset<db2csv_command@fit_var, fit_data_subset>`
# above.
#
# sim_value
# =========
# If the :ref:`simulate_command-name` has been run, this is the
# :ref:`data_sim_table@data_sim_value` for this
# :ref:`db2csv_command@data.csv@data_id` and
# :ref:`fit_command@simulate_index`
# in the previous fit command.
# If there is no *simulate_index*
# in the previous fit command, the
# value zero is used for the *simulate_index* .
#
# Covariates
# ==========
# For each covariate in the :ref:`covariate_table-name` there is a column with
# the corresponding *covariate_name* .
# For each covariate column and measurement row, the value in the
# covariate column is covariate value for this measurement minus
# the reference value for this covariate, i.e., the corresponding
# covariate difference
# :ref:`x_ij<avg_integrand@Data or Avgint Table Notation@Covariate Difference, x_ij>`
# in the model for the average integrand.
#
# {xrst_comment -------------------------------------------------------}
# predict.csv
# ***********
# {xrst_comment -------------------------------------------------------}
# If the :ref:`predict_command-name` has was executed,
# the CSV file *dir* / ``predict.csv`` is written.
# For each row of the :ref:`predict_table-name`
# there is a corresponding row in ``predict.csv`` .
#
# avgint_id
# =========
# is the avgint table
# :ref:`avgint_table@avgint_id` .
#
# avgint_extra_columns
# ====================
# Each column specified by the
# :ref:`option_table@Extra Columns@avgint_extra_columns`
# option is included in the ``predict.csv`` file.
#
# s_index
# =======
# This identifies the set model variables corresponding to the
# last :ref:`predict_command-name` executed.
# If the source for the predict command was
# :ref:`predict_command@source@sample` ,
# the model variables correspond to the rows on the
# sample table with the same :ref:`sample_table@sample_index`
# equal to *s_index* .
# Otherwise, *s_index* is empty and
# the model variables correspond to the
# :ref:`predict_command@source@fit_var` or
# :ref:`predict_command@source@truth_var` table
# depending on the source for the last predict command executed.
#
# avgint
# ======
# is the :ref:`average integrand<avg_integrand@Average Integrand, A_i>`
# :math:`A_i(u, \theta)`. The model variables :math:`(u, \theta)`
# correspond to the *s_index* , and measurement subscript :math:`i`
# denotes to the :ref:`avgint_table-name` information
# for this row of ``predict.csv`` ; i.e., *age_lo* , *age_up* ,
# ...
#
# age_lo
# ======
# is the avgint table
# :ref:`avgint_table@age_lower` .
#
# age_up
# ======
# is the avgint table
# :ref:`data_table@age_upper` .
#
# time_lo
# =======
# is the avgint table
# :ref:`data_table@time_lower` .
#
# time_up
# =======
# is the avgint table
# :ref:`data_table@time_upper` .
#
# integrand
# =========
# is the avgint table
# :ref:`integrand_table@integrand_name` .
#
# weight
# ======
# is the
# :ref:`weight_table@weight_name` for this row.
#
# node
# ====
# is the
# :ref:`node_table@node_name` for this row.
#
# group
# =====
# is the :ref:`subgroup_table@group_name` corresponding
# to the subgroup for this data row.
#
# subgroup
# ========
# is the
# :ref:`subgroup_table@subgroup_name` for this data row.
# This will correspond directly to the avgint table
# :ref:`avgint_table@subgroup_id` .
#
# Covariates
# ==========
# For each covariate in the :ref:`covariate_table-name` there is a column with
# the corresponding *covariate_name* .
# For each covariate column and measurement row, the value in the
# covariate column is covariate value in the :ref:`avgint_table-name`
# minus the reference value for this covariate. i.e., the corresponding
# covariate difference
# :ref:`x_ij<avg_integrand@Data or Avgint Table Notation@Covariate Difference, x_ij>`
# in the model for the average integrand.
# {xrst_comment -------------------------------------------------------}
# {xrst_toc_hidden
#    example/get_started/db2csv_command.py
# }
# Example
# *******
# The file :ref:`db2csv_command.py-name` contains an example and test
# using this command.
#
# {xrst_end db2csv_command}
def db2csv_command(database_file_arg) :
   import os
   import csv
   import dismod_at
   import sys
   import copy
   import math
   # -------------------------------------------------------------------------
   table_data     = dict()
   parent_node_id = None
   # -------------------------------------------------------------------------
   def adjusted_meas_std(density, eta, meas_value, avgint, residual) :
      from math import log
      from math import exp
      #
      if residual == None or residual == 0.0 :
         return None
      #
      # log
      is_log = density.startswith('log_') or density.startswith('cen_log_')
      #
      # linear case
      if not is_log :
         delta = (meas_value - avgint) / residual
         assert delta >= 0.0
         if delta > log( sys.float_info.max ) :
            return None
         #
         # sigma
         return delta
      #
      # log case
      #
      # delta
      delta = (log(meas_value + eta) - log(avgint + eta)) / residual
      assert delta >= 0.0
      if delta > log( sys.float_info.max ) :
         return None

      #
      # sigma
      sigma = (exp(delta) - 1.0) * (meas_value + eta)
      return sigma
   # -------------------------------------------------------------------------
   def round_to(x, n_digits) :
      if x == None or x == 0.0:
         return x
      first_digit = int( math.floor( math.log10( abs(x) ) ) )
      return round(x, n_digits - first_digit - 1)
   # -------------------------------------------------------------------------
   def check4table(table_name) :
      cursor  = table_name2cursor(table_name)
      cmd     = "SELECT * FROM sqlite_master WHERE type='table' AND name="
      cmd    += "'" + table_name + "';"
      info    = cursor.execute(cmd).fetchall()
      if len(info) == 0 :
         result  = False
      else :
         assert len(info) == 1
         result = True
      return result
   # -------------------------------------------------------------------------
   def check_table_columns(table_name, table_columns) :
      connection = table_name2connection(table_name)
      if len( table_name ) == 0 :
         return True
      (col_name, col_type) = dismod_at.get_name_type(connection, table_name)
      table_info           = dict( zip(col_name, col_type) )
      for pair in table_columns :
         name = pair[0]
         ty   = pair[1]
         if name not in table_info :
            msg = 'expected column ' + name + ' in table ' + table_name
            assert False, msg
         if  ty != table_info[name] :
            msg  = 'expected type ' + ty +  ' for column ' + name
            msg += ' of table ' + table_name
            assert False, msg
   # -------------------------------------------------------------------------
   def convert2output(value_in) :
      if value_in == None :
         value_out = ''
      elif type(value_in) is float :
         value_out = '%13.5g' % value_in
      else :
         value_out = str(value_in)
      return value_out
   # -------------------------------------------------------------------------
   def table_lookup(table_name, row_id, column_name) :
      if row_id == None :
         return ''
      value_in = table_data[table_name][row_id][column_name]
      return convert2output(value_in)
   # -------------------------------------------------------------------------
   def get_prior_info(row_out, prior_id_dict, fixed_effect) :
      extension2name = {'_v':'value_', '_a':'dage_', '_t':'dtime_' }
      for extension in extension2name :
         name         = extension2name[extension]
         key          = name + 'prior_id'
         prior_id     = prior_id_dict[key]
         const_value  = prior_id_dict['const_value']
         if extension == '_v' :
            if prior_id == None and const_value == None :
               msg = 'both value_prior_id and const_value are null '
               msg += 'in smooth_grid table\n'
               assert False, msg
            if prior_id != None and const_value != None :
               msg = 'both value_prior_id and const_value are not null '
               msg += 'in smooth_grid table\n'
               assert False, msg
         #
         field_out = 'density' + extension
         row_out[field_out] = ''
         if prior_id != None :
            density_id   = table_data['prior'][prior_id]['density_id']
            density_name = table_data['density'][density_id]['density_name']
            row_out[field_out] = density_name
            log_density      = density_name.startswith('log_')
            students_density = density_name in ['students', 'log_students']
         #
         for field_in in [ 'lower', 'upper', 'mean', 'std', 'eta', 'nu' ] :
            field_out = field_in + extension
            row_out[field_out] = ''
            if prior_id != None :
               value_out = table_data['prior'][prior_id][field_in]
               if field_in == 'nu' and not students_density :
                  value_out = None
               if field_in == 'eta' and not log_density :
                  if extension != '_v' or fixed_effect != 'true' :
                     value_out = None
               if field_in == 'std' and density_name == 'uniform' :
                     value_out = None
               row_out[field_out] = convert2output( value_out )
            elif extension == '_v' :
               if field_in in [ 'lower', 'upper', 'mean' ] :
                  row_out[field_out] = convert2output( const_value )
   # -------------------------------------------------------------------------
   def node_id2child(node_id) :
      if node_id == parent_node_id :
         return ''
      descendant_id = node_id
      while descendant_id != None :
         parent_id  = table_data['node'][descendant_id]['parent']
         if parent_id == node_id :
            msg  = 'db2csv_command: node_id ' + str(node_id)
            msg += ' is a descendant of itself, see the node table '
            assert False, msg
         if parent_id == parent_node_id :
            name = table_data['node'][descendant_id]['node_name']
            return name
         descendant_id = parent_id
      assert False
   # -------------------------------------------------------------------------
   def child_has_data() :
      result = set()
      for subset_row in table_data['data_subset'] :
         data_id         = subset_row['data_id']
         data_row        = table_data['data'][data_id]
         node_id         = data_row['node_id']
         integrand_id    = data_row['integrand_id']
         integrand_name  = table_lookup(
            'integrand', integrand_id, 'integrand_name'
         )
         hold_out        = data_row['hold_out'] != 0
         if subset_row['hold_out'] != 0 :
            hold_out = True
         if integrand_name in hold_out_integrand_list :
            hold_out = True
         if (not hold_out) and (node_id != parent_node_id) :
            # child_node_id
            child_node_id = node_id
            parent_id     = table_data['node'][child_node_id]['parent']
            while parent_id != parent_node_id :
               child_node_id = parent_id
               parent_id     = table_data['node'][child_node_id]['parent']
               if parent_id is None :
                  msg  = 'Cannot determine child for row in data_subset '
                  msg += 'table. Must re-run init command'
                  assert False, msg
            # update result
            result.add(child_node_id)
      return result
   #
   # database_dir
   database_dir       = os.path.split(database_file_arg)[0]
   # -------------------------------------------------------------------------
   # table_name2connection, table_name2cursor
   #
   file_name          = database_file_arg
   primary_connection = dismod_at.create_connection(
      file_name, new = False, readonly = True
   )
   primary_cursor     = primary_connection.cursor()
   option_table       = dismod_at.get_table_dict(primary_connection, 'option')
   #
   other_connection       = None
   other_cursor           = None
   other_input_table_list = list()
   for row in option_table :
      if row['option_name'] == 'other_database' :
         other_database   = row['option_value']
         if other_database != None :
            if not os.path.isabs( other_database ) :
               other_database = os.path.join( database_dir, other_database )
            other_connection = dismod_at.create_connection(
               other_database, new = False, readonly = True
            )
            other_cursor = other_connection.cursor()
      if row['option_name'] == 'other_input_table' :
         if row['option_value'] != None :
            other_input_table_list = row['option_value'].split(' ')
   #
   def table_name2connection(table_name) :
      if other_connection != None :
         if table_name in other_input_table_list :
            return other_connection
      return primary_connection
   #
   # table_name2cursor
   def table_name2cursor(table_name) :
      if other_connection != None :
         if table_name in other_input_table_list :
            return other_cursor
      return primary_cursor
   # -------------------------------------------------------------------------
   required_table_list  = [
      'age',
      'age_avg',
      'avgint',
      'bnd_mulcov',
      'covariate',
      'data',
      'data_subset',
      'density',
      'integrand',
      'log',
      'option',
      'mulcov',
      'node',
      'subgroup',
      'prior',
      'rate',
      'smooth',
      'smooth_grid',
      'start_var',
      'scale_var',
      'time',
      'var',
      'weight'
   ]
   for table in required_table_list :
      if not check4table(table):
         msg  = 'db2csv_command: the required table ' + table + '\n'
         msg += 'is missing from file ' + file_name + '\n'
         assert False, msg
   #
   have_table = dict()
   have_table['depend_var']      = check4table('depend_var')
   have_table['truth_var']       = check4table('truth_var')
   have_table['sample']          = check4table('sample')
   have_table['data_sim']        = check4table('data_sim')
   have_table['prior_sim']       = check4table('prior_sim')
   have_table['fit_var']         = check4table('fit_var')
   have_table['fit_data_subset'] = check4table('fit_data_subset')
   have_table['predict']         = check4table('predict')
   have_table['hes_fixed']       = check4table('hes_fixed')
   have_table['hes_random']      = check4table('hes_random')
   have_table['trace_fixed']     = check4table('trace_fixed')
   have_table['mixed_info']      = check4table('mixed_info')
   # ----------------------------------------------------------------------
   # check pairs of tables that should come togeather
   table_pair_list = [
      ( 'fit_var',    'fit_data_subset' ),
      ( 'data_sim',   'prior_sim'),
   ]
   for pair in table_pair_list :
      if have_table[pair[0]] != have_table[pair[1]] :
         msg = 'db2csv_command: '
         for table in pair :
            if have_table[table] :
               msg  += 'have ' + table + ' = true\n'
            else :
               msg  += 'have ' + table + ' = false\n'
         msg += 'in ' + file_name + '\n'
         assert False, msg
   #
   table_list  = copy.copy( required_table_list )
   for key in have_table :
      if have_table[key] :
         table_list.append(key)
   # ----------------------------------------------------------------------
   # table_data
   for table in table_list :
      connection        = table_name2connection(table)
      table_data[table] = dismod_at.get_table_dict(connection, table)
   # ----------------------------------------------------------------------
   # check tables that are supposed to be the same length
   pair_list = [
      [ 'var',            'start_var'],
      [ 'var',            'scale_var'],
      [ 'var',            'depend_var'],
      [ 'var',            'truth_var'],
      [ 'var',            'fit_var'],
      [ 'data_subset',    'fit_data_subset' ]
   ]
   have_table['start_var'] = True
   have_table['scale_var'] = True
   for [left, right] in pair_list :
      if have_table[right] :
         len_left  = len( table_data[left]  )
         len_right = len( table_data[right] )
         if len_left != len_right :
            msg  = 'db2csv_command: tables should have same length:\n'
            msg += 'length ' + left + '_table = ' + str(len_left) + '\n'
            msg += 'length ' + right + '_table = ' + str(len_right) + '\n'
            assert False, msg
   # ----------------------------------------------------------------------
   # check types in tables
   #
   # age, time
   for table_name in [ 'age', 'time' ] :
      table_columns = [ (table_name, 'real') ]
      check_table_columns(table_name, table_columns)
   #
   # integrand, density
   for table_name in [ 'integrand', 'density' ]:
      table_columns = [ (table_name + '_name', 'text') ]
      check_table_columns(table_name, table_columns)
   #
   # covariate
   table_name    = 'covariate'
   table_columns = [
      ('covariate_name',  'text'),
      ('reference',       'real'),
      ('max_difference',  'real'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # option
   table_name    = 'option'
   table_columns = [ ('option_name', 'text'), ('option_value', 'text') ]
   check_table_columns(table_name, table_columns)
   #
   # mulcov
   table_name    = 'mulcov'
   table_columns = [
      ('mulcov_type',            'text'),
      ('rate_id',             'integer'),
      ('integrand_id',        'integer'),
      ('covariate_id',        'integer'),
      ('group_id',            'integer'),
      ('group_smooth_id',     'integer'),
      ('subgroup_smooth_id',  'integer'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # node
   table_name    = 'node'
   table_columns = [
      ('node_name',  'text'),
      ('parent',     'integer'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # subgroup
   table_name    = 'subgroup'
   table_columns = [
      ('subgroup_name',     'text'),
      ('group_id',       'integer'),
      ('group_name',        'text'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # prior
   table_name    = 'prior'
   table_columns = [
      ('prior_name',        'text'),
      ('density_id',     'integer'),
      ('lower',             'real'),
      ('upper',             'real'),
      ('mean',              'real'),
      ('std',               'real'),
      ('eta',               'real'),
      ('nu',                'real'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # rate
   table_name    = 'rate'
   table_columns = [
      ('rate_name',             'text'),
      ('parent_smooth_id',   'integer'),
      ('child_smooth_id',    'integer'),
      ('child_nslist_id',    'integer'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # smooth
   table_name    = 'smooth'
   table_columns = [
      ('smooth_name',            'text'),
      ('n_age',                  'integer'),
      ('n_time',                 'integer'),
      ('mulstd_value_prior_id',  'integer'),
      ('mulstd_dage_prior_id',   'integer'),
      ('mulstd_dtime_prior_id',  'integer'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # smooth_grid
   table_name    = 'smooth_grid'
   table_columns = [
      ('smooth_id',              'integer'),
      ('age_id',                 'integer'),
      ('time_id',                'integer'),
      ('value_prior_id',         'integer'),
      ('dage_prior_id',          'integer'),
      ('dtime_prior_id',         'integer'),
      ('const_value',            'real'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # weight
   table_name    = 'weight'
   table_columns = [
      ('weight_name',            'text'),
      ('n_age',                  'integer'),
      ('n_time',                 'integer'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # weight_grid
   table_name    = 'weight_grid'
   table_columns = [
      ('weight_id',              'integer'),
      ('age_id',                 'integer'),
      ('time_id',                'integer'),
      ('weight',                 'real'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # avgint
   table_name    = 'avgint'
   table_columns = [
      ('integrand_id', 'integer'),
      ('node_id',      'integer'),
      ('subgroup_id',  'integer'),
      ('weight_id',    'integer'),
      ('age_lower',       'real'),
      ('age_upper',       'real'),
      ('time_lower',      'real'),
      ('time_upper',      'real'),
   ]
   check_table_columns(table_name, table_columns)
   #
   # data
   table_name     = 'data'
   table_columns += [
      ('hold_out',      'integer'),
      ('meas_value',    'real'),
      ('meas_std',      'real'),
      ('eta',           'real'),
      ('nu',            'real'),
      ('sample_size',   'integer'),
   ]
   check_table_columns(table_name, table_columns)
   # ----------------------------------------------------------------------
   # parent_node_id
   parent_node_id     = None
   parent_node_name   = None
   for row in table_data['option'] :
      if row['option_name'] == 'parent_node_id' :
         parent_node_id = int( row['option_value'] )
      if row['option_name'] == 'parent_node_name' :
         parent_node_name = row['option_value']
   if parent_node_id == None and parent_node_name == None :
      msg  = 'db2csv_command: neither parent_node_id nor parent_node_name\n'
      msg += 'is present in the option table\n'
      assert False, msg
   if parent_node_id == None :
      node_table = table_data['node']
      for node_id in range( len( node_table ) ) :
         if node_table[node_id]['node_name'] == parent_node_name :
            parent_node_id = node_id
   if parent_node_id == None :
      msg  = 'db2csv_command: parent_node_name in option table '
      msg += 'does not appear in the node table\n'
      assert False, msg
   # ----------------------------------------------------------------------
   # minimum_meas_cv
   minimum_meas_cv    = list()
   for row in table_data['integrand'] :
      minimum_meas_cv.append( row['minimum_meas_cv'] )
   # ----------------------------------------------------------------------
   # avgint_extra_columns
   avgint_extra_columns = []  # default
   for row in table_data['option'] :
      if row['option_name'] == 'avgint_extra_columns' :
         if row['option_value'] != None :
            avgint_extra_columns = row['option_value'].split()
   # ----------------------------------------------------------------------
   # data_extra_columns
   data_extra_columns = []  # default
   for row in table_data['option'] :
      if row['option_name'] == 'data_extra_columns' :
         if row['option_value'] != None :
            data_extra_columns = row['option_value'].split()
   # ----------------------------------------------------------------------
   # simulate_index, fit_simulate_index
   fit_simulate_index = False
   simulate_index     = None
   #
   # search for the last fit command in the log table
   connection = table_name2connection('log')
   log_data  = dismod_at.get_table_dict(connection, 'log')
   for i in range( len(log_data) ) :
      log_id        = len(log_data) - i - 1
      row           = log_data[log_id]
      if simulate_index == None and row['message_type'] == 'command' :
         message = row['message']
         tmp     = message.find('warm_start')
         if tmp > 0 :
            message = message[0 : tmp - 1]
         if message.startswith('begin fit fixed') :
            simulate_index = message[ len('begin fit fixed') : ].strip()
         if message.startswith('begin fit random') :
            simulate_index = message[ len('begin fit random') : ].strip()
         if message.startswith('begin fit both') :
            simulate_index = message[ len('begin fit both') : ].strip()
   if simulate_index == None :
      if have_table['fit_var'] :
         msg = 'Have fit_var table but cannot find '
         msg += 'fit command in the log table\n'
         assert False, msg
      simulate_index = ''
   fit_simulate_index = simulate_index != ''
   if fit_simulate_index and not have_table['data_sim'] :
      msg  = 'Previous fit command in log table used simulated data but\n'
      msg += 'cannot find data_sim table\n'
      assert False, msg
   if not have_table['data_sim'] :
      simulate_index = None
   else :
      if simulate_index == '' :
         simulate_index = 0
      else :
         simulate_index = int(simulate_index)
   # =========================================================================
   # option.csv
   # =========================================================================
   #
   # These values are extracted from the option table
   compress_age_size  = None
   compress_time_size = None
   #
   file_name  = os.path.join(database_dir, 'option.csv')
   csv_file   = open(file_name, 'w')
   header     = [ 'option_name', 'option_value' ]
   csv_writer = csv.DictWriter(csv_file, fieldnames=header)
   csv_writer.writeheader()
   #
   # option_list
   # This table must have the same values as in
   # devel/table/get_option_table.cpp
   # example/devel/table/get_option_table_xam.cpp
   option_list = [
      # BEGIN_SORT_THIS_LINE_PLUS_1
      [ "accept_after_max_steps_fixed",      "5"],
      [ "accept_after_max_steps_random",     "5"],
      [ "age_avg_split",                     ""],
      [ "asymptotic_rcond_lower",            "0.0"],
      [ "avgint_extra_columns",              ""],
      [ "bound_frac_fixed",                  "1e-2"],
      [ "bound_random",                      ""],
      [ "compress_interval",                 "0 0"],
      [ "data_extra_columns",                ""],
      [ "derivative_test_fixed",             "none"],
      [ "derivative_test_random",            "none"],
      [ "hold_out_integrand",                ""],
      [ "limited_memory_max_history_fixed",  "30"],
      [ "max_num_iter_fixed",                "100"],
      [ "max_num_iter_random",               "100"],
      [ "meas_noise_effect",                 "add_std_scale_all"],
      [ "method_random",                     "ipopt_random"],
      [ "ode_step_size",                     "10.0"],
      [ "other_database",                    ""],
      [ "other_input_table",                 ""],
      [ "parent_node_id",                    ""],
      [ "parent_node_name",                  ""],
      [ "print_level_fixed",                 "0"],
      [ "print_level_random",                "0"],
      [ "quasi_fixed",                       "true"],
      [ "random_seed",                       "0"],
      [ "rate_case",                         "iota_pos_rho_zero"],
      [ "splitting_covariate",               ""],
      [ "tolerance_fixed",                   "1e-8"],
      [ "tolerance_random",                  "1e-8"],
      [ "trace_init_fit_model",              "false"],
      [ "warn_on_stderr",                    "true"],
      [ "zero_sum_child_rate",               ""],
      [ "zero_sum_mulcov_group",             ""],
      # END_SORT_THIS_LINE_MINUS_1
   ]
   option_id = 0
   for row in table_data['option'] :
      found        = False
      for choice in option_list :
         if row['option_name'] == choice[0] :
            found = True
            choice[1] = row['option_value']
      if not found :
         msg  = 'Error in option table at option_id = ' + str(option_id)
         msg += '\noption_name = ' + row['option_name'] + ' is not valid'
         assert False, msg
      option_id += 1
   for row in option_list :
      if row[0] == 'parent_node_name' :
         node_table       = table_data['node']
         parent_node_name = node_table[parent_node_id]['node_name']
         if row[1] == "" or row[1] is None :
            row[1] = parent_node_name
         elif row[1] != parent_node_name :
            msg  = 'Error in option table parent_node_id has name '
            msg += parent_node_name
            msg += '\nbut parent_node_name is ' + row[1]
            assert False, msg
      row_out = { 'option_name' : row[0], 'option_value' : row[1] }
      if row[0] == 'hold_out_integrand' :
         if row[1] == "" or row[1] is None :
            hold_out_integrand_list = list()
         else :
            hold_out_integrand_list = row[1].split(' ')
      if row[0] == 'compress_interval' :
         compress_interval_list = row[1].split(' ')
         compress_age_size  = float( compress_interval_list[0] )
         compress_time_size = float( compress_interval_list[1] )
      csv_writer.writerow(row_out)
   csv_file.close()
   # =========================================================================
   # log.csv
   # =========================================================================
   file_name = os.path.join(database_dir, 'log.csv')
   csv_file  = open(file_name, 'w')
   header = ['message_type', 'table_name', 'row_id', 'unix_time', 'message']
   csv_writer = csv.DictWriter(csv_file, fieldnames=header)
   csv_writer.writeheader()
   for row in table_data['log'] :
      csv_writer.writerow(row)
   csv_file.close()
   # =========================================================================
   # age_avg.csv
   # =========================================================================
   file_name = os.path.join(database_dir, 'age_avg.csv')
   csv_file  = open(file_name, 'w')
   header = ['age']
   csv_writer = csv.DictWriter(csv_file, fieldnames=header)
   csv_writer.writeheader()
   for row in table_data['age_avg'] :
      csv_writer.writerow(row)
   csv_file.close()
   # =========================================================================
   # variable.csv
   # =========================================================================
   file_name = os.path.join(database_dir, 'variable.csv')
   csv_file  = open(file_name, 'w')
   #
   # compute: sam_avg, sam_std
   n_var   = len( table_data['var'] )
   sam_avg = n_var * [None]
   sam_std = n_var * [None]
   if have_table['sample'] :
      n_sample = len( table_data['sample'] )
      if n_sample % n_var != 0 :
         msg = 'length of sample table is not multiple of length var table'
         assert False, msg
      n_sample = n_sample / n_var
      sam_avg  = n_var * [0.]
      for row in table_data['sample'] :
         sample_index       = row['sample_index']
         var_id             = row['var_id']
         var_value          = row['var_value']
         sam_avg[var_id]   += var_value / float(n_sample)
      if n_sample > 1 :
         sam_std = n_var * [0.]
         for row in table_data['sample'] :
            sample_index     = row['sample_index']
            var_id           = row['var_id']
            var_value        = row['var_value']
            avg              = sam_avg[var_id]
            sq               = (var_value - avg) * (var_value - avg)
            sam_std[var_id] += sq
         for var_id in range( n_var ) :
            sum_sq          = sam_std[var_id];
            sam_std[var_id] = math.sqrt( sum_sq / float(n_sample - 1.0) )
   #
   header = [
      'var_id',
      'var_type',
      's_id',
      'm_id',
      'm_diff',
      'bound',
      'age',
      'time',
      'rate',
      'integrand',
      'covariate',
      'node',
      'group',
      'subgroup',
      'fixed',
      'depend',
      'fit_value',
      'start',
      'scale',
      'truth',
      'sam_avg',
      'sam_std',
      'res_value',
      'res_dage',
      'res_dtime',
      'lag_value',
      'lag_dage',
      'lag_dtime',
   ]
   root_list = 'lower,upper,mean,sim,std,eta,nu,density'.split(',')
   for extension in ['_v', '_a', '_t' ] :
      for root in root_list :
         header.append( root + extension )
   csv_writer = csv.DictWriter(csv_file, fieldnames=header)
   csv_writer.writeheader()
   var_id  = 0
   #
   # child_has_data
   child_has_data_set = child_has_data()
   #
   # group_id2name
   group_id2name = [ table_data['subgroup'][0]['group_name'] ]
   for row in table_data['subgroup'] :
      group_name = row['group_name']
      if group_name != group_id2name[-1] :
         group_id2name.append( group_name )
      #
   for row_in in table_data['var'] :
      row_out = dict()
      for field in header :
         row_out[field] = ''
      #
      row_out['var_id']    = var_id
      row_out['var_type']  = row_in['var_type']
      row_out['s_id']      = row_in['smooth_id']
      row_out['m_id']      = row_in['mulcov_id']
      row_out['age']       = table_lookup('age',  row_in['age_id'], 'age')
      row_out['time']      = table_lookup('time', row_in['time_id'], 'time')
      row_out['sam_avg']   = round_to(sam_avg[var_id], 3)
      row_out['sam_std']   = round_to(sam_std[var_id], 3)
      row_out['m_diff']    = table_lookup(
         'bnd_mulcov', row_in['mulcov_id'], 'max_cov_diff'
      )
      row_out['rate']      = table_lookup(
         'rate', row_in['rate_id'], 'rate_name'
      )
      row_out['integrand'] = table_lookup(
         'integrand', row_in['integrand_id'], 'integrand_name'
      )
      row_out['covariate'] = table_lookup(
         'covariate', row_in['covariate_id'], 'covariate_name'
      )
      row_out['node'] = table_lookup('node', row_in['node_id'], 'node_name')
      row_out['start'] = table_lookup('start_var', var_id, 'start_var_value')
      row_out['scale'] = table_lookup('scale_var', var_id, 'scale_var_value')
      #
      # fixed and group and sub
      if row_in['var_type'] == 'rate' :
         if row_in['node_id'] != parent_node_id :
            row_out['fixed'] = 'false'
         else :
            row_out['fixed'] = 'true'
      else :
         if row_in['subgroup_id'] != None :
            assert row_in['group_id'] == None
            subgroup_name = table_lookup(
               'subgroup', row_in['subgroup_id'], 'subgroup_name'
            )
            row_out['subgroup'] = subgroup_name
            row_out['fixed'] = 'false'
         else :
            assert row_in['group_id'] != None
            assert row_in['subgroup_id'] == None
            group_name  = group_id2name[ row_in['group_id'] ]
            row_out['group'] = group_name
            row_out['fixed'] = 'true'
      #
      # bound
      if row_out['fixed'] == 'true' :
         row_out['bound']    = table_lookup(
            'bnd_mulcov', row_in['mulcov_id'], 'max_mulcov'
         )
      else :
         bound_random = None
         for row in option_list :
            if row[0] == 'bound_random' :
               bound_random = row[1]
         assert bound_random is not None
         if row_in['node_id'] != None :
            if not row_in['node_id'] in child_has_data_set :
               bound_random = '0'
         row_out['bound'] = bound_random
      #
      # depend
      if have_table['depend_var'] :
         data_depend  = table_data['depend_var'][var_id]['data_depend']
         prior_depend = table_data['depend_var'][var_id]['prior_depend']
         assert data_depend in [ 0, 1 ]
         assert prior_depend in [ 0, 1 ]
         both = data_depend == 1 and prior_depend == 1
         if both :
            row_out['depend'] = 'both'
         elif data_depend == 1 :
            row_out['depend'] = 'data'
         elif prior_depend == 1 :
            row_out['depend'] = 'prior'
         else :
            row_out['depend'] = 'none'
      #
      # truth
      if have_table['truth_var'] :
         row_out['truth'] = \
             table_lookup('truth_var', var_id, 'truth_var_value')
      #
      # prior_sim table results
      if simulate_index != None :
         prior_sim_id = simulate_index * n_var + var_id
         row_out['sim_v'] = \
            table_lookup('prior_sim', prior_sim_id, 'prior_sim_value')
         row_out['sim_a'] = \
            table_lookup('prior_sim', prior_sim_id, 'prior_sim_dage')
         row_out['sim_t'] = \
            table_lookup('prior_sim', prior_sim_id, 'prior_sim_dtime')
      #
      # fit_var table results
      if have_table['fit_var'] :
         row_out['fit_value'] = \
             table_lookup('fit_var', var_id, 'fit_var_value')
         row_out['res_value'] = \
             table_lookup('fit_var', var_id, 'residual_value')
         row_out['res_dage']  = \
             table_lookup('fit_var', var_id, 'residual_dage')
         row_out['res_dtime'] = \
             table_lookup('fit_var', var_id, 'residual_dtime')
         row_out['lag_value'] = \
             table_lookup('fit_var', var_id, 'lagrange_value')
         row_out['lag_dage']  = \
             table_lookup('fit_var', var_id, 'lagrange_dage')
         row_out['lag_dtime'] = \
             table_lookup('fit_var', var_id, 'lagrange_dtime')
      #
      # information in prior table
      smooth_id = row_in['smooth_id']
      if row_in['var_type'] in ['mulstd_value','mulstd_dage','mulstd_dtime'] :
         prior_id_dict = {
            'value_prior_id' : None,
            'dage_prior_id'  : None,
            'dtime_prior_id' : None,
            'const_value'    : None,
         }
         smooth_id_dict = table_data['smooth'][smooth_id]
         key            = row_in['var_type'] + '_prior_id'
         prior_id       = smooth_id_dict[key]
         prior_id_dict['value_prior_id'] = prior_id
         get_prior_info(row_out, prior_id_dict, row_out['fixed'])
      else :
         age_id    = row_in['age_id']
         time_id   = row_in['time_id']
         for row in table_data['smooth_grid'] :
            match = row['smooth_id'] == smooth_id
            match = match and row['age_id'] == age_id
            match = match and row['time_id'] == time_id
            if match :
               prior_id_dict = row
               get_prior_info(row_out, prior_id_dict, row_out['fixed'])
      csv_writer.writerow(row_out)
      var_id += 1
   # ----------------------------------------------------------------------
   csv_file.close()
   # =========================================================================
   # data.csv
   # =========================================================================
   file_name = os.path.join(database_dir, 'data.csv')
   csv_file  = open(file_name, 'w')
   #
   header = ['data_id'] + data_extra_columns + [
      'child',
      'node',
      'group',
      'subgroup',
      'integrand',
      'weight',
      'age_lo',
      'age_up',
      'time_lo',
      'time_up',
      'd_out',
      's_out',
      'density',
      'eta',
      'nu',
      'ss',
      'meas_std',
      'meas_stdcv',
      'meas_sigma',
      'meas_value',
      'avgint',
      'residual'
   ]
   if simulate_index != None :
      header.append('sim_value')
   for row in table_data['covariate'] :
      header.append( row['covariate_name'] )
   csv_writer = csv.DictWriter(csv_file, fieldnames=header)
   csv_writer.writeheader()
   #
   row_out = dict()
   for field in header :
      row_out[field] = ''
   subset_id  = 0
   n_subset   = len( table_data['data_subset'] )
   for subset_row in table_data['data_subset'] :
      for field in header :
         row_out[field] = ''
      data_id   = subset_row['data_id']
      row_in    = table_data['data'][data_id]
      #
      # data_id
      row_out['data_id']     = data_id
      #
      # data_extra_columns
      for column in data_extra_columns :
         row_out[column] = row_in[column]
      #
      # columns that are directly copied from data table
      row_out['meas_std']    = convert2output( row_in['meas_std'] )
      row_out['eta']         = convert2output( subset_row['eta'] )
      row_out['nu']          = convert2output( subset_row['nu'] )
      row_out['ss']          = convert2output( subset_row['sample_size'] )
      row_out['meas_value']  = convert2output( row_in['meas_value'] )
      row_out['child']       = node_id2child( row_in['node_id'] )
      row_out['d_out']       = row_in['hold_out']
      row_out['s_out']       = subset_row['hold_out']
      #
      # integrand
      row_out['integrand'] = table_lookup(
         'integrand', row_in['integrand_id'], 'integrand_name'
      )
      # weight
      row_out['weight'] = table_lookup(
         'weight', row_in['weight_id'], 'weight_name'
      )
      # denisty
      row_out['density'] = table_lookup(
         'density', subset_row['density_id'], 'density_name'
      )
      # node
      row_out['node']  = table_lookup(
         'node', row_in['node_id'],  'node_name'
      )
      # group
      row_out['group'] = table_lookup(
         'subgroup', row_in['subgroup_id'], 'group_name'
      )
      # subgroup
      row_out['subgroup']   = table_lookup(
         'subgroup', row_in['subgroup_id'], 'subgroup_name'
      )
      #
      # age_lower, age_upper, time_lower, time_upper
      age_lower  = row_in['age_lower']
      age_upper  = row_in['age_upper']
      age_mid    = (age_lower + age_upper) / 2.0
      time_lower = row_in['time_lower']
      time_upper = row_in['time_upper']
      time_mid   = (time_lower + time_upper) / 2.0
      if age_upper - age_lower <= compress_age_size :
         age_lower  = age_mid
         age_upper  = age_mid
      if time_upper - time_lower <= compress_time_size :
         time_lower = time_mid
         time_upper = time_mid
      row_out['age_lo']  = convert2output(age_lower)
      row_out['age_up']  = convert2output(age_upper)
      row_out['time_lo'] = convert2output(time_lower)
      row_out['time_up'] = convert2output(time_upper)
      #
      # covariates
      covariate_id = 0
      for row in table_data['covariate'] :
         field_in  = 'x_' + str(covariate_id)
         field_out = row['covariate_name']
         reference = row['reference']
         if row_in[field_in] == None :
            row_out[field_out] = '0.0'
         else :
            row_out[field_out] = \
                convert2output(row_in[field_in] - reference)
         covariate_id += 1
      #
      # avgint, residual, meas_sigma
      if have_table['fit_var'] :
         row                 = table_data['fit_data_subset'][subset_id]
         row_out['avgint']   = convert2output( row['avg_integrand'] )
         row_out['residual'] = convert2output( row['weighted_residual'] )
         if not fit_simulate_index :
            meas_sigma = adjusted_meas_std(
               row_out['density']       ,
               subset_row['eta']        ,
               row_in['meas_value']     ,
               row['avg_integrand']     ,
               row['weighted_residual']
            )
            row_out['meas_sigma'] = convert2output(meas_sigma)
      #
      # meas_std, meas_stdcv
      if row_in['meas_std'] == None :
         assert row_out['density'] == 'binomial'
         if have_table['fit_var'] :
            n        = int( row_in['sample_size'] )
            p        = float( row_out['avgint'] )
            variance = n * p
            meas_std = math.sqrt( variance / (n * n) )
            row_out['meas_std']   = convert2output( meas_std )
            row_out['meas_stdcv'] = row_out['meas_std']
      else :
         meas_cv     = minimum_meas_cv[ row_in['integrand_id' ] ]
         meas_stdcv  =  meas_cv * abs( row_in['meas_value'] )
         meas_stdcv  = max( row_in['meas_std'], meas_stdcv)
         row_out['meas_stdcv'] = convert2output( meas_stdcv )
      #
      # sim_value
      if simulate_index != None :
         data_sim_id =  n_subset * simulate_index + subset_id
         sim_value = table_data['data_sim'][data_sim_id]['data_sim_value']
         row_out['sim_value'] = convert2output( sim_value )
      #
      csv_writer.writerow(row_out)
      subset_id += 1
   csv_file.close()
   # =========================================================================
   # predict.csv
   # =========================================================================
   if have_table['predict'] :
      file_name = os.path.join(database_dir, 'predict.csv')
      csv_file  = open(file_name, 'w')
      #
      header = ['avgint_id'] + avgint_extra_columns + [
         's_index',
         'avgint',
         'age_lo',
         'age_up',
         'time_lo',
         'time_up',
         'integrand',
         'weight',
         'node',
         'group',
         'subgroup',
      ]
      for row in table_data['covariate'] :
         header.append( row['covariate_name'] )
      csv_writer = csv.DictWriter(csv_file, fieldnames=header)
      csv_writer.writeheader()
      #
      for predict_row in table_data['predict'] :
         row_out     = dict()
         #
         avgint_id   = predict_row['avgint_id']
         avgint_row  = table_data['avgint'][avgint_id]
         #
         # avgint_id
         row_out['avgint_id'] = avgint_id
         #
         # avgint_extra_columns
         for column in avgint_extra_columns :
            row_out[column] = avgint_row[column]
         #
         # s_index
         row_out['s_index'] = predict_row['sample_index']
         #
         # avgint, age_lo, age_up, time_lo, time_up
         row_out['avgint']  = convert2output( predict_row['avg_integrand'] )
         row_out['age_lo']  = avgint_row['age_lower']
         row_out['age_up']  = avgint_row['age_upper']
         row_out['time_lo'] = avgint_row['time_lower']
         row_out['time_up'] = avgint_row['time_upper']
         #
         # integrand
         row_out['integrand'] = table_lookup(
            'integrand', avgint_row['integrand_id'], 'integrand_name'
         )
         # weight
         row_out['weight']    = table_lookup(
            'weight', avgint_row['weight_id'], 'weight_name'
         )
         # node
         row_out['node']      = table_lookup(
            'node', avgint_row['node_id'], 'node_name'
         )
         # group
         row_out['group'] = table_lookup(
            'subgroup', row_in['subgroup_id'], 'group_name'
         )
         # subgroup
         row_out['subgroup']   = table_lookup(
            'subgroup', row_in['subgroup_id'], 'subgroup_name'
         )
         # covariates
         covariate_id = 0
         for row in table_data['covariate'] :
            field_in  = 'x_' + str(covariate_id)
            field_out = row['covariate_name']
            reference = row['reference']
            if avgint_row[field_in] == None :
               row_out[field_out] = '0.0'
            else :
               row_out[field_out] = \
                  convert2output(avgint_row[field_in] - reference)
            covariate_id += 1
         #
         csv_writer.writerow(row_out)
      csv_file.close()
   # =========================================================================
   # hes_fixed.csv
   # =========================================================================
   if have_table['hes_fixed'] :
      file_name = os.path.join(database_dir, 'hes_fixed.csv')
      csv_file  = open(file_name, 'w')
      #
      header = ['row_var_id', 'col_var_id', 'hes_fixed_value']
      csv_writer = csv.DictWriter(csv_file, fieldnames=header)
      csv_writer.writeheader()
      for row in table_data['hes_fixed'] :
         csv_writer.writerow(row)
      csv_file.close()
   # =========================================================================
   # hes_random.csv
   # =========================================================================
   if have_table['hes_random'] :
      file_name = os.path.join(database_dir, 'hes_random.csv')
      csv_file  = open(file_name, 'w')
      #
      header = ['row_var_id', 'col_var_id', 'hes_random_value']
      csv_writer = csv.DictWriter(csv_file, fieldnames=header)
      csv_writer.writeheader()
      for row in table_data['hes_random'] :
         csv_writer.writerow(row)
      csv_file.close()
   # =========================================================================
   # trace_fixed.csv
   # =========================================================================
   if have_table['trace_fixed'] :
      file_name = os.path.join(database_dir, 'trace_fixed.csv')
      csv_file  = open(file_name, 'w')
      #
      header = [
         'iter',
         'obj_value',
         'inf_pr',
         'inf_du',
         'mu',
         'd_norm',
         'reg_size',
         'alpha_du',
         'alpha_pr',
         'ls_trials',
         'restoration'
      ]
      csv_writer = csv.DictWriter(csv_file, fieldnames=header)
      csv_writer.writeheader()
      for row in table_data['trace_fixed'] :
         row['reg_size'] = row['regularization_size']
         del row['regularization_size']
         for key in row :
            row[key] = convert2output(row[key])
         csv_writer.writerow(row)
      csv_file.close()
   # =========================================================================
   # mixed_info.csv
   # =========================================================================
   if have_table['mixed_info'] :
      file_name = os.path.join(database_dir, 'mixed_info.csv')
      csv_file  = open(file_name, 'w')
      #
      header = [
         'mixed_name',
         'mixed_value'
      ]
      csv_writer = csv.DictWriter(csv_file, fieldnames=header)
      csv_writer.writeheader()
      for row in table_data['mixed_info'] :
         csv_writer.writerow(row)
      csv_file.close()
