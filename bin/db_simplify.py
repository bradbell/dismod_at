#! /bin/python3
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
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
#
original_database  = 'ihme_db/data/475588/dbs/100/3/dismod.db'
# path to file that contains the simplified database
database           = 'ihme_db/temp.db'
# create new database including
new_database       = True
# fit without integrands that require the ode (new_database must be true)
fit_without_ode    = False
# fit with integrands that require the ode (fit_without_ode must be true)
fit_with_ode       = False
# Re-fit  with data density replaced by Students-t (fit_with_ode must be true)
fit_students       = False
# random seed to use when subseting data, if 0 use the clock choose seed
random_seed        = 1610591440
# print the help message for all the db_simplify routines and then exit
print_help         = False
# ----------------------------------------------------------------------
if not new_database :
	assert not fit_without_ode
if not fit_without_ode :
	assert not fit_with_ode
if not fit_with_ode :
	assert not fit_students
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
# ----------------------------------------------------------------------------
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
def sql_command(command) :
	# execute an sql command on this database
	print(command)
	new          = False
	connection   = dismod_at.create_connection(database, new)
	cursor       = connection.cursor()
	cursor.execute(command)
	connection.commit()
	result       = cursor.fetchall()
	connection.close()
	return result
# ----------------------------------------------------------------------------
def check_for_table(table_name) :
	command  = 'SELECT name FROM sqlite_master WHERE '
	command += "type = 'table' and name = '{}'".format(table_name)
	result   = sql_command(command)
	return len(result) > 0
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
def table_name2id(table, table_name) :
	# Return dictionary that maps a value in the name column to corresponding
	# row index; i.e. table id value.
	result = dict()
	name_column = table_name + '_name'
	for (row_id, row) in enumerate(table) :
		result[ row[name_column] ] = row_id
	return result
# ============================================================================
# Tables that do not change
# ============================================================================
#
# integrand_table, integrand_name2id
table_name = 'integrand'
(integrand_table, integrand_col_name, col_type) = get_table(table_name)
integrand_name2id = table_name2id(integrand_table, table_name)
#
# density_table, density_name2id
table_name = 'density'
(density_table, col_name, col_type) = get_table(table_name)
density_name2id = table_name2id(density_table, table_name)
#
# node_table, node_name2id
table_name = 'node'
(node_table, col_name, col_type) = get_table(table_name)
node_name2id = table_name2id(node_table, table_name)
#
# option_table
table_name = 'option'
(option_table, col_name, col_type) = get_table(table_name)
#
# rate_name2id
table_name = 'rate'
(table, col_name, col_type) = get_table(table_name)
rate_name2id = table_name2id(table, table_name)
# ============================================================================
# Tables that change
# ============================================================================
#
# age_table
table_name = 'age'
(age_table, age_col_name, age_col_type) = get_table(table_name)
#
# time_table
table_name = 'time'
(time_table, time_col_name, time_col_type) = get_table(table_name)
#
# prior_table
table_name = 'prior'
(prior_table, prior_col_name, prior_col_type) = get_table(table_name)
#
# smooth_table
table_name = 'smooth'
(smooth_table, smooth_col_name, smooth_col_type) = get_table(table_name)
#
# smooth_grid_table
table_name = 'smooth_grid'
(smooth_grid_table, smooth_grid_col_name, smooth_grid_col_type) =  \
	get_table(table_name)
