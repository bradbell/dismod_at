# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_no_children.py}
# {xrst_comment_ch #}
#
# Case with no Children; i.e., no Random Effects
# ##############################################
#
# Integrands
# **********
# There is an integrand that directly measures each of the model rates; i.e.,
# :ref:`avg_integrand@Integrand, I_i(a,t)@Sincidence` ,
# :ref:`avg_integrand@Integrand, I_i(a,t)@remission` ,
# :ref:`avg_integrand@Integrand, I_i(a,t)@mtexcess` , and
# :ref:`avg_integrand@Integrand, I_i(a,t)@mtother` .
# In addition the integrand prevalence is included, but is data values
# are for age zero which corresponds to the
# :ref:`rate_table@rate_name@pini` rate.
#
# Nodes
# *****
# There are four nodes in this example.
# The world node has one child, north_america.
# The north_america node has two children, united_states and canada.
# The :ref:`parent_node<option_table@Parent Node>` is canada which
# does not have any children.
#
# Data
# ****
# All of the data corresponds to canada.
# There is one data point for each integrand and it is the true value
# for the corresponding rate; i.e., there is no noise in this data.
# {xrst_spell_off}
# {xrst_code py}
integrand2rate = {
   'prevalence':  'pini'   ,
   'Sincidence':  'iota'   ,
   'remission':   'rho'    ,
   'mtexcess' :   'chi'    ,
   'mtother':     'omega'  ,
}
rate_true = {
   'pini'  : 1e-2 ,
   'iota'  : 2e-2 ,
   'rho'   : 3e-2 ,
   'chi'   : 4e-2 ,
   'omega' : 5e-2 ,
}
# The data is modeled as if it had noise.
# {xrst_code}
# {xrst_spell_on}
#
# Outlier
# =======
# There is also one outlier at the end of the data table with
# :ref:`data_table@hold_out` equal to one.
#
# Smoothing
# *********
# There is a smoothing the for each of the possible rates for the
# parent node canada.
# There is no child node smoothing.
# The value priors for this smoothing is uniform with lower limit 1e-4
# and upper limit 1.0. The mean 0.1, is only used as a starting point
# for the optimization.
# The time difference prior for this smoothing is
# gaussian with mean zero and standard deviation 1e-2.
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_no_children.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
test_program  = 'example/user/no_children.py'
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
# Note that the a, t values are not used for this example
def example_db (file_name) :
   def fun_rate_parent(a, t) :
      return ('prior_rate_parent', None, 'prior_gauss_zero')
   # ----------------------------------------------------------------------
   # age table
   age_list    = [    0.0, 50.0,    100.0 ]
   #
   # time table
   time_list   = [ 1995.0, 2005.0, 2015.0 ]
   #
   # integrand table
   integrand_table = [
      { 'name':'prevalence' },
      { 'name':'Sincidence' },
      { 'name':'remission' },
      { 'name':'mtexcess' },
      { 'name':'mtother' }
   ]
   #
   # node table: world -> north_america
   #             north_america -> (united_states, canada)
   node_table = [
      { 'name':'world',         'parent':'' },
      { 'name':'north_america', 'parent':'world' },
      { 'name':'united_states', 'parent':'north_america' },
      { 'name':'canada',        'parent':'north_america' }
   ]
   #
   # weight table:
   weight_table = list()
   #
   # covariate table: no covriates
   covariate_table = list()
   #
   # mulcov table
   mulcov_table = list()
   #
   # nslist_dict:
   nslist_dict = dict()
   #
   # avgint table:
   avgint_table = list()
   # ----------------------------------------------------------------------
   # data table: same order as list of integrands
   data_table = list()
   # values that are the same for all data rows
   row = {
      'node':        'canada',
      'subgroup':    'world',
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'time_lower':   2000.0,
      'time_upper':   2000.0,
      'age_lower':    0.0
   }
   # values that change between rows: (one data point for each integrand)
   for integrand_id in range( len(integrand_table) ) :
      integrand         = integrand_table[integrand_id]['name']
      meas_value        = rate_true[ integrand2rate[integrand] ]
      meas_std          = 0.2 * meas_value
      row['meas_value'] = meas_value
      row['meas_std']   = meas_std
      row['integrand']  = integrand
      if integrand == 'prevalence' :
         # prevalence is measured at age zero
         row['age_upper'] = 0.0
      else :
         # other integrands are averaged from age zero to one hundred
         row['age_upper'] = 100.0
      data_table.append( copy.copy(row) )
   #
   # add one outlyer at end of data table with hold_out true
   row['hold_out']   = True # if outlyer were false, fit would fail
   row['integrand']  = data_table[0]['integrand']
   row['meas_std']   = data_table[0]['meas_std']
   row['eta']        = None;
   row['age_upper']  = data_table[0]['age_upper']
   row['meas_value'] = 10. * data_table[0]['meas_value']
   data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      {  # prior_rate_parent
         'name':     'prior_rate_parent',
         'density':  'uniform',
         'lower':    1e-4,
         'upper':    1.0,
         'mean':     0.1,
      },{ # prior_gauss_zero
         'name':     'prior_gauss_zero',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      1e-2,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   last_time_id   = len(time_list) - 1
   smooth_table = [
      { # smooth_rate_parent
         'name':                     'smooth_rate_parent',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0, last_time_id ],
         'fun':                       fun_rate_parent
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'pini',
         'parent_smooth': 'smooth_rate_parent',
      },{
         'name':          'iota',
         'parent_smooth': 'smooth_rate_parent',
      },{
         'name':          'rho',
         'parent_smooth': 'smooth_rate_parent',
      },{
         'name':          'chi',
         'parent_smooth': 'smooth_rate_parent',
      },{
         'name':          'omega',
         'parent_smooth': 'smooth_rate_parent',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'canada'       },
      { 'name':'rate_case',              'value':'iota_pos_rho_pos' },

      { 'name':'quasi_fixed',            'value':'true'         },
      { 'name':'derivative_test_fixed',  'value':'first-order'  },
      { 'name':'max_num_iter_fixed',     'value':'100'          },
      { 'name':'print_level_fixed',      'value':'0'            },
      { 'name':'tolerance_fixed',        'value':'1e-10'        },

      { 'name':'derivative_test_random', 'value':'second-order' },
      { 'name':'max_num_iter_random',    'value':'100'          },
      { 'name':'print_level_random',     'value':'0'            },
      { 'name':'tolerance_random',       'value':'1e-10'        }
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
   n_smooth  = len( smooth_table )
# ===========================================================================
file_name  = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
# -----------------------------------------------------------------------
# connect to database
connection      = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
# -----------------------------------------------------------------------
# get variable and fit_var tables
var_table       = dismod_at.get_table_dict(connection, 'var')
node_table      = dismod_at.get_table_dict(connection, 'node')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
# 5 rates and two time points for each rate
assert len(var_table) == 10
for (var_id, row) in enumerate(var_table) :
   assert row['var_type'] == 'rate'
   #
   # node_name
   node_id   = row['node_id']
   node_name = node_table[node_id]['node_name']
   assert node_name == 'canada'
   #
   # rate_name
   rate_id   = row['rate_id']
   rate_name = rate_table[rate_id]['rate_name']
   #
   # fit_var_value
   fit_var_value = fit_var_table[var_id]['fit_var_value']
   #
   # check
   check  = rate_true[rate_name]
   err    = fit_var_value  / check - 1.0
   assert abs(err) < 1e-6
# -----------------------------------------------------------------------------
print('no_children.py: OK')
# END PYTHON
