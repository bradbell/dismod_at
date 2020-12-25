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
original_database  = 'ihme_db/dismod-iota-decimated.db'
# copy of /ihme/epi/at_cascade/data/475533/dbs/1/2/dismod.db
original_database  = 'ihme_db/data/475533/dbs/1/2/dismod.db'
# path to file that contains the simplified database
database           = 'ihme_db/temp.db'
# create new smplified database including fit results (otheriwse just plot)
new_database = True
# if creating a new database, run fit both without and then with prevalence
fit_twice = False
# ----------------------------------------------------------------------
# import dismod_at
import math
import os
import sys
import shutil
import subprocess
import copy
import numpy
#
if sys.argv[0] != 'bin/db_simplify.py' :
	msg = 'bin/db_simplify.py must be executed from its parent directory'
	sys.exit(msg)
#
# import dismod_at
sandbox = 'python/dismod_at'
if os.path.isdir(sandbox) :
	print('using ' + sandbox)
	sys.path.insert(0,  os.path.join( os.getcwd(), 'python' ) )
import dismod_at
#
# database
if new_database :
	shutil.copyfile(original_database, database)
# ===========================================================================
# General Purpose Utilities
# ===========================================================================
#
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
def table_name2id(table, table_name, row_name) :
	result = None
	column_name = table_name + '_name'
	for table_id in range( len(table) ) :
		row = table[table_id]
		if row[column_name] == row_name :
			result = table_id
	if result is None :
		msg = 'table_name2id: cannot find {} row in {} table'
		msg = msg.format(row_name, table_name)
		sys.exit(msg)
	return result
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
# ============================================================================
# Ploting Routines
# ============================================================================
# ----------------------------------------------------------------------------
# plot_data
def plot_data(integrand_name) :
	# directory where plots will be stored
	index = database.rfind('/')
	if index < 0 :
		directory = '.'
	else :
		directory = database[0 : index]
	#
	table_name = 'data_subset'
	(data_subset_table, col_name, col_type) = get_table(table_name)
	#
	table_name = 'fit_var'
	(fit_var_table, col_name, col_type) = get_table(table_name)
	#
	table_name = 'fit_data_subset'
	(fit_data_subset_table, col_name, col_type) = get_table(table_name)
	#
	table_name = 'data'
	(data_table, col_name, col_type) = get_table(table_name)
	#
	# this_integrand_id
	this_integrand_id = table_name2id(
		integrand_table, 'integrand', integrand_name
	)
	#
	n_list                  = 0
	avg_integrand_list      = list()
	weighted_residual_list  = list()
	meas_value_list         = list()
	age_list                = list()
	time_list               = list()
	node_list               = list()
	for data_subset_id in range( len(data_subset_table) ) :
		data_id        = data_subset_table[data_subset_id]['data_id']
		row            = data_table[data_id]
		integrand_id   = row['integrand_id']
		#
		if integrand_id == this_integrand_id :
			n_list += 1
			#
			meas_value  = row['meas_value']
			meas_value_list.append( meas_value )
			#
			age  = ( row['age_lower'] + row['age_upper'] ) / 2.0
			age_list.append( age )
			#
			time = ( row['time_lower'] + row['time_upper'] ) / 2.0
			time_list.append(time)
			#
			node_id    = row['node_id']
			node_list.append( node_id )
			#
			row  = fit_data_subset_table[data_subset_id]
			#
			avg_integrand = row['avg_integrand']
			avg_integrand_list.append( avg_integrand )
			#
			weighted_residual = row['weighted_residual']
			weighted_residual_list.append( weighted_residual )
	index_list = range(n_list)
	#
	# map node id to index in set of node_id's
	node_set   = list( set( node_list ) )
	for index in index_list :
		node_id = node_list[index]
		node_list[index] = node_set.index( node_id )
	#
	avg_integrand     = numpy.array( avg_integrand_list )
	meas_value        = numpy.array( meas_value_list )
	weighted_residual = numpy.array( weighted_residual_list )
	age               = numpy.array( age_list )
	time              = numpy.array( time_list )
	node              = numpy.array( node_list )
	index             = numpy.array( index_list )
	#
	y_median    = numpy.median( meas_value)
	y_max       = y_median * 1.5e2
	y_min       = y_median * 1.5e-4
	r_max       = 19.0
	r_min       = -19.0
	#
	avg_integrand = numpy.maximum( avg_integrand, y_min )
	avg_integrand = numpy.minimum( avg_integrand, y_max )
	#
	meas_value = numpy.maximum( meas_value, y_min )
	meas_value = numpy.minimum( meas_value, y_max )
	#
	weighted_residual = numpy.maximum( weighted_residual, r_min )
	weighted_residual = numpy.minimum( weighted_residual, r_max )
	#
	y_max_two  = [y_max, y_max]
	y_min_two  = [y_min, y_min]
	#
	r_max_two  = [r_max, r_max]
	r_zero_two = [ 0.0, 0.0   ]
	r_min_two  = [r_min, r_min]
	#
	point_size  = numpy.array( n_list * [ 1 ] )
	marker_size = numpy.array( n_list * [ 5 ] )
	#
	from matplotlib import pyplot
	import matplotlib.backends.backend_pdf
	file_name = directory + '/' + integrand_name + '.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	for x_name in [ 'index', 'node', 'age', 'time' ] :
		x          = eval( x_name )
		x_two      = [min(x), max(x)]
		#
		fig, axes = pyplot.subplots(3, 1, sharex=True)
		fig.subplots_adjust(hspace=0)
		#
		pyplot.subplot(3, 1, 1)
		y =  meas_value
		pyplot.scatter(x, y, marker='.', color='k', s = point_size)
		pyplot.ylabel(integrand_name)
		pyplot.yscale("log")
		for limit in [ y_max, y_min ] :
			flag = y == limit
			size = marker_size[flag]
			pyplot.scatter(x[flag], y[flag], marker='x', color='k', s=size )
		#
		pyplot.subplot(3, 1, 2)
		y = avg_integrand
		pyplot.scatter(x, y, marker='.', color='k', s = point_size)
		pyplot.ylabel('model')
		pyplot.yscale("log")
		for limit in [ y_max, y_min ] :
			flag = y == limit
			size = marker_size[flag]
			pyplot.scatter(x[flag], y[flag], marker='x', color='k', s=size )
		#
		pyplot.subplot(3, 1, 3)
		y = weighted_residual
		pyplot.scatter(x, y, marker='.', color='k', s = point_size)
		pyplot.plot(x_two, r_zero_two, linestyle='-', color='k')
		pyplot.ylabel('residual')
		for limit in [ r_max, r_min ] :
			flag = y == limit
			size = marker_size[flag]
			pyplot.scatter(x[flag], y[flag], marker='x', color='k', s=size )
		#
		pyplot.xlabel(x_name)
		pdf.savefig( fig )
	#
	pdf.close()
