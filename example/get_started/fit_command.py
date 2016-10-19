# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin fit_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section dismod_at fit: Example and Test$$
#
# $code
# $srcfile%
#	example/get_started/fit_command.py
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
example = 'example/get_started/fit_command.py'
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
for command in [ 'init', 'start', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'simulate' :
		number_simulate = '1'
		cmd.append(number_simulate)
	if command == 'start' :
		cmd.append('prior_mean')
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get variable and fit_var tables
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
# mulstd variables
for smooth_id in range( n_smooth ) :
	for var_type in [ 'mulstd_value', 'mulstd_dage', 'mulstd_dtime' ] :
		count = 0
		for var_id in range( len(var_table) ) :
			row   = var_table[var_id]
			match = row['var_type'] == var_type
			match = match and row['smooth_id'] == smooth_id
			if match :
				count += 1
		assert count == 0
#
# rate variables
parent_node_id = 0
child_node_id  = 1
check_tol      = 1e-3
n_rate         = 5;
rate_value     = list()
for rate_id in range(n_rate) :
	for node_id in [ parent_node_id, child_node_id ] :
		count = 0
		for var_id in range( len(var_table) ) :
			row   = var_table[var_id]
			match = row['var_type'] == 'rate'
			match = match and row['rate_id'] == rate_id
			match = match and row['node_id'] == node_id
			if match :
				count += 1
				#
				# check variable_value
				check          = rate_true[rate_id]
				fit_var_id     = var_id
				variable_value  = fit_var_table[fit_var_id]['variable_value']
				if node_id == parent_node_id :
					err = variable_value / check - 1.0
					if count == 1 :
						rate_value.append( variable_value )
				else :
					# child node
					err = variable_value / check
				assert abs(err) <= check_tol
				#
				# check residual_value
				residual_value  = fit_var_table[fit_var_id]['residual_value']
				if node_id == parent_node_id :
					# uniform prior
					assert residual_value == None
				else :
					# gaussian prior
					assert abs(residual_value) <= check_tol
				#
				# check residual_dage
				residual_dage  = fit_var_table[fit_var_id]['residual_dage']
				# only one age point in each smoothing grid
				assert residual_dage == None
				#
				# check residual_dtime
				residual_dtime  = fit_var_table[fit_var_id]['residual_dtime']
				time_id = row['time_id']
				if time_id == 2 :
					assert residual_dtime == None
				else :
					assert time_id == 0
					# gaussian prior
					assert abs(residual_dtime) <= check_tol
				#
				# check lagrange_value
				lagrange_value = fit_var_table[fit_var_id]['lagrange_value']
				assert lagrange_value == 0.0
				#
				# check lagrange_dage
				lagrange_dage = fit_var_table[fit_var_id]['lagrange_dage']
				assert lagrange_dage == 0.0
				#
				# check lagrange_dtime
				lagrange_dtime = fit_var_table[fit_var_id]['lagrange_dtime']
				assert lagrange_dtime == 0.0

		# number of point in smoothing for all rates
		assert count == 2
assert len(rate_value) == n_rate
# -----------------------------------------------------------------------
# get fit_data_subset table
fit_data_subset_dict = dismod_at.get_table_dict(connection, 'fit_data_subset')
data_table       = dismod_at.get_table_dict(connection, 'data')
density_dict      = dismod_at.get_table_dict(connection, 'density')
#
# all data included in subset
assert len(fit_data_subset_dict) == n_rate + 1;
#
# first n_rate date entries
check_tol = 1e-8
for data_id in range(n_rate) :
	rate_id       = data_id
	avg_integrand = fit_data_subset_dict[data_id]['avg_integrand']
	err           = avg_integrand / rate_value[rate_id] - 1.0;
	#
	weighted_residual = fit_data_subset_dict[data_id]['avg_integrand']
	meas_value        = data_table[data_id]['meas_value']
	meas_std          = data_table[data_id]['meas_std']
	density_id        = data_table[data_id]['density_id']
	assert density_dict[density_id]['density_name'] == 'gaussian'
	check             = (meas_value - avg_integrand) / meas_std;
	err               = weighted_residual / rate_value[rate_id] - 1.0;

#
# -----------------------------------------------------------------------
print('fit_command: OK')
# END PYTHON