#
# ============================================================================
# Utilities that depend on data table or fit results
# ============================================================================
# ----------------------------------------------------------------------------
def new_smoothing(age_grid, time_grid, value_prior, dage_prior, dtime_prior):
	# Add a new smoothing that has one prior that is used for all age and
	# time grid points. The smooth, smooth_grid, age, and time tables are
	# modified, but the new versions are not written by this routine.
	# The arguments value_prior, dage_prior, dtime_prior,
	# contain the priors used in the smmothing.
	#
	def table_value2id(table, col_name, value_list) :
		result = list()
		for value in value_list :
			match = None
			for (table_id, row) in enumerate(table) :
				if row[col_name] == value :
					match = table_id
			if match is None :
				match = len(table)
				row   = copy.copy( row )
				row[col_name] = value
				table.append(row)
			result.append(match)
		return result
	n_age          = len(age_grid)
	n_time         = len(time_grid)
	age_id_list    = table_value2id(age_table,  'age',  age_grid)
	time_id_list   = table_value2id(time_table, 'time', time_grid)
	new_smooth_id  = len( smooth_table )
	#
	# add value_prior to prior_table
	new_value_prior_id  = len(prior_table)
	prior_table.append( copy.copy( value_prior ) )
	#
	# add dage_prior to prior table
	new_dage_prior_id  = len(prior_table)
	prior_table.append( copy.copy( dage_prior   ) )
	#
	# add dtime_prior to prior table
	new_dtime_prior_id  = len(prior_table)
	prior_table.append( copy.copy( dtime_prior   ) )
	#
	# add row to smooth_table
	smooth_name    = 'smoothing_{}_dtime_prior'.format(new_smooth_id)
	row =  {
		'smooth_name'           : smooth_name    ,
		'n_age'                 : n_age          ,
		'n_time'                : n_time         ,
		'mulstd_value_prior_id' : None           ,
		'mulstd_dage_prior_id'  : None           ,
		'mulstd_dtime_prior_id' : None           ,
	}
	smooth_table.append(row)
	#
	# add rows to smooth_grid_table
	for i in range(n_age) :
		for j in range(n_time) :
			row = {
				'smooth_id'      : new_smooth_id                   ,
				'age_id'         : age_id_list[i]                  ,
				'time_id'        : time_id_list[j]                 ,
				'value_prior_id' : new_value_prior_id              ,
				'dage_prior_id'  : new_dage_prior_id               ,
				'dtime_prior_id' : new_dtime_prior_id              ,
				'const_value'    : None                            ,
			}
			smooth_grid_table.append(row)
	#
	# return the new smoothing
	return new_smooth_id
# ----------------------------------------------------------------------------
def new_zero_smooth_id (smooth_id) :
	# add a new smoothing that has the same grid as smooth_id smoothing
	# and that constrains to zero. Tables in argument list are both
	# inputs and outputs.
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
def get_integrand_data () :
	# return a dictionary that contains the data for each integrand name
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	#
	integrand_data = dict()
	for row in table :
		integrand_id   = row['integrand_id']
		integrand_name = integrand_table[integrand_id]['integrand_name']
		if integrand_name not in integrand_data :
			integrand_data[integrand_name] = list()
		integrand_data[integrand_name].append( row['meas_value'] )
	#
	return integrand_data
