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
original_database  = 'ihme_db/data/475533/dbs/1/2/dismod.db'
# path to file that contains the simplified database
database           = 'ihme_db/temp.db'
# create new simplified database including fit results (otherwise just plot)
new_database       = True
# If new_database is true, run fit both first without and then with ode data.
fit_ode            = True
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
#
# directory where plots are stored
index = database.rfind('/')
if index < 0 :
	plot_directory = '.'
else :
	plot_directory = database[0 : index]
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
# Utilities that depend on data tables
# ============================================================================
# ----------------------------------------------------------------------------
def get_integrand_list() :
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	integrand_set = set()
	for row in table :
		integrand_set.add( row['integrand_id'] )
	integrand_list = list()
	for integrand_id in integrand_set :
		integrand_name = integrand_table[integrand_id]['integrand_name']
		integrand_list.append( integrand_name )
	return integrand_list
# ----------------------------------------------------------------------------
# plot_rate
def plot_rate(rate_name) :
	# rate_table
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
	# creaate the mesghgird
	x = numpy.zeros( (n_age, n_time), dtype = float)
	y = numpy.zeros( (n_age, n_time), dtype = float)
	z = numpy.zeros( (n_age, n_time), dtype = float)
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
			x[i, j] = age_table[age_id]['age']
			y[i, j] = time_table[time_id]['time']
			z[i, j] = math.log10( fit_var_table[var_id]['fit_var_value'] )
	#
	#
	from matplotlib import pyplot
	fig = pyplot.figure()
	ax = fig.gca(projection='3d')
	#
	# x is age, y is time, z is the rate
	ax.plot_wireframe(x, y, z)
	#
	# axis labels
	ax.set_xlabel( 'age' )
	ax.yaxis.labelpad=10
	ax.set_ylabel( 'year' )
	ax.zaxis.labelpad=10
	ax.set_zlabel( 'log10 ' + rate_name )
	#
	# set z limits
	z_mean = numpy.mean(z)
	z_max  = round( z_mean + 2 )
	z_min  = round( z_mean - 2 )
	ax.axes.set_zlim3d(bottom=z_min, top=z_max)
	#
	import matplotlib.backends.backend_pdf
	file_name = plot_directory + '/' + rate_name + '.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	pdf.savefig( fig )
	pdf.close()
# ----------------------------------------------------------------------------
# plot_data
def plot_data(integrand_name) :
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
		pyplot.subplot(3, 1, 1)
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
		pyplot.subplot(3, 1, 2)
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
		pdf.savefig( fig )
	#
	pdf.close()
