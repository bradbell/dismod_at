# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_sim_log.py}
# {xrst_comment_ch #}
#
# Simulating Data with Log Transformed Distribution
# #################################################
#
# See Also
# ********
# :ref:`user_data_sim.py-name`
#
# Example Parameters
# ******************
# The following values are used to simulate the data
# {xrst_spell_off}
# {xrst_code py}
number_simulate   = 2000
iota_true         = 0.01
meas_value_global = iota_true * 1.5
eta_global        = iota_true * 1e-3
meas_std_global   = meas_value_global * 0.25
gamma_global      = meas_value_global * 0.25
# {xrst_code}
# {xrst_spell_on}
# Model
# *****
# The only non-zero model variable for this example is
# the rate of incidence for the world which is constant in age and time.
#
# Data
# ****
# There is only one data point for this example and it's integrand is
# :ref:`avg_integrand@Integrand, I_i(a,t)@Sincidence` .
# This data has a log transformed distribution with mean *iota_true* ,
# offset *eta_global* , and standard deviation
# *meas_std_global* .
#
# Covariate Multiplier
# ********************
# For this example there is one covariate multiplier.
# It is a :ref:`mulcov_table@mulcov_type@meas_noise` multiplier
# and the corresponding covariate value is one.
#
# Notation
# ********
#
# .. list-table::
#     :widths: auto
#
#     * - :math:`y`
#       - is the measurement value, *meas_value_global*
#     * - :math:`\mu`
#       - mean of the data, *iota_true*
#     * - :math:`\eta`
#       - offset in log transform, *eta_global*
#     * - :math:`\Delta`
#       - data measurement error, *meas_std_global*
#     * - :math:`\gamma`
#       - meta regression error, *gamma_global*
#     * - :math:`n`
#       - number of simulated data values,
#         :ref:`simulate_command@number_simulate`
#     * - :math:`z_i`
#       - *i*-th simulate data for :math:`i = 1, \ldots , n`
#
# delta
# *****
# The transformed standard deviation
# :ref:`delta<data_like@Transformed Standard Deviation, delta_i(theta)>`
# is given by
#
# .. math::
#
#  \delta = \log( y + \eta + \sigma ) - \log(y + \eta)
#
# sigma
# *****
# For this example we use the
# :ref:`data_like@Adjusted Standard Deviation, sigma_i(theta)@add_std_scale_none`
# option in the definition of the adjusted standard deviation
# :ref:`sigma<data_like@Adjusted Standard Deviation, sigma_i(theta)>`
# :math:`\sigma`; i.e.,
#
# .. math::
#
#  \sigma = \Delta + \gamma
#
# Simulations
# ***********
# The offset log transform of each simulated measurement :math:`z_i` has
# the following Gaussian distribution:
#
# .. math::
#
#  \log( z_i + \eta ) - \log( \mu + \eta ) \sim N(0, \delta^2 )
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_sim_log.py}
#
# $end
# ------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import subprocess
import copy
import time
import numpy
test_program = 'example/user/sim_log.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
   usage  = 'python3 ' + test_program + '\n'
   usage += 'where python3 is the python 3 program on your system\n'
   usage += 'and working directory is the dismod_at distribution directory\n'
   sys.exit(usage)
print(test_program)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
   sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/example/user directory
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
#
# random_seed_str
random_seed_str = str( int( time.time() ) )
# ----------------------------------------------------------------------------
# run a system command
def system_command(command) :
   print( ' '.join(command) )
   flag = subprocess.call( command )
   if flag != 0 :
      msg  = 'command failed: flag = ' + str(flag)
      msg += ', random_seed = ' + random_seed_str
      sys.exit(msg)
   return
# ------------------------------------------------------------------------
def fun_iota_parent(a, t) :
   return ('prior_iota_parent', None, None)
def fun_gamma(a, t):
   return ('prior_gamma', None, None)
