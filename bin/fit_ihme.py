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
# normal imports
import math
import os
import sys
import shutil
import subprocess
import copy
import numpy
import time
import importlib
#
print_developer_help = False
def execute_print_developer_help() :
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
if print_developer_help :
	# use a function to avoid defining global variables
	execute_print_developer_help()
#
user_help_message_dict = {
'usage':'''
usage:
fit_ihme.py data_dir disease which_fit [ random_seed ]
fit_ihme.py data_dir disease which_fit sample number_sample
fit_ihme.py help
fit_ihme.py help topic
''',

'data_dir':'''
data_dir:
This command line argument is the director on the local machine that
corresponds to /share/epi/at_cascade.
A copy of the IHME database, on the local machine, for the specified disease
has same relative path.  The subdirectory data_dir/disease is called
the disease directory. The file fit_ihme.log, in the disese directory, is the
log for the most recent fits for this disease. The temporary database temp.db
is also located in the disease directory. The sub-directories no_ode, yes_ode,
and students will contain the db2csv files (*.csv) and plots (*.pdf)
for the corresponding fits.
''',

'disease':'''
disease:
The command line argument must be one of the following:
	crohns, dialysis, kidney, t1_diabetes, unknown.
It may also correspond to a disease specific file called
	dismod_at/ihme/disease.py
that you have added below your site-packages directory.
See the following files for examples disease specific files:
	crohns.py, dialysis.py, kidney.py, t1_diabetes.py, unknown.py
The settings in a disease specific file are the part of the model that is
different for each disease. Currently, the following settings are included:
	relative_path
	max_per_integrand
	tolerance_fixed
	max_num_iter_fixed
	ode_hold_out_list
	max_covariate_effect
	set_mulcov_value
	parent_smoothing
	child_smoothing
This is a very minimal set and more settings would be useful.
The unknown disease is for fitting a database that is not on the IHME cluster
and is meant to be modified.
''',

'which_fit':'''
which_fit:
This command line argument must be one of the following:
'no_ode'    fit without ode integrands (no previous fit necessary).
'yes_ode'   fit with ode integrands    (previous no_ode fit necessary).
'students'  fit using students-t       (previous yes_ode fit necessary).
''',

'random_seed':'''
random_seed:
This command line argument is only present when which_fit is 'no_ode'.
It is an integer used to set the random seed in the dismod_at option table.
If random_seed is zero, the sysetem clock rounded to one second is used
to do random seeding.
''',

'sample':'''
sample:
If the explicit text 'sample' is present in the syntax, this command samples
from the posterior distribution for the model variables using a dismod_at
sample command.  These samples correspond to the fit specified by which_fit.
The dismod_at asymptotic sampling method is used. Running this command
adds a std error plot sub-plot to the rate plots in the corresponding fit
directory. It also adds the sample table to the correspnding database.
''',

'number_sample':'''
number_sample:
This specifies the number of samples of the posterior distribution
for the model variables that are simulated using the dismod_at sample comamnd.
''',

'relative_path':'''
relative_path:
This variable, in the python file for this disease,
is the relative path of database for this study; i.e.,
	/ihme/epi/at_cascage/relative_path
is the path on the IHME cluster. There also is a local copy at
	data_dir/realtive_path
is the path on the local machine; see the data_dir command line argument.
In the special case of the unknown disease, the relative path need not be
its location on the IHME cluster, but it must begin with 'data'.
''',

'max_per_integrand':'''
max_per_integrand:
This variable, in the python file for this disease,
is the maximum number of data points to include for any one integrand.
If the available data exceeds this number, a subset of size max_per_integrand
is randomly chosen.
''',

'tolerance_fixed':'''
tolerance_fixed:
This variable, in the python file for this disease,
is the convergence tolerance for the fixed effects.
''',

'max_num_iter_fixed':'''
max_num_iter_fixed:
This variable, in the python file for this disease,
is the maxium number of optimizer iterations when optimizing the ixed effects.
''',

'ode_hold_out_list':'''
ode_hold_out_list:
This variable, in the python file for this disease,
is a list of integrand names that are in includd when fitting without ode
but not with ode.
''',

'max_covariate_effect':'''
max_covariate_effect:
This variable, in the python file for this disease, is the
maximum absolute covariate effect; i.e, multiplier * (covariate - referece).
Note that exp(effect) multiplies a model value to get another model value
corresponding to a covariate value. Noise covariate multipliers are not
included in the maximum.
''',

'set_mulcov_value':'''
This option may be necessary to get statstics for a fit where
one or more of the covariate multipliers is at its upper or lower bound.
set_mulcov_value is a list with each entry a list with the following structure:
	[covariate_name, rate_or_integrand_name, mulcov_value]
covariate_name:         is the covariate in the dismod_at database.
rate_or_integrand_name: is the rate of integrand for this multiplier.
mulcov_value:           is the value we are setting the multiplier to.
This overrides the max_covariate_effect setting for multipliers in this list.
''',

'parent_smoothing':'''
parent_smoothng:
This variable, in the python file for this disease, is
an ordered dictionary where keys are rate names for the smoothings
that are changed. The values are None or functions f with the syntax
(age_grid, time_grid, value_prior, dage_prior, dtime_prior) = f(
	age_table, time_table, density_name2id, integrand_data
)
If the function f = parent_smooth[rate_name] is None, or if it returns None,
then that parent and child rates are removed from the fit; i.e. they are zero.
The integrand data is intended to help constructing priors; e.g.,
mtexcess can help construct a prior for chi. Note that this only works when
there is mtexcess data.

''',
'child_smoothing':'''
child_smoothing:
It the same as parent_smoothing except that it is for the child rates.
In addition setting a child smothing to None does not affect the
correpsonding parent rates.
''',

'whats_new_2021':'''

01-22:
1. Automatic detection and setting of proper rate_case.
2. Move relative_path, max_sample, max_num_iter_fixed to disease specific file.
3. Change the smoothing function to be None, instead of returning None,
   when setting a rate to zero. This is simpler.

01-23:
1. Add dialysis to the disease list.
2. Add the relative path to the log for each disease.
3. Setting parent rates to zero also sets corresponding child rates to zero.
4. Add help syntax to fit_ihme.py


01-24:
1. Print and log the actual limits (in addition to the median factor) for
   add_mesas_noise_mulcov and set_mulcov_bound.
2. Change the add_meas_noise_mulcov terms to use variances instead of
   standard deviations.

01-25:
1. Correct /ihme/epi/at_cascade -> /share/epi/at_cascade.
2. Fix some help spelling errors.
3. Change plot title to only put location on first line.

01-27:
1. Change command line order to data_dir, disease, which_fit, random_seed.
2. Change so continues from previous fit (do not have to redo fits).
3. Add the sample command see 'help number_sample'.
4. Automatically drop covarites multipliers for useless covariates because
   they would cause the statistics to fail.

01-28:
1. Plot standard errors as function of age, time and in same figure as rate.
2. Better avoidance of log of zero ploting rates and standard errors.
3. Do not plot standard errors for omega (for ihme they are always zero).
4. Fix problem with y axis titlles for rate plots outside of figure boundary.

01-29:
1. More discussion of the disease specific file in 'fit_ihme.py help disease'.
2. Better choice of smooth_name for the new smoothings added by fit_ihme.py.

02-02:
1. Fix the set_mulcov_bound routine.

02-03:
1. Increase maximum dynamic range in rate plots (from 1e5 -> 1e6).
2. Report errors and warnings during sample in fit_ihme.log.
3. Do not abort (just report) when statistics cannot be calculated.

02-10:
1. Change set_mulcov_bound to be specific for each integrand. This avoids the
   problem where a covariate is equal to its reference for one integrand
2. Change priors for parent pini from uniform to gaussian so it does not go
   wild during no_ode fit (when there is no data to determine pini).

02-17:
1. All of the rate_value covariate multipliers were mistakenly constrained to
   zero. This has been fixed.
2. The max_covariate_effect for crohns diesease was changed from 2.0 -> 4.0.
   The fit works fine this way and the optimal iota/x_0 multiplier is
   no longer at its upper bound (x_0 corresponds to sdi).

02-20
1. Fix set_mulcov_bound so it takes a covariates max_difference into account
   when setting the bound.
2. Change the max_covariate_effect for crohns back to 2.0.
3. Add set_mulcov_value diesease specific option.
4. Fix warning that dialysis does not come with install (it does)'

03-04:
1. Fix the printing of the set_mulcov_bound lower and upper bounds so they
   are integrand specific both on standard output aind in fit_ihme.log.
2. Remove the printing of 'stop' directly after fit_ihme.py: OK.

03-05:
1. If the parent_smoothing or child_smoothing functions returns None, it's
	the same as if the function is None; i.e., corresponding rates are zero.

03-22:
1. Print out median of data when setting eta in data likelihood.
2. Set reference for covariates after subsampling data.

03-24:
1. Change dialysis parent priors from uniform to log-gaussian with large std.
   This seems to speed up convergence without affecting results.
2. Change dialysis time grid to better reflect data times range and variability.

03-25:
2. Change dialysis.py to use global data.

05-16:
1. Change max_sample -> max_per_integrand
   to avoid confusion with sample command.
2. Split the documentation for number_sample into the help topics 'sample' and
   'number_sample' (so that every command line argument has a help topic).
3. Include a descriptive message above the list of help topics printed by
   fit_ihme.py help

05-17:
1. Add the 'unknown' disease type. This can be used to applying fit_ihme.py
   to a database that does not come from the /ihme/epi/at_cascade directory.
2. In the add_meas_noise routine choose the prior_name so that it better avoids
   conflict with a previous prior. If there already was a meas_noise multiplier
   for this covariate and integrand, use it and do not add another one.

06-10:
1. Use the new hold_out_integrand setting to switch between no_ode and yes_ode
   fitting (this simplified the fit_ihme.py code).

06-13:
1. Use the new hold_out_commnad to randomly subsample the data.
   This simplifies the fit_ihme.py code and also provides residuals
   for the data not inclueded because it is held out instead of removed.
2. random_seed is not used to set value in dismod_at option table
   so that the results of the hold_out_command can be reproducible.

06-16:
1. Use the new compres interval options to simplify the fit_ihme.py code.

06-22:
1. Use the new bnd_mulcov command to improve and simplify the fit_ihme.py code.

06-23:
1. Use the new data_density command to improve and simplify fit_ihme.py code.

06-24:
1. It is no longer necessary to subset the data so remove that step.
   Now we get residuals for data what was originaly help out; e.g. mtall.

07-05:
1. Move tolerance_fixed to the disease specific files.
2. Automatically add number at end of prior names in parent_smoothing and
   child_smoothing (so that prior names are unique).

07-15:
1. Set the new trace_init_fit_model option to true (this gives feed back
   during initialization of large models).
'''
}
# help cases
if len(sys.argv) == 2 :
	if sys.argv[1] == 'help' :
		msg = 'Get help for a specific topic using fit_ihme.py help topic\n'
		msg += 'where topic is one of the following:'
		print(msg)
		for key in user_help_message_dict :
			print( '    ' + key)
		sys.exit(0)
