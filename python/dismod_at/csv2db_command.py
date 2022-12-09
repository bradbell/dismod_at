# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin csv2db_command}
# {xrst_spell
#     dt
#     mtall
#     mtexcess
#     mtother
#     mtspecific
#     mtstandard
#     mtwith
#     relrisk
#     tincidence
# }
# {xrst_comment_ch #}
#
# Conversion of a Csv File to a Dismod_at Database
# ################################################
#
# Deprecated
# **********
# This command was deprecated on 2021-12-05.
#
# Syntax
# ******
#
# As Program
# ==========
# ``dismodat.py`` *database* *csv2db* *configure_csv* *measure_csv*
#
# As Python Function
# ==================
# ``dismod_at.csv2db_command`` ( *database* , *configure_csv* , *measure_csv* )
#
# See Also
# ********
# :ref:`get_started-name`
#
# Example
# *******
# This command has limited capability and is only meant as an example
# to help one get started using dismod_at.
# See :ref:`user_csv2db.py-name` for an example that uses this command.
#
# mtall
# *****
# The all cause mortality data ``mtall``
# in the *measure_csv* file has special meaning.
# We assume it has been converted to constraints on other cause
# mortality; see :ref:`csv2db_command@mtother` below.
# For this reason, it should not be included when fitting,
# and is only in *measure_csv* as a check that the desired values
# are satisfied (by checking residuals in a data fit).
# For this reason, :ref:`csv2db_command@hold_out` must be
# one for all the ``mtall`` data; i.e., it is not included during a fit.
#
# mtother
# *******
#
# Constraint
# ==========
# The other cause mortality data ``mtother``
# in the *measure_csv* file has special meaning.
# It is intended to represent the all cause mortality data
# as constraints on other cause mortality in the model.
# To be specific, the :ref:`csv2db_command@meas_value`
# is a constraint on :math:`\omega` at the corresponding age-time pairs.
#
# Age-Time Pairs
# ==============
# Each row with *integrand* = ``mtother`` must have
# *age_lower* == *age_upper* and
# *time_lower* == *time_upper* .
#
# Rectangular Grid
# ================
# The ``mtother`` data must be specified on a rectangular grid; i.e.,
# each age that appears, appears in one and only one row for every time
# that appears.
# This property can also be stated as
# each time that appears, appears in one and only one row for every age
# that appears.
#
# hold_out
# ========
# The :ref:`csv2db_command@hold_out` must be
# one for all the ``mtother`` data because
# it is a constraint, not data, during a fit.
#
# Rate Grid
# =========
# All of the
# :ref:`non zero rates<csv2db_command@configure_csv@non_zero_rates>`
# use the age-time grid corresponding the ``mtother`` data.
# In other words, they are modeled as piecewise bilinear between the
# age-time points at which ``mtother`` is specified.
#
# Predictions
# ***********
# The :ref:`avgint_table-name` is set up so that
# :ref:`predictions<predict_command-name>` for the integrands
# :ref:`csv2db_command@integrand@Sincidence` ,
# :ref:`csv2db_command@integrand@remission` ,
# :ref:`csv2db_command@integrand@mtexcess` ,
# corresponding to the value of the rates iota, rho, chi on the
# :ref:`csv2db_command@mtother@Rectangular Grid` .
# Only the non-zero rates are included.
# Predictions for
# :ref:`csv2db_command@integrand@prevalence` in the
# rectangular grid are also included.
#
# database
# ********
# This argument
# is an ``str`` containing the name of the dismod_at database
# file that is written by this command.
#
# configure_csv
# *************
# This argument
# is an ``str`` containing the configuration file name and must
# end with the ``.csv`` extension.
# The first row contains the following column names
# ``name`` , ``value`` .
# Column names that begin with ``c_`` are comments
# and will not be used by future versions of ``csv2db`` .
# The configuration options are documented by the corresponding
# name below.
#
# non_zero_rates
# ==============
# The *value* in this row is a list rates that are non-zero
# in the model.
# The possible rates are
# ``pini`` , ``iota`` , ``rho`` , ``chi`` , ``omega`` .
# The rates in the list are separated by a single space
# and ``omega`` must appear in the list.
# There is no default value for this value; i.e., it must appear.
#
# measure_csv
# ***********
# is an ``str`` containing the data file name
# and must end with the ``.csv`` extension.
# Each row of the data file corresponds to one data point.
# The first row of the file contains the column names.
# The other rows correspond to data points.
# Each column of the necessary columns in the data file is documented
# under its column name below.
# Column names that begin with ``c_`` are comments
# and will not be used by future versions of ``csv2db`` .
#
# integrand
# *********
# This column of *measure_csv* contains
# one of the valid integrands:
#
# Sincidence
# ==========
# The incidence rate relative to susceptible population:
# :math:`\iota`.
#
# remission
# =========
# The remission rate:
# :math:`\rho`.
#
# mtexcess
# ========
# The excess mortality rate:
# :math:`\chi`.
#
# mtother
# =======
# The other cause mortality rate:
# :math:`\omega`.
#
# mtwith
# ======
# The with condition mortality rate:
# :math:`\omega + \chi`.
#
# susceptible
# ===========
# The susceptible fraction of the population:
# :math:`S`.
#
# withC
# =====
# The with condition fraction of the population:
# :math:`C`.
#
# prevalence
# ==========
# The prevalence of the condition:
# :math:`P = C / [ S + C ]`.
#
# Tincidence
# ==========
# The incidence rate relative to the total population:
# :math:`\iota [ 1 - P ]`.
#
# mtspecific
# ==========
# The cause specific mortality rate:
# :math:`\chi P`.
#
# mtall
# =====
# The all cause mortality rate:
# :math:`\omega + \chi P`.
#
# mtstandard
# ==========
# The standardized mortality ratio:
# :math:`[ \omega + \chi ] / [ \omega + \chi P ]`.
#
# relrisk
# =======
# The relative risk:
# :math:`[ \omega + \chi ] / \omega`.
#
# age_lower
# *********
# This column of *measure_csv* contains
# The initial age for averaging the integrand for this row; :math:`b`.
#
# age_upper
# *********
# This column of *measure_csv* contains
# the final age for averaging the integrand for this row; :math:`c`.
#
# time_lower
# **********
# This column of *measure_csv* contains
# the initial time for averaging the integrand for this row; :math:`r`.
#
# time_upper
# **********
# This column of *measure_csv* contains
# the final time for averaging the integrand for this row; :math:`s`.
#
# meas_value
# **********
# This column of *measure_csv* contains
# the value of the average integrand plus measurement noise
#
# .. math::
#
#  e + \frac{1}{c-b} \frac{1}{s-r} \int_b^c \int_r^s I(a , t) \; da \; dt
#
# where :math:`e` is the measurement noise,
# :math:`a` and :math:`t` are the age and time integration variables,
# and :math:`I(a, t)` is the value of the integrand for this row.
#
# meas_std
# ********
# This column of *measure_csv* contains
# the standard deviation of the measurement noise :math:`e`.
#
# hold_out
# ********
# This column of *measure_csv* contains
# either zero or one. If it is one, this row is included
# dismod_at fits. Otherwise it is excluded.
# In either case, the residuals are computed for this row.
#
# {xrst_end csv2db_command}
# -----------------------------------------------------------------------------
def constant_weight_fun(a, t) :
   return 1.0