# ------------------------------------------------------------------------
def example_db (file_name) :
   # ----------------------------------------------------------------------
   # age table:
   age_list    = [ 0.0, 100.0 ]
   #
   # time table:
   time_list   = [ 1990.0, 2020.0 ]
   #
   # integrand table:
   integrand_table = [
       { 'name':'Sincidence', 'minimum_meas_cv':0.0 }
   ]
   #
   # node table:
   node_table = [ { 'name':'world', 'parent':'' } ]
   #
   # empty tables
   weight_table    = list()
   covariate_table = list()
   mulcov_table    = list()
   avgint_table    = list()
   nslist_dict = dict()
   #
   # covariate table
   covariate_table = [
      {'name':'one',  'reference':0.0}
   ]
   #
   # mulcov table
   mulcov_table = [ {
         'covariate':  'one',
         'type':       'meas_noise',
         'effected':   'Sincidence',
         'group':      'world',
         'smooth':     'smooth_gamma',
   } ]
   # ----------------------------------------------------------------------
   # data table:
   # values that are the same for all data rows
   row = {
      'meas_value':  meas_value_global,
      'eta':         eta_global,
      'weight':      '',
      'hold_out':     False,
      'time_lower':   2000.,
      'time_upper':   2000.,
      'age_lower':    40.0,
      'age_upper':    40.0,
      'subgroup':     'world',
      'density':      'log_gaussian',
      'meas_std':     meas_std_global,
      'node':         'world',
      'integrand':    'Sincidence',
      'one':          1.0,
   }
   data_table = [ row ]
   # ----------------------------------------------------------------------
   # prior_table
   # Note that the prior mean is equal to the true value for iota
   prior_table = [
      { # prior_iota_parent
         'name':     'prior_iota_parent',
         'density':  'uniform',
         'lower':    iota_true / 100.,
         'upper':    iota_true * 10.0,
         'mean':     iota_true ,
         'std':      None,
         'eta':      None
      },{
         # prior_gamma
         'name':     'prior_gamma',
         'density':  'uniform',
         'lower':    gamma_global,
         'upper':    gamma_global,
         'mean':     gamma_global,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   age_id   = 0
   time_id  = 0
   name     = 'smooth_iota_parent'
   fun      = fun_iota_parent
   smooth_table = [
      {'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
   ]
   name = 'smooth_gamma'
   fun  = fun_gamma
   smooth_table.append(
      {'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
   )
   # ----------------------------------------------------------------------
   # rate table:
   rate_table = [
      {  'name':          'iota',
         'parent_smooth': 'smooth_iota_parent',
         'child_smooth':  None,
         'child_nslist':  None
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero'   },
      { 'name':'parent_node_name',       'value':'world'               },
      { 'name':'random_seed',            'value':random_seed_str       },
      { 'name':'meas_noise_effect',      'value':'add_std_scale_none'  },

      { 'name':'quasi_fixed',            'value':'false'               },
      { 'name':'max_num_iter_fixed',     'value':'50'                  },
      { 'name':'print_level_fixed',      'value':'0'                   },
      { 'name':'tolerance_fixed',        'value':'1e-6'                },

      { 'name':'max_num_iter_random',    'value':'100'                 },
      { 'name':'print_level_random',     'value':'0'                   },
      { 'name':'tolerance_random',       'value':'1e-8'                }
   ]
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
      nslist_dict,
      rate_table,
      mulcov_table,
      option_table
   )
   # ----------------------------------------------------------------------
   return
# ===========================================================================
# file_name
file_name      = 'example.db'
#
# create example.db
example_db(file_name)
#
# program
program        = '../../devel/dismod_at'
#
# init database
system_command([ program, file_name, 'init' ])
#
# Note that the prior mean is equal to the true value for iota
system_command([ program, file_name, 'set', 'truth_var', 'prior_mean' ])
#
# create data_sim table
system_command([ program, file_name, 'simulate', str(number_simulate) ])
# -----------------------------------------------------------------------
# check simulated data
from math import log
connection         = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
data_sim_table     = dismod_at.get_table_dict(connection, 'data_sim')
data_table         = dismod_at.get_table_dict(connection, 'data')
data_subset_table  = dismod_at.get_table_dict(connection, 'data_subset')
residual_list      = list()
for row in data_sim_table :
   data_sim_value = row['data_sim_value']
   data_subset_id = row['data_subset_id']
   data_id        = data_subset_table[data_subset_id]['data_id']
   meas_value     = data_table[data_id]['meas_value']
   Delta          = data_table[data_id]['meas_std']
   eta            = data_table[data_id]['eta']
   sigma          = Delta + gamma_global
   delta          = log(meas_value + eta + sigma) - log(meas_value + eta)
   residual       = (log(data_sim_value + eta) - log(iota_true + eta) )/delta
   residual_list.append( residual )
residual_array  = numpy.array( residual_list )
residual_mean   = residual_array.mean()
residual_std    = residual_array.std(ddof=1)
# check that the mean of the residuals is within 2.5 standard deviations
assert abs(residual_mean) <=  2.5 / numpy.sqrt(number_simulate)
# check that the standard deviation of the residuals is near one
assert abs(residual_std - 1.0) <= 2.5 / numpy.sqrt(number_simulate)
# -----------------------------------------------------------------------------
print('fit_sim.py: OK')
# END PYTHON
