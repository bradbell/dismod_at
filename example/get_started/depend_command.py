# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin depend_command.py$$ $newlinech #$$
# $spell
#  dismod
# $$
#
# $nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section depend Command: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/depend_command.py'
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
dismod_at.system_command_prc( [program, file_name, 'depend'] )
# -----------------------------------------------------------------------
# connect to database
new        = False
connection = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get variable and depend_var tables
var_table         = dismod_at.get_table_dict(connection, 'var')
depend_var_table  = dismod_at.get_table_dict(connection, 'depend_var')
#
for var_id in range( len(var_table) ) :
   var_row         = var_table[var_id]
   depend_var_row  = depend_var_table[var_id]
   # data depends on both variables
   assert depend_var_row[ 'data_depend' ] == 1
   # uniform prior does not depend on argument value
   assert depend_var_row[ 'prior_depend' ] == 0
# -----------------------------------------------------------------------
print('depend_command: OK')
# END PYTHON
