#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_bnd_mulcov.py$$ $newlinech #$$
# $spell
#	Integrands
#	Sincidence
#	bnd_mulcov
#	covariate
#	euros
# $$
#
# $section Using Data Interval Compression$$
#
# $head Purpose$$
# This example shows how to use the
# $cref bnd_mulcov_command$$.
#
# $head Integrands$$
# For this example there are one integrand, $code Sincidence$$.
#
# $head Nodes$$
# There are two nodes, one called $code world$$ and the other called
# $code europe$$.
# The parent node for this example is europe.
# This is to demonstrate that the world node is not included when
# the bnd_mulcov command computes its limits for the covariate multiplier.
#
# $head True Iota$$
# For this example, the true incidence rates for europe is
# $srccode%py%
import math
mulcov_true       = -1e-3
europe_avg_income = 1e3
def iota_europe_true(income) :
	return 0.01 * math.exp( mulcov_true * (income - europe_avg_income) )
# %$$
# Here $icode europe_avg_income$$ is the reference value for
# income in europe.
#
# $head Parent Node$$
# For this example the parent node is europe and hence world data
# is not included in the $cref data_subset_table$$.
#
# $head Data$$
# There are three data points measuring Sincidence.
# The first point is for the world, and the other two are for europe.
# The data include a covariate column that is income per year in euros.
# The world data is not in the $cref data_subset_table$$ and
# hence is not used when converting the maximum covariate effect to bounds
# on the covariate multiplier.
#
# $head Model$$
# There is only one rate $icode iota$$ and it is constant as a function
# of age and time. In addition, there is one covariate multiplier for income.
#
# $head bnd_mulcov$$
# The maximum absolute effect for the income multiplier is
# $srccode%py%
max_abs_effect = 3.0
# %$$
# Note that the bound on the covariate multiplier have units
# while the maximum absolute effect does not.
# The income values in the europe data are
# $icode%europe_avg_income% / 2%$$ and
# $icode%europe_avg_income% * 2%$$.
# The corresponding difference from the income reference are
# $codei% - %europe_avg_income% / 2%$$ and
# $icode%europe_avg_income%$$.
# This yields the following value
# $cref/max_upper/bnd_mulcov_table/max_upper/$$:
# $codei%
#	%max_upper% = + %max_abs_effect% / %europe_avg_income%
# %$$.
#
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
import sys
import os
import csv
import distutils.dir_util
import copy
import math
test_program = 'example/user/bnd_mulcov.py'
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
	def fun_mulcov(a, t) :
		return ('prior_mulcov', None, None)
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 50.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
	#
	# integrand table:
	integrand_table = [
		 { 'name':'Sincidence' }
	]
	#
	# node table:
	node_table = [
		{ 'name':'world',  'parent':'' },
		{ 'name':'europe', 'parent':'world' },
	]
	#
	# weight table:
	weight_table = list()
	#
	# covariate table:
	covariate_table = [{
		'name':           'income',
		'reference':      europe_avg_income,
		'max_difference': None,
	}]
	#
	# mulcov table:
	mulcov_table = [{
		'covariate': 'income',
		'type':      'rate_value',
		'effected':  'iota',
		'smooth':    'smooth_mulcov',
	}]
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
		'integrand':   'Sincidence',
		'hold_out':    False,
		'density':     'gaussian',
		'weight':      '',
		'age_lower':    50.0,
		'age_upper':    50.0,
		'time_lower':   2000.,
		'time_upper':   2000.,
		'subgroup':     'world',
	}
	# Sincidence for world
	# (make income very large to demonstate it is not inclued by bnd_mulcov)
	income = 4.0 * europe_avg_income
	row['income']     = income
	row['node']       = 'world'
	row['meas_value'] = 2.0 * iota_europe_true(income)
	row['meas_std']   = row['meas_value'] / 10.0
	data_table.append( copy.copy(row) )
	#
	# first Sincidence for europe
	income = europe_avg_income / 2.0
	row['income']     = income
	row['node']       = 'europe'
	row['meas_value'] = iota_europe_true(income)
	row['meas_std']   = row['meas_value'] / 10.0
	data_table.append( copy.copy(row) )
	#
	# second Sincidence for europe
	income = europe_avg_income * 2.0
	row['income']     = income
	row['node']       = 'europe'
	row['meas_value'] = iota_europe_true(income)
	row['meas_std']   = row['meas_value'] / 10.0
	data_table.append( copy.copy(row) )
	#
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{
			# prior_iota
			'name':     'prior_iota',
			'density':  'uniform',
			'lower':    iota_europe_true(europe_avg_income) / 10.0,
			'upper':    iota_europe_true(europe_avg_income) * 10.0,
			'mean':     iota_europe_true(europe_avg_income) * 2.0,
		},{
			# prior_mulcov
			# The upper limit in this prior is much to large but
			# the bnd_mulcov command will fix this problem.
			'name':     'prior_mulcov',
			'density':  'uniform',
			'lower':    mulcov_true * 10.0,
			'upper':    0.0,
			'mean':     mulcov_true / 2.0,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{
			# smooth_iota
			'name':      'smooth_iota',
			'age_id':    [0],
			'time_id':   [0],
			'fun':      fun_iota
		},{
			# smooth_mulcov
			'name':      'smooth_mulcov',
			'age_id':    [0],
			'time_id':   [0],
			'fun':      fun_mulcov
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
		{ 'name':'parent_node_name',       'value':'europe'              },

		{ 'name':'quasi_fixed',            'value':'false'               },
		{ 'name':'max_num_iter_fixed',     'value':'50'                  },
		{ 'name':'print_level_fixed',      'value':'0'                   },
		{ 'name':'tolerance_fixed',        'value':'1e-9'                },

		{ 'name':'max_num_iter_random',    'value':'50'                  },
		{ 'name':'print_level_random',     'value':'0'                   },
		{ 'name':'tolerance_random',       'value':'1e-10'               },
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
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc(
	[ program, file_name, 'bnd_mulcov', str(max_abs_effect) ]
)
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
# -----------------------------------------------------------------------
# read database
new                   = False
connection            = dismod_at.create_connection(file_name, new)
var_table             = dismod_at.get_table_dict(connection, 'var')
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
bnd_mulcov_table      = dismod_at.get_table_dict(connection, 'bnd_mulcov')
connection.close()
#
# There are two variables in this model, iota and the covariate multiplier
assert len(var_table) == 2
assert len(fit_var_table) == 2
#
# check that this fit is accurate
for var_id in range( len(var_table) ) :
	var_type       = var_table[var_id]['var_type']
	fit_var_value  = fit_var_table[var_id]['fit_var_value']
	if var_type == 'rate' :
		true_value  = iota_europe_true(europe_avg_income)
		rel_error   = 1.0 - fit_var_value/true_value
		assert abs(rel_error) < 1e-5
	else :
		assert var_type == 'mulcov_rate_value'
		true_value  = mulcov_true
		rel_error   = 1.0 - fit_var_value/true_value
		assert abs(rel_error) < 1e-5
#
# check max_upper
assert len(bnd_mulcov_table) == 1
max_upper = bnd_mulcov_table[0]['max_upper']
check     = max_abs_effect / europe_avg_income
rel_err   = 1.0 - max_upper / check
assert abs(rel_error) < 1e-5
#
# -----------------------------------------------------------------------------
print('compress.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
