# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
iota_true                 = 0.01
# ------------------------------------------------------------------------
import sys
import os
import copy
import math
test_program  = 'test/user/hold_out.py'
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
# change into the build/test/user directory
if not os.path.exists('build/test/user') :
   os.makedirs('build/test/user')
os.chdir('build/test/user')
#
len_data_table = 20
random_seed    = 12345678
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
   # note that the a, t values are not used for this case
   def fun_iota(a, t) :
      return ('prior_iota', None, None)
   # ----------------------------------------------------------------------
   # age table:
   age_list    = [ 0.0, 100.0 ]
   #
   # time table:
   time_list   = [ 1990.0, 2200.0 ]
   #
   # integrand table:
   integrand_table = [
       { 'name':'Sincidence' }
   ]
   #
   # node table:
   node_table = [ { 'name':'world', 'parent':'' } ]
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
   # avgint table: empty
   avgint_table = list()
   #
   # nslist_dict:
   nslist_dict = dict()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   #
   # values that are the same for all data points
   row = {
      'density':     'gaussian',
      'meas_std':    iota_true / 10.,
      'weight':      '',
      'age_lower':    50.0,
      'age_upper':    50.0,
      'time_lower':   2000.,
      'time_upper':   2000.,
      'node':         'world',
      'subgroup':     'world',
   }
   # Sincidence data point with value 0.0 and data_table hold_out 1
   row['integrand']  = 'Sincidence'
   row['hold_out']   = True
   row['meas_value'] = 0.0
   data_table.append( copy.copy(row) )
   #
   # rest are Sincidence data with value iota_true and data_table hold_out 0
   row['integrand']  = 'Sincidence'
   row['hold_out']   = False
   row['meas_value'] = iota_true
   for data_id in range( 1 , len_data_table ) :
      data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota
         'name':     'prior_iota',
         'density':  'uniform',
         'lower':    iota_true / 10.0,
         'upper':    iota_true * 10.0,
         'mean':     iota_true * 2.0,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   name           = 'smooth_iota'
   fun            = fun_iota
   smooth_table = [
      {  'name':name,
         'age_id':[0],
         'time_id':[0],
         'fun':fun
      }
   ]
   # ----------------------------------------------------------------------
   # rate table:
   rate_table = [
      {  'name':          'iota',
         'parent_smooth': 'smooth_iota',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero'   },
      { 'name':'parent_node_name',       'value':'world'               },
      { 'name':'warn_on_stderr',         'value':'false'               },
      { 'name':'random_seed',            'value':str(random_seed)      },

      { 'name':'quasi_fixed',            'value':'false'               },
      { 'name':'max_num_iter_fixed',     'value':'50'                  },
      { 'name':'print_level_fixed',      'value':'0'                   },
      { 'name':'tolerance_fixed',        'value':'1e-8'                },

      { 'name':'max_num_iter_random',    'value':'50'                  },
      { 'name':'print_level_random',     'value':'0'                   },
      { 'name':'tolerance_random',       'value':'1e-10'               },
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
# Create database
file_name = 'example.db'
example_db(file_name)
#
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
#
integrand = 'Sincidence'
#
# hold out all the data (as a test of putting it back in)
max_fit   = "0"
dismod_at.system_command_prc(
   [ program, file_name, 'hold_out', integrand, max_fit ]
)
#
# hold out half of the data
max_fit   = len_data_table / 2
dismod_at.system_command_prc(
   [ program, file_name, 'hold_out', integrand, str(max_fit)  ]
)
# -----------------------------------------------------------------------
# read database
connection            = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
var_table             = dismod_at.get_table_dict(connection, 'var')
data_table            = dismod_at.get_table_dict(connection, 'data')
data_subset_table     = dismod_at.get_table_dict(connection, 'data_subset')
integrand_table       = dismod_at.get_table_dict(connection, 'integrand')
connection.close()
#
# only one varable in this model, iota
assert len(var_table) == 1
#
# all three data points are in the data_sebset table, so data_subset_id is
# the same as data_id (see data subset table documentation).
assert len(data_table) == len_data_table
assert len(data_subset_table) == len_data_table
#
# check hold_out in data table
fit_list = list()
for data_id in range(len_data_table) :
   assert data_id == data_subset_table[data_id]['data_id']
   #
   hold_out = data_table[data_id]['hold_out'] == 1
   if data_id == 0 :
      assert hold_out
      assert data_subset_table[data_id]['hold_out'] == 0
   else :
      assert not hold_out
   #
   hold_out = hold_out or data_subset_table[data_id]['hold_out'] == 1
   if not hold_out :
      fit_list.append( data_id )
assert len( fit_list ) == max_fit
#
# re-run hold_out command
dismod_at.system_command_prc(
   [ program, file_name, 'hold_out', integrand, str(max_fit)  ]
)
#
# data_subset_table
connection            = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
data_subset_table     = dismod_at.get_table_dict(connection, 'data_subset')
connection.close()
#
# check that setting random seed yeilds the exact same hold outs
check_list = list()
for data_id in range(len_data_table) :
   hold_out = data_table[data_id]['hold_out'] == 1
   hold_out = hold_out or data_subset_table[data_id]['hold_out'] == 1
   if not hold_out :
      check_list.append( data_id )
assert fit_list == check_list
#
# -----------------------------------------------------------------------------
print('hold_out.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
