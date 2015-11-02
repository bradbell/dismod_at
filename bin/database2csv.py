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
#
sys.path.append( os.path.join( os.getcwd(), 'python' ) )
import dismod_at
# ---------------------------------------------------------------------------
if sys.argv[0] != 'bin/database2csv.py' :
	msg  = 'bin/database2csv.py: must be executed from its parent directory'
	sys.exit(msg)
#
usage = 'bin/database2csv.py directory database_file'
if len(sys.argv) != 3 :
	sys.exit(usage)
#
directory_arg     = sys.argv[1]
database_file_arg = sys.argv[2]
# ----------------------------------------------------------------------------
file_name   = os.path.join(directory_arg, database_file_arg)
new         = False
connection  = dismod_at.create_connection(file_name, new)
cmd     = "SELECT * FROM sqlite_master WHERE type='table' AND name='fit_var'"
cursor  = connection.cursor()
result  = cursor.execute(cmd).fetchall()
have_fit_var = len(result) > 0
#
table_data  = dict()
table_list  = [
	'age',
	'covariate',
	'density',
	'integrand',
	'option',
	'mulcov',
	'node',
	'prior',
	'rate',
	'smooth_grid',
	'time',
	'var'
]
if have_fit_var :
	table_list.append('fit_var')
for table in table_list :
	table_data[table] = dismod_at.get_table_dict(connection, table)
# ----------------------------------------------------------------------------
def table_lookup(table_name, row_id, column_name) :
	if row_id == None :
		return ''
	value = table_data[table_name][row_id][column_name]
	if type(value) is float :
		value = '%13.5g' % value
	else :
		value = str(value)
	assert( value != '' )
	return value
# ----------------------------------------------------------------------------
def get_prior_info(row, prior_id) :
	for field in [ 'lower', 'upper', 'mean', 'std', 'eta' ] :
		row[field] = str( table_data['prior'][prior_id][field] )
	density_id = table_data['prior'][prior_id]['density_id']
	row['density'] = table_data['density'][density_id]['density_name']
	if row['std'] == None :
		row['std'] = ''
	if row['eta'] == None :
		row['eta'] = ''
# ----------------------------------------------------------------------------
file_name = os.path.join(directory_arg, 'var.csv')
csv_file  = open(file_name, 'w')
# ----------------------------------------------------------------------------
# parent_node_id
for row in table_data['option'] :
	if row['option_name'] == 'parent_node_id' :
		parent_node_id = int( row['option_value'] )
# ----------------------------------------------------------------------------

header = [
	'var_id',
	'var_type',
	'fixed',
	'age',
	'time',
	'rate',
	'integrand',
	'fit_value',
	'lower',
	'upper',
	'mean',
	'std',
	'eta',
	'density',
	'covariate',
	'node',
]
csv_writer = csv.DictWriter(csv_file, fieldnames=header)
csv_writer.writeheader()
var_id  = 0
row_out = dict()
for field in header :
	row_out[field] = ''
for row_in in table_data['var'] :
	row_out['var_id']    = var_id
	row_out['var_type']  = row_in['var_type']
	row_out['age']       = table_lookup('age',  row_in['age_id'], 'age')
	row_out['time']      = table_lookup('time', row_in['time_id'], 'time')
	row_out['rate']      = table_lookup('rate', row_in['rate_id'], 'rate_name')
	row_out['integrand'] = table_lookup(
		'integrand', row_in['integrand_id'], 'integrand_name'
	)
	row_out['covariate'] = table_lookup(
		'covariate', row_in['covariate_id'], 'covariate_name'
	)
	row_out['node'] = table_lookup('node', row_in['node_id'], 'node_name')
	#
	if have_fit_var :
		row_out['fit_value'] = table_lookup('fit_var', var_id, 'fit_var_value')
	#
	row_out['fixed'] = 'true'
	if row_in['var_type'] == 'rate' :
		if row_in['node_id'] != parent_node_id :
			row_out['fixed'] = 'false'
	#
	smooth_id = row_in['smooth_id']
	if row_in['var_type'].startswith('mulstd_') :
		row       = table_data['smooth'][smooth_id]
		prior_id  = row['mulstd_value_prior_id']
		get_prior_info(row_out, prior_id)
	else :
		age_id    = row_in['age_id']
		time_id   = row_in['time_id']
		for row in table_data['smooth_grid'] :
			match = row['smooth_id'] == smooth_id
			match = match and row['age_id'] == age_id
			match = match and row['time_id'] == time_id
			if match :
				prior_id   = row['value_prior_id']
				get_prior_info(row_out, prior_id)
	csv_writer.writerow(row_out)
	var_id += 1
# ----------------------------------------------------------------------------
csv_file.close()
