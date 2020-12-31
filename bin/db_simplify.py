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
# 2DO: Allow for covariate access by covariate_name.
# --------------------------------------------------------------------------
# Diabetes: /ihme/epi/at_cascade/data/475588/dbs/100/3/dismod.db
# Chrons:   /ihme/epi/at_cascade/data/475533/dbs/1/2/dismod.db'
original_database  = 'ihme_db/data/475588/dbs/100/3/dismod.db'
# path to file that contains the simplified database
database           = 'ihme_db/temp.db'
# create new simplified database including fit results (otherwise just plot)
new_database       = True
# if new_database is true, run fit both first without and then with ode data.
fit_ode            = False
# print the help message for all the db_simplify routines and then exit
print_help         = True
# ----------------------------------------------------------------------
# import dismod_at
import math
import os
import sys
import shutil
import subprocess
import copy
import numpy
import random
import time
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
#
# directory where plots are stored
index = database.rfind('/')
if index < 0 :
	plot_directory = '.'
else :
	plot_directory = database[0 : index]
#
if print_help :
	# print the help message for each db_simplify routine
	file_name = sys.argv[0]
	fp        = open(file_name, 'r')
	fp_data   = fp.read()
	print()
	#
	def more(index) :
		while fp_data[index] in ' \t' :
			index += 1
		if fp_data[index] == '#' :
			return index
		return -1
	#
	start  = 0
	index  = fp_data.find('\ndef ', start)
	while 0 <= index :
		start  = index + 5
		stop   = fp_data.find('\n', start) + 1
		output = fp_data[start : stop]
		while 0 <= more(stop) :
			start   = more(stop) + 2
			stop    = fp_data.find('\n', stop) + 1
			output += fp_data[start : stop]
		print( output )
		start = stop
		index  = fp_data.find('\ndef ', start)
	sys.exit(0)
# ===========================================================================
# General Purpose Utilities
# ===========================================================================
# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
def get_table (table_name) :
	# read a dismod_at table
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
# ----------------------------------------------------------------------------
def put_table (table_name, table, col_name, col_type) :
	# write a dismod_at table
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
# ----------------------------------------------------------------------------
def system_command (command_list) :
	# execute a command at the system level
	command_str = " ".join(command_list)
	print(command_str)
	run = subprocess.run(command_list)
	if run.returncode != 0 :
		if new_database :
			print('random_seed = ', random_seed )
		sys.exit('db_simplify.py: system command failed')
# ----------------------------------------------------------------------------
def table_name2id (table, table_name, row_name) :
	# map a table row name to the table_id
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
# ----------------------------------------------------------------------------
def new_zero_smooth_id (smooth_id, smooth_table, smooth_grid_table) :
	# add a new smoothing that has the same grid as smooth_id smoothing
	# and that constrains to zero
	if smooth_id is None :
		return None
	#
	new_smooth_id          = len(smooth_table)
	new_row                = copy.copy( smooth_table[smooth_id] )
	new_row['smooth_name'] = 'zero_smoothing #' + str( new_smooth_id )
	smooth_table.append( new_row )
	#
	for old_row in smooth_grid_table :
		if old_row['smooth_id'] == smooth_id :
			new_row = copy.copy( old_row )
			new_row['smooth_id']      = new_smooth_id
			new_row['value_prior_id'] = None
			new_row['dage_prior_id']  = None
			new_row['dtime_prior_id'] = None
			new_row['const_value']    = 0.0
			smooth_grid_table.append( new_row )
	return new_smooth_id
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
#
# age_table
table_name = 'age'
(age_table, col_name, col_type) = get_table(table_name)
#
# time_table
table_name = 'time'
(time_table, col_name, col_type) = get_table(table_name)
#
# option_table
table_name = 'option'
(option_table, col_name, col_type) = get_table(table_name)
# ============================================================================
# Utilities that depend on data table or fit results
# ============================================================================
# ----------------------------------------------------------------------------
def get_integrand_list (ode) :
	# If ode is true (false) get list of integrands that require
	# (do not require) the ode to model.
	integrand_model_uses_ode = [
		'prevalence', 'Tincidence', 'mtspecific', 'mtall', 'mtstandard'
	]
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	#
	integrand_set = set()
	for row in table :
		integrand_id   = row['integrand_id']
		integrand_name = integrand_table[integrand_id]['integrand_name']
		if ode :
			if integrand_name in integrand_model_uses_ode :
				integrand_set.add(integrand_name)
		else :
			if integrand_name not in integrand_model_uses_ode :
				integrand_set.add(integrand_name)
	integrand_list = list(integrand_set)
	#
	return integrand_list