if len(sys.argv) == 3 and sys.argv[1] == 'help' :
	if sys.argv[2] in user_help_message_dict :
		print( user_help_message_dict[sys.argv[2]] )
		sys.exit(0)
	msg = '{} is not a valid help topic'.format(sys.argv[2])
	print(msg)
	sys.exit(1)
#
# parse command line arguments
if len(sys.argv) not in [4, 5, 6] :
	print( user_help_message_dict['usage'] )
	sys.exit(1)
if len(sys.argv) == 6 and sys.argv[4] != 'sample' :
	msg = 'The text "sample" must preceed the number_sample argument'
	sys.exit(msg)
#
# data_dir_arg
data_dir_arg = sys.argv[1]
if not os.path.isdir(data_dir_arg) :
	msg = 'data_dir = {} is not a directory'.format(data_dir_arg)
	sys.exit(msg)
#
# disease_arg
disease_arg = sys.argv[2]
if disease_arg not in \
	[ 'crohns', 'dialysis', 'kidney', 't1_diabetes', 'unknown' ] :
	msg  = 'Warning: disease = {} is not one that comes with the install\n'
	msg += 'You must have added the file site-packages/dismod_at/{}.py'
	print( msg.format(disease_arg, disease_arg) )
#
# which_fit_arg
which_fit_arg = sys.argv[3]
if which_fit_arg not in [ 'no_ode', 'yes_ode', 'students' ] :
	msg = 'which_fit = {} is not one of following: no_ode, yes_ode, students'
	sys.exit( msg.format(which_fit_arg) )
