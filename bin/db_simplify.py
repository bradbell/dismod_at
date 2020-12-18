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
# 2DO: Allow for covariate access by ovariate_name.
# 2DO: Correct weight sum of weighted residuals in log case.
# 2DO: Envforce a minimum for all the standard deviations for one integrand.
# --------------------------------------------------------------------------
# copy of /ihme/epi/at_cascade/data/475533/dbs/1/2/dismod.db
original_database  = 'ihme_db/dismod-iota-decimated.db'
original_database  = 'ihme_db/data/475533/dbs/1/2/dismod.db'
# path to file that well be used for testing changes to database
database   = 'ihme_db/temp.db'
# ----------------------------------------------------------------------
# import dismod_at
import math
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
#
# database
shutil.copyfile(original_database, database)
# ===========================================================================
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
#
# run_fit
def run_fit() :
	# ------------------------------------------------------------------------
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
# ============================================================================
# Tables that do not change
# ============================================================================
# integrand_table
table_name = 'integrand'
(integrand_table, col_name, col_type) = get_table(table_name)
#
# density_table
table_name = 'density'
(density_table, col_name, col_type) = get_table(table_name)
#
# covariate_table
table_name = 'covariate'
(covariate_table, col_name, col_type) = get_table(table_name)
#
# node_table
table_name = 'node'
(node_table, col_name, col_type) = get_table(table_name)
# =============================================================================
# subset_data
def subset_data() :
	print('remove hold out and covariate out of bounds data')
	table_name = 'data'
	(table_in, col_name, col_type) = get_table(table_name)
	table_out     = list()
	col_key       = list()
	reference     = list()
	max_diff      = list()
	n_covariate   = len(covariate_table)
	for covariate_id in range( n_covariate ) :
		col_key.append( 'x_' + str(covariate_id) )
		row  = covariate_table[covariate_id]
		reference.append( float( row['reference'] ) )
		if row['max_difference'] is None :
			max_diff.append( math.inf )
		else :
			max_diff.append( float( row['max_difference'] ) )
	for row in table_in :
		keep = row['hold_out'] == 0
		if keep :
			for covariate_in in range( n_covariate ) :
				value      = float( row[col_key[covariate_id]] )
				difference = value - reference[covariate_id]
				keep       = keep and abs(difference) <= max_diff[covariate_id]
		if keep :
			table_out.append(row)
	put_table(table_name, table_out, col_name, col_type)
#
# subsample_data
def subsample_data(integrand_name, stride) :
	print( "subsample_data {} stride = {}".format(integrand_name, stride) )
	counter        = 0
	subsample_integrand_id = None
	for integrand_id in range( len(integrand_table) ) :
		row = integrand_table[integrand_id]
		if row['integrand_name'] == integrand_name :
			subsample_integrand_id = integrand_id
	table_name = 'data'
	(table_in, col_name, col_type) = get_table(table_name)
	table_out = list()
	for row in table_in :
		integrand_id    = row['integrand_id']
		if integrand_id != subsample_integrand_id :
			table_out.append(row)
		else :
			counter += 1
			if counter % stride == 1 :
				table_out.append(row)
	put_table(table_name, table_out, col_name, col_type)
#
# hold out data
def hold_out_data(integrand_name) :
	print( "hold_out {}".format(integrand_name) )
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		integrand_id    = row['integrand_id']
		integrand_name  = integrand_table[integrand_id]['integrand_name']
		if integrand_name == 'mtexcess' :
			row['hold_out'] = 1
	put_table(table_name, table, col_name, col_type)
#
# remove_integrand_data
def remove_integrand_data(integrand_name) :
	print( "remove_integrand_data {}".format(integrand_name) )
	remove_integrand_id = None
	for integrand_id in range( len(integrand_table) ) :
		row = integrand_table[integrand_id]
		if row['integrand_name'] == integrand_name :
			remove_integrand_id = integrand_id
	table_name = 'data'
	(table_in, col_name, col_type) = get_table(table_name)
	table_out = list()
	for row in table_in :
		integrand_id    = row['integrand_id']
		if integrand_id != remove_integrand_id :
			table_out.append(row)
	put_table(table_name, table_out, col_name, col_type)
#
# remove_node_data
def remove_node_data(node_name) :
	print( "remove_node_data {}".format(node_name) )
	remove_node_id = None
	for node_id in range( len(node_table) ) :
		row = node_table[node_id]
		if row['node_name'] == node_name :
			remove_node_id = node_id
	table_name = 'data'
	(table_in, col_name, col_type) = get_table(table_name)
	table_out = list()
	for row in table_in :
		node_id    = row['node_id']
		if node_id != remove_node_id :
			table_out.append(row)
	put_table(table_name, table_out, col_name, col_type)
#
# remove rate
def remove_rate(rate_name) :
	print( 'remove_rate {}'.format(rate_name) )
	table_name = 'rate'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		if row['rate_name'] == rate_name :
			row['parent_smooth_id'] = None
			row['child_smooth_id']  = None
			row['child_nslist_id']  = None
	put_table(table_name, table, col_name, col_type)
