#! /bin/python3
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin import_cascade$$ $newlinech #$$
#
# $section Import an IHME Cascade Study$$
#
# $head Syntax$$
# $codei%import_cascade.py %cascade_path% %option_file%$$
#
# $end
# ---------------------------------------------------------------------------
import sys
import os
import csv
import math
import collections
#
sys.path.append( os.path.join( os.getcwd(), 'python' ) )
import dismod_at
# ---------------------------------------------------------------------------
# cascade_dir:       directory name (not path) where cascade input files are
# cascade_path_dict: path to cascade input files
# option_table_in:   a dictionary containing values in option file
#
if sys.argv[0] != 'bin/import_cascade.py' :
	msg  = 'bin/import_cascasde.py: must be executed from its parent directory'
	sys.exit(msg)
#
usage = '''bin/import_cascade.py cascade_path option_csv

cascade_path: path where the directory where cascade input files are located
option_csv:   a csv file that contains the following (name, value) pairs
	time_lower:      the minimum value in the time grid
	time_upper:      the maximum value in the time grid
	number_time:     the number of values in the time grid
	child_value_std: value standard deviation for random effects
	child_dage_std:  dage standard deviation for random effects
	child_dtime_std: dtime standard deviation for random effects
'''
n_arg = len(sys.argv)
if n_arg != 3 :
	sys.exit(usage)
#
cascade_path = sys.argv[1]
option_csv   = sys.argv[2]
if not os.path.isdir( cascade_path ) :
	msg  = usage + '\n'
	msg += 'import_cascade: ' + cascade_path + ' is not a directory'
	sys.exit(msg)
if not os.path.isfile( option_csv ) :
	msg  = usage + '\n'
	msg += 'import_cascade: ' + option_csv + ' is not a file'
	sys.exit(msg)
#
cascade_dir       = os.path.basename(cascade_path)
#
cascade_name_list = [
	'data', 'rate_prior', 'simple_prior', 'effect_prior', 'integrand', 'value'
]
cascade_path_dict = dict()
for name in cascade_name_list :
	path = os.path.join(cascade_path, name + '.csv')
	if not os.path.isfile(path) :
		msg = 'import_cascade: ' + path + ' is not a file'
		sys.exit(msg)
	cascade_path_dict[name] = path
#
option_list = [
	'time_lower', 'time_upper', 'number_time',
	'child_value_std', 'child_dage_std', 'child_dtime_std'
]
option_table_in = dict()
file_ptr    = open(option_csv)
reader      = csv.DictReader(file_ptr)
for row in reader :
	option_table_in[ row['name'] ] = row['value']
for option in option_list :
	if option not in option_table_in :
		msg  = usage + '\n'
		msg += option + ' not in ' + option_csv
		sys.exit(msg)
if int( option_table_in['number_time'] ) < 2 :
	msg  = usage + '\n'
	msg += 'import_cascade: number_time in ' + option-csv + ' < 2'
	sys.exit(msg)
# ---------------------------------------------------------------------------
def float_or_none(string) :
	if string in [ 'nan', '_inf', '-inf', 'inf', '+inf' ] :
		return None
	return float(string)
# ---------------------------------------------------------------------------
# db_connection: database that is output by this program
if not os.path.isdir('build') :
	print('mkdir build')
	os.mkdir('build')
new = True
file_name        = os.path.join('build', cascade_dir + '.db')
db_connection    = dismod_at.create_connection(file_name, new)
# ---------------------------------------------------------------------------
# data_table_in:    data file as a list of dictionaries
# rate_prior_in:    rate prior file as a list of dictionaries
# simple_prior_in:  simple prior file as a dictionary or dictionaries
# value_table_in:   value file as a dictionary
cascade_data_dict = dict()
for name in cascade_path_dict :
	path      = cascade_path_dict[name]
	file_ptr  = open(path)
	reader    = csv.DictReader(file_ptr)
	#
	cascade_data_dict[name] = list()
	for row in reader :
		cascade_data_dict[name].append(row)
