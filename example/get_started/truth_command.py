# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin truth_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section dismod_at truth: Example and Test$$
#
# $code
# $verbatim%
#	example/get_started/truth_command.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
import subprocess
import distutils.dir_util
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/truth_command.py'
if sys.argv[0] != example  or len(sys.argv) != 1 :
	usage  = 'python3 ' + example + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# import dismod_at
sys.path.append( os.getcwd() + '/python' )
import dismod_at
#
# import get_started_db example
sys.path.append( os.getcwd() + '/example/get_started' )
import get_started_db
#
# change into the build/example/get_started directory
distutils.dir_util.mkpath('build/example/get_started')
os.chdir('build/example/get_started')
# ---------------------------------------------------------------------------
# create input tables
file_name              = 'example.db'
(n_smooth, rate_true)  = get_started_db.get_started_db(file_name)
# -----------------------------------------------------------------------
program        = '../../devel/dismod_at'
for command in [ 'init', 'start', 'fit', 'truth'  ] :
	cmd = [ program, file_name, command ]
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get fit and truth_var tables
fit_var_dict= dismod_at.get_table_dict(connection, 'fit_var')
truth_var_dict  = dismod_at.get_table_dict(connection, 'truth_var')
# -----------------------------------------------------------------------
# check result
assert len(fit_var_dict) == len(truth_var_dict)
for fit_var_id in range( len(fit_var_dict) ) :
	variable_value   = fit_var_dict[fit_var_id]['variable_value']
	truth_var_value = truth_var_dict[fit_var_id]['truth_var_value']
	if truth_var_value == 0.0 :
		assert variable_value == truth_var_value
	else :
		assert abs( variable_value / truth_var_value - 1.0 ) <= 1e-10
# -----------------------------------------------------------------------
print('truth_command: OK')
# END PYTHON
