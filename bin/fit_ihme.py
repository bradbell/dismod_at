#! /usr/bin/env python3
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# 2DO: IHME database uses x_0, x_1, ... for covariate names and hides the
#      real covariate name in c_covariate_name. db_simlify.py could be
#      changed to account for this. For now, fit_ihme only references
#      covariates by their covariate_id.
# ---------------------------------------------------------------------------
# The files on the IHME cluster are relative to /ihme/epi/at_cascade
# ============================================================================
# The python module dismod_at.ihme.disease defines the following variables,
# which contain the special settings for this disease:
#
# relative_path:
# The database for this study is location in
#	/ihme/epi/at_cascage/relative_path
# on the IHME cluster and in
#	data_dir/realtive_path on the local machine.
# see the data_dir command line argument.
#
# max_sample:
# Is the maximum number of samples to include for any one integrand.
# If the available samples exceeds this number, a subset of size max_sample
# is randomly chosen.
#
# max_num_iter_fixed:
# Is the maxium number of optimizer iterations when optimizing the
# fixed effects.
#
# ode_hold_out_list:
# list of integrand names that are in includd when fitting without ode
# but not with ode.
#
# max_covariate_effect:
# Maximum absolute covariate effect; i.e, multiplier * (covariate - referece).
# Note that exp(effect) multiplies a model value to get another model value
# corresponding to a covariate value. Noise covariate multipliers are not
# included in the maximum.
#
# parent_smoothng:
# An ordered dictionary where keys are rate names for the smoothings
# that are changed. The values are None or functions f with the syntax
# (age_grid, time_grid, value_prior, dage_prior, dtime_prior) = f(
#	 age_table, time_table, density_name2id, integrand_data
# )
# If the function f = parent_smooth[rate_name] is None,
# then that parent rate is removed from the fit; i.e. it is zero.
#
# child_smoothing:
# It the same as parent_smoothing except that it is the the child rates.
#
import sys
import os
#
# usage
if len(sys.argv) != 5 :
	usage='''
data_dir:
Directory on the local machine that corresponds to /ihme/epi/at_cascade.
A copy of the IHME database, on the local machine, for the specified disease
has same relative path.  The subdirectory data_dir/disease is called
the disease directory. The file fit_ihme.log, in the disese directory, is the
log for the most recent fits for this disease. The temporary database temp.db
is also located in the disease directory. The sub-directories no_ode, yes_ode,
and students will contain the db2csv files (*.csv) and plots (*.pdf)
for the corresponding fits.

which_fit:
The argument which_fit must be one of the following:
'no_ode'    for only fitting without the ode.
'yes_ode'   for fititng without ode and then with ode.
'students'  for fititng without ode, then with ode, then with students-t.

random_seed:
This is an integer value that seeds the random number generator that is
used to sub-sample the data. If this value is zero, the system clock
is used to seed the random number generator. The seed corresponding to
the clock changes every second and does not repeat.

disease:
The argument disease must be one of the following:
	 crohns, dialysis, kidney, t1_diabetes.
It may also correspond to a file called
	dismod_at/ihme/disease.py
that you have added below your site-packages directory.
'''
	usage += '\nfit_ihme.py data_dir which_fit random_seed disease'
	sys.exit(usage)
#
# This is used to print help for each of the routines in this file
print_help         = False
#
# data_dir_arg
data_dir_arg = sys.argv[1]
if not os.path.isdir(data_dir_arg) :
	msg = 'data_dir = {} is not a directory'.format(data_dir_arg)
	sys.exit(msg)
#
# which_fit
which_fit_arg = sys.argv[2]
if which_fit_arg not in [ 'no_ode', 'yes_ode', 'students' ] :
	msg = 'which_fit = {} is not one of following: no_ode, yes_ode, students'
	sys.exit( msg.format(which_fit_arg) )
#
# random_seed
random_seed_arg = sys.argv[3]
if not random_seed_arg.isdigit() :
	msg = 'random_seed = {} is not a positive integer without sign in front'
	sys.exit( msg.format(random_seed_arg) )
#
# disease
disease_arg = sys.argv[4]
if disease_arg not in [ 'crohns', 'kidney', 't1_diabetes' ] :
	msg  = 'Warning: disease = {} is not one that comes with the install\n'
	msg += 'You must have added the file site-packages/dismod_at/{}.py'
	print( msg.format(disease_arg, disease_arg) )
#
# fit_without_ode
fit_without_ode    = True
#
# fit_with_ode
fit_with_ode = which_fit_arg != 'no_ode'
#
# fit_students
fit_students = which_fit_arg == 'students'
#
# random_seed
random_seed = int(random_seed_arg)
#
# ============================================================================
# END: Settings user can change
# ============================================================================
if not fit_without_ode :
	assert not fit_with_ode
if not fit_with_ode :
	assert not fit_students
# ----------------------------------------------------------------------
# imports
#
# normale imports
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
# check for sandbox version of dismod_at
if os.path.isdir('python/dismod_at') :
	print('using sandbox version of python/dismod_at')
	sys.path.insert(0,  os.path.join( os.getcwd(), 'python' ) )
if os.path.isfile('bin/dismodat.py') :
	print('using sandbox version of bin/dismodat.py')
	os.environ['PATH'] = os.getcwd() + '/bin:' + os.environ['PATH']