# ----------------------------------------------------------------------------
def get_integrand_count () :
	# return a dictionary that contains the number of data points for each
	# integrand name
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	#
	integrand_count = dict()
	for row in table :
		integrand_id   = row['integrand_id']
		integrand_name = integrand_table[integrand_id]['integrand_name']
		if integrand_name not in integrand_count :
			integrand_count[integrand_name] = 0
		integrand_count[integrand_name] += 1
	#
	return integrand_count
# ----------------------------------------------------------------------------
def plot_rate (rate_name) :
	# plot the fit_var grid values for a specified rate.
	table_name = 'rate'
	(rate_table, col_name, col_type) = get_table(table_name)
	#
	# smooth_table
	table_name = 'smooth'
	(smooth_table, col_name, col_type) = get_table(table_name)
	#
	# var_table
	table_name = 'var'
	(var_table, col_name, col_type) = get_table(table_name)
	#
	# fit_var
	table_name = 'fit_var'
	(fit_var_table, col_name, col_type) = get_table(table_name)
	#
	# rate_id
	table_name   = 'rate'
	rate_id      = table_name2id(rate_table, table_name, rate_name)
	#
	#
	# parent_node_id
	parent_node_id = None
	for row in option_table :
		if row['option_name'] == 'parent_node_id' :
			parent_node_id = row['option_value']
		if row['option_name'] == 'parent_node_name' :
			table_name = 'node'
			node_name  = row['option_value']
			parent_note_id = table_name2id(table_name, node_table, node_name)
	if parent_node_id is None :
		msg = 'Cannot find parent_node_id or parent_node_name in option table'
		sys.exit(msg)
	parent_node_id = int( parent_node_id )
	#
	# class for compariing an (age_id, time_id) pairs
	class pair:
		def __init__(self, age_id, time_id) :
			self.age_id  = age_id
			self.time_id = time_id
		def __lt__(self, other) :
			if self.age_id != other.age_id :
				self_age   = age_table[ self.age_id ]['age']
				other_age  = age_table[ other.age_id ]['age']
				return self_age < other_age
			self_time  = time_table[ self.time_id ]['time']
			other_time = time_table[ other.time_id ]['time']
			return self_time < other_time
		def _eq_(self, other) :
			equal = self.age_id == other.age_id
			equal = equal and (self.time_id == other.time_id)
			return equla
		def __gt__(self, other) :
			return __lt__(other, self)
		def __le__(self, other) :
			return __lt__(self, other) or __eq__(self, other)
		def __ge__(self, other) :
			return __lt__(other, self) or __eq__(other, self)
		def __ne__(self, other) :
			return not __eq__(self, other)
	#
	# triple_list
	triple_list  = list()
	smooth_id    = None
	for (var_id, row) in enumerate(var_table) :
		if row['var_type'] == 'rate' :
			if row['rate_id'] == rate_id :
				if  row['node_id'] == parent_node_id :
					age_id  = row['age_id']
					time_id = row['time_id']
					triple_list.append( (age_id, time_id, var_id)  )
					if smooth_id is None :
						smooth_id = row['smooth_id']
					else :
						assert smooth_id == row['smooth_id']
	#
	# n_age, n_time
	n_age  = smooth_table[smooth_id]['n_age']
	n_time = smooth_table[smooth_id]['n_time']
	assert len(triple_list) == n_age * n_time
	#
	# sort triple_list first by age and then by time
	key = lambda triple : pair( triple[0], triple[1] )
	triple_list = sorted(triple_list, key = key )
	#
	# creaate the mesghgird (age, time, rate)
	age  = numpy.zeros( (n_age, n_time), dtype = float)
	time = numpy.zeros( (n_age, n_time), dtype = float)
	rate = numpy.zeros( (n_age, n_time), dtype = float)
	#
	for i in range(n_age) :
		for j in range(n_time) :
			k       = i * n_time + j
			triple  = triple_list[k]
			#
			age_id  = triple[0]
			time_id = triple[1]
			var_id  = triple[2]
			#
			age[i, j]  = age_table[age_id]['age']
			time[i, j] = time_table[time_id]['time']
			rate[i, j] = fit_var_table[var_id]['fit_var_value']
	#
	# rate limits
	rate_max  = numpy.max(rate)
	rate_min  = numpy.min(rate)
	if rate_max <= 0.0 :
		print( 'plot_rate: max({}) <= 0'.format(rate_name) )
		return
	rate_min  = max(rate_min, 1e-10 * rate_max)
	#
	from matplotlib import pyplot
	#
	# for each time, plot rate as a function of age
	fig_1 = pyplot.figure()
	for j in range(n_time) :
		x     = age[:,j]
		y     = rate[:,j]
		label = str( time[0,j] )
		pyplot.plot(x, y, label=label)
		#
		# axis labels
		pyplot.xlabel('age')
		pyplot.ylabel(rate_name)
		pyplot.yscale('log')
	pyplot.legend(title = 'time')
	#
	# for each age, plot rate as a function of time
	fig_2 = pyplot.figure()
	for i in range(n_age) :
		x     = time[i,:]
		y     = rate[i,:]
		label = str( age[i,0] )
		pyplot.plot(x, y, label=label)
		#
		# axis labels
		pyplot.xlabel('time')
		pyplot.ylabel(rate_name)
		pyplot.yscale('log')
	pyplot.legend(title = 'age')
	#
	import matplotlib.backends.backend_pdf
	file_name = plot_directory + '/' + rate_name + '.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	pdf.savefig( fig_1 )
	pdf.savefig( fig_2 )
	#
	pyplot.close( fig_1 )
	pyplot.close( fig_2 )
	#
	pdf.close()
