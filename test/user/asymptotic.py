# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $section A Simulate Data Speed Test$$
#
# $head random_seed$$
# is a non-negative integer specifying
# the $cref/random_seed/option_table/random_seed/$$ used during the simulation.
random_seed = '0'
#
# $head quasi_fixed$$
# This argument is $code true$$ or $code false$$ and specifies
# $cref/quasi_fixed/option_table/Optimize Fixed Only/quasi_fixed/$$
# in the option table.
quasi_fixed = 'false'
#
# $head ode_step_size$$
# This argument is a floating point value and specifies the
# $cref/
#  ode_step_size/
#  option_table/
#  Age Average Grid/
#  ode_step_size
# /$$
# in the option table.
# The total work for the test increases with proportional to the
# square of this step size
ode_step_size = '10'
#
# $head Fixed Effects$$
# $srccode%py%
iota_parent_true          = 0.05
# %$$
#
# $subhead iota_parent_true$$
# the value of $cref/iota/rate_table/rate_name/iota/$$ corresponding
# to the parent node.
#
# $head eta$$
# value of the offset $cref/eta/statistic/Notation/eta/$$ in the
# log transformation:
# $srccode%py%
eta = 1e-6
# %$$
#
# $head measure_cv$$
# the coefficient of variation for the simulated measurement noise.
# If you use a larger $icode measure_cv$$ you will probably need
# a larger number of data points; see $icode n_data$$ below.
# $srccode%py%
measure_cv = 0.001
# %$$
#
# $head n_data$$
# number of simulated data values.
# $srccode%py%
n_data = 2
# %$$
#
# $head age_list$$
# This following is both the $cref age_table$$ and the age
# grid points for the parent rate smoothing of $icode iota$$.
# The child rate smoothing has a grid point at the minimum
# and maximum age below.
# $srccode%py%
age_list = [ 0.0, 100.0 ]
# %$$
#
# $head time_list$$
# This following is both the $cref time_table$$ and the time
# grid points for the parent rate smoothing of $icode iota$$.
# The child rate smoothing has a grid point at the minimum
# and maximum time below.
# $srccode%py%
time_list = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
# %$$
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# values used to simulate data
# ------------------------------------------------------------------------
import sys
import os
import time
import numpy
test_program = 'test/user/asymptotic.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
   usage  = 'python3 ' + test_program + '\n'
   usage += 'where python3 is the python 3 program on your system\n'
   usage += 'and working directory is the dismod_at distribution directory\n'
   sys.exit(usage)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
   sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/tst/user directory
if not os.path.exists('build/test/user') :
   os.makedirs('build/test/user')
