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
import copy
import time as timer
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
option_dict = collections.OrderedDict([
	('rate_info','       are iota and rho zero or non-zero'),
	('time_lower','      minimum value in the time grid'),
	('time_upper','      maximum value in the time grid'),
	('number_time','     number of values in the time grid'),
	('child_value_std',' value standard deviation for random effects'),
	('child_dage_std','  dage standard deviation for random effects'),
	('child_dtime_std',' dtime standard deviation for random effects')
])
usage = '''bin/import_cascade.py cascade_path option_csv

cascade_path: path where the directory where cascade input files are located
option_csv:   a csv file that contains the following (name, value) pairs
'''
usage += 30 * '-' + ' options ' + 40 * '-' + '\n'
for key in option_dict :
	usage += key + ':' + option_dict[key] + '\n'
usage += 79 * '-' + '\n'
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
# ----------------------------------------------------------------------------
option_table_in = dict()
file_ptr    = open(option_csv)
reader      = csv.DictReader(file_ptr)
for row in reader :
	option_table_in[ row['name'] ] = row['value']
for option in option_dict :
	if option not in option_table_in :
		msg  = usage + '\n'
		msg += option + ' not in ' + option_csv
		sys.exit(msg)
for option in option_table_in :
	if option not in option_dict :
		msg  = usage + '\n'
		msg += option + ' in ' + option_csv + ' is not a valid option'
		sys.exit(msg)
#
if int( option_table_in['number_time'] ) < 2 :
	msg  = usage + '\n'
	msg += 'import_cascade: number_time in ' + option-csv + ' < 2'
	sys.exit(msg)
#
rate_info = option_table_in['rate_info']
info_list = [
	'iota_pos_rho_zero', 'iota_zero_rho_pos',
	'iota_zero_rho_pos', 'iota_pos_rho_pos'
]
if rate_info not in info_list :
	msg  = usage + '\n'
	msg += 'in ' + option_csv + ' rate_info = ' + rate_info + '\n'
	msg += 'is not one of the following:\n'
	for i in range( len( info_list ) ) :
		msg += info_list[i]
		if i + 1 < len( info_list ) :
			msg += ', '
	sys.exit(msg)
# ---------------------------------------------------------------------------
# float_or_none
#
def float_or_none(string) :
	if string in [ 'nan', '_inf', '-inf', 'inf', '+inf' ] :
		return None
	return float(string)
# ---------------------------------------------------------------------------
# gaussian_cascade2at
#
density_name2id = None
def gaussian_cascade2at(prior_name, cascade_prior_row) :
	assert density_name2id != None
	lower = float_or_none( cascade_prior_row['lower'] )
	upper = float_or_none( cascade_prior_row['upper'] )
	mean  = float( cascade_prior_row['mean'] )
	#
	if cascade_prior_row['std'] == 'inf' :
		density_id = density_name2id['uniform']
	else :
		density_id = density_name2id['gaussian']
	#
	#
	eta   = None
	return [ prior_name , lower, upper, mean, std, density_id, eta ]
# ---------------------------------------------------------------------------
# log_gaussian_cascade2at
#
density_name2id = None
def log_gaussian_cascade2at(prior_name, cascade_prior_row, eta) :
	assert density_name2id != None
	lower = float_or_none( cascade_prior_row['lower'] )
	upper = float_or_none( cascade_prior_row['upper'] )
	mean  = float( cascade_prior_row['mean'] )
	if cascade_prior_row['std'] == 'inf' :
		density_id = density_name2id['uniform']
	else :
		density_id = density_name2id['log_gaussian']
	#
	#
	return [ prior_name , lower, upper, mean, std, density_id, eta ]
# ---------------------------------------------------------------------------
# db_connection
#
if not os.path.isdir('build') :
	print('mkdir build')
	os.mkdir('build')
new = True
file_name        = os.path.join('build', cascade_dir + '.db')
db_connection    = dismod_at.create_connection(file_name, new)
# ---------------------------------------------------------------------------
# integrand_table_in: integrand file as a list of dictionaries
# data_table_in:      data file as a list of dictionaries
# rate_prior_in:      rate prior file as a list of dictionaries
# effect_prior_in:    effect file as a list of dictionaries
# simple_prior_in:    simple prior file as a dictionary or dictionaries
# value_table_in:     value file as a single dictionary
#
cascade_data_dict = dict()
for name in cascade_path_dict :
	path      = cascade_path_dict[name]
	file_ptr  = open(path)
	reader    = csv.DictReader(file_ptr)
	#
	cascade_data_dict[name] = list()
	for row in reader :
		cascade_data_dict[name].append(row)