# ----------------------------------------------------------------------------
def plot_rate (rate_name) :
	color_style_list = [
		('blue',       'dashed'),  ('lightblue',  'solid'),
		('red',        'dashed'),  ('pink',       'solid'),
		('green',      'dashed'),  ('lightgreen', 'solid'),
		('black',      'dashed'),  ('gray',       'solid'),
		('brown',      'dashed'),  ('sandybrown', 'solid'),
		('darkorange', 'dashed'),  ('gold',       'solid'),
		('purple',     'dashed'),  ('violet',     'solid'),
	]
	n_color_style = len( color_style_list )
	#
	# plot the fit_var grid values for a specified rate.
	table_name = 'rate'
	(rate_table, col_name, col_type) = get_table(table_name)
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
	rate_id = rate_name2id[rate_name]
	#
	# parent_node_id
	parent_node_id = None
	for row in option_table :
		if row['option_name'] == 'parent_node_id' :
			parent_node_id = row['option_value']
		if row['option_name'] == 'parent_node_name' :
			table_name = 'node'
			node_name  = row['option_value']
			parent_note_id = node_name2id[node_name]
	if parent_node_id is None :
		msg = 'Cannot find parent_node_id or parent_node_name in option table'
		sys.exit(msg)
	parent_node_id = int( parent_node_id )
	#
	# age and time limits in plots
	age_min = min( [ row['age'] for row in age_table ] )
	age_max = max( [ row['age'] for row in age_table ] )
	#
	time_min = min( [ row['time'] for row in time_table ] )
	time_max = max( [ row['time'] for row in time_table ] )
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
	if smooth_id == None :
		print('plot_rate: ' + rate_name + ' is identically zero')
		return
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
	rate_min = numpy.min(rate) * 0.95
	rate_max = numpy.max(rate) * 1.05
	#
	from matplotlib import pyplot
	#
	import matplotlib.backends.backend_pdf
	file_name = plot_directory + '/' + rate_name + '.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	# for each time, plot rate as a function of age
	if n_age > 1 :
		n_fig       = math.ceil( n_time / ( n_color_style - 1) )
		n_per_fig   = math.ceil( n_time / n_fig )
		color_index = -1
		assert n_per_fig < n_color_style
		for i_fig in range( n_fig ) :
			fig    = pyplot.figure()
			axis   = pyplot.subplot(1,1,1)
			start  = i_fig * n_per_fig
			if i_fig > 0 :
				start        = start - 1
				color_index -= 1
			stop   = min(n_time, start + n_per_fig )
			for j in range(start, stop) :
				color_index    = (color_index + 1) % n_color_style
				(color, style,) = color_style_list[color_index]
				x     = age[:,j]
				y     = rate[:,j]
				x     = [age_min] + x.tolist() + [age_max]
				y     = [y[0]]    + y.tolist() + [y[-1]]
				label = str( time[0,j] )
				pyplot.plot(x, y, label=label, color=color, linestyle=style)
				#
				# axis labels
				pyplot.xlabel('age')
				pyplot.ylabel(rate_name)
				pyplot.yscale('log')
				pyplot.ylim(rate_min, rate_max)
			for i in range(n_age) :
				x = age[i, 0]
				pyplot.axvline(x, color='black', linestyle='dotted', alpha=0.3)
			# Shrink current axis by 15% and place legend to right
			box = axis.get_position()
			axis.set_position([box.x0, box.y0, box.width * 0.85, box.height])
			axis.legend(
				title = 'time', loc='center left', bbox_to_anchor=(1, 0.5)
			)
			pdf.savefig( fig )
			pyplot.close( fig )
	#
	# for each age, plot rate as a function of time
	if n_time > 1 :
		n_fig       = math.ceil( n_age / (n_color_style - 1) )
		n_per_fig   = math.ceil( n_age / n_fig )
		color_index = -1
		assert n_per_fig < n_color_style
		for i_fig in range( n_fig ) :
			fig    = pyplot.figure()
			axis   = pyplot.subplot(1,1,1)
			start  = i_fig * n_per_fig
			if i_fig > 0 :
				start        = start - 1
				color_index -= 1
			stop   = min(n_age, start + n_per_fig )
			for i in range(start, stop) :
				color_index    = (color_index + 1) % n_color_style
				(color, style) = color_style_list[color_index]
				x              = time[i,:]
				y              = rate[i,:]
				x              = [time_min] + x.tolist() + [time_max]
				y              = [y[0]]     + y.tolist() + [y[-1]]
				label          = str( age[i,0] )
				pyplot.plot(x, y, label=label, color=color, linestyle=style)
				#
				# axis labels
				pyplot.xlabel('time')
				pyplot.ylabel(rate_name)
				pyplot.yscale('log')
				pyplot.ylim(rate_min, rate_max)
			for j in range(n_time) :
				x = time[0, j]
				pyplot.axvline(x, color='black', linestyle='dotted', alpha=0.3)
			# Shrink current axis by 15% and place legend to right
			box = axis.get_position()
			axis.set_position([box.x0, box.y0, box.width * 0.85, box.height])
			axis.legend(
				title = 'age', loc='center left', bbox_to_anchor=(1, 0.5)
			)
			pdf.savefig( fig )
			pyplot.close( fig )
	#
	pdf.close()