#
def uniform01_fun(a, t) :
   v  = 'uniform01'
   da = 'uniform'
   dt = 'uniform'
   return (v, da, dt)
#
def omega_constraint_fun(a, t, age_grid, time_grid, omega_grid) :
   i  = age_grid.index(a)
   j  = time_grid.index(t)
   v  = omega_grid[i, j]
   da = 'uniform'
   dv = 'uniform'
   return (v, da, dv)
# -----------------------------------------------------------------------------
def csv2db_command(database, configure_csv, measure_csv) :
   import sys
   import os
   import csv
   import copy
   import numpy
   import scipy.integrate
   import dismod_at
   # -------------------------------------------------------------------------
   for file_name in [ configure_csv, measure_csv ] :
      if not file_name.endswith('.csv') :
         msg  = 'csv2db: configure_csv file ' + file_name
         msg += ' does not end with .csv'
         sys.exit(msg)
      if not os.path.isfile(file_name) :
         msg = 'csv2db: cannot find configure_csv file ' + file_name
         sys.exit(msg)
   # -------------------------------------------------------------------------
   # file_option
   file_ptr    = open(configure_csv, 'r')
   reader      = csv.DictReader(file_ptr)
   file_option = dict()
   for row in reader :
      name              = row['name']
      value             = row['value']
      file_option[name] = value
   file_ptr.close()
   # required fields
   required = [ 'non_zero_rates' ]
   for name in required :
      if not name in file_option :
         msg  = 'csv2db: ' + name
         msg += ' not in configure_csv file ' + configure_csv
         sys.exit(msg)
   #
   if 'omega' not in file_option['non_zero_rates'] :
      msg = 'csv2db: omega not in non_zero_rates in ' + configure_csv
      sys.exit(msg)
   # -------------------------------------------------------------------------
   # file_data
   file_ptr   = open(measure_csv, 'r')
   reader     = csv.DictReader(file_ptr)
   file_data  = list()
   for row in reader :
      file_data.append( copy.copy(row) )
   file_ptr.close()
   #
   line  = 1
   mtother_found = False
   for row in file_data :
      line += 1
      row['age_lower']  = float( row['age_lower']  )
      row['age_upper']  = float( row['age_upper']  )
      row['time_lower'] = float( row['time_lower'] )
      row['time_upper'] = float( row['time_upper'] )
      row['meas_value'] = float( row['meas_value'] )
      row['meas_std']   = float( row['meas_std']   )
      row['hold_out']   = int(  row['hold_out']   )
      #
      if row['integrand'] == 'mtother' :
         mtother_found = True
         if row['age_lower'] != row['age_upper'] :
            msg  = 'csv2db: line ' + str(line)
            msg += ' of measure_csv ' + measure_csv + '\n'
            msg += 'age_lower not equal age_upper for mtother data.'
            sys.exit(msg)
         if row['time_lower'] != row['time_upper'] :
            msg  = 'csv2db: line ' + str(line)
            msg += ' of file ' + measure_csv + '\n'
            msg += 'time_lower not equal time_upper for mtother data.'
            sys.exit(msg)
      if row['hold_out'] == 0 :
         if row['integrand'] == 'mtall' or row['integrand'] == 'mtother' :
            msg  = 'csv2db: line ' + str(line)
            msg += ' of file ' + measure_csv + '\n'
            msg += 'hold_out is 0 and integrand is ' + row['integrand']
            sys.exit(msg)
      else :
         if row['hold_out'] != 1 :
            msg  = 'csv2db: line ' + str(line)
            msg += ' of file ' + measure_csv + '\n'
            msg += 'hold_out is not 0 or 1'
            sys.exit(msg)
   if not mtother_found :
      msg = 'csv2db: no mtother data in ' + measure_csv
      sys.exit(msg)
   # -------------------------------------------------------------------------
   # age_grid, time_grid, omega_grid
   age_set  = set()
   time_set = set()
   for row in file_data :
      if row['integrand'] == 'mtother' :
         age_set.add(  row['age_lower']  )
         time_set.add( row['time_lower'] )
   age_grid  = sorted(age_set)
   time_grid = sorted(time_set)
   n_age          = len(age_grid)
   n_time         = len(time_grid)
   count          = numpy.zeros( (n_age, n_time), dtype=int )
   omega_grid     = numpy.zeros( (n_age, n_time), dtype=float )
   for row in file_data :
      if row['integrand'] == 'mtother' :
         age        = row['age_lower']
         time       = row['time_lower']
         age_index  = age_grid.index(age)
         time_index = time_grid.index(time)
         omega_grid[age_index, time_index] = row['meas_value']
         count[age_index, time_index] += 1
   if not (count == 1).all() :
      msg = 'csv2db: the mtother data in ' + measure_csv
      msg += ' is not on a rectangular grid'
      sys.exit(msg)
   # -------------------------------------------------------------------------
   # age_min, age_max
   age_min = + float('inf')
   age_max = - float('inf')
   for row in file_data :
      age_min = min(age_min, row['age_lower'], row['age_upper'])
      age_max = max(age_max, row['age_lower'], row['age_upper'])
   # -------------------------------------------------------------------------
   # time_min, time_max
   time_min = + float('inf')
   time_max = - float('inf')
   for row in file_data :
      time_min = min(time_min, row['time_lower'], row['time_upper'])
      time_max = max(time_max, row['time_lower'], row['time_upper'])
   # -------------------------------------------------------------------------
   # age_list, time_list
   age_list  = copy.copy( age_grid )
   time_list = copy.copy( time_grid )
   #
   if age_min < age_list[0] :
      age_list.insert(0, age_min)
   if time_min < time_list[0] :
      time_list.insert(0, time_min)
   #
   if age_max > age_list[-1] :
      age_list.append(age_max)
   if time_max > time_list[-1] :
      time_list.append(time_max)
   # -------------------------------------------------------------------------
   # integrand_table
   integrand_table = [
      { 'name': 'Sincidence',     'minimum_meas_cv': 0.0 },
      { 'name': 'remission',      'minimum_meas_cv': 0.0 },
      { 'name': 'mtexcess',       'minimum_meas_cv': 0.0 },
      { 'name': 'mtother',        'minimum_meas_cv': 0.0 },
      { 'name': 'mtwith',         'minimum_meas_cv': 0.0 },
      { 'name': 'susceptible',    'minimum_meas_cv': 0.0 },
      { 'name': 'withC',          'minimum_meas_cv': 0.0 },
      { 'name': 'prevalence',     'minimum_meas_cv': 0.0 },
      { 'name': 'Tincidence',     'minimum_meas_cv': 0.0 },
      { 'name': 'mtspecific',     'minimum_meas_cv': 0.0 },
      { 'name': 'mtall',          'minimum_meas_cv': 0.0 },
      { 'name': 'mtstandard',     'minimum_meas_cv': 0.0 },
      { 'name': 'relrisk',        'minimum_meas_cv': 0.0 }
   ]
   # -------------------------------------------------------------------------
   # node_table
   node_table = [ { 'name':'no_name', 'parent': '' } ]
   # -------------------------------------------------------------------------
   # weight_table
   fun          = constant_weight_fun
   weight_table = [
      { 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
   ]
   # -------------------------------------------------------------------------
   # covariate_table
   covariate_table = list()
   # -------------------------------------------------------------------------
   # avgint_table
   rate2integrand = {
      'iota':  'Sincidence',
      'rho':   'remission',
      'chi':   'mtexcess',
      'omega': 'mtother'
   }
   non_zero_rates = file_option['non_zero_rates'].split()
   avgint_table   = list()
   for rate in non_zero_rates + [ 'prevalence' ]:
      if rate != 'pini' :
         if rate == 'prevalence' :
            integrand = 'prevalence'
         else :
            integrand = rate2integrand[rate]
         for age in age_grid :
            for time in time_grid :
               row = {
                     'integrand': integrand,
                     'node':      'no_name',
                     'subgroup':  'world',
                     'weight':    'constant',
                     'age_lower':  age,
                     'age_upper':  age,
                     'time_lower': time,
                     'time_upper': time
               }
               avgint_table.append(row)
   # -------------------------------------------------------------------------
   # data_table
   data_table = list()
   copy_column  = [
      'integrand',
      'hold_out',
      'age_lower',
      'age_upper',
      'time_lower',
      'time_upper',
      'meas_value',
      'meas_std'
   ]
   for row in file_data :
      if row['integrand'] != 'mtother' :
         data_row = dict()
         for field in copy_column :
            data_row[field] = row[field]
         data_row['node']    = 'no_name'
         data_row['weight']  = 'constant'
         data_row['density'] = 'gaussian'
         data_table.append( data_row )
   # -------------------------------------------------------------------------
   # prior_table
   prior_table = [
      {
         'name':      'uniform01',
         'lower':      1e-10,
         'upper':      1.0,
         'mean':       0.01,
         'density':    'uniform'
      }, {
         'name':      'uniform',
         'lower':      None,
         'upper':      None,
         'mean':       0.0,
         'density':    'uniform'
      }
   ]
   # -------------------------------------------------------------------------
   # smooth_table
   #
   # age_grid = age_list[start], ... age_list[start + len(age_grid) - 1]
   start = 0
   if age_list[0] != age_grid[0] :
      start = 1
   age_id = range(start, len(age_grid) + start)
   #
   # time_grid = time_list[start], ... time_list[start + len(time_grid) - 1]
   start = 0
   if time_list[0] != time_grid[0] :
      start = 1
   time_id = range(start, len(time_grid) + start)
   #
   # omega
   def omega_fun(a, t) :
      return omega_constraint_fun(a, t, age_grid, time_grid, omega_grid)
   smooth_table  = list()
   non_zero_rates = file_option['non_zero_rates'].split()
   for rate in non_zero_rates :
      if rate == 'omega' :
         smooth_table.append( {
            'name':     rate,
            'age_id':   age_id,
            'time_id':  time_id,
            'fun':      omega_fun,
      } )
      elif rate == 'pini' :
         smooth_table.append( {
            'name':     rate,
            'age_id':  [ age_id[0] ],
            'time_id':  time_id,
            'fun':      uniform01_fun
         } )
      else :
         smooth_table.append( {
            'name':     rate,
            'age_id':   age_id,
            'time_id':  time_id,
            'fun':      uniform01_fun,
      } )
   # -------------------------------------------------------------------------
   # rate_table
   rate_table     = list()
   for rate in non_zero_rates :
      rate_table.append( { 'name': rate, 'parent_smooth': rate } )
   # -------------------------------------------------------------------------
   # option_table
   option_table = [ { 'name': 'parent_node_name',  'value': 'no_name' } ]
   #
   non_zero_rates = file_option['non_zero_rates'].split()
   if 'iota' in non_zero_rates :
      if 'rho' in non_zero_rates :
         value = 'iota_pos_rho_pos'
      else :
         value = 'iota_pos_rho_zero'
   else :
      if 'rho' in non_zero_rates :
         value = 'iota_zero_rho_pos'
      else :
         value = 'iota_zero_rho_zero'
   option_table.append( { 'name': 'rate_case',  'value': value } )
   # -------------------------------------------------------------------------
   # empty tables
   nslist_table = list()
   mulcov_table = list()
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   # -------------------------------------------------------------------------
   file_name = database
   dismod_at.create_database(
      file_name,
      age_list,
      time_list,
      integrand_table,
      node_table,
      subgroup_table,
      weight_table,
      covariate_table,
      avgint_table,
      data_table,
      prior_table,
      smooth_table,
      nslist_table,
      rate_table,
      mulcov_table,
      option_table
   )