integrand_table_in = cascade_data_dict['integrand']
data_table_in      = cascade_data_dict['data']
rate_prior_in      = cascade_data_dict['rate_prior']
effect_prior_in    = cascade_data_dict['effect_prior']
simple_prior_in    = dict()
for row in cascade_data_dict['simple_prior'] :
	name                    = row['name']
	simple_prior_in[name] =  dict()
	for column in [ 'lower', 'upper', 'mean', 'std'  ] :
		simple_prior_in[name][column] = row[column]
value_table_in = dict()
for row in cascade_data_dict['value'] :
	value_table_in[ row['name'] ] = row['value']
# ---------------------------------------------------------------------------
# Output time table.
# time_list:
#
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
# Output age table.
# age_list:
#
age_dict = dict()
for rate in [ 'iota', 'rho', 'chi', 'omega' ] :
	drate           = 'd' + rate
	age_dict[rate]  = list()
	age_dict[drate] = list()
	for row in rate_prior_in :
		if row['type'] == rate :
			age_dict[rate].append( float( row['age'] ) )
		if row['type'] == drate :
			age_dict[drate].append( float( row['age'] ) )
#
# This program assumes only one age grid in rate_prior_in
age_list = sorted( age_dict['iota'] )
for rate in [ 'iota', 'rho', 'chi', 'omega' ] :
	drate = 'd' + rate
	assert age_dict[rate] == age_list
	assert age_dict[drate] == age_list[0:-1]
#
col_name = [ 'age' ]
col_type = [ 'real' ]
row_list  = list()
for age in age_list :
	row_list.append([age])
tbl_name = 'age'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# ---------------------------------------------------------------------------
# rate_prior_in_dict:
#
rate_prior_in_dict = dict()
for rate in [ 'iota', 'rho', 'chi', 'omega' ] :
	drate           = 'd' + rate
	rate_prior_in_dict[rate]  = list()
	rate_prior_in_dict[drate] = list()
	for row in rate_prior_in :
		if row['type'] == rate :
			rate_prior_in_dict[rate].append( row )
		if row['type'] == drate :
			rate_prior_in_dict[drate].append( row )
# ---------------------------------------------------------------------------
# covariate_name2id
#
header        = list( data_table_in[0].keys() )
covariate_name_list = list()
for name in header :
	if name.startswith('r_') or name.startswith('a_') :
		covariate_name_list.append(name)
covariate_name2id = collections.OrderedDict()
for covariate_id in range( len(covariate_name_list) ) :
	name                    = covariate_name_list[covariate_id]
	covariate_name2id[name] = covariate_id
# ---------------------------------------------------------------------------
# Output integrand table.
# integrand_name2id
#
integrand_name2id   = dict()
for integrand_id in range( len(integrand_table_in) ) :
	name                    = integrand_table_in[integrand_id]['integrand']
	integrand_name2id[name] = integrand_id
#
col_name = [ 'integrand_name', 'eta' ]
col_type = [ 'text',           'real']
row_list = list()
for row in integrand_table_in :
	integrand_name = row['integrand']
	if integrand_name == 'incidence' :
		integrand_name = 'Tincidence'
	row_list.append( [ integrand_name , float( row['eta'] ) ] )
tbl_name = 'integrand'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# ---------------------------------------------------------------------------
# Output density table
# density_name2id
#
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
# node_name2id
#
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
# Output weight table
# Output weight_grid table
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
# Output data table
#
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
#
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
# Start recording information for prior smooth and smooth_grid tables
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
# zero_prior_id,   zero_smooth_id
# one_prior_id,    one_smooth_id
# nolim_prior_id
#
prior_row_list       = list()
smooth_row_list      = list()
smooth_grid_row_list = list()
#
# zero_prior_id
zero_prior_id    = len( prior_row_list )
density_id       = density_name2id['uniform']
prior_row_list.append(
	[ 'zero_prior', 0.0, 0.0, 0.0, None, density_id, None ]
)
# one_prior_id
one_prior_id    = len( prior_row_list )
prior_row_list.append(
	[ 'one_prior', 1.0, 1.0, 0.0, None, density_id, None ]
)
# nolim_prior_id
nolim_prior_id    = len( prior_row_list )
prior_row_list.append(
	[ 'nolim_prior', None, None, 0.0, None, density_id, None ]
)
# zero_smooth_id
zero_smooth_id   = len( smooth_row_list )
smooth_row_list.append(
	[ 'zero_smooth',  1,   1,   one_prior_id, one_prior_id, one_prior_id ]
)
smooth_grid_row_list.append(
	[ zero_smooth_id, 0,   0,   zero_prior_id, None, None ]
)
# one_smooth_id
one_smooth_id   = len( smooth_row_list )
smooth_row_list.append(
	[ 'one_smooth',  1,   1,   one_prior_id, one_prior_id, one_prior_id ]
)
smooth_grid_row_list.append(
	[ one_smooth_id, 0,   0,   one_prior_id, None, None ]
)
# --------------------------------------------------------------------------
# child_smooth_id
#
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
# rate_dtime_prior_id
#
rate_dtime_prior_id = dict()
delta_time          = time_list[1] - time_list[0]
lower               = None
upper               = None
mean                = 0.0
std                 = delta_time / 10.
for rate in [ 'pini', 'iota', 'rho', 'chi', 'omega' ] :
	if rate == 'pini' :
		eta = 1e-7
	else :
		eta = value_table_in[ 'kappa_' + rate ]
	rate_dtime_prior_id[rate]  = len( prior_row_list )
	prior_row_list.append([
		rate + '_dtime_prior',
		lower,
		upper,
		mean,
		std,
		density_name2id['log_gaussian'],
		eta
	])