if os.path.isfile('build/devel/dismod_at') :
	print('using sandbox version of build/devel/dismod_at')
	os.environ['PATH'] = os.getcwd() + '/build/devel:' + os.environ['PATH']
	#
#
# dismod_at imports
import dismod_at
#
try :
	exec('import dismod_at.ihme.' + disease_arg + ' as specific' )
except :
	sys.exit('Cannot find dismod_at.ihme.' + disease_arg)
# ----------------------------------------------------------------------------
def execute_print_help() :
	# print the help message for each fit_ihme routine
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
		stop   = fp_data.find(':', start) + 2
		output = fp_data[start : stop]
		while 0 <= more(stop) :
			start   = more(stop) + 2
			stop    = fp_data.find('\n', stop) + 1
			output += fp_data[start : stop]
		print( output )
		start = stop
		index  = fp_data.find('\ndef ', start)
	sys.exit(0)
if print_help :
	# use a function to avoid defining global variables
	execute_print_help()
# ----------------------------------------------------------------------------
disease_directory = None
temp_database     = None
def setup() :
	# set up some globals
	global disease_directory
	global temp_database
	# directory where plots are stored
	original_database = data_dir_arg + '/' + specific.relative_path
	disease_directory = data_dir_arg + '/' + disease_arg
	temp_database     = disease_directory + '/temp.db'
	#
	if not os.path.exists(disease_directory) :
		os.mkdir(disease_directory)
	#
	# temp_database
	shutil.copyfile(original_database, temp_database)
	#
setup()
# ===========================================================================
# Utilities that do not use global data tables
# ===========================================================================
fp_log_file = None
def trace(message = None, operation = None) :
	# Print output and manage log file
	global fp_log_file
	# start
	if operation == 'start' :
		assert fp_log_file is None
		log_file    = disease_directory + '/fit_ihme.log'
		fp_log_file = open(log_file, 'w')
		return
	assert fp_log_file is not None
	#
	# message
	if operation is None :
		assert message is not None
		print(message)
		fp_log_file.write(message + '\n')
		return
	assert message is None
	#
	# stop
	assert operation == 'stop'
	fp_log_file.close()
	fp_log_file = None
	return
# ----------------------------------------------------------------------------
def plot_fit(which_fit) :
	# output plot files (*.pdf) and db2csv files (*.csv) in sub-directory
	# of the disease_directory.
	#
	assert which_fit in [ 'no_ode', 'yes_ode', 'students' ]
	directory = disease_directory + '/' + which_fit
	#
	# if necessary, create the plot sub-directory for this fit
	if os.path.exists(directory) :
		# erase previous contentes of this plot directory
		for name in os.listdir(directory) :
			os.remove(directory + '/' + name)
	if not os.path.exists(directory) :
		os.mkdir(directory)
	#
	integrand_list_yes_ode = get_integrand_list(True)
	integrand_list_no_ode  = get_integrand_list(False)
	integrand_list_all     = integrand_list_yes_ode + integrand_list_no_ode
	#
	# plot rate
	for (rate_id, row) in enumerate(rate_table) :
		if row['parent_smooth_id'] is not None :
			rate_name = row['rate_name']
			plot_rate(row['rate_name'], directory, which_fit)
	#
	# plot data
	for integrand_name in integrand_list_all :
		plot_integrand(integrand_name, directory, which_fit)
	#
	# plot prediction
	predict_integrand_list   = [ 'susceptible', 'withC' ]
	covariate_integrand_list = integrand_list_yes_ode
	plot_predict(
		covariate_integrand_list, predict_integrand_list, directory, which_fit
	)
	#
	# db2csv
	copy_database = directory + '/temp.db'
	shutil.copyfile(temp_database, copy_database )
	system_command( [ 'dismodat.py',  copy_database, 'db2csv' ] )
# ----------------------------------------------------------------------------
def case_study_title(location, which_fit = None) :
	# return the title for this study and fit
	text          = specific.relative_path.split('/')
	model_id      = text[1]
	sex           = text[4]
	disease       = disease_arg
	result        = location + ': ' + disease
	if which_fit is not None :
		result += '\n' + which_fit
	result += ': sex=' + sex + ' model_id=' + model_id
	return result
# ----------------------------------------------------------------------------
def get_table (table_name) :
	# read a dismod_at table
	new                  = False
	connection           = dismod_at.create_connection(temp_database, new)
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
	connection   = dismod_at.create_connection(temp_database, new)
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
	# execute an sql command on this temp_database
	trace(command)
	new          = False
	connection   = dismod_at.create_connection(temp_database, new)
	cursor       = connection.cursor()
	cursor.execute(command)
	connection.commit()
	result       = cursor.fetchall()
	connection.close()
	return result
# ----------------------------------------------------------------------------
def check_for_table(table_name) :
	# check if a table is in th database
	command  = 'SELECT name FROM sqlite_master WHERE '
	command += "type = 'table' and name = '{}'".format(table_name)
	result   = sql_command(command)
	return len(result) > 0
# ----------------------------------------------------------------------------
def system_command (command_list) :
	# Execute a command at the system level. Each use of this routine
	# is a step in the processing, not a utility. Hence it traces a message.
	msg  = '\nsystem_command\n'
	msg += " ".join(command_list)
	trace(msg)
	#
	# flush python's pending standard output in case this command
	# generates more standard output
	sys.stdout.flush()
	#
	run = subprocess.run(command_list)
	if run.returncode != 0 :
		print('random_seed = ', random_seed )
		sys.exit('fit_ihme.py: system command failed')
