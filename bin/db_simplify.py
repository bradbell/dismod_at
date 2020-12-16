#! /bin/python3
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# ----------------------------------------------------------------------
# path to file containing original database
original_database  = 'ihme_db/dismod.db'
# path to file that well be used for testing changes to database
database   = 'ihme_db/temp.db'
# ----------------------------------------------------------------------
# import dismod_at
import os
import sys
import shutil
import subprocess
import copy
#
# import dismod_at
sandbox = 'python/dismod_at'
if os.path.isdir(sandbox) :
    print('using ' + sandbox)
    sys.path.insert(0,  os.path.join( os.getcwd(), 'python' ) )
import dismod_at
# ---------------------------------------------------------------------------
# database
shutil.copyfile(original_database, database)
# ----------------------------------------------------------------------
# get_table
def get_table(table_name) :
	new                  = False
	connection           = dismod_at.create_connection(database, new)
	(col_name, col_type) = dismod_at.get_name_type(connection, table_name)
	table                = dismod_at.get_table_dict(connection, table_name)
	connection.close()
	primary_key  = table_name +  '_id'
	assert col_name[0] == primary_key
	del col_name[0]
	del col_type[0]
	return (table, col_name, col_type)
#
# put_table
def put_table(table_name, table, col_name, col_type) :
	new          = False
	connection   = dismod_at.create_connection(database, new)
	cursor       = connection.cursor()
	cmd          = 'DROP TABLE ' + table_name
	cursor.execute(cmd)
	#
	row_list = list()
	for row in table :
		this_row = list()
		for col in col_name :
			this_row.append( row[col] )
		row_list.append( this_row )
	#
	dismod_at.create_table(connection,table_name,col_name,col_type,row_list)
	connection.close()
#
# system_command
def system_command(command_list) :
	command_str = " ".join(command_list)
	print(command_str)
	run = subprocess.run(command_list)
	if run.returncode != 0 :
		sys.exit('db_simplify.py: system command failed')
# ----------------------------------------------------------------------
# integrand_table
table_name = 'integrand'
(integrand_table, col_name, col_type) = get_table(table_name)
# ----------------------------------------------------------------------
# density_table
table_name = 'density'
(density_table, col_name, col_type) = get_table(table_name)
# ----------------------------------------------------------------------
# rate_table
print('remove all rates except for iota')
table_name = 'rate'
(rate_table, col_name, col_type) = get_table(table_name)
for row in rate_table :
	if row['rate_name'] != 'iota' :
		row['parent_smooth_id'] = None
		row['child_smooth_id']  = None
		row['child_nslist_id']  = None
put_table(table_name, rate_table, col_name, col_type)
# ----------------------------------------------------------------------
print('remove all covariate multipliers')
table_name = 'mulcov'
(table, col_name, col_type) = get_table(table_name)
for row in table :
	row['group_smooth_id']    = None
	row['subgroup_smooth_id'] = None
put_table(table_name, table, col_name, col_type)
# ----------------------------------------------------------------------
print('hold out mtexcess data')
table_name = 'data'
(table, col_name, col_type) = get_table(table_name)
for row in table :
	integrand_id    = row['integrand_id']
	integrand_name  = integrand_table[integrand_id]['integrand_name']
	if integrand_name == 'mtexcess' :
		row['hold_out'] = 1
put_table(table_name, table, col_name, col_type)
# ----------------------------------------------------------------------
print('remove hold out data')
# note use of table_in and table_out for this operation
table_name = 'data'
(table_in, col_name, col_type) = get_table(table_name)
table_out = list()
for row in table :
	if row['hold_out'] == 0 :
		table_out.append(row)
put_table(table_name, table_out, col_name, col_type)
# ----------------------------------------------------------------------
print('change data density to gaussian')
#
# search for gaussian_density_id
gaussian_density_id = None
for density_id in range( len( density_table ) ) :
	if density_table[density_id]['density_name'] == 'gaussian' :
		gaussian_density_id = density_id
assert gaussian_density_id is not None
#
# now change all the data table to only used this gaussian density id
table_name = 'data'
(table, col_name, col_type) = get_table(table_name)
for row in table :
	row['density_id'] = gaussian_density_id
put_table(table_name, table, col_name, col_type)
# ----------------------------------------------------------------------
print('make iota constant in age and time')
# ----------------------------------------------------------------------
#
# add a one point smoothing to smooth_table
table_name = 'smooth'
(table, col_name, col_type) = get_table(table_name)
one_point_smooth_id         = len( table )
row =  {
	'smooth_name'           : 'my_constant' ,
	'n_age'                 : 1             ,
	'n_time'                : 1             ,
	'mulstd_value_prior_id' : None          ,
	'mulstd_dage_prior_id'  : None          ,
	'mulstd_dtime_prior_id' : None
}
table.append(row)
put_table(table_name, table, col_name, col_type)
#
# get parent_smooth_id for iota
iota_smooth_id = None
for row in rate_table :
	if row['rate_name'] == 'iota' :
		iota_smooth_id = row['parent_smooth_id']
assert iota_smooth_id is not None
#
# get any entry in smooth_grid_table for smoothing iota
table_name = 'smooth_grid'
(table, col_name, col_type) = get_table(table_name)
iota_smooth_grid_id = None
for smooth_grid_id in range( len(table) ) :
	row = table[smooth_grid_id]
	if row['smooth_id'] == iota_smooth_id :
		iota_smooth_grid_id = smooth_grid_id
assert iota_smooth_grid_id is not None
#
# make a copy of the smooth_grid table entry
row = copy.copy( table[iota_smooth_grid_id] )
#
# change to smooth_id ot the point point smoothing added above
row['smooth_id'] = one_point_smooth_id
#
# add this entry to the smooth grid table
table.append(row)
put_table(table_name, table, col_name, col_type)
#
# change the rate table to use this entry
table_name = 'rate'
(table, col_name, col_type) = get_table(table_name)
for row in table :
	if row['rate_name'] == 'iota' :
		row['parent_smooth_id'] = one_point_smooth_id
put_table(table_name, table, col_name, col_type)
# ----------------------------------------------------------------------
# fit fixed
system_command( [ 'dismod_at',  database, 'init' ] )
system_command( [ 'dismod_at',  database, 'fit', 'fixed' ] )
# ----------------------------------------------------------------------
# csv files and summary
index = database.rfind('/')
if index < 0 :
	directory = '.'
else :
	directory = database[0 : index]
system_command( [ 'dismodat.py',  database, 'db2csv' ] )
system_command( [ 'bin/csv_summary.py',  directory ] )
# ----------------------------------------------------------------------
print('db_simplify.py: OK')
sys.exit(0)