# --------------------------------------------------------------------------
# pini_smooth_id
#
prior_in         = simple_prior_in['p_zero']
name             = 'pini_prior'
prior_out        = gaussian_cascade2at(name, prior_in)
pini_prior_id    = len( prior_row_list )
prior_row_list.append(prior_out)
#
n_age          = 1
n_time         = len( time_list )
pini_smooth_id = len( smooth_row_list )
smooth_row_list.append(
	[ 'pini_smooth', n_age, n_time, one_prior_id, one_prior_id, one_prior_id ]
)
age_id        = 0
dage_prior_id = None
for time_id in range( n_time ) :
	if time_id < n_time - 1 :
		dtime_prior_id = rate_dtime_prior_id['pini']
	else :
		dtime_prior_id = None
	smooth_grid_row_list.append([
		pini_smooth_id,
		age_id,
		time_id,
		pini_prior_id,
		dage_prior_id,
		dtime_prior_id
	])
# --------------------------------------------------------------------------
# rate_smooth_id
#
rate_smooth_id         = dict()
rate_smooth_id['pini'] = pini_smooth_id
#
delta_age          = (age_list[-1] - age_list[0]) / (len(age_list) - 1)
for rate in [ 'iota', 'rho', 'chi', 'omega' ] :
	drate = 'd' + rate
	xi     = float( simple_prior_in['xi_' + rate]['mean'] )
	#
	# initialize some lists for this rate
	local_list     = list()
	local_list_id  = list()
	#
	dlocal_list    = list()
	dlocal_list_id = list()
	#
	# smooth_row_list
	n_age  = len(age_list)
	n_time = len(time_list)
	name   = rate + '_smooth'
	rate_smooth_id[rate] = len(smooth_row_list)
	smooth_row_list.append(
			[ name, n_age, n_time, one_prior_id, one_prior_id, one_prior_id ]
	)
	# need to fill in smooth_grid entries for this smoothing
	for age_id in range( n_age ) :
		# --------------------------------------------------------------------
		# determine value_prior_id
		name  = rate + '_' + str( len(local_list) ) + '_prior'
		prior_in = rate_prior_in_dict[rate][age_id]
		prior_at = gaussian_cascade2at(name, prior_in)
		(name, lower, upper, mean, std, density_id, eta) = prior_at
		#
		# check if this prior already specified
		element = (lower, upper, mean, std)
		if element not in local_list :
			local_list_id.append( len(prior_row_list) )
			local_list.append( element )
			prior_row_list.append( prior_at )
		value_prior_id = local_list_id [ local_list.index(element) ]
		if age_id + 1 < n_age :
			# ----------------------------------------------------------------
			# determine dage_prior_id
			name  = 'd' + rate + '_' + str( len(dlocal_list) ) + '_prior'
			prior_in = rate_prior_in_dict[drate][age_id]
			eta      = value_table_in[ 'kappa_' + rate ]
			prior_at = log_gaussian_cascade2at(name, prior_in, eta)
			(name, lower, upper, mean, std, density_id, eta) = prior_at
			#
			# ignoring cascade values for mean and std in this case
			mean = 0.0
			std  = xi * delta_age / 3.0 # using xi from cascade in this way
			#
			# check if this prior already specified
			element = (lower, upper, mean, std)
			if element not in dlocal_list :
				dlocal_list_id.append( len(prior_row_list) )
				dlocal_list.append( element )
				prior_row_list.append( prior_at )
			dage_prior_id = dlocal_list_id [ dlocal_list.index(element) ]
		else :
			dage_prior_id  = None
		# --------------------------------------------------------------------
		for time_id in range( n_time ) :
			if time_id + 1 < n_time :
				dtime_prior_id = rate_dtime_prior_id[rate]
			else :
				dtime_prior_id = None
			smooth_grid_row_list.append([
				rate_smooth_id[rate],
				age_id,
				time_id,
				value_prior_id,
				dage_prior_id,
				dtime_prior_id
			])
