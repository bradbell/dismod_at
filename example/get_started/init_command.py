# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin init_command.py$$ $newlinech #$$
# $spell
#	init
#	dismod
# $$
#
# $section dismod_at init: Example and Test$$
#
# $code
# $srcfile%
#	example/get_started/init_command.py
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
distutils.dir_util.mkpath('build/example/get_started')
os.chdir('build/example/get_started')
# ---------------------------------------------------------------------------
# create get_started.db
get_started_db.get_started_db()
# -----------------------------------------------------------------------
program        = '../../devel/dismod_at'
file_name      = 'get_started.db'
command        = 'init'
cmd            = [ program, file_name, command ]
#
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
# -----------------------------------------------------------------------
# connect to database
new        = False
connection = dismod_at.create_connection(file_name, new)
#
# check data_subset table
data_subset_table = dismod_at.get_table_dict(connection, 'data_subset');
assert len(data_subset_table) == 1
assert data_subset_table[0]['data_id'] == 0
# -----------------------------------------------------------------------
# check var table
var_table        = dismod_at.get_table_dict(connection, 'var')
integrand_list  = dismod_at.get_table_dict(connection, 'integrand')
covariate_table  = dismod_at.get_table_dict(connection, 'covariate')
smooth_table     = dismod_at.get_table_dict(connection, 'smooth')
rate_table       = dismod_at.get_table_dict(connection, 'rate')
assert len(var_table) == 2
for row in var_table :
	var_type     = row['var_type']
	smooth_id    = row['smooth_id']
	age_id       = row['age_id']
	time_id      = row['time_id']
	node_id      = row['node_id']
	rate_id      = row['rate_id']
	integrand_id = row['integrand_id']
	covariate_id = row['covariate_id']
	smooth_name  = smooth_table[smooth_id]['smooth_name']
	rate_name    = rate_table[rate_id]['rate_name']
	#
	assert age_id  == 0
	assert time_id == 0
	if var_type == 'mulcov_rate_value' :
		assert smooth_name    == 'smooth_income_multiplier'
		assert rate_name == 'omega'
		#
		covariate_name = covariate_table[covariate_id]['covariate_name']
		assert covariate_name == 'income'
		#
		node_id == None
		integrand_id  == None
	elif var_type == 'rate' :
		assert smooth_name  == 'smooth_omega_parent'
		assert rate_name == 'omega'
		assert node_id == 0
		#
		covariate_id == None
		integrand_id  == None
	else :
		assert False
# -----------------------------------------------------------------------
print('init_command: OK')
# END PYTHON