#
if len(sys.argv) == 4 and which_fit_arg == 'no_ode' :
	msg  = 'random_seed or sample is required when which_fit is no_ode.'
	sys.exit(msg)
#
# sample_command
# random_seed_arg
# number_sample_arg
sample_command    = False
random_seed_arg   = None
number_sample_arg = None
if 4 < len( sys.argv ) :
	sample_command = sys.argv[4] == 'sample'
	if not sample_command :
		if which_fit_arg != 'no_ode' :
			msg  = 'random_seed should not be present when '
			msg += 'which_fit is {}'.format(which_fit_arg)
			sys.exit(msg)
		if which_fit_arg == 'no_ode' :
			# random_seed
			random_seed_arg = sys.argv[4]
			if not random_seed_arg.isdigit() :
				msg  = 'random_seed = {} not a positive integer '
				msg += 'without sign in front'
				sys.exit( msg.format(random_seed_arg) )
	if sample_command :
		if len(sys.argv) != 6 :
			msg = 'sample comamnd requires number_sample argument'
			sys.exit(msg)
		# number sample
		number_sample_arg = sys.argv[5]
		if not number_sample_arg.isdigit() :
			msg  = 'number_sample = {} not a positive integer '
			msg += 'without sign in front'
			sys.exit( msg.format(number_sample_arg) )

# ----------------------------------------------------------------------
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
# dismod_at imports
import dismod_at
#
try :
	disease_module = f'dismod_at.ihme.{disease_arg}'
	specific=  importlib.import_module(disease_module, package = dismod_at)
except Exception as e:
	print('\n')
	print(e)
	msg  = f'You must add the file {disease_arg}.py to the directory\n'
	msg += 'site-packages/dismod_at/ihme'
	sys.exit(msg)
# ----------------------------------------------------------------------------
disease_directory = None
temp_database     = None
def setup() :
	# set up some globals
	global disease_directory
	global temp_database
	#
	# directory for this disease
	disease_directory = data_dir_arg + '/' + disease_arg
	#
	# temporary database that gets modified
	temp_database     = disease_directory + '/temp.db'
	#
	if sample_command :
		fit_database  = disease_directory + '/' + which_fit_arg
		fit_database += '/' + which_fit_arg + '.db'
		shutil.copyfile(fit_database, temp_database)
	elif which_fit_arg == 'no_ode' :
		original_database = data_dir_arg + '/' + specific.relative_path
		if not os.path.exists(disease_directory) :
			os.mkdir(disease_directory)
		shutil.copyfile(original_database, temp_database)
	elif which_fit_arg == 'yes_ode' :
		previous_database = disease_directory + '/no_ode/no_ode.db'
		shutil.copyfile(previous_database, temp_database)
	elif which_fit_arg == 'students' :
		previous_database = disease_directory + '/yes_ode/yes_ode.db'
		shutil.copyfile(previous_database, temp_database)
	else :
		assert False
	#
setup()
# ===========================================================================
# Utilities that do not use global data tables
# ===========================================================================
fp_log_file = None
def trace(message = None, operation = None) :
	# Print output and manage log file
	#
	global fp_log_file
	#
	# message
	if operation is None :
		assert fp_log_file is not None
		assert message is not None
		print(message)
		fp_log_file.write(message + '\n')
		return
	assert message is None
	#
	import datetime
	#
	# date_time (drop faction of a second)
	date_time = str( datetime.datetime.now() )
	date_time = date_time[ : date_time.rfind('.') ]
	#
	# start
	if operation == 'start' :
		assert fp_log_file is None
		log_file    = disease_directory + '/fit_ihme.log'
		fp_log_file = open(log_file, 'w')
		msg  = 79 * '-' + '\n'
		msg += 'Starting log file: {}\n\n'.format(date_time)
		fp_log_file.write( msg )
		return
	#
	# continue
	if operation == 'continue' :
		assert fp_log_file is None
		log_file    = disease_directory + '/fit_ihme.log'
		if not os.path.exists(log_file) :
			msg = 'which_fit = {} but cannot file the log file\n{}'
			sys.exit( msg.format(which_fit_arg, log_file) )
		fp_log_file = open(log_file, 'a')
		msg  = 79 * '-' + '\n'
		msg += 'Continuing log file: {}\n\n'.format(date_time)
		fp_log_file.write( msg )
		return
	#
	# stop
	assert operation == 'stop'
	fp_log_file.close()
	fp_log_file = None
	return