# ----------------------------------------------------------------------------
def table_name2id(table, table_name) :
	# Return dictionary that maps a value in the name column to corresponding
	# row index; i.e. table id value.
	result = dict()
	name_column = table_name + '_name'
	for (row_id, row) in enumerate(table) :
		result[ row[name_column] ] = row_id
	return result
# ----------------------------------------------------------------------------
def check_last_fit(which_fit) :
	table_name = 'log'
	(table, col_name, col_type) = get_table(table_name);
	#
	# search for end of log entry for last fit
	log_id   = len(table)
	end_fit  = False
	while not end_fit :
		assert log_id > 0
		log_id -= 1
		row     = table[log_id]
		if row['message_type'] == 'command' :
			message = row['message']
			end_fit = message.startswith('end fit')
	#
	begin_fit = False
	msg       = ''
	while not begin_fit :
		assert log_id > 0
		log_id      -= 1
		row          = table[log_id]
		message_type = row['message_type']
		if message_type == 'command' :
			message = row['message']
			assert message.startswith('begin fit')
			begin_fit = True
		elif message_type in [ 'warning', 'error' ] :
			msg += message_type + ' ' + row['message'] + '\n'
	#
	if msg == '' :
		msg = '\nfit_{} OK\n'.format(which_fit)
		trace(msg)
	else :
		msg = '\nfit_{} Errors and or Warnings::\n'.format(which_fit) + msg
		trace(msg)
	return
# ----------------------------------------------------------------------------
# ============================================================================
# API input tables that do not change
# ============================================================================
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
#
# subgropup_table
table_name = 'subgroup'
(subgroup_table, subgroup_col_name, subgroup_col_type) = get_table(table_name)
# ============================================================================
# API input tables that change
# (these tables are not affected by any dismod_at command).
# ============================================================================
#
# age_table
table_name = 'age'
(age_table, age_col_name, age_col_type) = get_table(table_name)
#
# avgint_table
table_name = 'avgint'
(avgint_table, avgint_col_name, avgint_col_type) = get_table(table_name)
#
# covariate_table
table_name = 'covariate'
(covariate_table, covariate_col_name, covariate_col_type) = \
	get_table(table_name)
#
# data_table
table_name = 'data'
(data_table, data_col_name, data_col_type) = get_table(table_name)
#
# integrand_table, integrand_name2id
table_name = 'integrand'
(integrand_table, integrand_col_name, col_type) = get_table(table_name)
integrand_name2id = table_name2id(integrand_table, table_name)
#
# mulcov_table
table_name = 'mulcov'
(mulcov_table, mulcov_col_name, mulcov_col_type) = get_table(table_name)
#
# prior_table
table_name = 'prior'
(prior_table, prior_col_name, prior_col_type) = get_table(table_name)
#
# rate_table
table_name = 'rate'
(rate_table, rate_col_name, rate_col_type) = get_table(table_name)
rate_name2id = table_name2id(rate_table, table_name)
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
# time_table
table_name = 'time'
(time_table, time_col_name, time_col_type) = get_table(table_name)
#
# ============================================================================
# Utilities that use global data tables but do not modify them
# ============================================================================
# ----------------------------------------------------------------------------
def relative_covariate(covariate_id) :
	value_set  = set()
	column_name = 'x_{}'.format(covariate_id)
	for row in data_table :
		value = row[column_name]
		if value is not None :
			value_set.add( value )
	# sex is an aboslute covariate and has 3 values, -0.5, 0.0, +0.5
	return len( value_set ) > 3
# ----------------------------------------------------------------------------
def get_integrand_list (ode) :
	# If ode is true (false) get list of integrands that require
	# (do not require) the ode to model.
	integrand_model_uses_ode = [
		'prevalence', 'Tincidence', 'mtspecific', 'mtall', 'mtstandard'
	]
	integrand_set = set()
	for row in data_table :
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
	# Sort so that order of integrands is always the same.
	# This is especially helpful when using variable.csv to se that
	# refactoring program has not introduced a bug.
	return sorted( integrand_list )
# ----------------------------------------------------------------------------
def get_integrand_data () :
	# return a dictionary that contains the data for each integrand name
	#
	integrand_data = dict()
	for row in data_table :
		integrand_id   = row['integrand_id']
		integrand_name = integrand_table[integrand_id]['integrand_name']
		if integrand_name not in integrand_data :
			integrand_data[integrand_name] = list()
		integrand_data[integrand_name].append( row['meas_value'] )
	#
	return integrand_data
# ----------------------------------------------------------------------------
def get_parent_node_id() :
	parent_node_id = None
	for row in option_table :
		if row['option_name'] == 'parent_node_id' :
			parent_node_id = int( row['option_value'] )
		if row['option_name'] == 'parent_node_name' :
			table_name = 'node'
			node_name  = row['option_value']
			parent_note_id = node_name2id[node_name]
	if parent_node_id is None :
		msg = 'Cannot find parent_node_id or parent_node_name in option table'
		sys.exit(msg)
	return parent_node_id