# ----------------------------------------------------------------------------
def plot_integrand (integrand_name) :
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
	this_integrand_id = integrand_name2id[integrand_name]
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
	if n_list < 2 :
		print('plot_integrand: ' + integrand_name + ' has less than 2 points')
		return
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
	r_norm      = numpy.linalg.norm( weighted_residual )
	r_avg_sq    = r_norm * r_norm / len( weighted_residual )
	r_max       = 4.0 * numpy.sqrt( r_avg_sq )
	r_min       = - r_max
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
	y_limit = [ 0.9 * y_min, 1.1 * y_max ]
	r_limit = [ 1.1 * r_min, 1.1 * r_max ]
	#
	point_size  = numpy.array( n_list * [ 1 ] )
	marker_size = numpy.array( n_list * [ 10 ] )
	#
	from matplotlib import pyplot
	import matplotlib.backends.backend_pdf
	file_name = plot_directory + '/' + integrand_name + '.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	for x_name in [ 'index', 'node', 'age', 'time' ] :
		x          = eval( x_name )
		#
		fig, axes = pyplot.subplots(3, 1, sharex=True)
		fig.subplots_adjust(hspace=0)
		#
		sp = pyplot.subplot(3, 1, 1)
		sp.set_xticklabels( [] )
		y =  meas_value
		pyplot.scatter(x, y, marker='.', color='black', s = point_size)
		pyplot.ylabel(integrand_name)
		pyplot.yscale('log')
		for limit in [ y_max, y_min ] :
			flag = y == limit
			size = marker_size[flag]
			pyplot.scatter(x[flag], y[flag], marker='+', color='red', s=size )
		pyplot.ylim(y_limit[0], y_limit[1])
		#
		sp = pyplot.subplot(3, 1, 2)
		sp.set_xticklabels( [] )
		y = avg_integrand
		pyplot.scatter(x, y, marker='.', color='black', s = point_size)
		pyplot.ylabel('model')
		pyplot.yscale('log')
		for limit in [ y_max, y_min ] :
			flag = y == limit
			size = marker_size[flag]
			pyplot.scatter(x[flag], y[flag], marker='+', color='red', s=size )
		pyplot.ylim(y_limit[0], y_limit[1])
		#
		# this plot at the bottom of the figure has its x tick labels
		pyplot.subplot(3, 1, 3)
		y = weighted_residual
		pyplot.scatter(x, y, marker='.', color='black', s = point_size)
		pyplot.ylabel('residual')
		for limit in [ r_max, r_min ] :
			flag = y == limit
			size = marker_size[flag]
			pyplot.scatter(x[flag], y[flag], marker='+', color='red', s=size )
		pyplot.ylim(r_limit[0], r_limit[1])
		y = 0.0
		pyplot.axhline(y, linestyle='solid', color='black', alpha=0.3 )
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
	covariate_id_list = list()
	for integrand_name in covariate_integrand_list :
		covariate_id = integrand_name2id[integrand_name]
		covariate_id_list.append( covariate_id )
	#
	# predict_id_list
	predict_id_list = list()
	for integrand_name in predict_integrand_list :
		predict_id = integrand_name2id[integrand_name]
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
			pyplot.scatter(x, y, marker='.', color='black', s=point_size )
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
	table_name = 'covariate'
	(covariate_table, col_name, col_type) = get_table(table_name)
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
	integrand_id =integrand_name2id[integrand_name]
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
		integrand_id = integrand_name2id[integrand_name]
	else :
		node_name is not None
		integrand_id = None
		node_id      = node_name2id[node_name]
	#
	#
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	for row in table :
		if integrand_id == row['integrand_id'] or node_id == row['node_id'] :
			row['hold_out'] = hold_out
	put_table(table_name, table, col_name, col_type)
