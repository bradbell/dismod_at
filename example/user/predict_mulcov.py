# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_predict_mulcov.py}
# {xrst_comment_ch #}
#
# Predict Covariate Multiplier Values
# ###################################
#
# Purpose
# *******
# This examples demonstrates covariate multiplier predictions.
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
# The reference value for *iota* is constant
# in age and time, but the value of the covariate multiplier that
# affects iota changes with age and time.
#
# Rate Table
# **********
# The :ref:`rate_table-name` specifies that the only
# :ref:`var_table@var_type@rate` variable is
# *iota* for north_america.
# In addition, it specifies the smoothing for this rate
# has one grid point.
#
# Mulcov Table
# ************
# The :ref:`mulcov_table-name` specifies that
# the covariate *alpha* is a bilinear function of age and time.
# In fact, it is equal to the function ``alpha_true`` defined
# as one of the problem parameters.
#
# Variables
# *********
# There are five model variables in this example:
#
# .. list-table::
#     :widths: auto
#
#     * - *iota_reference*
#       - There is one variable corresponding to the reference value for
#         :ref:`iota(a,t)<diff_eq@Incidence, iota(a, t)>` in north_america.
#     * - *alpha*
#       - There are four variables corresponding to the
#         :ref:`mulcov_table@mulcov_type@rate_value`
#         covariate multiplier that affect *iota* .
#
# Integrand Table
# ***************
# The :ref:`integrand_table-name` for this example includes
# :ref:`avg_integrand@Integrand, I_i(a,t)@Sincidence` and
# :ref:`avg_integrand@Integrand, I_i(a,t)@mulcov` .
# The ``mulcov_0`` integrand corresponds the value of *alpha* .
#
# Data Table
# **********
# There are four measurements of *Sincidence*
# in the :ref:`data_table-name` , one for (age, time) pair corresponding
# to an *alpha* model variable.
# No noise is added to the measurements, and the prior on
# *iota* constrains it to the *iota_reference* .
#
# Avgint Table
# ************
# There are four predictions of *alpha* requested by the
# :ref:`avgint_table-name` , one for each *alpha* model variable.
# The predictions are compared with the truth to see that the
# fit is prefect (there is no noise in the data).
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_predict_mulcov.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# begin problem parameters
age_list    = [  0.0,     100.0 ]
time_list   = [ 1990.0,  2015.0 ]
def alpha_true(age, time) :
   age_factor  = 1.0 + (age  - age_list[0])  / (age_list[1]  - age_list[0])
   time_factor = 1.0 + (time - time_list[0]) / (time_list[1] - time_list[0])
   return 0.1 * age_factor * time_factor
iota_reference   = 1e-2
income_reference = 1.0
# end problem parameters
# ---------------------------------------------------------------------------
import sys
import os
import copy
import math
test_program  = 'example/user/predict_mulcov.py'
check_program = sys.argv[0].replace('\\', '/')
if check_program != test_program  or len(sys.argv) != 1 :
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
   def fun_income(a, t) :
      return ('prior_income_value', 'prior_income_dage',  'prior_income_dtime')
   def fun_iota_parent(a, t) :
      return ('prior_iota_parent', None, None)
   # ----------------------------------------------------------------------
   #
   # node table
   node_table = [
      { 'name':'north_america', 'parent':''              },
   ]
   #
   # weight table:
   weight_table = list()
   #
   # integrand table
   integrand_table = [
      { 'name':'Sincidence' },
      { 'name':'mulcov_0' }
   ]
   #
   # covariate table:
   covariate_table = [{ 'name':'income', 'reference': income_reference }]
   #
   # mulcov table: one multiplier with mulcov_id = 0
   mulcov_table = [{
      'covariate': 'income',
      'type':      'rate_value',
      'effected':  'iota',
      'group':     'world',
      'smooth':    'smooth_income'
   } ]
   #
   # nslist_dict:
   nslist_dict = dict()
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota_parent
         'name':     'prior_iota_parent',
         'density':  'uniform',
         'lower':    iota_reference,
         'upper':    iota_reference,
         'mean':     iota_reference,
      },{ # prior_income_value
         'name':     'prior_income_value',
         'density':  'uniform',
         'mean':     0.0,
         'lower':   -1.0,
         'upper':   +1.0,
      },{ # prior_income_dage
         'name':     'prior_income_dage',
         'density':  'uniform',
         'mean':     0.0,
      },{ # prior_income_dtime
         'name':     'prior_income_dtime',
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
      }, { # smooth_income
         'name':                     'smooth_income',
         'age_id':                   [ 0 , 1],
         'time_id':                  [ 0 , 1],
         'fun':                      fun_income
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [{
         'name':          'iota',
         'parent_smooth': 'smooth_iota_parent',
   } ]
   # --------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'north_america'     },
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
      'node':        'north_america',
      'subgroup':    'world',
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'integrand':   'Sincidence',
      'income':      2.0,
   }
   for age in age_list :
      for time in time_list :
         alpha   = alpha_true(age, time)
         effect  = alpha * ( row['income'] - income_reference )
         iota    = iota_reference * math.exp( effect )
         row['age_lower']   = age
         row['age_upper']   = age
         row['time_lower']  = time
         row['time_upper']  = time
         row['meas_value']  = iota
         row['meas_std']    = iota / 10.0
         data_table.append( copy.copy(row) )
   # ----------------------------------------------------------------------
   # avgint table:
   avgint_table = list()
   # values that are the same for all rows
   does_not_matter = 5.0
   row = {
      'node':        'north_america',
      'subgroup':    'world',
      'integrand':   'mulcov_0',
      'weight':      '',
      'income':      does_not_matter,
   }
   for age in age_list :
      for time in time_list :
         row['age_lower']   = age
         row['age_upper']   = age
         row['time_lower']  = time
         row['time_upper']  = time
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
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
dismod_at.system_command_prc([ program, file_name, 'predict', 'fit_var' ])
# -----------------------------------------------------------------------
# connect to database
connection      = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
predict_table  = dismod_at.get_table_dict(connection, 'predict')
avgint_table   = dismod_at.get_table_dict(connection, 'avgint')
node_table     = dismod_at.get_table_dict(connection, 'node')
#
for row in predict_table :
   avg_integrand  = row['avg_integrand']
   avgint_id      = row['avgint_id']
   age            = avgint_table[avgint_id]['age_lower']
   time           = avgint_table[avgint_id]['time_lower']
   truth          = alpha_true(age, time)
   assert abs( 1.0 - avg_integrand / truth ) < 1e-6
# -----------------------------------------------------------------------------
print('predict_mulcov.py: OK')
# END PYTHON
