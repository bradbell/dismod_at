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
# $codei%import_cascade.py %dir%$$
#
# $end
# ---------------------------------------------------------------------------
import sys
import os
import csv
import collections
#
sys.path.append( os.path.join( os.getcwd(), 'python' ) )
import dismod_at
# ---------------------------------------------------------------------------
# cascade_dir:       directory name (not path) where cascade input files are
# cascade_path_dict: path to cascade input files
#
if sys.argv[0] != 'bin/import_cascade.py' :
	msg  = 'bin/import_cascasde.py: must be executed from its parent directory'
	sys.exit(msg)
#
usage = 'bin/import_cascade.py cascade_path'
n_arg = len(sys.argv)
if n_arg != 2 :
	sys.exit(usage)
#
cascade_path = sys.argv[1]
if not os.path.isdir( cascade_path ) :
	msg  = usage + '\n'
	msg += 'import_cascade: ' + cascade_path + ' is not a directory'
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
# ---------------------------------------------------------------------------
# db_connection: database that is output by this program
if not os.path.isdir('build') :
	print('mkdir build')
	os.mkdir('build')
new = True
file_name        = os.path.join('build', cascade_dir + '.db')
db_connection    = dismod_at.create_connection(file_name, new)
# ---------------------------------------------------------------------------
# cascade_data_dict: data for each cascade input file
# data_table_in:     cascade_data_dict['data'
cascade_data_dict = dict()
for name in cascade_path_dict :
	path      = cascade_path_dict[name]
	file_ptr  = open(path)
	reader    = csv.DictReader(file_ptr)
	#
	cascade_data_dict[name] = list()
	for row in reader :
		cascade_data_dict[name].append(row)
data_table_in = cascade_data_dict['data']
# ---------------------------------------------------------------------------
# covariate_name2id: mapping from covariate names to covariate_id value
header        = data_table_in[0].keys()
covariate_name_list = list()
for name in header :
	if name.startswith('r_') or name.startswith('a_') :
		covariate_name_list.append(name)
covariate_name2id = dict()
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
for row in data_table_in :
	integrand_id = integrand_name2id[ row['integrand'] ]
	density_id   = density_name2id[ row['data_like'] ]
	node_id      = node_name2id['world']
	for level in [ 'super', 'region', 'subreg', 'atom' ] :
		if row[level] != 'none' :
			node_id = node_name2id[ row[level] ]
# ---------------------------------------------------------------------------
print('import_cascade.py: OK')