#
# remove mulcov
def remove_mulcov(covariate_id) :
	print( 'remove_mulcov x_{}'.format(covariate_id) )
	table_name = 'mulcov'
	(table, col_name, col_type) = get_table(table_name)
	for row  in  table :
		if row['covariate_id'] == covariate_id :
			row['group_smooth_id']    = None
			row['subgroup_smooth_id'] = None
	put_table(table_name, table, col_name, col_type)
#
# set data_density
def set_data_density(integrand_name, density_name) :
	print('set_density {} {}'.format(integrand_name, density_name) )
	#
	# change_integrand_id
	this_integrand_id = None
	for integrand_id in range( len( integrand_table ) ) :
		if integrand_table[integrand_id]['integrand_name'] == integrand_name :
			this_integrand_id = integrand_id
	if this_integrand_id is None :
		msg = 'set_data_density: invalid intgrand_name = '+ integrand_name
		sys.exit( msg )
	#
	# this_density_id
	this_density_id = None
	for density_id in range( len( density_table ) ) :
		if density_table[density_id]['density_name'] == density_name :
			this_density_id = density_id
	if this_density_id is None :
		msg = 'set_data_density: invalid density_name = ' + density_name
		sys.exit( msg )
	#
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		if row['integrand_id'] == this_integrand_id :
			row['density_id'] = this_density_id
	put_table(table_name, table, col_name, col_type)
#
# constant_rate
def constant_rate(rate_name) :
	table_name = 'rate'
	(rate_table, col_name, col_type) = get_table(table_name)
	print( 'constant_rate {}'.format(rate_name) )
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
	# get parent_smooth_id for the rate
	rate_smooth_id = None
	for row in rate_table :
		if row['rate_name'] == rate_name :
			rate_smooth_id = row['parent_smooth_id']
	if rate_smooth_id is None :
		msg = 'constant_rate: The is no parent_smooth_id for ' + rate_name
		sys.exit(msg)
	#
	# get any entry in smooth_grid_table for smoothing iota
	table_name = 'smooth_grid'
	(table, col_name, col_type) = get_table(table_name)
	rate_smooth_grid_id = None
	for smooth_grid_id in range( len(table) ) :
		row = table[smooth_grid_id]
		if row['smooth_id'] == rate_smooth_id :
			rate_smooth_grid_id = smooth_grid_id
	assert rate_smooth_grid_id is not None
	#
	# make a copy of the smooth_grid table entry
	row = copy.copy( table[rate_smooth_grid_id] )
	#
	# change to smooth_id to the row added to smoothing above
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
		if row['rate_name'] == rate_name :
			row['parent_smooth_id'] = one_point_smooth_id
			row['child_smooth_id']  = None
			row['child_nslist_id']  = None
	put_table(table_name, table, col_name, col_type)
# ==========================================================================
print('set tolerance_fixed = 1e-6, max_num_iter_fixed = 30')
system_command( [
	'dismod_at',  database, 'set', 'option', 'tolerance_fixed', '1e-6'
] )
system_command( [
	'dismod_at',  database, 'set', 'option', 'max_num_iter_fixed', '30'
] )
# ----------------------------------------------------------------------
# Example simplifications
# ----------------------------------------------------------------------
# subset data
# subset_data()
#
# hold out mtexcess data
# integrand_name = 'mtexcess'
# hold_out_data(integrand_name)
#
# remove mtexcess data
# integrand_name = 'mtexcess'
# remove_integrand_data(integrand_name)
#
# remove omega and chi variables
# for rate_name in [ 'omega', 'chi' ] :
#	remove_rate(rate_name)
#
# remove all covariate multipliers
# for covariate_id in range( len(covariate_table) ) :
#	remove_mulcov(covariate_id)
#
# set Sincendence and prevalence density to gaussian
# density_name = 'gaussian'
# for integrand_name in [ 'Sincidence', 'prevalence' ] :
#	set_data_density(integrand_name, density_name)
#
# subsample Sincedence and prevalence
# stride = 1000
# for integrand_name in [ 'Sincidence', 'prevalence' ] :
#	subsample_data(integrand_name, stride)
#
# remove Mexico data
# node_name = 'Mexico'
# remove_node_data(node_name)
#
# set iota to a constant rate
# rate_name = 'iota'
# constant_rate(rate_name)
# ----------------------------------------------------------------------
# subset to only data in the fit
subset_data()
#
# subsample mtexcess data
stride         = 10
integrand_name = 'mtexcess'
subsample_data(integrand_name, stride)
#
# remove all covariate multipliers
for covariate_id in range( len(covariate_table) ) :
	remove_mulcov(covariate_id)
#
# set all data density to gaussian
density_name = 'gaussian'
for integrand_name in [ 'Sincidence', 'prevalence', 'mtexcess' ] :
	set_data_density(integrand_name, density_name)
#
# run fit and summary
run_fit()
# ----------------------------------------------------------------------
print('db_simplify.py: OK')
sys.exit(0)