# ----------------------------------------------------------------------------
def plot_rate(rate_name, directory, which_fit) :
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
	parent_node_id   = get_parent_node_id()
	#
	# parent_node_name
	parent_node_name = node_table[parent_node_id]['node_name']
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
		trace('plot_rate: ' + rate_name + ' is identically zero')
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
	file_name = directory + '/' + rate_name + '.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	# for each time, plot rate as a function of age
	n_fig       = math.ceil( n_time / ( n_color_style - 1) )
	n_per_fig   = math.ceil( n_time / n_fig )
	color_index = -1
	assert n_per_fig < n_color_style
	for i_fig in range( n_fig ) :
		fig    = pyplot.figure()
		axis   = pyplot.subplot(1,1,1)
		axis.set_title( case_study_title(parent_node_name, which_fit) )
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

	# for each age, plot rate as a function of time
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
def plot_integrand(integrand_name, directory, which_fit) :
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
	# this_integrand_id
	this_integrand_id = integrand_name2id[integrand_name]
	#
	# parent_node_name
	parent_node_id   = get_parent_node_id()
	parent_node_name = node_table[parent_node_id]['node_name']
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
		msg = 'plot_integrand: ' + integrand_name + ' has less than 2 points'
		trace(msg)
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
	file_name = directory + '/' + integrand_name + '.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	for x_name in [ 'index', 'node', 'age', 'time' ] :
		x          = eval( x_name )
		#
		fig, axes = pyplot.subplots(3, 1, sharex=True)
		fig.subplots_adjust(hspace=0)
		#
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
		if x_name == 'index' :
			pyplot.title( case_study_title(parent_node_name, which_fit) )
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
# =============================================================================
# Routines that Only Change Data Table
# =============================================================================
# -----------------------------------------------------------------------------
# subset_data:
def subset_data () :
	# remove datat table rows that are held out or have covariates
	# that are out of bounds
	msg  = '\nsubset_data\n'
	msg += 'removing hold out and covariate out of bounds data'
	trace(msg)
	#
	# Need global becasue we will use an assingnment to data_table
	global data_table
	#
	table_in      = data_table
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
	data_table = table_out
	table_name = 'data'
	put_table('data', data_table, data_col_name, data_col_type)
# -----------------------------------------------------------------------------
def random_subsample_data(integrand_name, max_sample) :
	# for a specified integrand, sample at most max_sample entries.
	# This does random sampling that can be seeded by calling random.seed.
	# The origianl order of the data is preserved (in index plots)
	# by sorting the subsample.
	#
	integrand_id =integrand_name2id[integrand_name]
	#
	# Need global becasue we will use an assingnment to data_table
	global data_table
	#
	# indices for this integrand
	table_in   = data_table
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
	data_table = table_out
	table_name = 'data'
	put_table(table_name, data_table, data_col_name, data_col_type)
	#
	msg  = '\nrandom_subsample_data\n'
	msg += 'number of {} samples: in = {} out = {}'
	trace( msg.format(integrand_name, n_sample_in, n_sample_out) )
# -----------------------------------------------------------------------------
def hold_out_data (integrand_name=None, node_name= None, hold_out=None) :
	# for a specified integrand or node, set the hold_out to 0 or 1
	if integrand_name is None and node_name is None :
		sys.exit('hold_out_data: both integrand_name and node_name are None')
	if integrand_name is not None and node_name is not None :
		sys.exit('hold_out_data: both integrand_name and node_name not None')
	#
	if hold_out not in [ 0, 1] :
		msg = 'hold_out_data: hold_out is not zero or one'
		sys.exit(msg)
	#
	msg  = '\nhold_out_data\n'
	if integrand_name is not None :
		msg += 'integrand = {}'.format(integrand_name)
	if node_name is not None :
		msg += 'node = {}'.format(node_name)
	msg += ', hold_out = {}'.format(hold_out)
	trace(msg)
	#
	if integrand_name is not None :
		node_id      = None
		integrand_id = integrand_name2id[integrand_name]
	else :
		node_name is not None
		integrand_id = None
		node_id      = node_name2id[node_name]
	#
	#
	for row in data_table :
		if integrand_id == row['integrand_id'] or node_id == row['node_id'] :
			row['hold_out'] = hold_out
	#
	table_name = 'data'
	put_table(table_name, data_table, data_col_name, data_col_type)
# -----------------------------------------------------------------------------
def set_data_likelihood (
		integrand_data, integrand_name, density_name, factor_eta=None, nu=None
) :
	# For a specified integrand, set its density, eta, and nu.
	# The default value for eta and nu is None.
	# If factor_eta is not None, eta is set to the factor times the median
	# value for the integrand.
	assert (factor_eta is None) or 0.0 <= factor_eta
	#
	msg  = '\nset_data_likelihood\n'
	msg += 'integrand = {}'.format(integrand_name)
	msg += ', density = {}'.format(density_name)
	if factor_eta is not None :
		msg += ', eta = m*{}'.format(factor_eta)
	if nu is not None :
		msg += ', nu = {}'.format(nu)
	if factor_eta is not None :
		msg += '\n' + 12 * ' ' + 'where m is the meadian of the'
		msg += ' {} data'.format(integrand_name)
	trace( msg )
	#
	# integrand_id
	integrand_id =integrand_name2id[integrand_name]
	#
	# density_id
	density_id = density_name2id[density_name]
	#
	if factor_eta is None :
		eta = None
	else :
		median = numpy.median( integrand_data[integrand_name] )
		eta = factor_eta * median
	for row in data_table :
		if row['integrand_id'] == integrand_id :
			row['density_id'] = density_id
			row['eta']        = eta
			row['nu']         = nu
	#
	table_name = 'data'
	put_table(table_name, data_table, data_col_name, data_col_type)
