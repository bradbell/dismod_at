# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_const_random.py}
# {xrst_comment_ch #}
#
# Fitting With Non-Zero Constant Random Effects in Smoothing Grid
# ###############################################################
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_const_random.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# Values used to simulate data, child values are in log space; e.g.,
# actual iota_child_1 = exp( iota_true['child_1'] ) * iota_true['world')
iota_true = {'world':0.01, 'child_1':-0.7, 'child_2':+0.7}
chi_true  = {'world':0.1,  'child_1':+0.7, 'child_2':-0.7}
n_data    = 41
# ------------------------------------------------------------------------
import sys
import os
import copy
test_program = 'example/user/const_random.py'
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
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
#
def example_db (file_name) :
   def fun_iota_world(a, t) :
      return ('prior_iota_world', None, None)
   def fun_iota_child(a, t) :
      return ('prior_iota_child', None, None)
   def fun_chi_world(a, t) :
      return (chi_true['world'],  None, None)
   def fun_chi_child_1(a, t) :
      return (chi_true['child_1'], None, None)
   def fun_chi_child_2(a, t) :
      return (chi_true['child_2'], None, None)
   # ------------------------------------------------------------------------
   # function that creates example database
   # ----------------------------------------------------------------------
   # age table:
   age_list    = [ 0.0, 5.0, 15.0, 35.0, 50.0, 75.0, 90.0, 100.0 ]
   #
   # time table:
   time_list   = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
   #
   # integrand table:
   integrand_table = [
       { 'name':'prevalence' }
   ]
   #
   # node table:
   node_table = [
      { 'name':'world',   'parent':''     },
      { 'name':'child_1', 'parent':'world'},
      { 'name':'child_2', 'parent':'world'}
   ]
   #
   # weight table:
   weight_table = list()
   #
   # covariate table:
   covariate_table = list()
   #
   # mulcov table:
   mulcov_table = list()
   #
   # avgint table:
   avgint_table = list()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   # values that are the same for all data rows
   row = {
      'meas_value':  0.0,             # not used (will be simulated)
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'time_lower':   2000.,
      'time_upper':   2000.,
      'subgroup':     'world'
   }
   # values that change between rows:
   for data_id in range( n_data ) :
      fraction = data_id / float(n_data-1)
      age      = age_list[0] + (age_list[-1] - age_list[0])*fraction
      row['age_lower'] = age
      row['age_upper'] = age
      row['integrand'] = 'prevalence'
      row['meas_std']  = 0.001
      row['data_name'] = 'd' + str(data_id)
      node_id          = data_id % len( node_table )
      row['node']      = node_table[node_id]['name']
      data_table.append( copy.copy(row) )
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota_world
         'name':     'prior_iota_world',
         'density':  'uniform',
         'lower':    iota_true['world'] / 10.,
         'upper':    iota_true['world'] * 10.,
         'mean':     iota_true['world'] * 3.0,
      },{ # prior_iota_child
         'name':     'prior_iota_child',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      2.0
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      {
         'name':                     'smooth_iota_world',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_iota_world
      },{
         'name':                     'smooth_iota_child',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_iota_child
      },{
         'name':                     'smooth_chi_world',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_chi_world
      },{
         'name':                     'smooth_chi_child_1',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_chi_child_1
      },{
         'name':                     'smooth_chi_child_2',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_chi_child_2
      }
   ]
   # ----------------------------------------------------------------------
   # rate table:
   rate_table = [
      {  'name':          'iota',
         'parent_smooth': 'smooth_iota_world',
         'child_smooth':  'smooth_iota_child',
      },{  'name':          'chi',
         'parent_smooth': 'smooth_chi_world',
         'child_nslist':  'nslist_chi_children'
      }
   ]
   # ----------------------------------------------------------------------
   # nslist_table:
   nslist_table = dict()
   nslist_table['nslist_chi_children'] = [
      ('child_1', 'smooth_chi_child_1'),
      ('child_2', 'smooth_chi_child_2')
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },
      { 'name':'parent_node_name',       'value':'world'        },
      { 'name':'ode_step_size',          'value':'10.0'         },
      { 'name':'random_seed',            'value':'0'            },

      { 'name':'quasi_fixed',            'value':'true'         },
      { 'name':'max_num_iter_fixed',     'value':'50'           },
      { 'name':'print_level_fixed',      'value':'0'            },
      { 'name':'tolerance_fixed',        'value':'1e-8'         },

      { 'name':'max_num_iter_random',    'value':'50'           },
      { 'name':'print_level_random',     'value':'0'            },
      { 'name':'tolerance_random',       'value':'1e-8'         }
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
new           = False
connection    = dismod_at.create_connection(file_name, new)
var_table     = dismod_at.get_table_dict(connection, 'var')
rate_table    = dismod_at.get_table_dict(connection, 'rate')
node_table    = dismod_at.get_table_dict(connection, 'node')
# -----------------------------------------------------------------------
# truth table:
tbl_name     = 'truth_var'
col_name     = [ 'truth_var_value' ]
col_type     = [ 'real' ]
row_list     = list()
var_id2true  = list()
for var_id in range( len(var_table) ) :
   var_row         = var_table[var_id]
   assert var_row['var_type'] == 'rate'
   #
   var_type  = var_row ['var_type']
   node_id   = var_row ['node_id']
   rate_id   = var_row['rate_id']
   rate_name = rate_table[rate_id]['rate_name']
   node_name = node_table[node_id]['node_name']
   #
   truth_var_value = None
   if rate_name == 'iota' :
      value = iota_true[node_name]
   elif rate_name == 'chi' :
      value = chi_true[node_name]
   else :
      assert False
   row_list.append( [ value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------
# Simulate one data set and fit it
dismod_at.system_command_prc([ program, file_name, 'simulate', '1' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both', '0' ])
# -----------------------------------------------------------------------
# check fit results
new           = False
connection    = dismod_at.create_connection(file_name, new)
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
#
for var_id in range( len(var_table) ) :
   fit_value = fit_var_table[var_id]['fit_var_value']
   var_row   = var_table[var_id]
   node_id   = var_row ['node_id']
   rate_id   = var_row['rate_id']
   rate_name = rate_table[rate_id]['rate_name']
   node_name = node_table[node_id]['node_name']
   if rate_name == 'iota' :
      relerr = fit_value / iota_true[node_name] - 1.0
      ok     = abs(relerr)  < .05
      if not ok :
         print( "iota relative error = ", relerr )
         assert False
   else :
      assert fit_value == chi_true[node_name]
# -----------------------------------------------------------------------------
print('const_random.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