# ----------------------------------------------------------------------------
# plot_predict
def plot_predict() :
	# directory where plots will be stored
	index = database.rfind('/')
	if index < 0 :
		directory = '.'
	else :
		directory = database[0 : index]
	#
	table_name = 'avgint'
	(avgint_table, col_name, col_type) = get_table(table_name)
	#
	table_name = 'predict'
	(predict_table, col_name, col_type) = get_table(table_name)
	#
	integrand_id_info = dict()
	for predict_row in predict_table :
		avgint_id      = predict_row['avgint_id']
		avg_integrand  = predict_row['avg_integrand']
		#
		row = avgint_table[avgint_id]
		#
		integrand_id   = row['integrand_id']
		if integrand_id not in integrand_id_info :
			info = dict()
			info['age']           = list()
			info['time']          = list()
			info['node']          = list()
			info['index']         = list()
			info['avg_integrand'] = list()
		else :
			info = integrand_id_info[integrand_id]
		#
		age  = ( row['age_lower'] + row['age_upper'] ) / 2.0
		info['age'].append( age )
		#
		time = ( row['time_lower'] + row['time_upper'] ) / 2.0
		info['time'].append(time)
		#
		node_id    = row['node_id']
		info['node'].append( node_id )
		#
		index         = len( info['index'] )
		info['index'].append( index )
		#
		info['avg_integrand'].append( avg_integrand )
		#
		integrand_id_info[integrand_id] = info
	#
	integrand_id_list  = integrand_id_info.keys()
	n_predict          = len( predict_table )
	n_integrand        = len( integrand_id_list )
	n_per_integrand    = int( n_predict / n_integrand )
	#
	point_size  =  n_per_integrand * [ 1 ]
	#
	from matplotlib import pyplot
	import matplotlib.backends.backend_pdf
	file_name = directory + '/predict.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	for x_name in [ 'index', 'node', 'age', 'time' ] :
		#
		fig, axes = pyplot.subplots(n_integrand, 1, sharex=True)
		fig.subplots_adjust(hspace=0)
		#
		plot_index = 0
		for integrand_id in integrand_id_list :
			plot_index += 1
			pyplot.subplot(n_integrand, 1, plot_index)
			info = integrand_id_info[integrand_id]
			integrand_name = integrand_table[integrand_id]['integrand_name']
			#
			y  = info['avg_integrand']
			x  = info[x_name]
			pyplot.scatter(x, y, marker='.', color='k', s=point_size )
			pyplot.yscale("log")
			pyplot.ylabel( integrand_name )
		pyplot.xlabel(x_name)
		pdf.savefig( fig )
	#
	pdf.close()