# ----------------------------------------------------------------------------
# plot_predict
def plot_predict(covariate_integrand_name, predict_integrand_list) :
	#-----------------------------------------------------------------------
	# create avgint table
	# For row in data table corresponding to covariate_integrand_name
	#	For each integrand in the predict_integrand_list
	#		write a row in avgint table with covariates and integrand
	#-----------------------------------------------------------------------
	# covariate_integrand_id
	table_name   = 'integrand'
	covariate_integrand_id = table_name2id(
		integrand_table, table_name, covariate_integrand_name
	)
	# integrand_id_list
	integrand_id_list = list()
	for integrand_name in predict_integrand_list :
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
		if covariate_integrand_id == row_in['integrand_id'] :
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
	# ------------------------------------------------------------------------
	# Predict for this avgint table
	system_command([ 'dismod_at', database, 'predict', 'fit_var' ])
	#
	table_name = 'predict'
	(predict_table, col_name, col_type) = get_table(table_name)
	# ------------------------------------------------------------------------
	n_predict          = len( predict_table )
	n_integrand        = len( predict_integrand_list )
	n_per_integrand    = int( n_predict / n_integrand )
	#
	integrand_id_info = dict()
	for (predict_id, predict_row) in enumerate(predict_table) :
		avg_integrand = predict_row['avg_integrand']
		#
		assert predict_id == predict_row['avgint_id']
		row = avgint_table[predict_id]
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
	point_size  =  n_per_integrand * [ 1 ]
	#
	from matplotlib import pyplot
	import matplotlib.backends.backend_pdf
	file_name = plot_directory + '/predict.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	for x_name in [ 'index', 'node', 'age', 'time' ] :
		#
		fig, axes = pyplot.subplots(n_integrand, 1, sharex=True)
		fig.subplots_adjust(hspace=0)
		#
		plot_index = 0
		for integrand_name in predict_integrand_list :
			table_name = 'integrand'
			integrand_id = table_name2id(
				integrand_table, table_name, integrand_name
			)
			#
			plot_index += 1
			pyplot.subplot(n_integrand, 1, plot_index)
			info = integrand_id_info[integrand_id]
			integrand_name = integrand_table[integrand_id]['integrand_name']
			#
			y  = info['avg_integrand']
			x  = info[x_name]
			pyplot.scatter(x, y, marker='.', color='k', s=point_size )
			pyplot.yscale('log')
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
# This does random sampling that can be seeded by calling random.seed.
# The origianl order of the data is preserved by sorting the subsample.
def subsample_data(integrand_name, stride) :
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
	if stride == 1 :
		n_sample_out = n_sample_in
	else :
		n_sample_out = int( n_sample_in / stride + 1.0 )
		count_list = random.sample(count_list,  n_sample_out)
		count_list = sorted( count_list )
	#
	# sample the integrand
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
	msg = "subsample_data {} stride = {} n_sample in = {} out = {}"
	print( msg.format(integrand_name, stride, n_sample_in, n_sample_out) )
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
# for a specified integrand, set its data density to a specified value
def set_data_density(integrand_name, density_name) :
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
#
# set_minimum_meas_std:
# Set the minimum measurement standard deviation or an integrand using
# median_meas_value_cv, a multiplier for the median value for the integrand.
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
	#
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
def set_mulcov_zero(covariate_id, restore= None) :
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
	# -------------------------------------------------------------------------
	restore = list()
	for (mulcov_id, row)  in  enumerate( mulcov_table ) :
		if row['covariate_id'] == covariate_id :
			group_smooth_id           = row['group_smooth_id']
			row['group_smooth_id']    = new_zero_smooth_id(group_smooth_id)
			subgroup_smooth_id        = row['subgroup_smooth_id']
			row['subgroup_smooth_id'] = new_zero_smooth_id(subgroup_smooth_id)
			restore.append( (mulcov_id, group_smooth_id, subgroup_smooth_id) )
	#
	put_table('mulcov',      mulcov_table, mulcov_col_name, mulcov_col_type)
	put_table('smooth',      smooth_table, smooth_col_name, smooth_col_type)
	put_table('smooth_grid', grid_table,   grid_col_name,   grid_col_type)
	return restore
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
# subsample mtexcess (because there is way more than other data)
# stride         = 10
# integrand_name = 'mtexcess'
# subsample_data(integrand_name, stride)
#
# now further subsample all data (for speed of testing)
# stride = 5
# for integrand_name in integrand_list :
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
# constrain all x_0 covariate multipliers to be zero
# covariate_id = 0
# restore_mulcov_x_0 = set_mulcov_zero(covariate_id)
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
# set the minimum measurement standard deviation and cv
# median_meas_value_cv = 1e-2
# minimum_meas_cv      = 1e-1
# for integrand_name in [ 'Sincidence', 'prevalence' ] :
#	set_minimum_meas_std(integrand_name, median_meas_value_cv)
#	set_minimum_meas_cv(integrand_name, minimum_meas_cv)
#
# hold_out_data:
# hold_out       = 1
# integrand_name = 'prevalence'
# hold_out_data(integrand_name, hold_out)
#
# ----------------------------------------------------------------------
# Actual Changes
# ----------------------------------------------------------------------
# list of integrand that require ode
ode_integrand_list = [
	'prevalence', 'Tincidence', 'mtspecific', 'mtall', 'mtstandard'
]
# list of integrands that are present in this database
integrand_list = get_integrand_list()
#
# seed used by subsample_data (None means use system clock)
seed  = None
random.seed(seed)
if new_database :
	#
	# remove all hold out data and data past covariate limits
	subset_data()
	integrand_list = get_integrand_list()
	#
	# set options
	set_option('tolerance_fixed',    '1e-6')
	set_option('max_num_iter_fixed', '100')
	set_option('zero_sum_child_rate', 'iota chi')
	#
	# subsample mtexcess (because there is way more than other data)
	if 'mtexcess' in integrand_list :
		stride         = 10
		integrand_name = 'mtexcess'
		subsample_data(integrand_name, stride)
	#
	# now further subsample all data (for speed of testing)
	stride = 20
	for integrand_name in integrand_list :
		subsample_data(integrand_name, stride)
	#
	# set the minimum measurement standard deviation and cv
	median_meas_value_cv = 1e-2
	minimum_meas_cv      = 1e-1
	for integrand_name in integrand_list :
		set_minimum_meas_std(integrand_name, median_meas_value_cv)
		set_minimum_meas_cv(integrand_name, minimum_meas_cv)
	#
	# constrain all x_0 covariate multipliers to be zero
	covariate_id = 0
	restore_mulcov_x_0 = set_mulcov_zero(covariate_id)
	#
	# take ode integrands out of fit
	for integrand_name in integrand_list :
		if integrand_name in ode_integrand_list :
			hold_out       = 1
			hold_out_data(integrand_name, hold_out)
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
		hold_out       = 0
		for integrand_name in integrand_list :
			if integrand_name in ode_integrand_list :
				hold_out_data(integrand_name, hold_out)
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
	# predict fit_var (for plot_predict)
	system_command([ 'dismod_at', database, 'predict', 'fit_var' ])
#
# plot rate
for rate_name in [ 'iota', 'chi' ] :
	plot_rate(rate_name)
#
# plot data
for integrand_name in integrand_list :
	plot_data(integrand_name)
#
# plot prediction
covariate_integrand_name = 'prevalence'
predict_integrand_list   = [ 'susceptible', 'withC', 'mtother' ]
plot_predict(covariate_integrand_name, predict_integrand_list)
#
# db2cvs
system_command( [ 'dismodat.py',  database, 'db2csv' ] )
print('integrand_list = ', integrand_list)
# ----------------------------------------------------------------------
print('db_simplify.py: OK')
sys.exit(0)
