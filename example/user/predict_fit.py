# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_predict_fit.py}
# {xrst_comment_ch #}
#
# Predict Average Integrand Using Results of a Fit
# ################################################
#
# Purpose
# *******
# This examples used the :ref:`fit both<fit_command@variables@both>` command
# to estimate the model variables.
# It then uses the :ref:`predict fit_var<predict_command@source@fit_var>`
# command to compute the
# :ref:`avg_integrand@Integrand, I_i(a,t)@susceptible`
# population :math:`S(a)` at age :math:`a = 50`.
#
# Note Table
# **********
# ::
#
#           north_america
#            /          \
#      united_states   canada
#
# Problem Parameters
# ******************
# The following values are used to simulate the data and set the priors
# for fitting the data:
# {xrst_literal
#     begin problem parameters
#     end problem parameters
# }
#
# Age and Time Values
# *******************
# The age and time values do not affect the fitting for this problem
# because all the functions are constant in age and time.
# This follows from the fact that all of the smoothings have one age
# and one time point.
#
# Rate Table
# **********
# The :ref:`rate_table-name` only specifies that the only
# :ref:`var_table@var_type@rate` variables are
# *iota* for the parent and children.
# In addition, it specifies the smoothings for these rates
# each of which has one grid point.
#
# Variables
# *********
# There are three model variables in this example:
#
# .. list-table::
#     :widths: auto
#
#     * - *iota_north_america*
#       - The true value for
#         :ref:`iota(a,t)<math_ode@Incidence, iota(a, t)>` in north_america.
#     * - *canada_effect*
#       - The true model value for the canada
#         :ref:`child rate effect<model_variables@Random Effects, u@Child Rate Effects>` on iota.
#     * - *united_states_effect*
#       - The true model value for the united_states
#         :ref:`child rate effect<model_variables@Random Effects, u@Child Rate Effects>` on iota.
#
# Integrand Table
# ***************
# The :ref:`integrand_table-name` for this example includes
# :ref:`avg_integrand@Integrand, I_i(a,t)@Sincidence` and
# :ref:`avg_integrand@Integrand, I_i(a,t)@susceptible` .
#
# Data Table
# **********
# There are three measurements of *Sincidence*
# in the :ref:`data_table-name` , one for each node.
# No noise is added to the measurements, and the priors on
# *iota* are uniform, so the fit should correspond to the
# model values used to simulate the data.
#
# Avgint Table
# ************
# There are three predictions of the susceptible population at
# age 50 specified in the :ref:`avgint_table-name` , one for each node.
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_predict_fit.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# begin problem parameters
iota_north_america   = 1e-2
canada_effect        = 0.2
united_states_effect = - canada_effect
# end problem parameters
# ---------------------------------------------------------------------------
import sys
import os
import copy
import math
test_program = 'example/user/predict_fit.py'
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
# ---------------------------------------------------------------------------
def example_db (file_name) :
   def fun_iota_child(a, t) :
      return ('prior_iota_child', None,  None)
   def fun_iota_parent(a, t) :
      return ('prior_iota_parent', None, None)
   # ----------------------------------------------------------------------
   # age table
   age_list    = [  0.0,     100.0 ]
   #
   # time table
   time_list   = [ 1995.0,  2015.0 ]
   node_table = [
      { 'name':'north_america', 'parent':''              },
      { 'name':'united_states', 'parent':'north_america' },
      { 'name':'canada',        'parent':'north_america' }
   ]
   #
   # weight table:
   weight_table = list()
   # integrand table
   integrand_table = [
      { 'name':'Sincidence' },
      { 'name':'susceptible' }
   ]
   #
   # covariate table: no covriates
   covariate_table = list()
   #
   # mulcov table
   mulcov_table = list()
   #
   # nslist_dict:
   nslist_dict = dict()
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota_parent
         'name':     'prior_iota_parent',
         'density':  'uniform',
         'lower':    iota_north_america / 100.0,
         'upper':    iota_north_america * 100.0,
         'mean':     iota_north_america * 3.0,
      },{ # prior_iota_child
         'name':     'prior_iota_child',
         'density':  'uniform',
         'mean':     0.0,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      { # smooth_iota_parent
         'name':                     'smooth_iota_parent',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_iota_parent
      }, { # smooth_iota_child
         'name':                     'smooth_iota_child',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_iota_child
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'iota',
         'parent_smooth': 'smooth_iota_parent',
         'child_smooth':  'smooth_iota_child',
      }
   ]
   # --------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'north_america'     },
      { 'name':'ode_step_size',          'value':'1.0'               },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },

      { 'name':'quasi_fixed',            'value':'true'         },
      { 'name':'max_num_iter_fixed',     'value':'30'           },
      { 'name':'print_level_fixed',      'value':'0'            },
      { 'name':'tolerance_fixed',        'value':'1e-10'        },
   ]
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   row = {
      'density':     'log_gaussian',
      'eta':         '0.0',
      'weight':      '',
      'hold_out':     False,
      'time_lower':   2000.0,
      'time_upper':   2000.0,
      'age_lower':    0.0,
      'age_upper':    100.0,
      'integrand':   'Sincidence',
      'subgroup':     'world',
   }
   # north_america
   row['node']       = 'north_america';
   row['meas_value'] = iota_north_america
   row['meas_std']   = row['meas_value'] / 10.0
   data_table.append( copy.copy(row) )
   # canada
   row['node'] = 'canada';
   row['meas_value'] = math.exp(canada_effect) * iota_north_america
   row['meas_std']   = row['meas_value'] / 10.0
   data_table.append( copy.copy(row) )
   # united_states
   row['node'] = 'united_states';
   row['meas_value'] = math.exp(united_states_effect) * iota_north_america
   row['meas_std']   = row['meas_value'] / 10.0
   data_table.append( copy.copy(row) )
   # ----------------------------------------------------------------------
   # avgint table:
   avgint_table = list()
   # values that are the same for all data rows
   row = {
      'integrand':   'susceptible',
      'weight':      '',
      'time_lower':   2000.0,
      'time_upper':   2000.0,
      'age_lower':    50.0,
      'age_upper':    50.0,
      'subgroup':     'world'
   }
   row['node'] = 'north_america'
   avgint_table.append( copy.copy(row) )
   row['node'] = 'canada'
   avgint_table.append( copy.copy(row) )
   row['node'] = 'united_states'
   avgint_table.append( copy.copy(row) )
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
# ===========================================================================
file_name             = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
dismod_at.system_command_prc([ program, file_name, 'predict', 'fit_var' ])
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
predict_table  = dismod_at.get_table_dict(connection, 'predict')
avgint_table   = dismod_at.get_table_dict(connection, 'avgint')
node_table     = dismod_at.get_table_dict(connection, 'node')
#
# check that all the avgint_table values were predicted (no subsetting)
assert len(predict_table) == 3
#
# S(a) = exp( - iota * a )
iota_canada        = math.exp(canada_effect) * iota_north_america
iota_united_states = math.exp(united_states_effect) * iota_north_america
S_north_america    = math.exp( - iota_north_america * 50.0 )
S_canada           = math.exp( - iota_canada * 50.0 )
S_united_states    = math.exp( - iota_united_states * 50.0 )
truth = {
   'north_america' : S_north_america,
   'canada'        : S_canada,
   'united_states' : S_united_states
}
for i in range(3) :
   avgint_id = predict_table[i]['avgint_id']
   node_id   = avgint_table[avgint_id]['node_id']
   node      = node_table[node_id]['node_name']
   check     = truth[node]
   value     = predict_table[i]['avg_integrand']
   assert( abs( value / check - 1.0 ) ) < 1e-6
# -----------------------------------------------------------------------------
print('predict_fit.py: OK')
# END PYTHON
