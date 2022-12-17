# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin csv2db_command$$ $newlinech #$$
# $spell
#  Csv
#  Dismod
#  str
#  pini
#  integrands
#  Sincidence
#  mtexcess
#  mtother
#  Tincidence
#  mtspecific
#  mtall
#  mtstandard
#  relrisk
#  std
#  mtwith
#  dismodat.py
#  std
# $$
#
# $nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section Conversion of a Csv File to a Dismod_at Database$$
#
# $head Deprecated$$
# This command was deprecated on 2021-12-05.
#
# $head Syntax$$
#
# $subhead As Program$$
# $codei%dismodat.py %database% %csv2db% %configure_csv% %measure_csv%$$
#
# $subhead As Python Function$$
# $codei%dismod_at.csv2db_command(%database%, %configure_csv%, %measure_csv%)%$$
#
# $head See Also$$
# $cref get_started$$
#
# $head Example$$
# This command has limited capability and is only meant as an example
# to help one get started using dismod_at.
# See $cref user_csv2db.py$$ for an example that uses this command.
#
# $head mtall$$
# The all cause mortality data $code mtall$$
# in the $icode measure_csv$$ file has special meaning.
# We assume it has been converted to constraints on other cause
# mortality; see $cref/mtother/csv2db_command/mtother/$$ below.
# For this reason, it should not be included when fitting,
# and is only in $icode measure_csv$$ as a check that the desired values
# are satisfied (by checking residuals in a data fit).
# For this reason, $cref/hold_out/csv2db_command/hold_out/$$ must be
# one for all the $code mtall$$ data; i.e., it is not included during a fit.
#
# $head mtother$$
#
# $subhead Constraint$$
# The other cause mortality data $code mtother$$
# in the $icode measure_csv$$ file has special meaning.
# It is intended to represent the all cause mortality data
# as constraints on other cause mortality in the model.
# To be specific, the $cref/meas_value/csv2db_command/meas_value/$$
# is a constraint on $latex \omega$$ at the corresponding age-time pairs.
#
# $subhead Age-Time Pairs$$
# Each row with $icode%integrand% = mtother%$$ must have
# $icode%age_lower% == %age_upper%$$ and
# $icode%time_lower% == %time_upper%$$.
#
# $subhead Rectangular Grid$$
# The $code mtother$$ data must be specified on a rectangular grid; i.e.,
# each age that appears, appears in one and only one row for every time
# that appears.
# This property can also be stated as
# each time that appears, appears in one and only one row for every age
# that appears.
#
# $subhead hold_out$$
# The $cref/hold_out/csv2db_command/hold_out/$$ must be
# one for all the $code mtother$$ data because
# it is a constraint, not data, during a fit.
#
# $subhead Rate Grid$$
# All of the
# $cref/non zero rates/csv2db_command/configure_csv/non_zero_rates/$$
# use the age-time grid corresponding the $code mtother$$ data.
# In other words, they are modeled as piecewise bilinear between the
# age-time points at which $code mtother$$ is specified.
#
# $head Predictions$$
# The $cref avgint_table$$ is set up so that
# $cref/predictions/predict_command/$$ for the integrands
# $cref/Sincidence/csv2db_command/integrand/Sincidence/$$,
# $cref/remission/csv2db_command/integrand/remission/$$,
# $cref/mtexcess/csv2db_command/integrand/mtexcess/$$,
# corresponding to the value of the rates iota, rho, chi on the
# $cref/rectangular grid/csv2db_command/mtother/Rectangular Grid/$$.
# Only the non-zero rates are included.
# Predictions for
# $cref/prevalence/csv2db_command/integrand/prevalence/$$ in the
# rectangular grid are also included.
#
# $head database$$
# This argument
# is an $code str$$ containing the name of the dismod_at database
# file that is written by this command.
#
# $head configure_csv$$
# This argument
# is an $code str$$ containing the configuration file name and must
# end with the $code .csv$$ extension.
# The first row contains the following column names
# $code name$$, $code value$$.
# Column names that begin with $code c_$$ are comments
# and will not be used by future versions of $code csv2db$$.
# The configuration options are documented by the corresponding
# name below.
#
# $subhead non_zero_rates$$
# The $icode value$$ in this row is a list rates that are non-zero
# in the model.
# The possible rates are
# $code pini$$, $code iota$$, $code rho$$, $code chi$$, $code omega$$.
# The rates in the list are separated by a single space
# and $code omega$$ must appear in the list.
# There is no default value for this value; i.e., it must appear.
#
# $head measure_csv$$
# is an $code str$$ containing the data file name
# and must end with the $code .csv$$ extension.
# Each row of the data file corresponds to one data point.
# The first row of the file contains the column names.
# The other rows correspond to data points.
# Each column of the necessary columns in the data file is documented
# under its column name below.
# Column names that begin with $code c_$$ are comments
# and will not be used by future versions of $code csv2db$$.
#
# $head integrand$$
# This column of $icode measure_csv$$ contains
# one of the valid integrands:
#
# $subhead Sincidence$$
# The incidence rate relative to susceptible population:
# $latex \iota $$.
#
# $subhead remission$$
# The remission rate:
# $latex \rho $$.
#
# $subhead mtexcess$$
# The excess mortality rate:
# $latex \chi $$.
#
# $subhead mtother$$
# The other cause mortality rate:
# $latex \omega $$.
#
# $subhead mtwith$$
# The with condition mortality rate:
# $latex \omega + \chi $$.
#
# $subhead susceptible$$
# The susceptible fraction of the population:
# $latex S $$.
#
# $subhead withC$$
# The with condition fraction of the population:
# $latex C $$.
#
# $subhead prevalence$$
# The prevalence of the condition:
# $latex P = C / [ S + C ] $$.
#
# $subhead Tincidence$$
# The incidence rate relative to the total population:
# $latex \iota [ 1 - P ] $$.
#
# $subhead mtspecific$$
# The cause specific mortality rate:
# $latex \chi P $$.
#
# $subhead mtall$$
# The all cause mortality rate:
# $latex \omega + \chi P $$.
#
# $subhead mtstandard$$
# The standardized mortality ratio:
# $latex [ \omega + \chi ] / [ \omega + \chi P ] $$.
#
# $subhead relrisk$$
# The relative risk:
# $latex [ \omega + \chi ] / \omega $$.
#
# $head age_lower$$
# This column of $icode measure_csv$$ contains
# The initial age for averaging the integrand for this row; $latex b$$.
#
# $head age_upper$$
# This column of $icode measure_csv$$ contains
# the final age for averaging the integrand for this row; $latex c$$.
#
# $head time_lower$$
# This column of $icode measure_csv$$ contains
# the initial time for averaging the integrand for this row; $latex r$$.
#
# $head time_upper$$
# This column of $icode measure_csv$$ contains
# the final time for averaging the integrand for this row; $latex s$$.
#
# $head meas_value$$
# This column of $icode measure_csv$$ contains
# the value of the average integrand plus measurement noise
# $latex \[
#  e + \frac{1}{c-b} \frac{1}{s-r} \int_b^c \int_r^s I(a , t) \; da \; dt
# \] $$
# where $latex e$$ is the measurement noise,
# $latex a$$ and $latex t$$ are the age and time integration variables,
# and $latex I(a, t)$$ is the value of the integrand for this row.
#
# $head meas_std$$
# This column of $icode measure_csv$$ contains
# the standard deviation of the measurement noise $latex e$$.
#
# $head hold_out$$
# This column of $icode measure_csv$$ contains
# either zero or one. If it is one, this row is included
# dismod_at fits. Otherwise it is excluded.
# In either case, the residuals are computed for this row.
#
# $end
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