# ----------------------------------------------------------------------------
# plot_data
def plot_data (integrand_name) :
	# Plot the data, model, and residual values for a specified integrand.
	# Covariate values used for each model point are determined by
	# correspondign data point.
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
	# add 1 to index so index zero not hidden by y-axis
	index             = numpy.array( index_list ) + 1
	#
	y_median    = numpy.median( meas_value)
	y_max       = y_median * 1e+3
	y_min       = y_median * 1e-3
	r_max       = 20.0
	r_min       = -20.0
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
	marker_size = numpy.array( n_list * [ 7 ] )
	#
	from matplotlib import pyplot
	import matplotlib.backends.backend_pdf
	file_name = plot_directory + '/' + integrand_name + '.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	for x_name in [ 'index', 'node', 'age', 'time' ] :
		x          = eval( x_name )
		x_two      = [min(x), max(x)]
		#
		fig, axes = pyplot.subplots(3, 1, sharex=True)
		fig.subplots_adjust(hspace=0)
		#
		sp = pyplot.subplot(3, 1, 1)
		sp.set_xticklabels( [] )
		y =  meas_value
		pyplot.scatter(x, y, marker='.', color='k', s = point_size)
		pyplot.ylabel(integrand_name)
		pyplot.yscale('log')
		for limit in [ y_max, y_min ] :
			flag = y == limit
			size = marker_size[flag]
			pyplot.scatter(x[flag], y[flag], marker='x', color='k', s=size )
		limits     = [min(x), max(x), y_min, y_max]
		pyplot.axis(limits)
		#
		sp = pyplot.subplot(3, 1, 2)
		sp.set_xticklabels( [] )
		y = avg_integrand
		pyplot.scatter(x, y, marker='.', color='k', s = point_size)
		pyplot.ylabel('model')
		pyplot.yscale('log')
		for limit in [ y_max, y_min ] :
			flag = y == limit
			size = marker_size[flag]
			pyplot.scatter(x[flag], y[flag], marker='x', color='k', s=size )
		limits     = [min(x), max(x), y_min, y_max]
		pyplot.axis(limits)
		#
		# this plot at the bottom of the figure has its x tick labels
		pyplot.subplot(3, 1, 3)
		y = weighted_residual
		pyplot.scatter(x, y, marker='.', color='k', s = point_size)
		pyplot.plot(x_two, r_zero_two, linestyle='-', color='k')
		pyplot.ylabel('residual')
		for limit in [ r_max, r_min ] :
			flag = y == limit
			size = marker_size[flag]
			pyplot.scatter(x[flag], y[flag], marker='x', color='k', s=size )
		limits     = [min(x), max(x), r_min, r_max]
		pyplot.axis(limits)
		#
		pyplot.xlabel(x_name)
		#
		pdf.savefig( fig )
		pyplot.close( fig )
	#
	pdf.close()
