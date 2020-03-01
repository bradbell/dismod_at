# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin predict_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section predict Command: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
import distutils.dir_util
from math import exp
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/predict_command.py'
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
dismod_at.system_command_prc( [program, file_name, 'init'] )
dismod_at.system_command_prc( [program, file_name, 'fit', 'both'] )
dismod_at.system_command_prc( [program, file_name, 'predict', 'fit_var'] )
# -----------------------------------------------------------------------
# connect to database
new        = False
connection = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get variable and predict tables
var_table      = dismod_at.get_table_dict(connection, 'var')
fit_var_table  = dismod_at.get_table_dict(connection, 'fit_var')
avgint_table   = dismod_at.get_table_dict(connection, 'avgint')
predict_table  = dismod_at.get_table_dict(connection, 'predict')
assert len(avgint_table) == 1
#
for var_id in range( len(var_table) ) :
	var_row     = var_table[var_id]
	fit_row     = fit_var_table[var_id]
	var_type    = var_row['var_type']
	if var_type == 'mulcov_rate_value' :
		income_multiplier = fit_row['fit_var_value']
	elif var_type == 'rate' :
		omega_world = fit_row['fit_var_value']
	else :
		assert False
#
# use fact that avgint_id == avgint_subset_id and number_sample == 1
avgint_row = avgint_table[0]
#
assert avgint_row['age_lower'] == avgint_row['age_upper']
age     = avgint_row['age_lower']
income  = avgint_row['x_0']
adjusted_omega = omega_world * exp(income_multiplier * income)
model_value    = exp( - adjusted_omega * age)
#
assert len(predict_table) == len(avgint_table)
predict_row   = predict_table[0]
avg_integrand = predict_row['avg_integrand']
assert abs( avg_integrand / model_value - 1.0 ) < 1e-7
#
# -----------------------------------------------------------------------
print('predict_command: OK')
# END PYTHON