# ---------------------------------------------------------------------------
def compress_age_time_interval(integrand_name, age_size, time_size) :
	# For the specified integrand, compress age and time intervalces that are
	# less than the specified size to a single point.
	msg  = '\ncompress_age_time_interval\n'
	msg += 'Use midpoint for intervals less than or equal specified size\n'
	msg += 'integrand = {}'.format(integrand_name)
	msg += ', age_size= {}'.format(age_size)
	msg += ', time_size= {}'.format(time_size)
	trace(msg)
	#
	# integrand_id
	integrand_id =integrand_name2id[integrand_name]
	#
	for row in data_table :
		if row['integrand_id'] == integrand_id :
			age_lower  = row['age_lower']
			age_upper  = row['age_upper']
			time_lower = row['time_lower']
			time_upper = row['time_upper']
			if age_upper - age_lower <= age_size :
				avg              = (age_upper + age_lower) / 2.0
				row['age_lower'] = avg
				row['age_upper'] = avg
			if time_upper - time_lower <= time_size :
				avg               = (time_upper + time_lower) / 2.0
				row['time_lower'] = avg
				row['time_upper'] = avg
	#
	table_name = 'data'
	put_table(table_name, data_table, data_col_name, data_col_type)
# ============================================================================
# Routines that Change Other Tables
# ============================================================================
# ----------------------------------------------------------------------------
# plot_predict
def plot_predict(
		covariate_integrand_list, predict_integrand_list, directory, which_fit
	) :
	# Plot the model predictions for each integrand in the predict integrand
	# list. The is one such plot for each integrand in the covariate integrand
	# list (which determines the covariate values used for the predictions).
	# The avgint and predict tables are overwritten by this procedure.
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
	system_command([ 'dismod_at', temp_database, 'predict', 'fit_var' ])
	#
	table_name = 'predict'
	(predict_table, col_name, col_type) = get_table(table_name)
	# ------------------------------------------------------------------------
	# initialize
	from matplotlib import pyplot
	import matplotlib.backends.backend_pdf
	file_name = directory + '/predict.pdf'
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
# --------------------------------------------------------------------------
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
# 2DO: Remove this when bounds on mulcov work
def new_zero_smooth_id (smooth_id) :
	# add a new smoothing that has the same grid as smooth_id smoothing
	# and that constrains to zero. The smooth and smooth_grid tables are
	# modified by this routine but they are not written out.
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
def new_bounded_smooth_id (smooth_id, lower, upper) :
	# add a new smoothing that has the same grid as smooth_id smoothing
	# and that constrains value to be within the specified lower and upper
	# bounds.The prior, smooth and smooth_grid tables are modified but
	# they are not written out. The lower and upper bounds can be None.
	if smooth_id is None :
		return None
	#
	if lower is None and upper is None :
		mean = 0.0
	elif lower is not None and upper is not None :
		mean = (lower + upper) / 2.0
	elif lower is None :
		if upper >= 0.0 :
			mean = 0.0
		else :
			mean = upper
	else :
		assert upper is None
		if lower <= 0.0 :
			mean = 0.0
		else :
			mean = lower
	#
	# smooth_table
	new_smooth_id = len(smooth_table)
	new_row                = copy.copy( smooth_table[smooth_id] )
	new_row['smooth_name'] = 'bound_smoothing #' + str( new_smooth_id )
	smooth_table.append( new_row )
	#
	new_prior_id  = len(prior_table)
	density_id    = density_name2id['uniform']
	value_prior  = {
		'prior_name' : 'smoothing_{}_centerd_prior'.format(new_smooth_id),
		'density_id' : density_id,
		'lower'      : lower,
		'upper'      : upper,
		'mean'       : mean,
		'std'        : None,
		'eta'        : None,
		'nu'         : None,
	}
	prior_table.append( value_prior )
	#
	for old_row in smooth_grid_table :
		if old_row['smooth_id'] == smooth_id :
			new_row = copy.copy( old_row )
			new_row['smooth_id']      = new_smooth_id
			new_row['value_prior_id'] = new_prior_id
			new_row['dage_prior_id']  = None
			new_row['dtime_prior_id'] = None
			new_row['const_value']    = None
			smooth_grid_table.append( new_row )
	return new_smooth_id
# -----------------------------------------------------------------------------
def identically_one_covariate () :
	# Return the covariate_id for a covariate that is one for every data point,
	# has refefence value is zero, and max_difference value is null.
	# (If no such covariate exists, one is created.)
	# This routine is a utility, not a step in the processing,
	# and hence does not print a summary message.
	#
	# is_one
	n_covariate = len(covariate_table)
	is_one      = list()
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
	for key in covariate_col_name :
		row[key] = None
	row['covariate_name'] = 'ones'
	row['reference']      = 0.0
	#
	covariate_id = len(covariate_table)
	covariate_table.append(row)
	#
	table_name    = 'covariate'
	put_table(
		table_name, covariate_table, covariate_col_name, covariate_col_type
	)
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
	return covariate_id