# ----------------------------------------------------------------------------
# plot_predict
def plot_predict (covariate_integrand_list, predict_integrand_list) :
	# Plot the model predictions for each integrand in the predict integrand
	# list. The is one such plot for each integrand in the covariate integrand
	# list (which determines the covariate values used for the predictions).
	dummy_variable_used_to_end_doc_string = None
	# -----------------------------------------------------------------------
	# create avgint table
	# For each covariate_integrand
	#	For data row corresponding to this covariate_integrand
	#		For each predict_intgrand
	#			write a row with specified covariates for predict_integrand
	#-----------------------------------------------------------------------
	#
	# covariate_id_list
	table_name        = 'integrand'
	covariate_id_list = list()
	for integrand_name in covariate_integrand_list :
		covariate_id = table_name2id(
			integrand_table, table_name, integrand_name
		)
		covariate_id_list.append( covariate_id )
	#
	# predict_id_list
	table_name      = 'integrand'
	predict_id_list = list()
	for integrand_name in predict_integrand_list :
		predict_id = table_name2id(
			integrand_table, table_name, integrand_name
		)
		predict_id_list.append( predict_id )
	#
	# data_table
	table_name = 'data'
	(data_table, data_col_name, data_col_type) = get_table(table_name)
	#
	# data_subset_table
	table_name = 'data_subset'
	(subset_table, subset_col_name, subset_col_type) = get_table(table_name)
	#
	# exclude_list
	# columns that are in data table and not in avgint table
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
	# initialize
	avgint_table = list()
	data_rows    = dict()
	#
	for covariate_integrand_id in covariate_id_list :
		count = 0
		for row_subset in subset_table :
			row_in  = data_table[ row_subset['data_id'] ]
			if covariate_integrand_id == row_in['integrand_id'] :
				count += 1
				row_out = dict()
				for col in avgint_col_name :
					row_out[col] = row_in[col]
				#
				for integrand_id in predict_id_list :
					row = copy.copy(row_out)
					row['integrand_id'] = integrand_id
					avgint_table.append( row )
		data_rows[covariate_integrand_id] = count
	#
	table_name = 'avgint'
	put_table(table_name, avgint_table, avgint_col_name, avgint_col_type)
	# ------------------------------------------------------------------------
	# Predict for this avgint table
	system_command([ 'dismod_at', database, 'predict', 'fit_var' ])
	#
	table_name = 'predict'
	(predict_table, col_name, col_type) = get_table(table_name)
	# ------------------------------------------------------------------------
	# initialize
	from matplotlib import pyplot
	import matplotlib.backends.backend_pdf
	file_name = plot_directory + '/predict.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	predict_id = 0
	for covariate_integrand_id in covariate_id_list :
		n_data_rows        = data_rows[covariate_integrand_id]
		avg_integrand_list = dict()
		age_list           = list()
		for integrand_id in predict_id_list :
			avg_integrand_list[integrand_id] = list()
		for i in range(n_data_rows) :
			first = True
			for integrand_id in predict_id_list :
				#
				# predict_row
				predict_row = predict_table[predict_id]
				assert predict_id == predict_row['avgint_id']
				#
				avg_integrand = predict_row['avg_integrand']
				avg_integrand_list[integrand_id].append( avg_integrand )
				#
				row  = avgint_table[predict_id]
				assert integrand_id == row['integrand_id']
				#
				if first :
					age  = ( row['age_lower'] + row['age_upper'] ) / 2.0
					age_list.append( age )
				first = False
				#
				# for next row
				predict_id += 1
		#
		point_size            =  n_data_rows * [ 1 ]
		n_predict_integrand   = len(predict_integrand_list)
		#
		fig, axes = pyplot.subplots(n_predict_integrand, 1, sharex=True)
		fig.subplots_adjust(hspace=0)
		#
		plot_index = 0
		for integrand_id in predict_id_list :
			integrand_name = integrand_table[integrand_id]['integrand_name']
			#
			# Last plot at the bottom of the figure has its x tick labels
			plot_index += 1
			sp = pyplot.subplot(n_predict_integrand, 1, plot_index)
			if plot_index < n_predict_integrand :
				sp.set_xticklabels( [] )
			y  = avg_integrand_list[integrand_id]
			x  = age_list
			pyplot.scatter(x, y, marker='.', color='k', s=point_size )
			pyplot.yscale('log')
			pyplot.ylabel( integrand_name )
		pyplot.xlabel('age')
		covariate_name = \
			integrand_table[covariate_integrand_id]['integrand_name']
		pyplot.suptitle('Covariate Integrand = ' + covariate_name )
		#
		pdf.savefig( fig )
		pyplot.close( fig )
	assert predict_id == len(predict_table)
	#
	pdf.close()
