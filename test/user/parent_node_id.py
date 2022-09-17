# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
import sys
import os
import copy
import subprocess
# ---------------------------------------------------------------------------
# check execution is from distribution directory
test_program = 'test/user/parent_node_id.py'
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
# import get_started_db example
sys.path.append( os.getcwd() + '/example/get_started' )
import get_started_db
#
# change into the test/user directory
if not os.path.exists('build/test/user') :
   os.makedirs('build/test/user')
os.chdir('build/test/user')
# ---------------------------------------------------------------------------
# create get_started.db
get_started_db.get_started_db()
# -----------------------------------------------------------------------
# In option table add row specifying parent_node_id
new            = False
file_name      = 'get_started.db'
connection     = dismod_at.create_connection(file_name, new)
option_table   = dismod_at.get_table_dict(connection, 'option')
node_table     = dismod_at.get_table_dict(connection, 'node')
parent_node_id = None
for row in option_table :
   if row['option_name'] == 'parent_node_name' :
      name = row['option_value']
      for node_id in range( len( node_table ) ) :
         if node_table[node_id]['node_name'] == name :
            assert parent_node_id == None
            parent_node_id = str( node_id )
assert parent_node_id != None
cmd  = "INSERT INTO option ('option_name', 'option_value')"
cmd += " VALUES('parent_node_id', '" + parent_node_id + "')"
dismod_at.sql_command(connection, cmd)
connection.close()
# -----------------------------------------------------------------------
program  = '../../devel/dismod_at'
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
new        = False
connection = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get variable and fit_var tables
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
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
print('parent_node_id.py: OK')