# ----------------------------------------------------------------------------
def new_fit_directory(which_fit) :
	assert which_fit in [ 'no_ode', 'yes_ode', 'students' ]
	#
	# fit_directory
	fit_directory = disease_directory + '/' + which_fit
	#
	# if necessary, create this fit directory
	if os.path.exists(fit_directory) :
		# erase previous contentes of this fit_directory
		for name in os.listdir(fit_directory) :
			os.remove(fit_directory + '/' + name)
	if not os.path.exists(fit_directory) :
		os.mkdir(fit_directory)
	#
	# database for this fit
	fit_database = fit_directory + '/' + which_fit + '.db'
	shutil.copyfile(temp_database, fit_database )
	#
	# csv files
	system_command( [ 'dismodat.py',  fit_database, 'db2csv' ] )
	#
	integrand_list_yes_ode = get_integrand_list(True)
	integrand_list_no_ode  = get_integrand_list(False)
	integrand_list_all     = integrand_list_yes_ode + integrand_list_no_ode
	#
	# plot rate
	for row in rate_table :
		if row['parent_smooth_id'] is not None :
			rate_name = row['rate_name']
			plot_rate(row['rate_name'], fit_directory, which_fit)
	#
	# plot data
	for integrand_name in integrand_list_all :
		plot_integrand(integrand_name, fit_directory, which_fit)
	#
	# plot predictions
	predict_integrand_list   = [ 'susceptible', 'withC' ]
	covariate_integrand_list = integrand_list_yes_ode
	plot_predict(
		covariate_integrand_list,
		predict_integrand_list,
		fit_directory,
		which_fit
	)