# =============================================================================
# Routines that Change Data Table
# =============================================================================
# -----------------------------------------------------------------------------
# subset_data:
def subset_data () :
	# remove datat table rows that are held out or have covariates
	# that are out of bounds
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
# -----------------------------------------------------------------------------
def subsample_data (integrand_name, max_sample) :
	# for a specified integrand, sample at most max_sample entries.
	# This does random sampling that can be seeded by calling random.seed.
	# The origianl order of the data is preserved (in index plots)
	# by sorting the subsample.
	#
	integrand_id = table_name2id(integrand_table, 'integrand', integrand_name)
	#
	table_name = 'data'
	(table_in, col_name, col_type) = get_table(table_name)
	#
	# indices for this integrand
	count_list = list()
	count      = 0
	for row in table_in :
		if row['integrand_id'] == integrand_id :
			count_list.append(count)
			count += 1
	n_sample_in = count
	#
	# subsample of indices for this integrand
	n_sample_out = min(max_sample, n_sample_in)
	if n_sample_out < n_sample_in :
		count_list = random.sample(count_list,  n_sample_out)
		count_list = sorted( count_list )
	#
	# subsample the integrand
	index  = 0
	count  = 0
	table_out = list()
	for row in table_in :
		if row['integrand_id'] != integrand_id :
			table_out.append(row)
		else :
			if index < n_sample_out :
				if count_list[index] == count :
					table_out.append(row)
					index += 1
			count += 1
	assert index == n_sample_out
	assert count == n_sample_in
	#
	put_table(table_name, table_out, col_name, col_type)
	#
	msg = "subsample_data {} sample in = {} out = {}"
	print( msg.format(integrand_name, n_sample_in, n_sample_out) )
# -----------------------------------------------------------------------------
def hold_out_data (integrand_name=None, node_name= None, hold_out=None) :
	# for a specified integrand or node, set the hold_out to 0 or 1
	msg = 'integrand={}, node={}, hold_out={}'
	print( msg.format(integrand_name, node_name, hold_out) )
	if integrand_name is None and node_name is None :
		sys.exit('hold_out_data: both integrand_name and node_name are None')
	if integrand_name is not None and node_name is not None :
		sys.exit('hold_out_data: both integrand_name and node_name not None')
	#
	if hold_out not in [ 0, 1] :
		msg = 'hold_out_data: hold_out is not zero or one'
		sys.exit(msg)
	if integrand_name is not None :
		node_id      = None
		integrand_id = table_name2id(
			integrand_table, 'integrand', integrand_name
		)
	else :
		node_name is not None
		integrand_id = None
		node_id      = table_name2id(
			node_table, 'node', node_name
		)
	#
	#
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		if integrand_id == row['integrand_id'] or node_id == row['node_id'] :
			row['hold_out'] = hold_out
	put_table(table_name, table, col_name, col_type)