# =============================================================================
# Routines that Change Data Table
# =============================================================================
# subset_data:
# remove rows that are held out or have covariates out of bounds
def subset_data() :
	print('remove hold out and covariate out of bounds data')
	#
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
# subsample_data:
# for a specified integrand, only sample one row in stride
def subsample_data(integrand_name, stride) :
	#
	integrand_id = table_name2id(integrand_table, 'integrand', integrand_name)
	#
	table_name = 'data'
	(table_in, col_name, col_type) = get_table(table_name)
	#
	count_in  = 0;
	count_out = 0;
	table_out = list()
	for row in table_in :
		if row['integrand_id'] != integrand_id :
			table_out.append(row)
		else :
			if count_in % stride == 0 :
				table_out.append(row)
				count_out += 1
			count_in += 1
	put_table(table_name, table_out, col_name, col_type)
	#
	msg = "subsample_data {} stride = {} count_in = {} count_out = {}"
	print( msg.format(integrand_name, stride, count_in, count_out) )
#
# hold_out_data:
# for a specified integrand, set the hold_out to 0 or 1
def hold_out_data(integrand_name, hold_out) :
	print( "{} hold_out = {}".format(integrand_name, hold_out) )
	#
	if hold_out not in [ 0, 1] :
		msg = 'hold_out_data: hold_out is not zero or one'
		sys.exit(msg)
	#
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		integrand_id  = row['integrand_id']
		if integrand_name  == integrand_table[integrand_id]['integrand_name'] :
			row['hold_out'] = hold_out
	put_table(table_name, table, col_name, col_type)
#
# remove_node_data:
# for a specified node, remove all its data
def remove_node_data(node_name) :
	print( "remove_node_data {}".format(node_name) )
	#
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
# set_data_dentity:
# for a specified intergrand, set its data density to a specified value
def set_data_density(integrand_name, density_name) :
	print('set_density {} {}'.format(integrand_name, density_name) )
	#
	# integrand_id
	integrand_id = table_name2id(integrand_table, 'integrand', integrand_name)
	#
	# density_id
	density_id = table_name2id(density_table, 'density', density_name)
	#
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		if row['integrand_id'] == integrand_id :
			row['density_id'] = density_id
	put_table(table_name, table, col_name, col_type)
