#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_hold_out.py$$ $newlinech #$$
# $spell
#	num_iter
#	py
#	Integrands
#	Sincidence
#	exp
# $$
#
# $section Using hold_out in Data, Subset Data, and Option Tables$$
#
# $head Purpose$$
# This example shows how to use hold_out in the
# $cref/data_table/data_table/hold_out/$$,
# $cref/option_table/option_table/hold_out_integrand/$$, and
# $cref hold_out_command$$.
#
# $head Integrands$$
# For this example there are two integrand,
# $code Sincidence$$ and $code prevalence$$.
#
# $head Nodes$$
# There is only one node called $code world$$ for this example.
# There are no random effects because there are no child nodes.
#
# $head Data$$
#
# $subhead prevalence$$
# All of the prevalence data is zero, but it is held out using
# $cref/hold_out_integrand/option_table/hold_out_integrand/$$.
#
# $subhead Sincidence$$
# There are three incidence data points.
# The first Sincidence data value is zero and it is held out using
# the data table $cref/hold_out/data_table/hold_out/$$ equal to one.
# The other two Sincidence data are
# the true value for incidence and have the data table hold_out
# equal to zero.
# The $cref hold_out_command$$ is used to randomly select one of these
# two points to be held out.
#
# $head Model$$
# There is only one rate $icode iota$$ and it is constant in age and time.
# The corresponding model for the Sincidence data is $icode iota$$.
# The corresponding mode for the prevalence data is
# $codei%1 - exp( %iota% * %age%)%$$.
#
# $head Fit$$
# Because the zero prevalence data and zero incidence data is held out,
# the fitting value for $icode iota$$ is very close to the true value
# for incidence.
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# values used to simulate data
iota_true                 = 0.01
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import copy
import math
test_program = 'example/user/hold_out.py'
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
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
	# note that the a, t values are not used for this case
	def fun_iota(a, t) :
		return ('prior_iota', None, None)
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 5.0, 15.0, 35.0, 50.0, 75.0, 90.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
	#
	# integrand table:
	integrand_table = [
		 { 'name':'Sincidence' },
	     { 'name':'prevalence' }
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
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	#
	# values that are the same for all data points
	row = {
		'density':     'gaussian',
		'meas_std':    iota_true / 10.,
		'weight':      '',
		'age_lower':    50.0,
		'age_upper':    50.0,
		'time_lower':   2000.,
		'time_upper':   2000.,
		'node':         'world',
		'subgroup':     'world',
	}
	# Sincidence data point with value 0.0 and data_table hold_out 1
	row['integrand']  = 'Sincidence'
	row['hold_out']   = True
	row['meas_value'] = 0.0
	data_table.append( copy.copy(row) )
	#
	# Two Sincidence data points with value iota_true and data_table hold_out 0
	row['integrand']  = 'Sincidence'
	row['hold_out']   = False
	row['meas_value'] = iota_true
	data_table.append( copy.copy(row) )
	data_table.append( copy.copy(row) )
	#
	# prevalence data point with value 0.0 and held out in data table 0
	row['integrand']  = 'prevalence'
	row['meas_value'] = 0.0
	row['hold_out']   = False;
	data_table.append( copy.copy(row) )
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
		{	'name':name,
			'age_id':[0],
			'time_id':[0],
			'fun':fun
		}
	]
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_iota',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero'   },
		{ 'name':'parent_node_name',       'value':'world'               },
		{ 'name':'warn_on_stderr',         'value':'false'               },

		{ 'name':'quasi_fixed',            'value':'false'               },
		{ 'name':'max_num_iter_fixed',     'value':'50'                  },
		{ 'name':'print_level_fixed',      'value':'0'                   },
		{ 'name':'tolerance_fixed',        'value':'1e-8'                },

		{ 'name':'max_num_iter_random',    'value':'50'                  },
		{ 'name':'print_level_random',     'value':'0'                   },
		{ 'name':'tolerance_random',       'value':'1e-10'               },
	]
	# hold out all prevalence data no matter what hold_out in data table
	option_table.append(
		{ 'name':'hold_out_integrand',     'value':'prevalence'          }
	)
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
		nslist_table,
		rate_table,
		mulcov_table,
		option_table
	)
	# ----------------------------------------------------------------------
	return