# --------------------------------------------------------------------------
# Output rate table
col_name = [  'rate_name', 'parent_smooth_id', 'child_smooth_id'  ]
col_type = [  'text',      'integer',         'integer'          ]
row_list = list()
#
for rate in [ 'pini', 'iota', 'rho', 'chi', 'omega' ] :
	row_list.append(
		[ rate, rate_smooth_id[rate], child_smooth_id ]
)
tbl_name = 'rate'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# --------------------------------------------------------------------------
# Output mulcov table
col_name2type = collections.OrderedDict([
	('mulcov_type',     'text'    ),
	('rate_id',         'integer' ),
	('integrand_id',    'integer' ),
	('covariate_id',    'integer' ),
	('smooth_id',       'integer' )
])
col_name = list( col_name2type.keys() )
col_type = list( col_name2type.values() )
row_list = list()
#
for row in effect_prior_in :
	effect = row['effect']
	if effect in [ 'zeta', 'beta' ] :
		integrand    = row['integrand']
		covariate    = row['name']
		covariate_id = covariate_name2id[covariate]
		#
		if row['effect'] == 'zeta' :
			mulcov_type  = 'meas_std'
			integrand_id = integrand_name2id[integrand]
			rate_id      = None
			name         = integrand
		elif integrand in [ 'incidence', 'remission', 'mtexcess' ] :
			mulcov_type       = 'rate_value'
			integrand_id      = None
			# rate table order is pini, iota, rho, chi, omega
			if integrand == 'incidence' :
				rate_id = 1
				name    = 'iota'
			if integrand == 'remission' :
				rate_id = 2
				name    = 'rho'
			if integrand == 'mtexcess' :
				rate_id = 3
				name    = 'chi'
		else :
			mulcov_type  = 'meas_value'
			integrand_id = integrand_name2id[integrand]
			rate_id      = None
		#
		prior_name   = name + '_' + covariate + '_prior'
		prior_at     = gaussian_cascade2at(prior_name, row)
		prior_id     = len( prior_row_list )
		prior_row_list.append( prior_at )
		#
		smooth_name  = name + '_' + covariate + '_smooth'
		smooth_id    = len( smooth_row_list )
		smooth_row_list.append(
			[ smooth_name,  1, 1, one_prior_id, one_prior_id, one_prior_id ]
		)
		smooth_grid_row_list.append(
			[ smooth_id, 0, 0, one_prior_id, None, None ]
		)
		#
		row_list.append(
			[ mulcov_type, rate_id, integrand_id, covariate_id, smooth_id ]
		)
#
tbl_name = 'mulcov'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
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
# ---------------------------------------------------------------------------
# Output option table.
col_name = [ 'option_name', 'option_value' ]
col_type = [ 'text unique', 'text' ]
row_list = [
	[ 'parent_node_id', str(node_name2id['world'])       ],
	[ 'ode_step_size',  value_table_in['integrate_step'] ],
	[ 'number_sample',  '10'                             ],
	[ 'random_seed',    str(int( timer.time() ))         ],
	[ 'rate_info',      option_table_in['rate_info']     ],
	[ 'tolerance',      '1e-8'                           ],
	[ 'max_num_iter',   '50'                             ],
	[ 'print_level',     '5'                             ],
	[ 'derivative_test', 'none'                          ]
]
tbl_name = 'option'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# --------------------------------------------------------------------------
# avg_case table
col_name2type = collections.OrderedDict([
	('integrand_id',   'integer'     ),
	('node_id',        'integer'     ),
	('weight_id',      'integer'     ),
	('age_lower',      'real'        ),
	('age_upper',      'real'        ),
	('time_lower',     'real'        ),
	('time_upper',     'real'        )
])
col_name = list( col_name2type.keys() )
col_type = list( col_name2type.values() )
for j in range( len(covariate_name2id) ) :
	col_name.append( 'x_%s' % j )
	col_type.append( 'real' )
eight_id = 0
node_id   = node_name2id['world']
row_list = list()
row      = (7 + len(covariate_name2id) ) * [0]
row[1]  = node_id
row[2]  = weight_id
for time_id in range( len(time_list) ) :
	row[5]  = time_list[time_id]
	row[6]  = time_list[time_id]
	for age_id in range(len(time_list) ) :
		row[3]  = age_list[age_id]
		row[4]  = age_list[age_id]
		for integrand in integrand_name2id :
			row[0]  = integrand_name2id[integrand]
			row_list.append( copy.copy(row) )
tbl_name = 'avg_case'
dismod_at.create_table(db_connection, tbl_name, col_name, col_type, row_list)
# --------------------------------------------------------------------------
print('import_cascade.py: OK')