# ----------------------------------------------------------------------------
def case_study_title(location, which_fit = None) :
	# return the title for this study and fit
	if disease_arg == 'unknown' :
		return 'unknown'
	sex_map       = { '1':'male', '2':'female', '3':'both' }
	text          = specific.relative_path.split('/')
	#
	version       = text[1]
	sex           = sex_map[ text[4] ]
	disease       = disease_arg
	result        = location
	if which_fit is not None :
		result += '\n' + which_fit
	result += ', {}, {}, version={}'.format(disease, sex, version);
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
def system_command (command_list, abort_on_error=True) :
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
		if abort_on_error :
			sys.exit('fit_ihme.py: system command failed')
		else :
			print('fit_ihme.py: system command failed')
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
def check_last_command(command, which_fit) :
	# Trace any errros or warnings and return.
	# In addition, return True for no errors or warnings and False otherwise.
	table_name = 'log'
	(table, col_name, col_type) = get_table(table_name);
	#
	# search for end of log entry for last fit
	log_id              = len(table)
	begin_last_command  = False
	while not begin_last_command :
		assert log_id > 0
		log_id -= 1
		row     = table[log_id]
		if row['message_type'] == 'command' :
			message = row['message']
			begin_last_command = message.startswith('begin ')
	#
	# check for the expected command
	if not message.startswith(f'begin {command}') :
		msg  = f'expected begin {command} but fouund\n'
		msg += message
		sys.exit(msg)
	#
	msg             = ''
	log_id         += 1
	end_last_command = log_id == len(table)
	while not end_last_command :
		row          = table[log_id]
		message_type = row['message_type']
		if message_type == 'command' :
			message = row['message']
			assert message.startswith(f'end {command}')
			end_last_command = True
		else :
			if message_type in [ 'warning', 'error' ] :
				msg += message_type + ' ' + row['message'] + '\n'
			log_id += 1
			if log_id == len(table) :
				end_last_command = True
				msg += f'Did not find end for this {command} command\n'
	#
	if msg == '' :
		msg = '\n{}_{} OK\n'.format(command, which_fit)
		trace(msg)
		return True
	else :
		msg = f'\n{command}_{which_fit} Errors and or Warnings::\n' + msg
		trace(msg)
		return False
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
	# Plot the fit_var parent node model values for a specified rate.
	# The values are plotted on the parent_node grid
	# If sample command, the standad deviation for each rate is also plotted.
	# These are repeated, once with age as the independent variables,
	# and the other with time as the independent variable.
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
	# sample
	if sample_command :
		table_name = 'sample'
		(sample_table, col_name, col_type) = get_table(table_name)
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
	# n_age, n_time, n_var
	n_age  = smooth_table[smooth_id]['n_age']
	n_time = smooth_table[smooth_id]['n_time']
	n_var  = len(var_table)
	#
	# sort triple_list first by age and then by time
	key = lambda triple : pair( triple[0], triple[1] )
	triple_list = sorted(triple_list, key = key )
	#
	# creaate the mesghgird (age, time, rate)
	age  = numpy.zeros( (n_age, n_time), dtype = float)
	time = numpy.zeros( (n_age, n_time), dtype = float)
	rate = numpy.zeros( (n_age, n_time), dtype = float)
	if sample_command :
		n_sample = int( number_sample_arg )
		assert len(sample_table) == n_sample * n_var
		std  = numpy.zeros( (n_age, n_time), dtype = float)
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
			if sample_command and rate_name != 'omega' :
				sumsq = 0.0
				for k in range(n_sample) :
					sample_id = k * n_var + var_id
					var_value = sample_table[sample_id]['var_value']
					sumsq    += (var_value - rate[i, j])**2
				std[i, j] = numpy.sqrt(sumsq / n_sample)
	#
	rate_max  = numpy.max(rate) * 1.05
	rate_min  = numpy.min(rate) * 0.95
	rate_min  = max(rate_min , rate_max * 1e-6)
	n_subplot = 1
	if sample_command and rate_name != 'omega' :
		std_max   = numpy.max(std) * 1.05
		std_min   = numpy.min(std) * 0.95
		std_min   = max(std_min, std_max * 1e-5)
		# no standard error subplot when it is identically zero
		if std_min > 0 :
			n_subplot = 2
	#
	from matplotlib import pyplot
	#
	import matplotlib.backends.backend_pdf
	file_name = directory + '/' + rate_name + '.pdf'
	pdf = matplotlib.backends.backend_pdf.PdfPages(file_name)
	#
	# ------------------------------------------------------------------------
	# for each time, plot rate and possibly std as a function of age
	# ------------------------------------------------------------------------
	n_fig       = math.ceil( n_time / ( n_color_style - 1) )
	n_per_fig   = math.ceil( n_time / n_fig )
	assert n_per_fig < n_color_style
	#
	color_index = -1
	#
	for i_fig in range( n_fig ) :
		# save for possible re-use by second subplot
		save_color_index = color_index
		#
		fig    = pyplot.figure()
		fig.subplots_adjust( hspace = .01 )
		#
		# axis for subplot and title for figure
		axis   = pyplot.subplot(n_subplot, 1, 1)
		axis.set_title( case_study_title(parent_node_name, which_fit) )
		#
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
			# avoid values less than or equal zero
			y     = numpy.maximum(y, rate_min)
			# extend as constant to min and max age
			x     = [age_min] + x.tolist() + [age_max]
			y     = [y[0]]    + y.tolist() + [y[-1]]
			# label used by legend
			label = str( time[0,j] )
			#
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
		axis.set_position([
			box.x0 + box.width*.05 , box.y0, box.width*0.85, box.height
		])
		axis.legend(
			title = 'time', loc='center left', bbox_to_anchor=(1, 0.5)
		)
		# --------------------------------------------------------------------
		if n_subplot == 2 :
			# restart colors so are the same as for first subplot
			# (only need one legend for both subplots)
			color_index = save_color_index
			#
			# ais for subplot (uses same title as figure)
			axis   = pyplot.subplot(n_subplot, 1, 2)
			#
			start  = i_fig * n_per_fig
			if i_fig > 0 :
				start        = start - 1
				color_index -= 1
			stop   = min(n_time, start + n_per_fig )
			for j in range(start, stop) :
				color_index    = (color_index + 1) % n_color_style
				(color, style,) = color_style_list[color_index]
				x     = age[:,j]
				y     = std[:,j]
				# avoid values less than or equal zero
				y     = numpy.maximum(y, std_min)
				# extend as constant to min and max age
				x     = [age_min] + x.tolist() + [age_max]
				y     = [y[0]]    + y.tolist() + [y[-1]]
				# label used by legend
				label = str( time[0,j] )
				#
				pyplot.plot(x, y, label=label, color=color, linestyle=style)
				#
				# axis labels
				pyplot.xlabel('age')
				pyplot.ylabel('std error')
				pyplot.yscale('log')
				pyplot.ylim(std_min, std_max)
			for i in range(n_age) :
				x = age[i, 0]
				pyplot.axvline(x, color='black', linestyle='dotted', alpha=0.3)
			# Shrink current axis by 15% but do not need legend this time
			box = axis.get_position()
			axis.set_position([
				box.x0 + box.width*.05 , box.y0, box.width*0.85, box.height
			])
		# --------------------------------------------------------------------
		pdf.savefig( fig )
		pyplot.close( fig )
	# ------------------------------------------------------------------------
	# for each age, plot rate as a function of time
	# ------------------------------------------------------------------------
	n_fig       = math.ceil( n_age / (n_color_style - 1) )
	n_per_fig   = math.ceil( n_age / n_fig )
	assert n_per_fig < n_color_style
	#
	color_index = -1
	#
	for i_fig in range( n_fig ) :
		# save for possible re-use by second subplot
		save_color_index = color_index
		#
		# new figure
		fig    = pyplot.figure()
		fig.subplots_adjust( hspace = .01 )
		#
		# axis for subplot and title for figure
		axis   = pyplot.subplot(n_subplot, 1 ,1)
		axis.set_title( case_study_title(parent_node_name, which_fit) )
		#
		start  = i_fig * n_per_fig
		if i_fig > 0 :
			start        = start - 1
			color_index -= 1
		stop   = min(n_age, start + n_per_fig )
		for i in range(start, stop) :
			color_index    = (color_index + 1) % n_color_style
			(color, style) = color_style_list[color_index]
			x     = time[i,:]
			y     = rate[i,:]
			# avoid values less than or equal zero
			y     = numpy.maximum(y, rate_min)
			# extend as constant to min and max time
			x     = [time_min] + x.tolist() + [time_max]
			y     = [y[0]]     + y.tolist() + [y[-1]]
			# label used by legend
			label = str( age[i,0] )
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
		axis.set_position([
			box.x0 + box.width*.05 , box.y0, box.width*0.85, box.height
		])
		axis.legend(
			title = 'age', loc='center left', bbox_to_anchor=(1, 0.5)
		)
		# --------------------------------------------------------------------
		if n_subplot == 2 :
			# restart colors so are the same as for first subplot
			# (only need one legend for both subplots)
			color_index = save_color_index
			#
			# axis for subplot (uses same title as figure)
			axis   = pyplot.subplot(n_subplot, 1, 2)
			#
			start  = i_fig * n_per_fig
			if i_fig > 0 :
				start        = start - 1
				color_index -= 1
			stop   = min(n_age, start + n_per_fig )
			for i in range(start, stop) :
				color_index    = (color_index + 1) % n_color_style
				(color, style) = color_style_list[color_index]
				x     = time[i,:]
				y     = std[i,:]
				# avoid values less than or equal zero
				y     = numpy.maximum(y, std_min)
				# extend as constant to min and max time
				x     = [time_min] + x.tolist() + [time_max]
				y     = [y[0]]     + y.tolist() + [y[-1]]
				# label used by legend
				label = str( age[i,0] )
				pyplot.plot(x, y, label=label, color=color, linestyle=style)
				#
				# axis labels
				pyplot.xlabel('time')
				pyplot.ylabel('std error')
				pyplot.yscale('log')
				pyplot.ylim(std_min, std_max)
			for j in range(n_time) :
				x = time[0, j]
				pyplot.axvline(x, color='black', linestyle='dotted', alpha=0.3)
			# Shrink current axis by 15% but do not ned legent this time
			box = axis.get_position()
			axis.set_position([
				box.x0 + box.width*.05 , box.y0, box.width*0.85, box.height
			])
		# --------------------------------------------------------------------
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
# -----------------------------------------------------------------------------
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
# ============================================================================
# Routines that Change Input Tables
# ============================================================================
def new_smoothing(
	smooth_name, age_grid, time_grid, value_prior, dage_prior, dtime_prior
) :
	# Add a new smoothing that has one prior that is used for all age and
	# time grid points. The smooth, smooth_grid, age, and time tables are
	# modified, but the new versions are not written by this routine.
	# The arguments value_prior, dage_prior, dtime_prior,
	# contain the priors used in the smothing.
	# The prior_id is added to the prior names to help make them uniquue.
	# The argument smooth_name is the name used in the smoothing grid table
	# for this smoothing.
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
	#
	def copy_prior(prior) :
		new_id               = len( prior_table )
		prior_name           = '{}_{}'.format( prior['prior_name'] , new_id )
		result               = copy.copy( prior )
		result['prior_name'] = prior_name
		return result
	n_age          = len(age_grid)
	n_time         = len(time_grid)
	age_id_list    = table_value2id(age_table,  'age',  age_grid)
	time_id_list   = table_value2id(time_table, 'time', time_grid)
	new_smooth_id  = len( smooth_table )
	#
	# add value_prior to prior_table
	new_value_prior_id  = len(prior_table)
	prior_table.append( copy_prior( value_prior ) )
	#
	# add dage_prior to prior table
	new_dage_prior_id  = len(prior_table)
	prior_table.append( copy_prior( dage_prior   ) )
	#
	# add dtime_prior to prior table
	new_dtime_prior_id  = len(prior_table)
	prior_table.append( copy_prior( dtime_prior   ) )
	#
	# add row to smooth_table
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
	new_row['smooth_name'] = 'bound_smoothing_' + str( new_smooth_id )
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
def zero_parent_and_child_rate(rate_name) :
	# zero a rate's parent and child values
	#
	msg = '\nzero_parent_and_child_rate\nrate= {}'.format(rate_name)
	trace( msg)
	#
	for row in rate_table :
		if row['rate_name'] == rate_name :
				row['parent_smooth_id'] = None
				row['child_smooth_id']  = None
				row['child_nslist_id']  = None
	put_table('rate', rate_table, rate_col_name, rate_col_type)