# -----------------------------------------------------------------------------
def set_data_likelihood (integrand_name, density_name, eta, nu) :
	# For a specified integrand, set its density, eta, and nu
	msg = 'set_data_likelihood: for {} to {}, eta = {}, nu = {}'
	print( msg.format(integrand_name, density_name, eta, nu) )
	#
	# integrand_id
	integrand_id =integrand_name2id[integrand_name]
	#
	# density_id
	density_id = density_name2id[density_name]
	#
	# table
	table_name = 'data'
	(table, col_name, col_type) = get_table(table_name)
	#
	for row in table :
		if row['integrand_id'] == integrand_id :
			row['density_id'] = density_id
			row['eta']        = eta
			row['nu']        = nu
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
	integrand_id =integrand_name2id[integrand_name]
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
# -----------------------------------------------------------------------------
def identically_one_covariate () :
	# Return the covariate_id for a covariate that is one for every data point,
	# has refefence value is zero, and max_difference value is null.
	# (If no such covariate exists, one is created.)
	#
	table_name = 'data'
	(data_table, data_col_name, data_col_type) = get_table(table_name)
	#
	table_name = 'avgint'
	(avgint_table, avgint_col_name, avgint_col_type) = get_table(table_name)
	#
	table_name = 'covariate'
	(covariate_table, cov_col_name, cov_col_type) = get_table(table_name)
	n_covariate = len(covariate_table)
	#
	# is_one
	is_one = list()
	for row in covariate_table :
		reference      = row['reference']
		max_difference = row['max_difference']
		is_one.append( reference == 0.0 and max_difference is None)
	for row in data_table :
		for covariate_id in range(n_covariate) :
			key = 'x_' + str(covariate_id)
			is_one[covariate_id] &= row[key] == 1.0
	#
	# check for existing column in data
	for covariate_id in range(n_covariate) :
		if is_one[covariate_id] :
			return covariate_id
	#
	# add row to covariate table
	row = dict()
	for key in cov_col_name :
		row[key] = None
	row['covariate_name'] = 'ones'
	row['reference']      = 0.0
	#
	covariate_id = len(covariate_table)
	table_name    = 'covariate'
	covariate_table.append(row)
	put_table(table_name, covariate_table, cov_col_name, cov_col_type)
	#
	# add column to data_table
	key = 'x_' + str(covariate_id)
	data_col_name.append(key)
	data_col_type.append('real')
	for row in data_table :
		row[key] = 1.0
	table_name    = 'data'
	put_table(table_name, data_table, data_col_name, data_col_type)
	#
	# add same column to avgint table
	avgint_col_name.append(key)
	avgint_col_type.append('real')
	for row in avgint_table :
		row[key] = 1.0
	table_name    = 'avgint'
	put_table(table_name, avgint_table, avgint_col_name, avgint_col_type)
	#
	#
	return covariate_id
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
	# set the reference value for a specified covariate where reference_name
	# is 'mean' or 'median'
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
			covariate_value.append( row[key] )
	#
	if reference_name == 'median' :
		new_reference = numpy.median(covariate_value)
	elif reference_name == 'mean' :
		new_reference = numpy.mean(covariate_value)
	else :
		assert False
	#
	table_name  = 'covariate'
	(table, col_name, col_type) = get_table(table_name)
	#
	old_reference = table[covariate_id]['reference']
	table[covariate_id]['reference'] = new_reference
	#
	msg = 'set_covariate_reference for x_{} from {} to {}'
	print( msg.format(covariate_id, old_reference, new_reference) )
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
				group_smooth_id
			)
			subgroup_smooth_id        = row['subgroup_smooth_id']
			row['subgroup_smooth_id'] = new_zero_smooth_id(
				subgroup_smooth_id
			)
			restore.append( (mulcov_id, group_smooth_id, subgroup_smooth_id) )
	#
	put_table('mulcov',      mulcov_table, mulcov_col_name, mulcov_col_type)
	put_table('smooth',      smooth_table, smooth_col_name, smooth_col_type)
	put_table('smooth_grid',
		smooth_grid_table, smooth_grid_col_name, smooth_grid_col_type
	)
	return restore