#
# set_minimum_meas_std:
# for a specified integrand, set the minimm measurement standard deviation
# median_meas_value_cv is a multiplier for the median value for the integrand
def set_minimum_meas_std(integrand_name, median_meas_value_cv) :
	msg ='set {} median_meas_value_cv {}'
	msg = msg.format(integrand_name, median_meas_value_cv)
	print(msg)
	#
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	#
	# integrand_id
	integrand_id = table_name2id(integrand_table, 'integrand', integrand_name)
	#
	sub_table   = list()
	meas_value  = list()
	for row in table :
		if row['integrand_id'] == integrand_id :
			sub_table.append(row)
			meas_value.append( row['meas_value'] )
	median          = numpy.median(meas_value)
	minimum_meas_std = median * median_meas_value_cv
	#
	for row in sub_table :
		if row['meas_std'] < minimum_meas_std :
			row['meas_std'] = minimum_meas_std
	put_table(table_name, table, col_name, col_type)
# ============================================================================
# Routines that Change Other Tables
# ============================================================================
# remove_rate;
# remove both the parent and child variables for a rate
def remove_rate(rate_name) :
	print( 'remove_rate {}'.format(rate_name) )
	#
	table_name = 'rate'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		if row['rate_name'] == rate_name :
			row['parent_smooth_id'] = None
			row['child_smooth_id']  = None
			row['child_nslist_id']  = None
	put_table(table_name, table, col_name, col_type)
#
# set_covariate_reference:
def set_covariate_reference(covariate_id, reference_name) :
	msg = 'set_covariate_reference: x_{} to {}'
	print( msg.format(covariate_id, reference_name) )
	#
	reference_list = [ 'median', 'mean' ]
	if reference_name not in reference_list :
		msg = 'reference_name not one of following'
		for r in reference_list :
			msg += ', ' + r
		sys.exit(msg)

	#
	table_name  = 'data'
	(table, col_name, col_type) = get_table(table_name)
	#
	# covariate_value
	key             = 'x_' + str(covariate_id)
	covariate_value = list()
	for row in table :
		if row[key] is not None :
			covariate_value = row[key]
	#
	if reference_name == 'median' :
		reference = numpy.median(covariate_value)
	elif reference_name == 'mean' :
		reference = numpy.mean(covariate_value)
	else :
		assert False
	#
	table_name  = 'covariate'
	(table, col_name, col_type) = get_table(table_name)
	#
	table[covariate_id]['reference'] = reference
	#
	put_table(table_name, table, col_name, col_type)
#
# set_mulcov_zero:
# set all of the multipliers for a specified covariate to zero without
# changing the order or size of the var table
def set_mulcov_zero(covariate_id) :
	print( 'set_mulcov_zero x_{}'.format(covariate_id) )
	#
	table_name = 'mulcov'
	(mulcov_table, mulcov_col_name, mulcov_col_type) = get_table(table_name)
	#
	table_name = 'smooth'
	(smooth_table, smooth_col_name, smooth_col_type) = get_table(table_name)
	#
	table_name = 'smooth_grid'
	(grid_table, grid_col_name, grid_col_type) = get_table(table_name)
	#
	def new_zero_smooth_id(smooth_id) :
		if smooth_id is None :
			return None
		#
		new_smooth_id   = len(smooth_table)
		new_row         = copy.copy( smooth_table[smooth_id] )
		new_row['smooth_name'] = 'zero_smoothing #' + str( new_smooth_id )
		smooth_table.append( new_row )
		#
		for old_row in grid_table :
			if old_row['smooth_id'] == smooth_id :
				new_row = copy.copy( old_row )
				new_row['smooth_id']      = new_smooth_id
				new_row['value_prior_id'] = None
				new_row['dage_prior_id']  = None
				new_row['dtime_prior_id'] = None
				new_row['const_value']    = 0.0
				grid_table.append( new_row )
		return new_smooth_id
	#
	for row  in  mulcov_table :
		if row['covariate_id'] == covariate_id :
			smooth_id                 = row['group_smooth_id']
			row['group_smooth_id']    = new_zero_smooth_id(smooth_id)
			smooth_id                 = row['subgroup_smooth_id']
			row['subgroup_smooth_id'] = new_zero_smooth_id(smooth_id)
	#
	put_table('mulcov',      mulcov_table, mulcov_col_name, mulcov_col_type)
	put_table('smooth',      smooth_table, smooth_col_name, smooth_col_type)
	put_table('smooth_grid', grid_table,   grid_col_name,   grid_col_type)