def zero_child_rate(rate_name) :
	# zero a rate's child values
	#
	msg = '\nzero_child_rate\nrate= {}'.format(rate_name)
	trace( msg)
	#
	for row in rate_table :
		if row['rate_name'] == rate_name :
				row['child_smooth_id']  = None
				row['child_nslist_id']  = None
	put_table('rate', rate_table, rate_col_name, rate_col_type)
# -----------------------------------------------------------------------------
def set_covariate_reference (covariate_id, reference_name) :
	# set the reference value for a specified covariate where reference_name
	# is 'mean' or 'median'. This also removes any bounds on the covariate
	# difference (it is assumed data outsude the desired bounds has already
	# been removed).
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
	max_difference = covariate_table[covariate_id]['max_difference']
	if max_difference is None :
		max_difference = numpy.inf
	#
	covariate_table[covariate_id]['reference'] = new_reference
	#
	msg  = '\nset_covariate_reference\n'
	msg += 'for covariate = {}'.format(covariate_name)
	msg += ', covariate_id = {}'.format(covariate_id)
	msg += ', reference_name = {}'.format(reference_name)
	msg += '\nold_reference = {:.5g}'.format(old_reference)
	msg += ', new_reference = {:.5g}'.format(new_reference)
	msg += '\nmax_difference = {:.5g}'.format(max_difference)
	trace( msg )
	#
	table_name = 'covariate'
	put_table(
		table_name, covariate_table, covariate_col_name, covariate_col_type
	)