# -----------------------------------------------------------------------------
def parent_rate_smoothing(
	rate_name, age_grid, time_grid, value_prior, dage_prior, dtime_prior
) :
	# Set a smoothing for a parent rate
	msg = 'parent_rate_smoothing: {}\nage_grid = {}\ntime_grid = {}'
	msg = msg.format(rate_name, age_grid, time_grid)
	print( msg )
	#
	# add the smothing
	smooth_id = new_smoothing(
		age_grid, time_grid, value_prior, dage_prior, dtime_prior
	)
	#
	# change rate_table
	table_name = 'rate'
	(rate_table, rate_col_name, rate_col_type) = get_table(table_name)
	for row in rate_table :
		if row['rate_name'] == rate_name :
			row['parent_smooth_id'] = smooth_id
	#
	# write out the tables that changed
	for row in prior_table :
		if row['prior_name'].startswith('smoothing_') :
			print( row['prior_name'] )
	put_table('age',         age_table,   age_col_name,   age_col_type)
	put_table('time',        time_table,  time_col_name,  time_col_type)
	put_table('prior',       prior_table, prior_col_name, prior_col_type)
	put_table('smooth',      smooth_table, smooth_col_name, smooth_col_type)
	put_table('rate',        rate_table,  rate_col_name, rate_col_type)
	put_table('smooth_grid',
		smooth_grid_table, smooth_grid_col_name, smooth_grid_col_type
	)
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
# -----------------------------------------------------------------------------
def add_meas_noise_mulcov(integrand_name, group_id, value, lower, upper) :
	# Add a meas_noise covariate multiplier with a specified integrand,
	# group_id, initial value, lower, and upper limit.
	# Note that meas_noise multipliers can't have
	# ramdom effect (so the subgroup id is null in the mulcov table).
	msg  = 'add_meas_noice_mulcov:'
	msg += 'for {} group {}: lower=(), upper={}, value={}'
	print( msg.format(integrand_name, group_id, lower, upper, value) )
	#
	table_name = 'mulcov'
	(mulcov_table, mulcov_col_name, mulcov_col_type) = get_table(table_name)
	#
	# integrand_id
	integrand_id = integrand_name2id[integrand_name]
	#
	# covariate_id
	covariate_id = identically_one_covariate()
	#
	# mulcov_id
	mulcov_id = len(mulcov_table)
	#
	# prior used in one point smoothing
	density_id = density_name2id['uniform']
	value_prior = {
		'prior_name' : integrand_name + '_meas_noise_value_prior'    ,
		'density_id' : density_id     ,
		'lower'      : lower          ,
		'upper'      : upper          ,
		'mean'       : value          ,
		'std'        : None           ,
		'eta'        : None           ,
		'nu'         : None           ,
	}
	dage_prior  = copy.copy( value_prior )
	dtime_prior = copy.copy( value_prior )
	dage_prior['prior_name']  =  integrand_name + '_meas_noise_dage_prior'
	dtime_prior['prior_name'] =  integrand_name + '_meas_noise_dtime_prior'
	#
	# new one point smoothing
	age_grid  = [ age_table[0]['age'] ]
	time_grid = [ time_table[0]['time'] ]
	smooth_id = new_smoothing(
		age_grid, time_grid, value_prior, dage_prior, dtime_prior
	)
	#
	# new row in mulcov_table
	row = dict()
	for col in mulcov_col_name :
		row[col] = None
	row['mulcov_type']      = 'meas_noise'
	row['covariate_id']     = covariate_id
	row['integrand_id']     = integrand_id
	row['group_id']         = group_id
	row['group_smooth_id']  = smooth_id
	mulcov_table.append( row )
	#
	# write out the tables that changed
	put_table('prior',       prior_table, prior_col_name, prior_col_type)
	put_table('smooth',      smooth_table, smooth_col_name, smooth_col_type)
	put_table('mulcov',      mulcov_table,  mulcov_col_name, mulcov_col_type)
	put_table('smooth_grid',
		smooth_grid_table, smooth_grid_col_name, smooth_grid_col_type
	)