# -----------------------------------------------------------------------------
def set_data_density (integrand_name, density_name) :
	# for a specified integrand, set its data density to a specified value
	msg = 'set_data_density {} {}'.format(integrand_name, density_name)
	#
	# integrand_id
	integrand_id = table_name2id(integrand_table, 'integrand', integrand_name)
	#
	# density_id
	density_id = table_name2id(density_table, 'density', density_name)
	#
	# table
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	#
	for row in table :
		if row['integrand_id'] == integrand_id :
			row['density_id'] = density_id
	#
	put_table(table_name, table, col_name, col_type)
# -----------------------------------------------------------------------------
def set_minimum_meas_std (integrand_name, median_meas_value_cv) :
	# Set the minimum measurement standard deviation or an integrand using
	# median_meas_value_cv, a multiplier for the median value for the integrand.
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
	#
	put_table(table_name, table, col_name, col_type)
# ============================================================================
# Routines that Change Other Tables
# ============================================================================
# -----------------------------------------------------------------------------
def remove_rate (rate_name) :
	# remove both the parent and child variables for a rate
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
# -----------------------------------------------------------------------------
def set_covariate_reference (covariate_id, reference_name) :
	# set the reference value for a specified covariate
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
# -----------------------------------------------------------------------------
def set_mulcov_zero (covariate_id, restore= None) :
	# set all of the multipliers for a specified covariate to zero without
	# changing the order or size of the var table
	if restore is None :
		print( 'set_mulcov_zero x_{}'.format(covariate_id) )
	else :
		print( 'restore mulcov x_{}'.format(covariate_id) )
	#
	table_name = 'mulcov'
	(mulcov_table, mulcov_col_name, mulcov_col_type) = get_table(table_name)
	#
	table_name = 'smooth'
	(smooth_table, smooth_col_name, smooth_col_type) = get_table(table_name)
	#
	table_name = 'smooth_grid'
	(grid_table, grid_col_name, grid_col_type) = get_table(table_name)
	# -------------------------------------------------------------------------
	if restore is not None :
		for (mulcov_id, group_smooth_id, subgroup_smooth_id) in restore :
			row = mulcov_table[ mulcov_id ]
			assert row['covariate_id'] == covariate_id
			row['group_smooth_id']     = group_smooth_id
			row['subgroup_smooth_id']  = subgroup_smooth_id
		#
		put_table('mulcov',  mulcov_table, mulcov_col_name, mulcov_col_type)
		return None
	# -------------------------------------------------------------------------
	restore = list()
	for (mulcov_id, row)  in  enumerate( mulcov_table ) :
		if row['covariate_id'] == covariate_id :
			group_smooth_id           = row['group_smooth_id']
			row['group_smooth_id']    = new_zero_smooth_id(
				group_smooth_id, smooth_table, grid_table
			)
			subgroup_smooth_id        = row['subgroup_smooth_id']
			row['subgroup_smooth_id'] = new_zero_smooth_id(
				subgroup_smooth_id, smooth_table, grid_table
			)
			restore.append( (mulcov_id, group_smooth_id, subgroup_smooth_id) )
	#
	put_table('mulcov',      mulcov_table, mulcov_col_name, mulcov_col_type)
	put_table('smooth',      smooth_table, smooth_col_name, smooth_col_type)
	put_table('smooth_grid', grid_table,   grid_col_name,   grid_col_type)
	return restore