# -----------------------------------------------------------------------------
def zero_rate(rate_name, zero_parent, zero_children) :
	# zero as rate's parent (fixed efffects) or children (random effects)
	assert zero_parent or zero_children
	#
	msg = '\nzero_rate\n'
	msg += 'rate = {}'.format(rate_name)
	msg += ', zero_parent = {}'.format(zero_parent)
	msg += ', zero_children = {}'.format(zero_children)
	trace( msg)
	#
	for row in rate_table :
		if row['rate_name'] == rate_name :
			if zero_parent :
				row['parent_smooth_id'] = None
			if zero_children :
				row['child_smooth_id']  = None
				row['child_nslist_id']  = None
	put_table('rate', rate_table, rate_col_name, rate_col_type)
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
	# covariate_value
	key             = 'x_' + str(covariate_id)
	covariate_value = list()
	for row in data_table :
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
	covariate_name = covariate_table[covariate_id]['covariate_name']
	old_reference  = covariate_table[covariate_id]['reference']
	#
	covariate_table[covariate_id]['reference'] = new_reference
	#
	msg  = '\nset_covariate_reference\n'
	msg += 'for covariate = {}'.format(covariate_name)
	msg += ', covariate_id = {}'.format(covariate_id)
	msg += ', reference_name = {}'.format(reference_name)
	msg += '\nold_reference = {:.5g}'.format(old_reference)
	msg += ', new_reference = {:.5g}'.format(new_reference)
	trace( msg )
	#
	table_name = 'covariate'
	put_table(
		table_name, covariate_table, covariate_col_name, covariate_col_type
	)
# -----------------------------------------------------------------------------
# 2DO: remvoe this when bounds on mulcov work
def set_mulcov_zero (covariate_id, restore= None) :
	# set all of the multipliers for a specified covariate to zero without
	# changing the order or size of the var table
	covariate_name = covariate_table[covariate_id]['covariate_name']
	msg            = 'covariate = {}'.format(covariate_name)
	msg           += ', covariate_id = {}'.format(covariate_id)
	if restore is None :
		msg  = '\nset_mulcov_zero\n' + msg
	else :
		msg  = '\nrestore_mulcov\n' + msg
	trace( msg )
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
def set_mulcov_bound(max_covariate_effect, covariate_id) :
	# Set bounds for all of the multipliers for a specified covariate so
	# corresponding effect is bounded by disease_specific_max_covariate_effect.
	# Noise covariate multipliers are not included.
	#
	msg  = '\nset_mulcov_bound\n'
	msg += 'covariate = x_{}'.format(covariate_id)
	trace( msg )
	#
	# reference for this covariate
	reference = covariate_table[covariate_id]['reference']
	#
	# covariate minus reference
	difference  = list()
	column_name = 'x_{}'.format(covariate_id)
	for row in data_table :
		if data_table :
			value = row[column_name]
			if value is not None :
				difference.append( value - reference )
	#
	# maximum and minimum difference
	min_difference = min(difference)
	max_difference = max(difference)
	#
	# bounds on covariate multiplier
	if max_difference == 0.0 :
		upper = None
	else :
		upper = max_covariate_effect / max_difference
	if min_difference == 0.0 :
		lower = None
	else :
		lower = max_covariate_effect / min_difference
	#
	for row in mulcov_table :
		if row['covariate_id'] == covariate_id :
			if row['mulcov_type'] != 'meas_noise' :
				group_smooth_id = row['group_smooth_id']
				group_smooth_id = new_bounded_smooth_id(
					group_smooth_id, lower, upper
				)
				row['group_smooth_id'] = group_smooth_id
				#
				subgroup_smooth_id = row['subgroup_smooth_id']
				subgroup_smooth_id = new_bounded_smooth_id(
					subgroup_smooth_id, lower, upper
				)
				row['subgroup_smooth_id'] = subgroup_smooth_id
	#
	put_table('prior',   prior_table,  prior_col_name,  prior_col_type)
	put_table('mulcov',  mulcov_table, mulcov_col_name, mulcov_col_type)
	put_table('smooth',  smooth_table, smooth_col_name, smooth_col_type)
	put_table('smooth_grid',
		smooth_grid_table, smooth_grid_col_name, smooth_grid_col_type
	)
	return
# -----------------------------------------------------------------------------
def set_rate_smoothing(parent_rate, rate_name,
	age_grid, time_grid, value_prior, dage_prior, dtime_prior
) :
	# parent_rate: is True (False) for parent (child) smoohting
	# rate_name:   the rate we are seting the smoohting for
	# age_grid:    list of ages specificing age grid for smoothing
	# time_grid:   list of times specificing time grid for smoothing
	# value_piror: dict containing prior for the rate values
	# dage_prior:  dict containing priot for dage smoothing
	# dtime_piror: dict containt prior for dtime smoohting
	#
	msg  = '\nrate_smoothing\n'
	msg += '{:11} = {} '.format('rate',  parent_rate)
	msg += '\n{:11} = {} '.format('rate',  rate_name)
	msg += '\n{:11} = {}'.format('age_grid',  age_grid)
	msg += '\n{:11} = {}'.format('time_grid', time_grid)
	def msg_prior(name, prior) :
		label        = name + '_prior'
		result       = '\n{:11} = '.format(label)
		density_name = density_table[ prior['density_id'] ]['density_name']
		result      += density_name
		for key in [ 'lower', 'upper', 'mean', 'std', 'eta', 'nu' ] :
			if prior[key] is not None :
				result += ', {}:{}'.format(key, prior[key])
		return result
	msg += msg_prior('value', value_prior)
	msg += msg_prior('dage',  dage_prior)
	msg += msg_prior('dtime', dtime_prior)
	trace( msg )
	#
	# add the smothing
	smooth_id = new_smoothing(
		age_grid, time_grid, value_prior, dage_prior, dtime_prior
	)
	#
	# change rate_table
	for row in rate_table :
		if row['rate_name'] == rate_name :
			if parent_rate :
				row['parent_smooth_id'] = smooth_id
			else :
				row['child_smooth_id'] = smooth_id
	#
	# write out the tables that changed
	for row in prior_table :
		if row['prior_name'].startswith('smoothing_') :
			trace( row['prior_name'] )
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
	# Set option specified by name to its value where name and value are
	# strings. The routine system_command to prints the processing message
	# for this operation.
	system_command( [
		'dismod_at',  temp_database, 'set', 'option', name , value
	] )