# ===========================================================================
# Create database
file_name = 'example.db'
example_db(file_name)
#
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
#
integrand = 'Sincidence'
max_fit   = "1"
dismod_at.system_command_prc(
	[ program, file_name, 'hold_out', integrand, max_fit ]
)
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
# -----------------------------------------------------------------------
# read database
new                   = False
connection            = dismod_at.create_connection(file_name, new)
var_table             = dismod_at.get_table_dict(connection, 'var')
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
data_table            = dismod_at.get_table_dict(connection, 'data')
data_subset_table     = dismod_at.get_table_dict(connection, 'data_subset')
integrand_table       = dismod_at.get_table_dict(connection, 'integrand')
fit_data_subset_table = dismod_at.get_table_dict(connection, 'fit_data_subset')
connection.close()
#
# only one varable in this model, iota
assert len(var_table) == 1
assert len(fit_var_table) == 1
#
# all four data points are in the data_sebset table, so data_subset_id is
# the same as data_id (see data subset table documentation).
assert len(data_table) == 4
assert len(data_subset_table) == 4
assert len(fit_data_subset_table) == 4
#
# check hold_out in data table
assert data_table[0]['hold_out'] == 1
assert data_table[1]['hold_out'] == 0
assert data_table[2]['hold_out'] == 0
assert data_table[3]['hold_out'] == 0
#
# check hold_out in data_subset table
count_data_subset_hold_out = 0
for data_id in range(4) :
	assert data_id == data_subset_table[data_id]['data_id']
	hold_out        = data_table[data_id]['hold_out']
	integrand_id    = data_table[data_id]['integrand_id']
	integrand_name  = integrand_table[integrand_id]['integrand_name']
	if hold_out == 1 :
		assert data_subset_table[data_id]['hold_out'] == 0
	if integrand_name != 'Sincidence' :
		assert data_subset_table[data_id]['hold_out'] == 0
	if data_subset_table[data_id]['hold_out'] == 1 :
		count_data_subset_hold_out += 1
assert count_data_subset_hold_out == 1
#
# check fitted value for iota
iota_fit = fit_var_table[0]['fit_var_value']
assert abs( 1.0 - iota_fit / iota_true ) < 1e-6
#
# check residual for the second and third data points
for data_id in [1, 2] :
	data_row       = data_table[data_id]
	fit_row        = fit_data_subset_table[data_id]
	integrand_id   = data_row['integrand_id']
	integrand_name = integrand_table[integrand_id]['integrand_name']
	meas_std       = data_row['meas_std']
	assert integrand_name == 'Sincidence'
	assert data_row['hold_out'] == 0
	assert fit_row['weighted_residual'] * meas_std < 1e-6
#
# check residual for the first data point (which should be -iota_true)
data_row       = data_table[0]
fit_row        = fit_data_subset_table[0]
integrand_id   = data_row['integrand_id']
integrand_name = integrand_table[integrand_id]['integrand_name']
meas_std       = data_row['meas_std']
assert integrand_name == 'Sincidence'
assert data_row['hold_out'] == 1
assert data_row['meas_value'] == 0
# un-weight the residual and flip sign
model_minus_data = - fit_row['weighted_residual'] * meas_std
assert abs( 1.0 - model_minus_data / iota_true ) < 1e-6
#
# check residual for the fourth data point (which should be - true prevalence)
data_row       = data_table[3]
fit_row        = fit_data_subset_table[3]
integrand_id   = data_row['integrand_id']
integrand_name = integrand_table[integrand_id]['integrand_name']
meas_std       = data_row['meas_std']
age            = data_row['age_lower']
assert integrand_name == 'prevalence'
assert data_row['hold_out'] == 0
assert data_row['meas_value'] == 0
assert age == data_row['age_upper']
# un-weight the residual and flip sign
model_minus_data = - fit_row['weighted_residual'] * meas_std
prevalence_true  = 1.0 - math.exp( - iota_true * age )
assert abs( 1.0 - model_minus_data / prevalence_true ) < 1e-6
#
# -----------------------------------------------------------------------------
print('hold_out.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