# -----------------------------------------------------------------------------
def constant_rate (rate_name, prior) :
	# Set a rate to be constant in age and time specified prior
	print( 'constant_rate {}'.format(rate_name) )
	#
	# add row to prior_table
	table_name = 'prior'
	(table, col_name, col_type) = get_table(table_name)
	new_prior_id = len(table)
	table.append( copy.copy( prior ) )
	put_table(table_name, table, col_name, col_type)
	#
	# add row to smooth_table
	table_name = 'smooth'
	(table, col_name, col_type) = get_table(table_name)
	new_smooth_id  = len( table )
	smooth_name    = prior['prior_name'] + '_smooth'
	row =  {
		'smooth_name'           : smooth_name   ,
		'n_age'                 : 1             ,
		'n_time'                : 1             ,
		'mulstd_value_prior_id' : None          ,
		'mulstd_dage_prior_id'  : None          ,
		'mulstd_dtime_prior_id' : None          ,
	}
	table.append(row)
	put_table(table_name, table, col_name, col_type)
	#
	# add row to smooth_grid_table
	table_name = 'smooth_grid'
	(table, col_name, col_type) = get_table(table_name)
	row = {
		'smooth_id'      : new_smooth_id ,
		'age_id'         : 0             ,
		'time_id'        : 0             ,
		'value_prior_id' : new_prior_id  ,
		'dage_prior_id'  : None          ,
		'dtime_prior_id' : None          ,
		'const_value'    : None          ,
	}
	table.append(row)
	put_table(table_name, table, col_name, col_type)
	#
	# change rate_table
	table_name = 'rate'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		if row['rate_name'] == rate_name :
			row['parent_smooth_id'] = new_smooth_id
			row['child_smooth_id']  = None
			row['child_nslist_id']  = None
	put_table(table_name, table, col_name, col_type)
# -----------------------------------------------------------------------------
def set_option (name, value) :
	# set a specified option table name to a specified option table value
	system_command( [
		'dismod_at',  database, 'set', 'option', name , value
	] )
# -----------------------------------------------------------------------------
def set_minimum_meas_cv (integrand_name, minimum_meas_cv) :
	# set the minimum cv for a specified integrand
	print( 'set {} minimum_meas_cv {}'.format(integrand_name, minimum_meas_cv) )
	#
	table_name = 'integrand'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		if row['integrand_name'] == integrand_name :
			row['minimum_meas_cv'] = minimum_meas_cv
	put_table(table_name, table, col_name, col_type)

# ==========================================================================
# Example Changes
# ==========================================================================
#
# set_option:
# set_option('tolerance_fixed',    '1e-6')
# set_option('max_num_iter_fixed', '30')
#
# set reference value for covariates to median
# (must do this before subset_data)
# covariate_id    = 0
# reference_name  = 'median'
# set_covariate_reference(covariate_id, reference_name)
#
# subset_data:
# subset_data()
#
# subsample all data (for speed of testing)
# max_sample = 100
# for integrand_name in integrand_list :
#	subsample_data(integrand_name, max_sample)
# constrain all x_0 covariate multipliers to be zero
# covariate_id = 0
# restore_mulcov_x_0 = set_mulcov_zero(covariate_id)
#
# remove_rate:
# for rate_name in [ 'omega', 'chi' ] :
#	remove_rate(rate_name)
#
# constant_rate:
# table_name   = 'density'
# density_name = 'log_gaussian'
# density_id   = table_name2id(density_table, table_name, density_name)
# prior = {
#	'prior_name' : 'constant_pini' ,
#	'density_id' : density_id      ,
#	'lower'      : 0               ,
#	'upper'      : 1e-5            ,
#	'mean'       : 0               ,
#	'std'        : 1e-10           ,
#	'eta'        : 1e-10           ,
#	'nu'         : None            ,
#}
#rate_name  = 'pini'
#constant_rate(rate_name, prior)
#
# set_data_density:
# density_name = 'gaussian'
# for integrand_name in [ 'Sincidence', 'prevalence' ] :
#	set_data_density(integrand_name, density_name)
#
# set the minimum measurement standard deviation and cv
# median_meas_value_cv = 1e-2
# minimum_meas_cv      = 1e-1
# for integrand_name in [ 'Sincidence', 'prevalence' ] :
#	set_minimum_meas_std(integrand_name, median_meas_value_cv)
#	set_minimum_meas_cv(integrand_name, minimum_meas_cv)
#
# hold out Korea before subset_data() do it gets removed
#	hold_out_data(node_name = 'Republic of Korea', hold_out = 1)
#	hold_out_data(node_name = 'Japan', hold_out = 1)
# ----------------------------------------------------------------------
# Actual Changes
# ----------------------------------------------------------------------
#
if not new_database :
	# list of integrands in database
	integrand_list_yes_ode = get_integrand_list(True)
	integrand_list_no_ode  = get_integrand_list(False)
	integrand_list_all     = integrand_list_yes_ode + integrand_list_no_ode