#
# constant_rate:
# Set a specified rate to be constant by using one of its parent priors
def constant_rate(rate_name) :
	print( 'constant_rate {}'.format(rate_name) )
	#
	table_name = 'rate'
	(rate_table, col_name, col_type) = get_table(table_name)
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
#
# set_option:
# set a specified option to a specified value
def set_option(name, value) :
	system_command( [
		'dismod_at',  database, 'set', 'option', name , value
	] )
#
# set_start_var:
def set_start_var(table_name) :
	if table_name != 'fit_var' :
		msg = 'set_start_var: table name is not fit_var'
		sys.exit(msg)
	(fit_var_table, col_name, col_type)   = get_table(table_name)
	table_name = 'start_var'
	#
	(start_var_table, col_name, col_type) = get_table(table_name)
	for var_id in range( len(start_var_table) ) :
		row_start = start_var_table[var_id]
		row_fit   = fit_var_table[var_id]
		row_start['start_var_value'] = row_fit['fit_var_value']
	put_table(table_name, start_var_table, col_name, col_type)
#
# set_minimum_meas_cv:
# set the minimum cv for a specified integrand
def set_minimum_meas_cv(integrand_name, minimum_meas_cv) :
	print( 'set {} minimum_meas_cv {}'.format(integrand_name, minimum_meas_cv) )
	#
	table_name = 'integrand'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		if row['integrand_name'] == integrand_name :
			row['minimum_meas_cv'] = minimum_meas_cv
	put_table(table_name, table, col_name, col_type)
#
# avgint_table:
# create avgint table corresponding to an integrand
# (copies columns in data table that are not necessary in avgint table)
def avgint_from_data(data_integrand_name, integrand_name_list) :
	msg = 'avgint_table: data integrand = {}, integrand_name_list = {}'
	msg = msg.format(data_integrand_name, integrand_name_list)
	#
	# data_integrand_id
	table_name   = 'integrand'
	data_integrand_id = table_name2id(
		integrand_table, table_name, data_integrand_name
	)
	# integrand_id_list
	integrand_id_list = list()
	for integrand_name in integrand_name_list :
		integrand_id = table_name2id(
			integrand_table, table_name, integrand_name
		)
		integrand_id_list.append(integrand_id)
	#
	# data_table
	table_name = 'data'
	(data_table, data_col_name, data_col_type) = get_table(table_name)
	#
	# data_subset_table
	table_name = 'data_subset'
	(subset_table, subset_col_name, subset_col_type) = get_table(table_name)
	#
	# data table columns that are not in avgint table
	exclude_list = [
		'data_name',
		'density_id',
		'hold_out',
		'meas_value',
		'meas_std',
		'eta',
		'nu',
	]
	#
	# avgint_col_name, avgint_col_type
	avgint_col_name = list()
	avgint_col_type = list()
	for i in range( len( data_col_name ) ) :
		if data_col_name[i] not in exclude_list :
			avgint_col_name.append( data_col_name[i] )
			avgint_col_type.append( data_col_type[i] )
	#
	# avgint_table
	avgint_table = list()
	for row_subset in subset_table :
		row_in  = data_table[ row_subset['data_id'] ]
		if data_integrand_id == row_in['integrand_id'] :
			row_out = dict()
			for col in avgint_col_name :
				row_out[col] = row_in[col]
			#
			for integrand_id in integrand_id_list :
				row = copy.copy(row_out)
				row['integrand_id'] = integrand_id
				avgint_table.append( row )
	#
	table_name = 'avgint'
	put_table(table_name, avgint_table, avgint_col_name, avgint_col_type)

