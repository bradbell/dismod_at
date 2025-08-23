# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin fit_command.py}
# {xrst_comment_ch #}
#
# fit Command: Example and Test
# #############################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end fit_command.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
import subprocess
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/fit_command.py'
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
for command in [ 'init', 'fit' ] :
   cmd = [ program, file_name, command ]
   if command == 'fit' :
      variables = 'both'
      cmd.append(variables)
   print( ' '.join(cmd) )
   flag = subprocess.call( cmd )
   if flag != 0 :
      sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
connection = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
# -----------------------------------------------------------------------
# get variable and fit_var tables
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
#
for var_id in range( len(var_table) ) :
   var_row     = var_table[var_id]
   fit_row     = fit_var_table[var_id]
   var_type    = var_row['var_type']
   if var_type == 'mulcov_rate_value' :
      income_multiplier = fit_row['fit_var_value']
      assert income_multiplier == -1e-3
   elif var_type == 'rate' :
      omega_world = fit_row['fit_var_value']
      assert abs( omega_world / 1e-2 - 1.0) < 1e-6
   else :
      assert False
# -----------------------------------------------------------------------
print('fit_command: OK')
# END PYTHON
