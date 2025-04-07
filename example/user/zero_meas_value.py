# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
r'''
{xrst_begin user_zero_meas_value.py}
{xrst_spell
  bmi
}

Exclude Measurement Value Covariate Multipliers From Predictions
################################################################

Purpose
*******
The measurement value covariate multipliers are intended to represent
biases in measurements.
The :ref:`predict_command@zero_meas_value` option for the predict command
enables one to exclude these biases during predictions.

no_effect_iota_true
*******************
The only rate in this example is iota.
The value of this rate, before any effects, used to simulate the data is
{xrst_code py}'''
no_effect_iota_true = 1e-2
r'''{xrst_code}

reference_income
****************
Income is a covariate between zero and one and its reference value is 1:
{xrst_code py}'''
reference_income    = 1.0;
r'''{xrst_code}

beta_income_true
****************
The higher income, the more likely that incidence will
be recorded. We model this using a meas_value covariate multiplier
one income. The value of this multiplier used to simulate the data is
{xrst_code py}'''
beta_income_true    = 0.5;
r'''{xrst_code}

reference_bmi
*************
Body mass index, bmi, is a covariate and its reference value is 20:
{xrst_code py}'''
reference_bmi       = 20.0;
r'''{xrst_code}

beta_bmi_true
*************
The high bmi, the more likely that the disease will be contracted.
We model this using a rate_value covariate multiplier on bmi.
The value of this multiplier (used to simulate the data is
{xrst_code py}'''
beta_bmi_true       = 1.0 / 5.0
r'''{xrst_code}

n_data
******
The number of simulated data points for this example is
{xrst_code py}'''
n_data               = 10
r'''{xrst_code}

Source Code
***********
{xrst_literal
   # BEGIN PYTHON
   # END PYTHON
}

{xrst_end user_zero_meas_value.py}
'''
# ----------------------------------------------------------------------------
# BEGIN PYTHON
# ---------------------------------------------------------------------------
import sys
import os
import copy
import math
test_program  = 'example/user/zero_meas_value.py'
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
   def fun_group(a, t) :
      return ('prior_group_value', None,  None)
   def fun_subgroup(a, t) :
      return ('prior_subgroup_value', None,  None)
   def fun_iota_parent(a, t) :
      return ('prior_iota_value', None, None)
   # ----------------------------------------------------------------------
   # age_list
   age_list    = [  0.0,     100.0 ]
   #
   # time_list
   time_list   = [ 1990.0,  2020.0 ]
   #
   # node_table
   node_table = [
      { 'name':'world',       'parent':'' },
      { 'name':'high_income', 'parent':'world' },
   ]
   #
   # integrand_table
   integrand_table = [ { 'name':'Sincidence' } ]
   #
   # covariate_table:
   covariate_table = [
      { 'name':'income', 'reference': reference_income} , # x_0
      { 'name':'bmi',    'reference': reference_bmi} ,    # x_1
   ]
   #
   # mulcov_table:
   mulcov_table = [
      {  # income
         'covariate': 'income',
         'type':      'meas_value',
         'effected':  'Sincidence',
         'group':     'world',
         'smooth':    'smooth_group',
         'subsmooth': 'smooth_subgroup',
      },{# bmi
         'covariate': 'bmi',
         'type':      'rate_value',
         'effected':  'iota',
         'group':     'world',
         'smooth':    'smooth_group',
         'subsmooth': 'smooth_subgroup',
      },
   ]
   #
   # prior_table
   prior_table = [
      { # prior_iota_value
         'name':     'prior_iota_value',
         'density':  'uniform',
         'lower':    no_effect_iota_true,
         'upper':    no_effect_iota_true,
         'mean':     no_effect_iota_true,
      },{ # prior_group_value
         'name':     'prior_group_value',
         'density':  'uniform',
         'mean':     0.0,
         'lower':   -1.0,
         'upper':   +1.0,
      },{ # prior_subgroup_value
         'name':     'prior_subgroup_value',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      10.0,
      },
   ]
   #
   # smooth table
   smooth_table = [
      {  # smooth_iota_parent
         'name':                     'smooth_iota_parent',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_iota_parent
      },{ # smooth_group
         'name':                     'smooth_group',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_group,
      },{ # smooth_subgroup
         'name':                     'smooth_subgroup',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_subgroup,
      }
   ]
   #
   # rate table
   rate_table = [{
         'name':          'iota',
         'parent_smooth': 'smooth_iota_parent',
   } ]
   #
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'world'             },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },

      { 'name':'quasi_fixed',            'value':'true'         },
      { 'name':'max_num_iter_fixed',     'value':'30'           },
      { 'name':'print_level_fixed',      'value':'5'            },
      { 'name':'tolerance_fixed',        'value':'1e-10'        },
   ]
   #
   # data_table:
   data_table = list()
   row = {
      'node':        'world',
      'subgroup':    'world',
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'integrand':   'Sincidence',
      'age_lower':   50.0,
      'age_upper':   50.0,
      'time_lower':  2000.0,
      'time_upper':  2000.0,
   }
   for data_id in range(n_data) :
      income             = data_id / n_data
      bmi                = 15 * ( 1 + abs(data_id - n_data / 2) / n_data )
      effect             = beta_income_true * (income - reference_income)
      effect            += beta_bmi_true * (bmi - reference_bmi)
      meas_value         = no_effect_iota_true * math.exp( effect )
      meas_std           = meas_value / 10.0
      row['income']      = income
      row['bmi']         = bmi
      row['meas_value']  = meas_value
      row['meas_std']    = meas_std
      data_table.append( copy.copy(row) )
   #
   # avgint_table
   avgint_table = list()
   for row in data_table :
      row_copy = copy.copy(row)
      for key in [ 'density', 'hold_out', 'meas_value', 'meas_std' ] :
         del row_copy[key]
      avgint_table.append( row_copy )
   #
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   # ----------------------------------------------------------------------
   # create database
   dismod_at.create_database(
      file_name              = file_name ,
      age_list               = age_list  ,
      time_list              = time_list ,
      integrand_table        = integrand_table ,
      node_table             = node_table ,
      subgroup_table         = subgroup_table ,
      weight_table           = list() ,
      covariate_table        = covariate_table ,
      avgint_table           = avgint_table ,
      data_table             = data_table ,
      prior_table            = prior_table ,
      smooth_table           = smooth_table ,
      nslist_dict            = dict() ,
      rate_table             = rate_table ,
      mulcov_table           = mulcov_table ,
      option_table           = option_table ,
   )
# ===========================================================================
file_name             = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
dismod_at.system_command_prc(
   [ program, file_name, 'predict', 'fit_var', 'zero_meas_value' ]
)
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
   row_avgint     = avgint_table[avgint_id]
   bmi            = row_avgint['x_1']
   effect         = beta_bmi_true * (bmi - reference_bmi)
   # note that only the bmi effect is included in this check
   check          = no_effect_iota_true * math.exp(effect)
   assert abs( 1.0 - avg_integrand / check ) < 1e-7
# -----------------------------------------------------------------------------
print('zero_meas_value.py: OK')
# END PYTHON