# -----------------------------------------------------------------------------
def add_meas_noise_mulcov(integrand_data, integrand_name, group_id, factor) :
	# Add a meas_noise covariate multiplier for a specified integrand.
	# integrand_data: is the current result of get_integrand_data.
	# group_id: specifies the group for the covariate multiplier.
	#
	# factor: is a dictionary with following keys: mean, lower, upper.
	# For each key the factor multipliers the median of the data for this
	# integrand to get the corresponding value in the uniform prior for the
	# covariate multiplier.
	#
	# Note that meas_noise multipliers can't have
	# ramdom effect (so the subgroup id is null in the mulcov table).
	assert 0.0             <= factor['lower']
	assert factor['lower'] <= factor['mean']
	assert factor['mean']  <= factor['upper']
	#
	group_name = None
	for row in subgroup_table :
		if row['group_id'] == group_id :
			group_name = row['group_name']
	assert group_name is not None
	#
	msg  = '\nadd_meas_noise_mulcov\n'
	msg += 'integrand   = {}'.format(integrand_name)
	msg += ', group = {}'.format(group_name)
	msg += '\nvalue_prior = uniform'
	msg += ', lower:m*{}'.format(factor['lower'])
	msg += ', mean:m*{}'.format(factor['mean'])
	msg += ', upper:m*{}'.format(factor['upper'])
	msg += '\n              where m is the median of the'
	msg += ' {} data'.format(integrand_name)
	trace( msg )
	#
	median = numpy.median( integrand_data[integrand_name] )
	lower  = median * factor['lower']
	mean   = median * factor['mean']
	upper  = median * factor['upper']
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
		'mean'       : mean           ,
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
# remove child randome effecst for chi
#	rate_name     = 'chi'
#	zero_parent   = False
#	zero_children = True
#	zero_rate(rate_name, zero_parent, zero_children)
#
# hold out Korea before subset_data() do it gets removed
#	hold_out_data(node_name = 'Republic of Korea', hold_out = 1)
#	hold_out_data(node_name = 'Japan', hold_out = 1)
# ----------------------------------------------------------------------
# Actual Changes
# ----------------------------------------------------------------------
# start new fit_ihme.log file
trace(operation = 'start')
#
# print the title for this study
parent_node_id   = get_parent_node_id()
parent_node_name = node_table[parent_node_id]['node_name']
title            = case_study_title(parent_node_name)
line             = len(title) * '-'
trace( '\n' + line + '\n' + title + '\n' + line + '\n' )
#
# erase the database log table so log is just for this session
sql_command('DROP TABLE IF EXISTS log')
#
# start_time
start_time = time.time()
#
# ----------------------------------------------------------------------------
# Changes to database
# ----------------------------------------------------------------------------
# seed used to randomly subsample data
if random_seed == 0 :
	random_seed = int( time.time() )
random.seed(random_seed)
#
# remove all hold out data and data past covariate limits
subset_data()
#
# set reference value for x_0 to its median
reference_name  = 'median'
for covariate_id in range( len(covariate_table) ) :
	if relative_covariate(covariate_id) :
		set_covariate_reference(covariate_id, reference_name)
#
# subsetting the data can remove some integrands
integrand_list_yes_ode = get_integrand_list(True)
integrand_list_no_ode  = get_integrand_list(False)
integrand_list_all     = integrand_list_yes_ode + integrand_list_no_ode
#
# integrand_data
integrand_data = get_integrand_data()
#
# randomly subsample
for integrand_name in integrand_list_all :
	random_subsample_data(integrand_name, specific.max_sample)
#
# Set parent rate priros for this disease
for rate_name in specific.parent_smoothing :
	fun = specific.parent_smoothing[rate_name]
	#
	if fun is None :
		zero_parent   = True
		zero_children = False
		zero_rate(rate_name, zero_parent, zero_children)
	else :
		parent_rate = True
		result = fun( age_table, time_table, density_name2id, integrand_data )
		(age_grid, time_grid, value_prior, dage_prior, dtime_prior) = result
		set_rate_smoothing(parent_rate, rate_name,
			age_grid, time_grid, value_prior, dage_prior, dtime_prior
		)