if new_database :
	# seed used by subsample_data
	random_seed = int( time.time() )
	random.seed(random_seed)
	#
	# remove all hold out data and data past covariate limits
	subset_data()
	#
	# subsetting the data can remove some integrands
	integrand_list_yes_ode = get_integrand_list(True)
	integrand_list_no_ode  = get_integrand_list(False)
	integrand_list_all     = integrand_list_yes_ode + integrand_list_no_ode
	#
	# set options
	set_option('tolerance_fixed',    '1e-6')
	set_option('max_num_iter_fixed', '100')
	set_option('zero_sum_child_rate', 'iota chi')
	set_option('bound_random',        '3')
	#
	# subsample data for speed of testing
	integrand_count = get_integrand_count()
	#
	integrand_name = 'mtexcess'
	max_sample     = int( integrand_count[integrand_name] / 1 )
	subsample_data(integrand_name, max_sample)
	#
	integrand_name = 'Sincidence'
	max_sample     = int( integrand_count[integrand_name] / 1 )
	subsample_data(integrand_name, max_sample)
	#
	integrand_name = 'prevalence'
	max_sample     = int( integrand_count[integrand_name] / 1 )
	subsample_data(integrand_name, max_sample)
	#
	# set the minimum measurement standard deviation and cv
	median_meas_value_cv = 1e-2
	minimum_meas_cv      = 1e-1
	for integrand_name in integrand_list_all :
		set_minimum_meas_std(integrand_name, median_meas_value_cv)
		set_minimum_meas_cv(integrand_name, minimum_meas_cv)
	#
	# constrain all x_0 covariate multipliers to be zero
	covariate_id = 0
	restore_mulcov_x_0 = set_mulcov_zero(covariate_id)
	#
	# take ode integrands out of fit
	for integrand_name in integrand_list_yes_ode :
		hold_out_data(integrand_name = integrand_name, hold_out = 1)
	#
	# init
	system_command([ 'dismod_at', database, 'init'])
	#
	# fit both
	system_command([ 'dismod_at', database, 'fit', 'both'])
	#
	if fit_ode :
		# save fit_var table because we will re-run init
		table_name = 'fit_var'
		(fit_var_table, col_name, col_type) = get_table(table_name)
		#
		# put ode integrands data back in the fit
		for integrand_name in integrand_list_yes_ode :
			hold_out_data(integrand_name = integrand_name, hold_out = 0)
		#
		# remove constraint on x_0 covariate multipliers
		covariate_id = 0
		set_mulcov_zero(covariate_id, restore_mulcov_x_0)
		#
		# re-run init because set_mul_cov_zero is lazy and does not make
		# the necessary changes to smooth_id in var table
		system_command([ 'dismod_at', database, 'init'])
		#
		# set_start_var equal to fit_var from previous fit
		table_name = 'start_var'
		(start_var_table, col_name, col_type) = get_table(table_name)
		for (var_id, row) in enumerate(start_var_table) :
			row['start_var_value'] = fit_var_table[var_id]['fit_var_value']
		put_table(table_name, start_var_table, col_name, col_type)
		#
		# fit both
		system_command([ 'dismod_at', database, 'fit', 'both'])
#
# plot rate
for rate_name in [ 'iota', 'chi' ] :
	plot_rate(rate_name)
#
# plot data
for integrand_name in integrand_list_all :
	plot_data(integrand_name)
#
# plot prediction
covariate_integrand_name = 'prevalence'
predict_integrand_list   = [ 'susceptible', 'withC' ]
covariate_integrand_list = integrand_list_yes_ode
plot_predict(covariate_integrand_list, predict_integrand_list)
#
# db2cvs
system_command( [ 'dismodat.py',  database, 'db2csv' ] )
# ----------------------------------------------------------------------
print('integrands  = ', integrand_list_all )
if new_database :
	print('random_seed = ', random_seed )
print('db_simplify.py: OK')
sys.exit(0)