# ==========================================================================
# Example Changes Note Currently Used
# ==========================================================================
#
# remove all rates except iota
# for rate_name in [ 'pini', 'rho', 'chi', 'omega' ] :
#	remove_rate(rate_name)
#
# set the minimum measurement standard deviation and cv
# median_meas_value_cv = 1e-2
# minimum_meas_cv      = 1e-1
# for integrand_name in integrand_list_all:
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
# start_time
start_time = time.time()
if not new_database :
	# list of integrands in database
	integrand_list_yes_ode = get_integrand_list(True)
	integrand_list_no_ode  = get_integrand_list(False)
	integrand_list_all     = integrand_list_yes_ode + integrand_list_no_ode
else :
	# seed used to subsample_data
	if random_seed == 0 :
		random_seed = int( time.time() )
	random.seed(random_seed)
	#
	# remove all hold out data and data past covariate limits
	subset_data()
	#
	# set reference value for x_0 to its median
	covariate_id    = 0
	reference_name  = 'median'
	set_covariate_reference(covariate_id, reference_name)
	#
	# subsetting the data can remove some integrands
	integrand_list_yes_ode = get_integrand_list(True)
	integrand_list_no_ode  = get_integrand_list(False)
	integrand_list_all     = integrand_list_yes_ode + integrand_list_no_ode
	#
	# subsample mtexcess
	for integrand_name in integrand_list_all :
		max_sample = 500
		subsample_data(integrand_name, max_sample)
	#
	# integrand_data
	integrand_data = get_integrand_data()
	# ------------------------------------------------------------------------
	#
	# set smoothing for pini
	rate_name    = 'pini'
	age_grid     = [ 0.0 ]
	time_grid    = [ float(time) for time in range(2000, 2020, 5) ]
	median       = numpy.median( integrand_data['prevalence'] )
	density_name = 'gaussian'
	density_id   = density_name2id[density_name]
	value_prior = {
		'prior_name' : 'parent_smoothing_pini_value_prior' ,
		'density_id' : density_id      ,
		'lower'      : 0.0             ,
		'upper'      : 1.0             ,
		'mean'       : 0.0             ,
		'std'        : 1.0             ,
		'eta'        : None            ,
		'nu'         : None            ,
	}
	dage_prior = {
		'prior_name' : 'parent_smoothing_pini_dage_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 1.0            ,
		'eta'        : None           ,
		'nu'         : None           ,
	}
	dtime_prior = {
		'prior_name' : 'parent_smooting_pini_dtime_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 1.0            ,
		'eta'        : None           ,
		'nu'         : None           ,
	}
	parent_rate_smoothing(
		rate_name, age_grid, time_grid, value_prior, dage_prior, dtime_prior
	)
	#
	# set smoothing for iota
	rate_name    = 'iota'
	age_grid     = [ float(age)  for age in range(30, 90, 10) ]
	age_grid     = [10.0, 15.0, 20.0, 25.0] + age_grid
	time_grid    = [ float(time) for time in range(1990, 2020, 5) ]
	density_name = 'log_gaussian'
	density_id   = density_name2id[density_name]
	value_prior = {
		'prior_name' : 'parent_smoothing_iota_value_prior' ,
		'density_id' : density_id      ,
		'lower'      : 1e-19           ,
		'upper'      : 0.1             ,
		'mean'       : 1e-5            ,
		'std'        : 5.0             ,
		'eta'        : 1e-6            ,
		'nu'         : None            ,
	}
	dage_prior = {
		'prior_name' : 'parent_smoothing_iota_dage_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.05           ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	dtime_prior = {
		'prior_name' : 'parent_smooting_iota_dtime_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.02           ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	parent_rate_smoothing(
		rate_name, age_grid, time_grid, value_prior, dage_prior, dtime_prior
	)
	#
	# set smoothing for chi
	rate_name    = 'chi'
	age_grid     = [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 10.0, 15.0, 20.0 ]
	age_grid    += [ float(age)  for age in range(30, 110, 10) ]
	time_grid    = [ float(time) for time in range(1990, 2020, 5) ]
	density_name = 'log_gaussian'
	density_id   = density_name2id[density_name]
	value_prior = {
		'prior_name' : 'parent_smoothing_chi_value_prior' ,
		'density_id' : density_id      ,
		'lower'      : 1e-19           ,
		'upper'      : 1.0             ,
		'mean'       : 1e-3            ,
		'std'        : 5.0             ,
		'eta'        : 1e-6            ,
		'nu'         : None            ,
	}
	dage_prior = {
		'prior_name' : 'parent_smoothing_chi_dage_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.2            ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	dtime_prior = {
		'prior_name' : 'parent_smooting_chi_dtime_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.02           ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	parent_rate_smoothing(
		rate_name, age_grid, time_grid, value_prior, dage_prior, dtime_prior
	)
	# ------------------------------------------------------------------------
	#
	# set options
	set_option('tolerance_fixed',    '1e-6')
	set_option('max_num_iter_fixed', '100')
	set_option('zero_sum_child_rate', 'iota chi')
	set_option('bound_random',        '3')
	set_option('meas_noise_effect',   'add_std_scale_none')
	#
	# add measurement noise covariates
	group_id       = 0
	for integrand_name in integrand_list_all :
		median = numpy.median( integrand_data[integrand_name] )
		# value  = 0.0
		value  = median * 1e-2
		lower  = value
		upper  = value
		add_meas_noise_mulcov(integrand_name, group_id, value, lower, upper)
	#
	# constrain all x_0 covariate multipliers to be zero
	covariate_id = 0
	restore_mulcov_x_0 = set_mulcov_zero(covariate_id)
	#
	# hold out all ode integrand data
	for integrand_name in integrand_list_yes_ode :
		hold_out_data(integrand_name = integrand_name, hold_out = 1)
	#
	# init
	system_command([ 'dismod_at', database, 'init'])
	#
	if fit_without_ode :
		#
		# fit both
		t0 = time.time()
		system_command([ 'dismod_at', database, 'fit', 'both'])
		print( 'fit_without_ode time = ', round(time.time() - t0), ' seconds')
		#
		if fit_with_ode :
			#
			# Do not include mtexcess data in this fit.
			# It was used to initialize chi, but the fit is done using
			# the mtspecific data.
			hold_out_data(integrand_name = 'mtexcess', hold_out = 1)
			#
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
			t0 = time.time()
			system_command([ 'dismod_at', database, 'fit', 'both'])
			print( 'fit_with_ode time = ', round(time.time() - t0), ' seconds')
			#
			if fit_students :
				#
				# change data likelihood to use students-t
				density_name = 'log_students'
				nu           = 5
				for integrand_name in integrand_list_all :
					median = numpy.median( integrand_data[integrand_name] )
					eta    = median * 1e-2
					set_data_likelihood(integrand_name, density_name, eta, nu)
				#
				# use previous fit as starting point
				system_command([
					'dismod_at', database, 'set', 'start_var', 'fit_var'
				])
				#
				# fit both
				t0 = time.time()
				system_command([ 'dismod_at', database, 'fit', 'both'])
				t1 = time.time()
				print( 'fit_students time = ', round(t1- t0), ' seconds')
#
if check_for_table('fit_var') :
	#
	# plot rate
	for rate_name in [ 'iota', 'chi' ] :
		plot_rate(rate_name)
	#
	# plot data
	for integrand_name in integrand_list_all :
		plot_integrand(integrand_name)
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
print( 'Total time = ', round(time.time() - start_time), ' seconds')
print('db_simplify.py: OK')
sys.exit(0)