data_table_in  = cascade_data_dict['data']
rate_prior_in  = cascade_data_dict['rate_prior']
simple_prior_in = dict()
for row in cascade_data_dict['simple_prior'] :
	name                    = row['name']
	simple_prior_in[name] =  dict()
	for column in [ 'lower', 'upper', 'mean', 'std'  ] :
		simple_prior_in[name][column] = row[column]
value_table_in = dict()
for row in cascade_data_dict['value'] :
	value_table_in[ row['name'] ] = row['value']
# ---------------------------------------------------------------------------
# Output time table
# time_list:
time_lower = float( option_table_in['time_lower'] )
time_upper = float( option_table_in['time_upper'] )
n_time     = int( option_table_in['number_time'] )
time_list      = list()
for i in range( n_time ) :
	time = ( time_lower * (n_time-i-1) + time_upper * i )/(n_time-1)
	time_list.append(time)
#
col_name = [ 'time' ]
col_type = [ 'real' ]
row_list       = list()
for time in time_list :
	row_list.append([time])
tbl_name = 'time'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# ---------------------------------------------------------------------------
# Output age table
# age_list:
age_set = set()
for row in rate_prior_in :
	age_set.add( float(row['age']) )
age_list = sorted( age_set)
#
col_name = [ 'age' ]
col_type = [ 'real' ]
row_list  = list()
for age in age_list :
	row_list.append([age])
tbl_name = 'age'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# ---------------------------------------------------------------------------
# covariate_name2id: mapping from covariate names to covariate_id value
header        = data_table_in[0].keys()
covariate_name_list = list()
for name in header :
	if name.startswith('r_') or name.startswith('a_') :
		covariate_name_list.append(name)
covariate_name2id = collections.OrderedDict()
for covariate_id in range( len(covariate_name_list) ) :
	name                    = covariate_name_list[covariate_id]
	covariate_name2id[name] = covariate_id
# ---------------------------------------------------------------------------
# integrand_name2id: mapping from integrand name to integrand_id value
integrand_name_set = set()
for row in data_table_in :
	integrand_name_set.add( row['integrand'] )
integrand_name_list = list( integrand_name_set )
integrand_name2id   = dict()
for integrand_id in range( len(integrand_name_list) ) :
	name                    = integrand_name_list[integrand_id]
	integrand_name2id[name] = integrand_id
# ---------------------------------------------------------------------------
# Output density table
# density_name2id: mapping from density name to density_id value
col_name = [  'density_name'   ]
col_type = [  'text'        ]
row_list = [
	['uniform'],
	['gaussian'],
	['laplace'],
	['log_gaussian'],
	['log_laplace']
]
tbl_name = 'density'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
#
density_name2id = dict()
for density_id in range( len(row_list) ) :
	name                  = row_list[density_id][0]
	density_name2id[name] = density_id
# ---------------------------------------------------------------------------
# Output node table
# node_name2id: mapping from area name to node_id value
col_name      = [ 'node_name', 'parent'  ]
col_type      = [ 'text',      'integer' ]
row_list      = list()
node_name2id  = dict()
#
# world
row_list.append( [ 'world', None ] )
node_name2id['world'] = 0
#
for row in data_table_in :
	parent = node_name2id['world']
	for level in [ 'super', 'region', 'subreg', 'atom' ] :
		name = row[level]
		if name != 'none' :
			if not name in node_name2id :
				node_id = len(row_list)
				row_list.append( [ name , parent ] )
				node_name2id[name] = node_id
			parent = node_name2id[name]
