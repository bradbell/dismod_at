# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin init_command.py}
# {xrst_comment_ch #}
#
# init Command: Example and Test
# ##############################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end init_command.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/init_command.py'
if sys.argv[0] != example  or len(sys.argv) != 1 :
   usage  = 'python3 ' + example + '\n'
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
# import get_started_db example
sys.path.append( os.getcwd() + '/example/get_started' )
import get_started_db
#
# change into the build/example/get_started directory
if not os.path.exists('build/example/get_started') :
   os.makedirs('build/example/get_started')
os.chdir('build/example/get_started')
# ---------------------------------------------------------------------------
# create get_started.db
get_started_db.get_started_db()
# -----------------------------------------------------------------------
program        = '../../devel/dismod_at'
file_name      = 'get_started.db'
dismod_at.system_command_prc( [program, file_name, 'init'] )
# -----------------------------------------------------------------------
# connect to database
connection = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
#
# check data_subset table
data_subset_table = dismod_at.get_table_dict(connection, 'data_subset');
assert len(data_subset_table) == 1
assert data_subset_table[0]['data_id'] == 0
# -----------------------------------------------------------------------
# check var table and determine prior_mean for each variable
var_table        = dismod_at.get_table_dict(connection, 'var')
covariate_table  = dismod_at.get_table_dict(connection, 'covariate')
smooth_table     = dismod_at.get_table_dict(connection, 'smooth')
smooth_grid      = dismod_at.get_table_dict(connection, 'smooth_grid')
prior_table      = dismod_at.get_table_dict(connection, 'prior')
rate_table       = dismod_at.get_table_dict(connection, 'rate')
assert len(var_table) == 2
prior_mean       = list()
for row in var_table :
   var_type     = row['var_type']
   smooth_id    = row['smooth_id']
   age_id       = row['age_id']
   time_id      = row['time_id']
   node_id      = row['node_id']
   rate_id      = row['rate_id']
   integrand_id = row['integrand_id']
   covariate_id = row['covariate_id']
   mulcov_id    = row['mulcov_id']
   smooth_name  = smooth_table[smooth_id]['smooth_name']
   rate_name    = rate_table[rate_id]['rate_name']
   #
   assert age_id  == 0
   assert time_id == 0
   #
   # determine prior_mean for this variable
   prior_id = len( prior_table )
   for grid_row in smooth_grid :
      if grid_row['smooth_id'] == smooth_id :
         assert grid_row['age_id'] == age_id
         assert grid_row['time_id'] == time_id
         prior_id = grid_row['value_prior_id']
   assert prior_id < len( prior_table )
   prior_mean.append( prior_table[prior_id]['mean'] )
   #
   if var_type == 'mulcov_rate_value' :
      assert smooth_name == 'smooth_income_multiplier'
      assert rate_name == 'omega'
      assert mulcov_id == 0
      #
      covariate_name = covariate_table[covariate_id]['covariate_name']
      assert covariate_name == 'income'
      #
      assert node_id == None
      assert integrand_id  == None
   elif var_type == 'rate' :
      assert smooth_name == 'smooth_omega_parent'
      assert rate_name == 'omega'
      assert node_id == 0
      #
      assert covariate_id == None
      assert integrand_id  == None
      assert mulcov_id == None
   else :
      assert False
# -----------------------------------------------------------------------
# check start_var and scale_var tables
start_var = dismod_at.get_table_dict(connection, 'start_var')
scale_var = dismod_at.get_table_dict(connection, 'scale_var')
for var_id in range( len(var_table) ) :
   assert start_var[var_id]['start_var_value'] == prior_mean[var_id]
   assert scale_var[var_id]['scale_var_value'] == prior_mean[var_id]
# -----------------------------------------------------------------------
print('init_command: OK')
# END PYTHON