os.chdir('build/test/user')
# ------------------------------------------------------------------------
def example_db (file_name) :
   # note that the a, t values are not used for this case
   def fun_rate_child(a, t) :
      return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
   def fun_iota_parent(a, t) :
      return ('prior_iota_parent', 'prior_gauss_zero', 'prior_gauss_zero')
   import copy
   import dismod_at
   import math
   # ----------------------------------------------------------------------
   # age table: uses age_list defined above
   #
   # time table: uses time_list defined above
   #
   # integrand table:
   integrand_table = [
      { 'name':'Sincidence' },
      { 'name':'prevalence' }
   ]
   #
   # node table:
   node_table = [ { 'name':'world', 'parent':'' } ]
   #
   # weight table:
   weight_table = list()
   #
   # covariate table:
   covariate_table = [
      {'name':'income', 'reference':0.5},
      {'name':'sex',    'reference':0.0, 'max_difference':0.6}
   ]
   #
   # mulcov table:
   # income has been scaled the same as sex so man use same smoothing
   mulcov_table = list()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   # values that are the same for all data rows
   age = 50.0
   row = {
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'age_lower':    age,
      'age_upper':    age,
      'time_lower':   2000.,
      'time_upper':   2000.,
      'node':         'world',
      'subgroup':     'world',
   }
   # values that change between rows:
   for data_id in range( n_data ) :
      row['income']    = data_id / float(n_data-1)
      row['sex']       = ( data_id % 3 - 1.0 ) / 2.0
      row['integrand'] = integrand_table[ data_id % 2 ]['name']
      if row['integrand'] == 'Sincidence' :
         # no noise measurement
         row['meas_value'] = iota_parent_true
      elif row['integrand'] == 'prevalence' :
         # no noise measurement
         row['meas_value'] = 1.0 - math.exp(- age * iota_parent_true)
      else :
         assert(False)
      row['meas_std']   = measure_cv * row['meas_value']
      data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      {  # prior_gauss_zero
         'name':     'prior_gauss_zero',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      0.01,
      },{ # prior_iota_parent
         'name':     'prior_iota_parent',
         'density':  'uniform',
         'lower':    iota_parent_true / 30.0,
         'upper':    iota_parent_true * 30.0,
         'mean':     iota_parent_true / 3.0
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   name      = 'smooth_iota_parent'
   fun       = fun_iota_parent
   age_grid  = list( range( len(age_list) ) )
   time_grid = [0]
   smooth_table = [ {
      'name':name, 'age_id':age_grid, 'time_id':time_grid, 'fun':fun
   } ]
   # ----------------------------------------------------------------------
   # rate table:
   rate_table = [
      {  'name':          'iota',
         'parent_smooth': 'smooth_iota_parent',
         'child_smooth': None,
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },
      { 'name':'parent_node_name',       'value':'world'            },
      { 'name':'ode_step_size',          'value':ode_step_size      },
      { 'name':'random_seed',            'value':random_seed        },

      { 'name':'quasi_fixed',            'value':quasi_fixed        },
      { 'name':'derivative_test_fixed',  'value':'none'             },
      { 'name':'max_num_iter_fixed',     'value':'100'              },
      { 'name':'print_level_fixed',      'value':'0'                },
      { 'name':'tolerance_fixed',        'value':'1e-8'             },
      { 'name':'accept_after_max_steps_fixed',     'value':'10'     },
      { 'name':'limited_memory_max_history_fixed', 'value':'30'     },

      { 'name':'derivative_test_random', 'value':'none'             },
      { 'name':'max_num_iter_random',    'value':'100'              },
      { 'name':'print_level_random',     'value':'0'                },
      { 'name':'tolerance_random',       'value':'1e-8'             }
   ]
   # ----------------------------------------------------------------------
   # avgint table: empty
   avgint_table = list()
   # ----------------------------------------------------------------------
   # nslist_table:
   nslist_table = dict()
   # ----------------------------------------------------------------------
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   # ----------------------------------------------------------------------
   # create database
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
   # ----------------------------------------------------------------------
   return
# ===========================================================================
# Run the init command to create the var table
file_name  = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
# -----------------------------------------------------------------------
# read database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table        = dismod_at.get_table_dict(connection, 'var')
rate_table     = dismod_at.get_table_dict(connection, 'rate')
covariate_table= dismod_at.get_table_dict(connection, 'covariate')
# -----------------------------------------------------------------------
# truth table:
tbl_name     = 'truth_var'
col_name     = [ 'truth_var_value' ]
col_type     = [ 'real' ]
row_list     = list()
var_id2true  = list()
for var_id in range( len(var_table) ) :
   var_info        = var_table[var_id]
   truth_var_value = None
   var_type = var_info['var_type']
   assert( var_type == 'rate' )
   rate_id   = var_info['rate_id']
   rate_name = rate_table[rate_id]['rate_name']
   assert rate_name == 'iota'
   node_id   = var_info['node_id']
   assert node_id == 0
   truth_var_value = iota_parent_true
   var_id2true.append( truth_var_value )
   row_list.append( [ truth_var_value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------
# simulate_command
dismod_at.system_command_prc(
   [ program, file_name, 'simulate', '1' ]
)
# fit_command
dismod_at.system_command_prc(
   [ program, file_name, 'fit', 'fixed', '0' ]
)
# sample_command
dismod_at.system_command_prc(
   [ program, file_name, 'sample', 'asymptotic', 'both', '100', '0' ]
)
# -----------------------------------------------------------------------
# result tables
new           = False
connection    = dismod_at.create_connection(file_name, new)
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
log_dict      = dismod_at.get_table_dict(connection, 'log')
sample_table  = dismod_at.get_table_dict(connection, 'sample')
connection.close()
# -----------------------------------------------------------------------
# determine random seed the was used
if int(random_seed) == 0 :
   for row in log_dict :
      if row['message_type'] == 'command' :
         message = row['message'].split()
         if message[0] == 'begin' and message[1] == 'simulate' :
            random_seed = int(row['unix_time'])
   assert random_seed != 0
# -----------------------------------------------------------------------
# sample_mean, sample_std
n_var    = len(var_table)
n_sample = int( len(sample_table) / n_var )
assert len(sample_table) == n_sample * n_var
sample_array    = numpy.zeros( (n_sample, n_var) , dtype=float )
for row in sample_table :
   sample_index = row['sample_index']
   var_id       = row['var_id']
   var_value    = row['var_value']
   sample_array[sample_index, var_id] = var_value
sample_mean = numpy.mean(sample_array, axis=0)
sample_std  = numpy.std(sample_array, axis=0, ddof=1)
#
# -----------------------------------------------------------------------
# check fit, sample_mean, and sample_std
assert( len(fit_var_table) == n_var )
max_error       = 0.0
for var_id in range( n_var ) :
   row          = fit_var_table[var_id]
   fit_value    = row['fit_var_value']
   true_value   = var_id2true[var_id]
   if true_value == 0.0 :
      max_error = max(abs(fit_value), max_error)
   else :
      max_error = max( abs(fit_value / true_value - 1.0), max_error)
   mean_value    = sample_mean[var_id]
   std_value     = sample_std[var_id]
   max_error     = max(abs(mean_value - fit_value), max_error)
   max_error     = max(std_value, max_error)
if max_error > 5e-2 :
   print('random_seed      = ', random_seed)
   print('max_error        = ', max_error)
   print('asymptotic.py: Error')
   assert(False)
# -----------------------------------------------------------------------------
print('asymptotic.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