tbl_name = 'node'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# ---------------------------------------------------------------------------
# output weight and weight_grid table:
#
col_name =   [ 'weight_name', 'n_age',   'n_time'   ]
col_type =   [ 'text',        'integer', 'integer'  ]
row_list = [ [ 'weight_one',  1,          1         ] ]
tbl_name = 'weight'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
#
col_name =   [  'weight_id', 'age_id',   'time_id',  'weight' ]
col_type =   [  'integer',   'integer',  'integer',  'real'   ]
row_list = [ [  0,           0,          0,           1.0     ] ]
tbl_name = 'weight_grid'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# ---------------------------------------------------------------------------
# output data table:
col_name2type = collections.OrderedDict([
	# required columns
	('integrand_id', 'integer'),
	('density_id',   'integer'),
	('node_id',      'integer'),
	('weight_id',    'integer'),
	('hold_out',     'integer'),
	('meas_value',   'real'   ),
	('meas_std',     'real'   ),
	('age_lower',    'real'   ),
	('age_upper',    'real'   ),
	('time_lower',   'real'   ),
	('time_upper',   'real'   )
] )
col_name = list( col_name2type.keys() )
col_type = list( col_name2type.values() )
for name in covariate_name2id :
	col_name.append( 'x_%s' % covariate_name2id[name] )
	col_type.append( 'real' )
#
row_list      = list()
for row_in in data_table_in :
	integrand_id = integrand_name2id[ row_in['integrand'] ]
	density_id   = density_name2id[ row_in['data_like'] ]
	weight_id    = 0
	hold_out     = int( row_in['hold_out'] )
	meas_value   = float( row_in['meas_value'] )
	meas_std     = float( row_in['meas_stdev'] )
	age_lower    = float( row_in['age_lower'] )
	age_upper    = float( row_in['age_upper'] )
	time_lower   = float( row_in['time_lower'] )
	time_upper   = float( row_in['time_upper'] )
	#
	node_id      = node_name2id['world']
	for level in [ 'super', 'region', 'subreg', 'atom' ] :
		if row_in[level] != 'none' :
			node_id = node_name2id[ row_in[level] ]
	#
	row_out = [
		integrand_id,
		density_id,
		node_id,
		weight_id,
		hold_out,
		meas_value,
		meas_std,
		age_lower,
		age_upper,
		time_lower,
		time_upper
	]
	for name in covariate_name2id :
		value        = row_in[name]
		row_out.append(value)
	row_list.append( row_out )
tbl_name = 'data'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# ---------------------------------------------------------------------------
# Output covariate table
col_name   = [ 'covariate_name',  'reference', 'max_difference' ]
col_type   = [ 'text',             'real',     'real'           ]
row_list   = list()
for name in covariate_name2id :
	if name.startswith('r_') :
		cov_sum   = 0.0
		cov_count = 0
		for row in data_table_in :
			cov_value = float( row[name] )
			if not math.isnan(cov_value) :
				cov_sum   += cov_value
				cov_count += 1
		if cov_count == 0 :
			reference = 0.0
		else :
			reference = cov_sum / cov_count
		max_difference = None
	else :
		assert name.startswith('a_')
		reference = 0.0
		if name == 'a_sex' :
			max_difference = 0.6
		else :
			max_difference = None
	row_list.append( [name , reference, max_difference] )
