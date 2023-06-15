# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_other_database.py}
# {xrst_comment_ch #}
#
# Using Data Interval Compression
# ###############################
#
# Purpose
# *******
# This example shows how to use the
# :ref:`option_table@Other Database` options.
#
# Integrands
# **********
# For this example there are one integrand, ``Sincidence`` .
#
# Nodes
# *****
# There is only one node called ``world`` for this example.
# There are no random effects because there are no child nodes.
#
# True Iota
# *********
# For this example, the true incidence rate *iota* is the following
# constant value:
# {xrst_code py}
iota_true = 0.01
# {xrst_code}
#
# Model
# *****
# There is only one rate *iota* and it constant w.r.t age and time.
#
# Data
# ****
# There is one data point measuring Sincidence
# with the true value of *iota* .
# but faster to compute.
#
# other_input_table
# *****************
# For this example,
# the :ref:`option_table@Other Database@other_input_table` is
# {xrst_code py}
other_input_table = 'data node'
# {xrst_code}
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_other_database.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
import sys
import shutil
import os
import csv
import copy
import math
test_program = 'example/user/other_database.py'
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
def example_db (file_name) :
   # note that the a, t values are not used for this case
   def fun_iota(a, t) :
      return ('prior_iota', None, None)
   # ----------------------------------------------------------------------
   # age table:
   age_list    = [ 0.0, 50.0, 100.0 ]
   #
   # time table:
   time_list   = [ 1990.0, 2000.0, 2010.0, 2020.0 ]
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
      'integrand':   'Sincidence',
      'hold_out':    False,
      'density':     'gaussian',
      'meas_std':    iota_true / 10.,
      'weight':      '',
      'time_lower':   2000.,
      'time_upper':   2000.,
      'node':         'world',
      'subgroup':     'world',
      'meas_value':   iota_true,
      'age_lower':    50.0,
      'age_upper':    50.0,
      'time_lower':   2000.0,
      'time_upper':   2000.0,
   }
   data_table.append(row)
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

      { 'name':'quasi_fixed',            'value':'false'               },
      { 'name':'max_num_iter_fixed',     'value':'50'                  },
      { 'name':'print_level_fixed',      'value':'0'                   },
      { 'name':'tolerance_fixed',        'value':'1e-9'                },

      { 'name':'max_num_iter_random',    'value':'50'                  },
      { 'name':'print_level_random',     'value':'0'                   },
      { 'name':'tolerance_random',       'value':'1e-10'               },

      { 'name':'other_database',         'value':'other.db'            },
      { 'name':'other_input_table',      'value':other_input_table     },
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
#
# example.db
file_name = 'example.db'
example_db(file_name)
#
# other.db
shutil.copyfile(file_name, 'other.db')
#
# example.db
connection = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
for table_name in other_input_table.split() :
   command    = 'DROP TABLE ' + table_name
   dismod_at.sql_command(connection, command)
command    = 'VACUUM;'
dismod_at.sql_command(connection, command)
connection.close()
#
assert os.stat(file_name).st_size < os.stat('other.db').st_size
#
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
# -----------------------------------------------------------------------
# read database
connection            = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
var_table             = dismod_at.get_table_dict(connection, 'var')
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
#
# There is only one variables, iota
assert len(var_table) == 1
assert len(fit_var_table) == 1
#
# check that the fit is accurate
for var_id in range( len(var_table) ) :
   true_value  = iota_true
   fit_value   = fit_var_table[var_id]['fit_var_value']
   rel_error   = 1.0 - fit_value/true_value
   assert abs(rel_error) < 1e-6
#
# Test db2csv
dismod_at.db2csv_command(file_name)
# -----------------------------------------------------------------------------
print('other_database.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