# ==========================================================================
# Example Changes
# ==========================================================================
#
# set_option:
# set_option('tolerance_fixed',    '1e-6')
# set_option('max_num_iter_fixed', '30')
#
# subset_data:
# subset_data()
#
# remove_rate:
# for rate_name in [ 'omega', 'chi' ] :
#	remove_rate(rate_name)
#
# set_data_density:
# density_name = 'gaussian'
# for integrand_name in [ 'Sincidence', 'prevalence' ] :
#	set_data_density(integrand_name, density_name)
#
# subsample_data:
# stride = 10
# for integrand_name in [ 'Sincidence', 'prevalence' ] :
#	subsample_data(integrand_name, stride)
#
# remove_node_data:
# node_name = 'Mexico'
# remove_node_data(node_name)
#
# constant_rate:
# rate_name = 'iota'
# constant_rate(rate_name)
#
# set_minumum_meas_cv:
# minimum_meas_cv = 0.5
# for integrand_name in [ 'Sincidence', 'prevalence', 'mtexcess' ] :
#	set_minimum_meas_cv(integrand_name, minimum_meas_cv)
#
# set_start_var:
# table_name = 'fit_var'
# set_start_var(table_name)
#
# hold_out_data:
# hold_out       = 0
# integrand_name = 'prevalence'
# hold_out_data(integrand_name, hold_out)
#
# ----------------------------------------------------------------------
# Actual Changes
# ----------------------------------------------------------------------
integrand_list = [ 'Sincidence', 'mtexcess', 'prevalence' ]
if new_database :
	# set options
	set_option('tolerance_fixed',    '1e-6')
	set_option('max_num_iter_fixed', '100')
	set_option('zero_sum_child_rate', 'iota chi')
	#
	# set reference value for covariates to median
	# (must do this before subset_data)
	covariate_id    = 0
	reference_name  = 'median'
	set_covariate_reference(covariate_id, reference_name)
	#
	# remove all hold hout data and data past covriate limits
	subset_data()
	#
	# subsample mtexcess (because there is way more than other data)
	stride = 10
	integrand_name = 'mtexcess'
	subsample_data(integrand_name, stride)
	#
	# now further subasmple all data (for speed of testing)
	stride = 10
	for integrand_name in integrand_list :
		subsample_data(integrand_name, stride)
	#
	# constrain all covariate multipliers to be zero
	for covariate_id in range( len(covariate_table) ) :
		set_mulcov_zero(covariate_id)
	#
	# change density to gaussian
	density_name   = 'gaussian'
	for integrand_name in [ 'Sincidence', 'mtexcess', 'prevalence' ] :
		set_data_density(integrand_name, density_name)
	#
	# set the minimum measurement standard deviation and cv
	median_meas_value_cv = 1e-1
	minimum_meas_cv      = 1e-1
	for integrand_name in [ 'Sincidence', 'prevalence' ] :
		set_minimum_meas_std(integrand_name, median_meas_value_cv)
		set_minimum_meas_cv(integrand_name, minimum_meas_cv)
	#
	# take prevalence out of fit
	hold_out       = 1
	integrand_name = 'prevalence'
	hold_out_data(integrand_name, hold_out)
	#
	# init
	system_command([ 'dismod_at', database, 'init'])
	#
	# avgint_table
	data_integrand_name = 'prevalence'
	integrand_name_list = [ 'susceptible', 'withC', 'mtother' ]
	avgint_from_data(data_integrand_name, integrand_name_list)
	#
	# fit both
	system_command([ 'dismod_at', database, 'fit', 'both'])
	#
	if fit_twice :
		#
		# set_start_var = fit_var
		table_name = 'fit_var'
		set_start_var(table_name)
		#
		# put prevalence data back in the fit
		hold_out       = 1
		integrand_name = 'prevalence'
		hold_out_data(integrand_name, hold_out)
		#
		# fit both
		system_command([ 'dismod_at', database, 'fit', 'both'])
	#
	# predict fit_var (for plot_predict)
	system_command([ 'dismod_at', database, 'predict', 'fit_var' ])
#
for integrand_name in [ 'Sincidence', 'mtexcess', 'prevalence' ] :
	plot_data(integrand_name)
plot_predict()
#
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