tbl_name = 'covariate'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# ---------------------------------------------------------------------------
# Start information for prior, smooth, and smooth_grid tables
# ---------------------------------------------------------------------------
# prior_col_name2type:
# smooth_col_name2type:
# smooth_grid_col_name2type:
prior_col_name2type = collections.OrderedDict([
	('prior_name',     'text'   ),
	('lower',          'real'   ),
	('upper',          'real'   ),
	('mean',           'real'   ),
	('std',            'real'   ),
	('density_id',     'integer'),
	('eta',            'real'   ),
])
smooth_col_name2type = collections.OrderedDict([
	('smooth_name',            'text'   ),
	('n_age',                  'integer'),
	('n_time',                 'integer'),
	('mulstd_value_prior_id',  'integer'),
	('mulstd_dage_prior_id',   'integer'),
	('mulstd_dtime_prior_id',  'integer')
])
#
smooth_grid_col_name2type = collections.OrderedDict([
	('smooth_id',       'integer'),
	('age_id',          'integer'),
	('time_id',         'integer'),
	('value_prior_id',  'integer'),
	('dage_prior_id',   'integer'),
	('dtime_prior_id',  'integer')
])
# --------------------------------------------------------------------------
# zero_prior_id, one_prior_id, no_prior_id,
# zero_smooth_id, one_smooth_id, and corresponding
# values in prior_row_list, smooth_row_list, smooth_grid_row_list
prior_row_list       = list()
smooth_row_list      = list()
smooth_grid_row_list = list()
#
zero_prior_id    = len( prior_row_list )
density_id       = density_name2id['uniform']
prior_row_list.append(
	[ 'zero_prior', 0.0, 0.0, 0.0, None, density_id, None ]
)
one_prior_id    = len( prior_row_list )
prior_row_list.append(
	[ 'one_prior', 1.0, 1.0, 0.0, None, density_id, None ]
)
no_prior_id    = len( prior_row_list )
prior_row_list.append(
	[ 'no_prior', None, None, 0.0, None, density_id, None ]
)
#
zero_smooth_id   = len( smooth_row_list )
smooth_row_list.append(
	[ 'zero_smooth',  1,   1,   one_prior_id, one_prior_id, one_prior_id ]
)
smooth_grid_row_list.append(
	[ zero_smooth_id, 0,   0,   zero_prior_id, None, None ]
)
#
one_smooth_id   = len( smooth_row_list )
smooth_row_list.append(
	[ 'one_smooth',  1,   1,   one_prior_id, one_prior_id, one_prior_id ]
)
smooth_grid_row_list.append(
	[ one_smooth_id, 0,   0,   one_prior_id, None, None ]
)
# --------------------------------------------------------------------------
# dtime_prior_id and corresponding value in prior_row_list
delta_time      = time_list[1] - time_list[0]
lower           = None
upper           = None
mean            = 0.0
std             = delta_time / 10.
eta             = 1e-7
dtime_prior_id  = len( prior_row_list )
prior_row_list.append([
	'dtime_prior',
	lower,
	upper,
	mean,
	std,
	density_name2id['log_gaussian'],
	eta
])
# --------------------------------------------------------------------------
# pini_prior_id, pini_smooth_id, and corresponding values in
# prior_row_list, smooth_row_list, smooth_grid_row_list
pini_prior_id    = len( prior_row_list )
#
prior_in         = simple_prior_in['p_zero']
eta              = None
if float(prior_in['std']) == float('inf') :
	density_id = density_name2id['uniform']
else :
	density_id = density_name2id['gaussian']
prior_row_list.append([
	'pini_prior',
	float_or_none( prior_in['lower'] ),
	float_or_none( prior_in['upper'] ),
	float( prior_in['mean']  ),
	float_or_none( prior_in['std']   ),
	density_id,
	eta
])
pini_smooth_id = len( smooth_row_list )
n_age          = 1
n_time         = len( time_list )
smooth_row_list.append(
	[ 'pini_smooth', n_age, n_time, one_prior_id, one_prior_id, one_prior_id ]
)
age_id        = 0
dage_prior_id = None
for time_id in range( n_time ) :
	if time_id < n_time - 1 :
		dt_prior_id = dtime_prior_id
	else :
		dt_prior_id = None
	smooth_grid_row_list.append([
		pini_smooth_id,
		age_id,
		time_id,
		pini_prior_id,
		dage_prior_id,
		dt_prior_id
	])
# --------------------------------------------------------------------------
# Add pirors for all the rate values:
# value_prior_list:
value_prior_list = list()
#
prior_in_set = set()
for row in rate_prior_in :
	if row['type'] in [ 'iota', 'rho', 'chi', 'omega' ] :
		lower = float_or_none( row['lower'] )
		upper = float_or_none( row['upper'] )
		mean  = float( row['mean'] )
		std   = float_or_none( row['std'] )
		prior_in_set.add( (lower, upper, mean, std) )
#
for element in prior_in_set :
	if std == None :
		density_id = density_name2id['uniform']
	else :
		density_id = density_name2id['gaussian']
	(lower, upper, mean, std) = element
	eta      = None
	prior_id = len( prior_row_list )
	name     = 'rate_' + str(prior_id) + '_prior'
	prior_row_list.append(
		[ name , lower, upper, mean, std, density_id, eta ]
	)
	value_prior_list.append( [ prior_id, element ] )