# -----------------------------------------------------------------------------
def set_mulcov_value(covariate_name, rate_or_integrand_name, mulcov_value) :
	# Set the value for a specific covariate multiplier.
	# The corresponding multiplier must be in the covariate table.
	# Noise covariate multipliers are not included.
	#
	match_type = None
	for row in mulcov_table :
		match_this_row  = False
		covariate_id    = row['covariate_id']
		cov_name        = covariate_table[covariate_id]['covariate_name']
		mulcov_type   = row['mulcov_type']
		if cov_name == covariate_name and mulcov_type == 'rate_value' :
			rate_id  = row['rate_id']
			if rate_id is None :
				rate_name = None
			else :
				rate_name     = rate_table[rate_id]['rate_name']
			if rate_name == rate_or_integrand_name :
				assert match_type is None
				match_type     = 'rate'
				match_this_row = True
		if cov_name == covariate_name and mulcov_type == 'meas_value' :
			integrand_id  = row['integrand_id']
			if integrand_id is None :
				integrand_name = None
			else :
				integrand_name =integrand_table[integrand_id]['integrand_name']
			if integrand_name == rate_or_integrand_name :
				assert match_type is None
				match_type     = 'integrand'
				match_this_row = True
		if match_this_row :
			lower = mulcov_value
			upper = mulcov_value
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
	msg  = '\nset_mulcov_value\n'
	msg += 'covariate = {}, {}  = {}, value = {:.5g}'
	msg  = msg.format(
		covariate_name, match_type, rate_or_integrand_name, mulcov_value
	)
	if match_type is None :
		sys.exit(msg)
	trace( msg )
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
	msg += '{:11} = {} '.format('parent',  parent_rate)
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
	smooth_id   = len(smooth_table)
	smooth_name = '{}_smoothing_{}'.format(rate_name, smooth_id)
	smooth_id = new_smoothing(
		smooth_name, age_grid, time_grid, value_prior, dage_prior, dtime_prior
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
	# Add a meas_noise covariate multiplier for a specified integrand
	# if one does not already exist. Note that meas_noise multipliers can't
	# have ramdom effect (so the subgroup id is null in the mulcov table).
	#
	# integrand_data: is the current result of get_integrand_data.
	# integrand_name: specifies which integrand the multiplier is for.
	# group_id: specifies the group for the covariate multiplier.
	#
	# factor:
	# is a dictionary with following keys: mean, lower, upper.
	# For each key the factor multipliers the absolute value of the
	# median of the data for this integrand to get the corresponding value
	# in the uniform prior for the square root of the covariate multiplier.
	# In other words, the factor is times a value is in standard deviation
	# units, while the prior values are in variance units.
	#
	# return:
	# if there already is a meas_noise covariate multiplier for this
	# integrand the new multiplier is not added and this routine returns false.
	# Otherwise it returns true and the new multiplier is reported using trace.
	assert 0.0             <= factor['lower']
	assert factor['lower'] <= factor['mean']
	assert factor['mean']  <= factor['upper']
	#
	# integrand_id
	integrand_id = integrand_name2id[integrand_name]
	#
	# check if there already is a meas_noise multiplier for this integrand
	for row in mulcov_table :
		if row['mulcov_type'] == 'meas_noise' :
			if row['integrand_id'] == integrand_id :
				return False
	#
	group_name = None
	for row in subgroup_table :
		if row['group_id'] == group_id :
			group_name = row['group_name']
	assert group_name is not None
	#
	median = abs( numpy.median( integrand_data[integrand_name] ) )
	lower  = ( median * factor['lower'] )**2
	mean   = ( median * factor['mean']  )**2
	upper  = ( median * factor['upper'] )**2
	#
	msg  = '\nadd_meas_noise_mulcov\n'
	msg += 'integrand = {}, group = {}, uniform value prior\n'
	msg  = msg.format(integrand_name, group_name)
	msg += 'lower = (|median|*{})^2 = {:.5g}\n'.format(factor['lower'], lower)
	msg += 'mean  = (|median|*{})^2 = {:.5g}\n'.format(factor['mean'],  mean)
	msg += 'upper = (|median|*{})^2 = {:.5g}\n'.format(factor['upper'], upper)
	msg += 'where median is the median of the {} data'.format(integrand_name)
	trace( msg )
	#
	# covariate_id
	covariate_id = identically_one_covariate()
	#
	# mulcov_id
	mulcov_id = len(mulcov_table)
	#
	# prior used in one point smoothing
	density_id = density_name2id['uniform']
	prior_name = integrand_name + '_meas_noise_value_prior'
	value_prior = {
		'prior_name' : prior_name     ,
		'density_id' : density_id     ,
		'lower'      : lower          ,
		'upper'      : upper          ,
		'mean'       : mean           ,
		'std'        : None           ,
		'eta'        : None           ,
		'nu'         : None           ,
	}
	dage_prior  = copy.copy( value_prior )
	prior_name = integrand_name + '_meas_noise_dage_prior'
	dage_prior['prior_name']  =  prior_name
	#
	dtime_prior = copy.copy( value_prior )
	prior_name = integrand_name + '_meas_noise_dtime_prior'
	dtime_prior['prior_name'] = prior_name
	#
	# new one point smoothing
	age_grid  = [ age_table[0]['age'] ]
	time_grid = [ time_table[0]['time'] ]
	smooth_id = len(smooth_table)
	smooth_name = '{}_noise_smoothing_{}'.format(integrand_name, smooth_id)
	smooth_id = new_smoothing(
		smooth_name, age_grid, time_grid, value_prior, dage_prior, dtime_prior
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
	#
	# return true because we have added the covariate multiplier
	return True
# ============================================================================
# Begin Main Program
# ============================================================================
#
# start_time
start_time = time.time()
#
# ----------------------------------------------------------------------------
# Changes to database
# ----------------------------------------------------------------------------
# integrand lists
integrand_list_yes_ode = get_integrand_list(True)
integrand_list_no_ode  = get_integrand_list(False)
integrand_list_all     = integrand_list_yes_ode + integrand_list_no_ode
# --------------------------------------------------------------------
if sample_command :
	#
	# continue the fit_ihme.log file
	trace(operation = 'continue')
	#
	# dismod_at sample command
	system_command(
		[
			'dismod_at',
			temp_database,
			'sample',
			'asymptotic',
			'both',
			number_sample_arg,
		], abort_on_error = False
	)
	if not check_last_command('sample', which_fit_arg) :
		print('Exiting due to problems with this sample command\n')
		sys.exit(0)
	#
	# fit_directory
	fit_directory = disease_directory + '/' + which_fit_arg
	#
	# copy the database (with samples in it)
	fit_database = fit_directory + '/' + which_fit_arg + '.db'
	shutil.copyfile(temp_database, fit_database )
	#
	# csv files (with samples)
	system_command( [ 'dismodat.py',  fit_database, 'db2csv' ] )
	#
	#
	for row in rate_table :
		if row['parent_smooth_id'] is not None :
			rate_name = row['rate_name']
			plot_rate(row['rate_name'], fit_directory, which_fit_arg)
	#
	msg  = '\nTotal time   = '
	msg += str( round( time.time() - start_time ) ) + ' seconds'
	trace( msg )
	trace('fit_ihme.py: OK')
	sys.exit(0)
# --------------------------------------------------------------------
if which_fit_arg == 'no_ode'  :
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
	if disease_arg != 'unknown' :
		msg = 'original database\n/ihme/epi/at_cascade/{}\n'
		trace( msg.format(specific.relative_path) )
	#
	# erase the database log table so log is just for this session
	sql_command('DROP TABLE IF EXISTS log')
	#
	# set reference value for x_0 to its median
	reference_name  = 'median'
	for covariate_id in range( len(covariate_table) ) :
		if relative_covariate(covariate_id) :
			set_covariate_reference(covariate_id, reference_name)
	#
	# integrand_data
	integrand_data = get_integrand_data()
	#
	# Set parent rate priors for this disease
	zero_parent_rate_set = set()
	for rate_name in specific.parent_smoothing :
		fun = specific.parent_smoothing[rate_name]
		#
		if fun is None :
			zero_parent_and_child_rate(rate_name)
			zero_parent_rate_set.add(rate_name)
		else :
			result = fun(
				age_table, time_table, density_name2id, integrand_data
			)
			if result is None :
				zero_parent_and_child_rate(rate_name)
				zero_parent_rate_set.add(rate_name)
			else :
				parent_rate = True
				(age_grid,time_grid,value_prior,dage_prior,dtime_prior)= result
				set_rate_smoothing(parent_rate, rate_name,
					age_grid, time_grid, value_prior, dage_prior, dtime_prior
				)
	#
	# Set child rate priors for this disease
	for rate_name in specific.child_smoothing :
		fun = specific.child_smoothing[rate_name]
		#
		if fun is None :
			if rate_name not in zero_parent_rate_set :
				zero_child_rate(rate_name)
		else :
			result = fun(
				age_table, time_table, density_name2id, integrand_data
			)
			if result is None :
				zero_child_rate(rate_name)
			else :
				parent_rate = False
				(age_grid,time_grid,value_prior,dage_prior,dtime_prior)= result
				set_rate_smoothing(parent_rate, rate_name,
					age_grid, time_grid, value_prior, dage_prior, dtime_prior
				)
	#
	# add measurement noise covariates
	group_id = 0
	factor   = { 'lower':1e-1, 'mean':1e-1, 'upper':1e-1 }
	for integrand_name in integrand_list_all :
		added = add_meas_noise_mulcov(
			integrand_data, integrand_name, group_id, factor
		)
		if not added :
			msg  = 'found meas_noise multiplier for ' + integrand_name
			msg += ' in original database'
			trace(msg)
	#
	# Covariate multipliers that we are setting a specific value for
	for row in specific.set_mulcov_value :
		covariate_name         = row[0]
		rate_or_integrand_name = row[1]
		mulcov_value           = row[2]
		set_mulcov_value(covariate_name, rate_or_integrand_name, mulcov_value)
	# -----------------------------------------------------------------------
	# init:
	system_command([ 'dismod_at', temp_database, 'init'])
	# ------------------------------------------------------------------------
	# randomly subsample the data
	max_fit_str = str(specific.max_per_integrand)
	for integrand_name in integrand_list_all :
		system_command([
			'dismod_at', temp_database, 'hold_out', integrand_name, max_fit_str
		])
	#
	# set bound for the covariate effect
	max_abs_effect_str = str(specific.max_covariate_effect)
	system_command([
		'dismod_at', temp_database, 'bnd_mulcov', max_abs_effect_str
	])
	# ------------------------------------------------------------------------
	# set options
	set_option('tolerance_fixed',       str(specific.tolerance_fixed) )
	set_option('max_num_iter_fixed',    str(specific.max_num_iter_fixed))
	set_option('quasi_fixed',           'false')
	set_option('zero_sum_child_rate',   'iota rho chi')
	set_option('bound_random',          '3')
	set_option('meas_noise_effect',     'add_var_scale_none')
	set_option('trace_init_fit_model',  'true')
	#
	# random_seed
	assert random_seed_arg is not None
	set_option('random_seed',         random_seed_arg)
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
	set_option('rate_case',           rate_case)
	#
	# hold_out_integrand
	hold_out_integrand = copy.copy( integrand_list_yes_ode )
	if iota_zero :
		hold_out_integrand.append('Sincidence')
	if rho_zero :
		hold_out_integrand.append('remission')
	if chi_zero :
		hold_out_integrand.append('mtexcess')
	hold_out_integrand = ' '.join(hold_out_integrand)
	set_option('hold_out_integrand',  hold_out_integrand)
	#
	# compress age and time intervals
	set_option('compress_interval', "10.0 10.0")
	# -------------------------------------------------------------------------
	# fit both
	t0 = time.time()
	system_command([ 'dismod_at', temp_database, 'fit', 'both'])
	msg  = 'fit_no_ode time = '
	msg += str( round( time.time() - t0 ) ) + ' seconds'
	trace(msg)
	#
	if not check_last_command('fit', which_fit_arg) :
		print('Exiting due to problems with this fit command\n')
		sys.exit(0)
	new_fit_directory(which_fit_arg)
# ------------------------------------------------------------------------
if which_fit_arg == 'yes_ode'  :
	#
	# continue the fit_ihme.log file
	trace(operation = 'continue')
	#
	# use previous fit as starting point
	system_command([
		'dismod_at', temp_database, 'set', 'start_var', 'fit_var'
	])
	#
	# hold_out_integrand
	hold_out_integrand = copy.copy( specific.ode_hold_out_list )
	iota_zero = rate_table[ rate_name2id['iota'] ]['parent_smooth_id'] is None
	rho_zero  = rate_table[ rate_name2id['rho'] ]['parent_smooth_id'] is None
	chi_zero  = rate_table[ rate_name2id['chi'] ]['parent_smooth_id'] is None
	if iota_zero :
		hold_out_integrand.append('Sincidence')
	if rho_zero :
		hold_out_integrand.append('remission')
	if chi_zero :
		hold_out_integrand.append('mtexcess')
	hold_out_integrand = ' '.join(hold_out_integrand)
	set_option('hold_out_integrand',  hold_out_integrand)
	#
	# fit both
	t0 = time.time()
	system_command([ 'dismod_at', temp_database, 'fit', 'both'])
	msg  = 'fit_yes_ode time = '
	msg += str( round( time.time() - t0 ) ) + ' seconds'
	trace(msg)
	#
	if not check_last_command('fit', which_fit_arg) :
		print('Exiting due to problems with this fit command\n')
		sys.exit(0)
	new_fit_directory(which_fit_arg)
# --------------------------------------------------------------------
if which_fit_arg == 'students'  :
	#
	# continue the fit_ihme.log file
	trace(operation = 'continue')
	#
	# change data likelihood to use students-t
	integrand_data = get_integrand_data()
	density_name   = 'log_students'
	factor_eta_str = '1e-2'
	nu_str         = '5'
	for integrand_name in integrand_list_all :
		system_command([
			'dismod_at', temp_database, 'data_density',
			integrand_name, density_name, factor_eta_str, nu_str
		])
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
	if not check_last_command('fit', which_fit_arg) :
		print('Exiting due to problems with this fit command\n')
		sys.exit(0)
	new_fit_directory(which_fit_arg)
# ----------------------------------------------------------------------
msg  = '\n'
msg += '\nTotal time   = '
msg += str( round( time.time() - start_time ) ) + ' seconds'
trace( msg )
trace('fit_ihme.py: OK')
sys.exit(0)
