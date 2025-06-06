# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# 1. Check meas_std, meas_stdcv, and meas_sigma in data.csv
# ---------------------------------------------------------------------------
#
iota_true       = 1e-1
meas_value      = 1.1 * iota_true
meas_std        = 1e-5
minimum_meas_cv = 1e-1
meas_stdcv      = max(minimum_meas_cv * meas_value, meas_std)
gamma_noise     = meas_stdcv * meas_stdcv
eta             = meas_value / 100.0
# ---------------------------------------------------------------------------
import sys
import os
import subprocess
import copy
import csv
from math import log, exp, sqrt
test_program  = 'test/user/db2csv.py'
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
# ---------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def constant_weight_fun(a, t) :
   return 1.0
def fun_iota_parent(a, t) :
   return ('prior_iota_parent', None, None)
def fun_iota_child(a, t) :
   return ('prior_iota_child', None, None)
def fun_noise(a, t) :
   return ('prior_noise', None, None)
def fun_flat(a, t) :
   return ('prior_flat', None, None)
# ------------------------------------------------------------------------
def example_db (file_name) :
   # ----------------------------------------------------------------------
   # age table
   age_list    = [    0.0,   100.0 ]
   #
   # time table
   time_list   = [ 1990.0,  2010.0 ]
   #
   # integrand table
   integrand_table = [
       { 'name':'Sincidence', 'minimum_meas_cv':minimum_meas_cv },
   ]
   #
   # node table: north_america
   node_table = [
      { 'name':'north_america', 'parent':'' },
      { 'name':'canada',        'parent':'north_america' },
   ]
   #
   # weight table: The constant function 1.0 (one age and one time point)
   fun = constant_weight_fun
   weight_table = [
      { 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
   ]
   #
   # covariate table:
   covariate_table = [
      {'name':'one', 'reference':0.0}
   ]
   #
   # mulcov table:
   mulcov_table = [
      {  # meas_noise covariates not affected by max_abs_effect
         'covariate': 'one',
         'type':      'meas_noise',
         'effected':  'Sincidence',
         'group':     'world',
         'smooth':    'smooth_noise'
      },{
         # This covariate held out of fit because max_abs_effect is 0
         'covariate': 'one',
         'type':      'meas_value',
         'effected':  'Sincidence',
         'group':     'world',
         'smooth':    'smooth_flat'
      }
   ]
   #
   # nslist_dict:
   nslist_dict = dict()
   #
   # avgint_table
   avgint_table = list()
   # ----------------------------------------------------------------------
   # data table: same order as age_list
   data_table = list()
   # values that are the same for all data rows
   row = {
      'node':        'north_america',
      'subgroup':    'world',
      'weight':      'constant',
      'hold_out':     False,
      'time_lower':   2000.0,
      'time_upper':   2000.0,
      'integrand':   'Sincidence',
      'age_lower':    50.0,
      'age_upper':    50.0,
      'meas_value':   meas_value,
      'meas_std':     meas_std,
      'eta':          eta,
      'one':          1.0
   }
   row['density'] = 'gaussian'
   data_table.append( copy.copy(row) )
   row['density'] = 'log_gaussian'
   data_table.append( copy.copy(row) )
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      {  # prior_iota_parent
         'name':     'prior_iota_parent',
         'density':  'uniform',
         'lower':    iota_true / 10.0,
         'mean':     iota_true,
         'upper':    10.0 *iota_true
      },{  # prior_iota_child
         'name':     'prior_iota_child',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      1.0,
      },{  # prior_noise
         'name':     'prior_noise',
         'density':  'uniform',
         'lower':    0.00,
         'mean':     gamma_noise,
         'upper':    10.0 * gamma_noise
      },{  # priot_flat
         'name':     'prior_flat',
         'density':  'uniform',
         'mean':     0.0,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      {  # smooth_iota_parent
         'name':                     'smooth_iota_parent',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_iota_parent
      },{  # smooth_iota_child
         'name':                     'smooth_iota_child',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_iota_child
      },{  # smooth_noise
         'name':                     'smooth_noise',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_noise
      },{  # smooth_flat
         'name':                     'smooth_flat',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_flat
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {  'name':          'iota',
         'parent_smooth': 'smooth_iota_parent',
         'child_smooth':  'smooth_iota_child',
      },
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'max_num_iter_fixed', 'value':'-1' },
      { 'name':'parent_node_name',   'value':'north_america' },
      { 'name':'meas_noise_effect',  'value':'add_var_scale_none' },
      { 'name':'print_level_fixed',  'value':'0' },
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
   return
#
# create the database
file_name  = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
command_list = [
   # db2csv used to crash when hold_out_integrand was null in option table
   [ program  , file_name, 'set' , 'option', 'hold_out_integrand', '' ],
   #
   [ program  , file_name, 'init' ],
   [ program  , file_name, 'bnd_mulcov', '0.0' ],
   [ program  , file_name, 'fit', 'fixed' ],
   # db2csv used to crash if last fit had a warm_start
   [ program  , file_name, 'fit', 'fixed', 'warm_start' ],
]
for command in command_list :
   dismod_at.system_command_prc( command )
# must go back to distribution directory to run bin/dismodat.py in sandbox
os.chdir('../../..')
python_exe = dismod_at.python3_executable
program    = 'python/bin/dismodat.py'
database   = 'build/test/user/' + file_name
command    = [ python_exe, program, database, 'db2csv' ]
dismod_at.system_command_prc( command )
# -----------------------------------------------------------------------
# variable.csv
file_name    = open('build/test/user/variable.csv', 'r')
reader       = csv.DictReader(file_name)
count_mulcov = 0
for row in reader :
   # Check bnd_mulcov informaiton
   if row['var_type'].startswith('mulcov_') :
      count_mulcov += 1
      max_cov_diff = float( row['m_diff'] )
      assert max_cov_diff == 1.0
      if row['var_type'] == 'mulcov_meas_noise' :
         assert row['bound'] == ''
      else :
         assert float( row['bound'] ) == 0.0
   # canada has no data check that its random effect bounds are zero
   if row['var_type'] == 'rate' :
      node_name = row['node']
      if node_name == 'canada' :
         assert float( row['bound'] ) == 0.0
      else :
         assert node_name == 'north_america'
         assert row['bound'] == ''
# -----------------------------------------------------------------------
# data.csv
file_name = open('build/test/user/data.csv', 'r')
reader    = csv.DictReader(file_name)
for row in reader :
   #
   # meas_std
   result = float( row['meas_std'] )
   assert abs( 1.0 - result / meas_std) < 1e-4
   #
   # meas_stdcv
   result = float( row['meas_stdcv'] )
   assert abs( 1.0 - result / meas_stdcv) < 1e-4
   #
   # sigma
   sigma = sqrt( meas_stdcv * meas_stdcv + gamma_noise)
   #
   result = float( row['meas_sigma'] )
   assert abs( 1.0 - result / sigma) < 1e-4
# -----------------------------------------------------------------------
# mixed_info.csv
file_name = open('build/test/user/mixed_info.csv', 'r')
reader    = csv.DictReader(file_name)
mixed_name_list = [
   "n_fixed",
   "n_random",
   "quasi_fixed",
   "A_nr",
   "A_nnz",
   "ran_like_fun.size_var",
   "fix_like_fun.size_var"
]
mixed_info_table = list()
for row in reader :
   mixed_info_table.append(row)
for mixed_name in mixed_name_list :
   found = False
   for row in mixed_info_table :
      if row['mixed_name'] == mixed_name :
         found = True
   assert found
# -----------------------------------------------------------------------------
print('db2csv.py: OK')
sys.exit(0)