# --------------------------------------------------------------------------
# Add pirors for all the rate dage:
# dage_prior_list:
dage_prior_list = list()
#
sqrt_dage = math.sqrt( (age_list[-1] - age_list[0]) / (len(age_list) - 1) )
for drate in [ 'diota', 'drho', 'dchi', 'domega' ] :
	prior_in_set = set()
	for row in rate_prior_in :
		if row['type'] == drate :
			lower = float_or_none( row['lower'] )
			upper = float_or_none( row['upper'] )
			mean  = float( row['mean'] )
			std   = float_or_none( row['std'] )
			assert lower in [ None, 0.0 ]
			assert upper in [ 0.0, None ]
			assert mean == 0.0
			assert std  == None
			prior_in_set.add( (lower, upper, mean, std) )
	name  = 'xi_' + drate[1:]
	xi    = float( simple_prior_in[name]['mean'] )
	name  = 'kappa_' + drate[1:]
	eta   = value_table_in[name]
	for element in prior_in_set :
		prior_id   = len( prior_row_list )
		(lower, upper, mean, std) = element
		name       = drate + '_' + str(prior_id) + '_prior'
		std        = xi * sqrt_dage
		density_id = density_name2id['log_gaussian']
		prior_row_list.append(
			[ name , lower, upper, mean, std, density_id, eta ]
		)
		dage_prior_list.append( [ prior_id, element ] )
# --------------------------------------------------------------------------
# Add priors and smoothing for the children
# child_smooth_id
lower      = None
upper      = None
mean       = 0.0
density_id = density_name2id['gaussian']
eta        = None
#
std    = float( option_table_in['child_value_std'] )
name   = 'child_value_piror'
child_value_prior_id = len( prior_row_list )
prior_row_list.append(
		[ name , lower, upper, mean, std, density_id, eta ]
)
#
std    = float( option_table_in['child_dage_std'] )
name   = 'child_dage_piror'
child_dage_prior_id = len( prior_row_list )
prior_row_list.append(
		[ name , lower, upper, mean, std, density_id, eta ]
)
#
std    = float( option_table_in['child_dtime_std'] )
name   = 'child_dtime_piror'
child_dtime_prior_id = len( prior_row_list )
prior_row_list.append(
		[ name , lower, upper, mean, std, density_id, eta ]
)
#
name            = 'child_smooth'
n_age           = len(age_list)
n_time          = len(time_list)
child_smooth_id = len(smooth_row_list)
smooth_row_list.append(
		[ name , n_age, n_time, one_prior_id, one_prior_id, one_prior_id ]
)
smooth_id      = child_smooth_id
value_prior_id = child_value_prior_id
for i in range( n_age ) :
	if i + 1 < n_age :
		dage_prior_id = child_dage_prior_id
	else :
		dage_prior_id = None
	for j in range( n_time ) :
		if j + 1 < n_time :
			dtime_prior_id = child_dtime_prior_id
		else :
			dtime_prior_id = None
		smooth_grid_row_list.append(
			[ smooth_id, i, j, value_prior_id, dage_prior_id, dtime_prior_id ]
	)
# --------------------------------------------------------------------------
# Output rate table and add smoothing for the rates
col_name = [  'rate_name', 'parent_smooth_id', 'child_smooth_id'  ]
col_type = [  'text',      'integer',         'integer'          ]
row_list = list()
#


# --------------------------------------------------------------------------
# Output, prior, smooth, and smooth_grid tables
# --------------------------------------------------------------------------
col_name = list( prior_col_name2type.keys() )
col_type = list( prior_col_name2type.values() )
row_list = prior_row_list
tbl_name = 'prior'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
#
col_name = list( smooth_col_name2type.keys() )
col_type = list( smooth_col_name2type.values() )
row_list = smooth_row_list
tbl_name = 'smooth'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
#
col_name = list( smooth_grid_col_name2type.keys() )
col_type = list( smooth_grid_col_name2type.values() )
row_list = smooth_grid_row_list
tbl_name = 'smooth_grid'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# --------------------------------------------------------------------------
print('import_cascade.py: OK')