#
# Set child rate priros for this disease
for rate_name in specific.child_smoothing :
	fun = specific.child_smoothing[rate_name]
	#
	if fun is None :
		zero_parent   = False
		zero_children = True
		zero_rate(rate_name, zero_parent, zero_children)
	else :
		parent_rate = False
		result = fun( age_table, time_table, density_name2id, integrand_data )
		(age_grid, time_grid, value_prior, dage_prior, dtime_prior) = result
		set_rate_smoothing(parent_rate, rate_name,
			age_grid, time_grid, value_prior, dage_prior, dtime_prior
		)
#
# iota_zero, rho_zero, chi_zero
iota_zero = rate_table[ rate_name2id['iota'] ]['parent_smooth_id'] is None
rho_zero  = rate_table[ rate_name2id['rho'] ]['parent_smooth_id'] is None
chi_zero  = rate_table[ rate_name2id['chi'] ]['parent_smooth_id'] is None
#
# rate_case
if iota_zero :
	if rho_zero :
		rate_case = 'iota_zero_rho_zero'
	else :
		rate_case = 'iota_zero_rho_pos'
else :
	if rho_zero :
		rate_case = 'iota_pos_rho_zero'
	else :
		rate_case = 'iota_pos_rho_pos'
#
# set options
set_option('tolerance_fixed',     '1e-8')
set_option('max_num_iter_fixed',  str(specific.max_num_iter_fixed))
set_option('quasi_fixed',         'false')
set_option('zero_sum_child_rate', 'iota rho chi')
set_option('bound_random',        '3')
set_option('meas_noise_effect',   'add_std_scale_none')
set_option('rate_case',           rate_case)
#
# add measurement noise covariates
group_id = 0
factor   = { 'lower':1e-2, 'mean':1e-2, 'upper':1e-2 }
for integrand_name in integrand_list_all :
	add_meas_noise_mulcov(integrand_data, integrand_name, group_id, factor)
#
# compress age and time intervals
age_size  = 10.0
time_size = 10.0
for integrand_name in integrand_list_all :
	compress_age_time_interval(integrand_name, age_size, time_size)
#
# set bounds for all the covariates
n_covariate = len( covariate_table )
for covariate_id in range( n_covariate ) :
	set_mulcov_bound(specific.max_covariate_effect, covariate_id)
#
# hold out all ode integrand data
for integrand_name in integrand_list_yes_ode :
	hold_out_data(integrand_name = integrand_name, hold_out = 1)
#
# hold out integrands that do not have corresponding rates
if iota_zero :
	hold_out_data(integrand_name = 'Sincidence', hold_out = 1)
if rho_zero :
	hold_out_data(integrand_name = 'remission', hold_out = 1)
if chi_zero :
	hold_out_data(integrand_name = 'mtexcess', hold_out = 1)
#
# init
system_command([ 'dismod_at', temp_database, 'init'])
# -----------------------------------------------------------------------------
if fit_without_ode :
	#
	# fit both
	t0 = time.time()
	system_command([ 'dismod_at', temp_database, 'fit', 'both'])
	msg  = 'fit_without time = '
	msg += str( round( time.time() - t0 ) ) + ' seconds'
	trace(msg)
	#
	which_fit = 'no_ode'
	check_last_fit(which_fit)
	plot_fit(which_fit)
	# ------------------------------------------------------------------------
	if fit_with_ode :
		#
		# use previous fit as starting point
		system_command([
			'dismod_at', temp_database, 'set', 'start_var', 'fit_var'
		])
		#
		# put ode integrands data back in the fit
		for integrand_name in integrand_list_yes_ode :
			hold_out_data(integrand_name = integrand_name, hold_out = 0)
		#
		# exclude integerands that are just used to get starting value
		for integrand_name in specific.ode_hold_out_list :
			hold_out_data(integrand_name = integrand_name, hold_out = 1)
		#
		# fit both
		t0 = time.time()
		system_command([ 'dismod_at', temp_database, 'fit', 'both'])
		msg  = 'fit_with_ode time = '
		msg += str( round( time.time() - t0 ) ) + ' seconds'
		trace(msg)
		#
		which_fit = 'yes_ode'
		check_last_fit(which_fit)
		plot_fit(which_fit)
		# --------------------------------------------------------------------
		if fit_students :
			# change data likelihood to use students-t
			density_name = 'log_students'
			factor_eta   = 1e-2
			nu           = 5
			for integrand_name in integrand_list_all :
				set_data_likelihood(integrand_data,
					integrand_name, density_name, factor_eta, nu
				)
			#
			# use previous fit as starting point
			system_command([
				'dismod_at', temp_database, 'set', 'start_var', 'fit_var'
			])
			#
			# fit both
			t0 = time.time()
			system_command([ 'dismod_at', temp_database, 'fit', 'both'])
			t1 = time.time()
			msg  = 'fit_students time = '
			msg += str( round( time.time() - t0 ) ) + ' seconds'
			trace(msg)
			#
			which_fit = 'students'
			check_last_fit(which_fit)
			plot_fit(which_fit)
# ----------------------------------------------------------------------
msg  = '\n'
msg += '\nintegrands   = ' + str( integrand_list_all )
msg += '\nrandom_seed  = ' + str( random_seed )
msg += '\nTotal time   = '
msg += str( round( time.time() - start_time ) ) + ' seconds'
trace( msg )
trace('fit_ihme.py: OK')
trace('stop')
sys.exit(0)
