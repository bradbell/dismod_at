#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin csv2db_function$$ $newlinech #$$
# $spell
#	Csv
#	Dismod
#	str
#	pini
#	integrands
#	Sincidence
#	mtexcess
#	mtother
#	Tincidence
#	mtspecific
#	mtall
#	mtstandard
#	relrisk
#	std
#	mtwith
# $$
#
# $section Example Conversion of a Csv File to a Dismod_at Database$$
#
# $head Under Construction$$
# This example is under construction.
#
# $head Syntax$$
# $codei%csv2db(%option_csv%, %data_csv%)%$$
#
# $head option_csv$$
# is an $code str$$ containing the option file name and must end with the
# $code .csv$$ extension.
# The first row contains the following column names
# $code name$$, $code value$$.
# Column names that begin with $code c_$$ are comments
# and will not be used by future versions of $code csv2db$$.
# The options are documented by the corresponding name value below
#
# $subhead database_name$$
# The $icode value$$ in this row is the name of the
# dismod_at database that is written by $code csv2db$$
# There is no default value for this name; i.e., it must appear.
#
# $subhead non_zero_rates$$
# The $icode value$$ in this row is a list rates that are non-zero
# in the model.
# The possible rates are
# $code pini$$, $code iota$$, $code rho$$, $code chi$$, $code omega$$.
# The rates in the list are separated by a single space
# and $code omega$$ must appear in the list.
# There is no default value for this name; i.e., it must appear.
#
# $head data_csv$$
# is an $code str$$ containing the data file name
# and must end with the $code .csv$$ extension.
# Each row of the data file corresponds to one data point.
# The first row of the file contains the column names.
# The other rows correspond to data points.
# Each column of the necessary columns in the data file is documented
# under its column name below.
# Column names that begin with $code c_$$ are comments
# and will not be used by future versions of $code csv2db$$.
#
# $head integrand$$
# This column of $icode data_csv$$ contains
# one of the following integrands:
#
# $subhead Sincidence$$
# The incidence rate relative to susceptible population:
# $latex \iota $$.
#
# $subhead remission$$
# The remission rate:
# $latex \rho $$.
#
# $subhead mtexcess$$
# The excess mortality rate:
# $latex \chi $$.
#
# $subhead mtother$$
# The other cause mortality rate:
# $latex \omega $$.
# Other cause mortality data is special in that it must satisfy
# $icode%age_lower% == %age_upper%$$,
# $icode%time_lower% == %time_upper%$$.
# In addition, it must be specified on a rectangular grid; i.e.,
# each age that appears must appear once and only once for every time, and
# each time that appears must appear once and only once for every age.
# The $code mtother$$ data is converted to a constraint on $latex \omega$$.
#
# $subhead mtwith$$
# The with condition mortality rate:
# $latex \omega + \chi $$.
#
# $subhead susceptible$$
# The susceptible fraction of the population:
# $latex S $$.
#
# $subhead withC$$
# The with condition fraction of the population:
# $latex C $$.
#
# $subhead prevalence$$
# The prevalence of the condition:
# $latex P = C / [ S + C ] $$.
#
# $subhead Tincidence$$
# The incidence rate relative to the total population:
# $latex \iota [ 1 - P ] $$.
#
# $subhead mtspecific$$
# The cause specific mortality rate:
# $latex \chi P $$.
#
# $subhead mtall$$
# The all cause mortality rate:
# $latex \omega + \chi P $$.
# We assume the $cref/mtother/csv2db_function/integrand/mtother/$$
# data is a surrogate
# for the $code mtall$$ data.
# The $code mtall$$ data is included as a check that the desired values
# are satisfied (by checking residuals in a data fit).
# For this reason, $cref/hold_out/csv2db_function/hold_out/$$ must be
# one for all the $code mtall$$ data.
#
# $subhead mtstandard$$
# The standardized mortality ratio:
# $latex [ \omega + \chi ] / [ \omega + \chi P ] $$.
#
# $subhead relrisk$$
# The relative risk:
# $latex [ \omega + \chi ] / \omega $$.
#
# $head age_lower$$
# This column of $icode data_csv$$ contains
# The initial age for averaging the integrand for this row; $latex b$$.
#
# $head age_upper$$
# This column of $icode data_csv$$ contains
# the final age for averaging the integrand for this row; $latex c$$.
#
# $head time_lower$$
# This column of $icode data_csv$$ contains
# the initial time for averaging the integrand for this row; $latex r$$.
#
# $head time_upper$$
# This column of $icode data_csv$$ contains
# the final time for averaging the integrand for this row; $latex s$$.
#
# $head meas_value$$
# This column of $icode data_csv$$ contains
# the value of the average integrand plus measurement noise
# $latex \[
#	e + \frac{1}{c-b} \frac{1}{s-r} \int_b^c \int_r^s I(a , t) \; da \; dt
# \] $$
# where $latex e$$ is the measurement noise,
# $latex a$$ and $latex t$$ are the age and time integration variables,
# and $latex I(a, t)$$ is the value of the integrand for this row.
#
# $head meas_std$$
# This column of $icode data_csv$$ contains
# the standard deviation of the measurement noise $latex e$$.
#
# $head hold_out$$
# This column of $icode data_csv$$ contains
# either zero or one. If it is one, this row is included
# dismod_at fits. Otherwise it is excluded.
# In either case, the residuals are computed for this row.
#
# $end
# -----------------------------------------------------------------------------
# BEGIN IMPORT
import sys
import os
import dismod_at
import csv
import copy
import numpy
import scipy.integrate
import distutils.dir_util
# END IMPORT
# BEGIN CSV2DB_SOURCE
# -----------------------------------------------------------------------------
def constant_weight_fun(a, t) :
	return 1.0
#
def uniform10_fun(a, t) :
	v  = 'uniform01'
	da = None
	dt = None
	return (v, da, dy)
#
def omega_constraint_fun(a, t, age_grid, time_grid, omega_grid) :
	i  = age_grid.index(a)
	j  = time_grid.index(t)
	v  = omega[i, j]
	da = None
	dv = None
	return (v, da, dv)
# -----------------------------------------------------------------------------
def csv2db(option_csv, data_csv) :
	#
	for file_name in [ option_csv, data_csv ] :
		if not file_name.endswith('.csv') :
			msg  = 'csv2db: option_csv file ' + file_name
			msg += ' does not end with .csv'
			sys.exit(msg)
		if not os.path.isfile(file_name) :
			msg = 'csv2db: cannot find option_csv file ' + file_name
			sys.exit(msg)
	# -------------------------------------------------------------------------
	# file_option
	file_ptr    = open(option_csv, 'r')
	reader      = csv.DictReader(file_ptr)
	file_option = dict()
	for row in reader :
		name              = row['name']
		value             = row['value']
		file_option[name] = value
	file_ptr.close()
	required = [ 'database_name', 'non_zero_rates' ]
	for name in required :
		if not name in file_option :
			msg = 'csv2db: ' + name + ' not in option_csv file ' + option_csv
			sys.exit(msg)
	if 'omega' not in file_option['non_zero_rates'] :
		msg = 'csv2db: omega not in non_zero_rates in ' + option_csv
		sys.exit(msg)
	# -------------------------------------------------------------------------
	# file_data
	file_ptr   = open(data_csv, 'r')
	reader     = csv.DictReader(file_ptr)
	file_data  = list()
	for row in reader :
		file_data.append( copy.copy(row) )
	file_ptr.close()
	#
	line  = 1
	mtother_found = False
	for row in file_data :
		line += 1
		row['age_lower']  = float( row['age_lower']  )
		row['age_upper']  = float( row['age_upper']  )
		row['time_lower'] = float( row['time_lower'] )
		row['time_upper'] = float( row['time_upper'] )
		row['meas_value'] = float( row['meas_value'] )
		row['meas_std']   = float( row['meas_std']   )
		row['hold_out']   = bool(  row['hold_out']   )
		#
		if row['integrand'] == 'mtother' :
			found = True
			if row['age_lower'] != row['age_upper'] :
				msg  = 'csv2db: line ' + str(line)
				msg += ' of data_csv ' + data_csv + '\n'
				msg += 'age_lower not equal age_upper for mtother data.'
				sys.exit(msg)
			if row['time_lower'] != row['time_upper'] :
				msg  = 'csv2db: line ' + str(line)
				msg += ' of file ' + data_csv + '\n'
				msg += 'time_lower not equal time_upper for mtother data.'
				sys.exit(msg)
		if row['integrand'] == 'mtall' :
			if not row['hold_out'] :
				msg  = 'csv2db: line ' + str(line)
				msg += ' of file ' + data_csv + '\n'
				msg += 'hold_out is not 1 for mtall data.'
				sys.exit(msg)
	if not mtother_found :
		msg = 'csv2db: no mtother data in ' + data_csv
		sys.exit(msg)
	# -------------------------------------------------------------------------
	# age_grid, time_grid, omega_grid
	age_set  = set()
	time_set = set()
	for row in file_data :
		if row['integrand'] == 'mtother' :
			age_set.add(  row['age_lower']  )
			time_set.add( row['time_lower'] )
	age_grid  = sorted(age_set)
	time_grid = sorted(time_set)
	n_age          = len(age_grid)
	n_time         = len(time_grid)
	count          = numpy.zeros( (n_age, n_time), dtype=int )
	omega_grid     = numpy.zeros( (n_age, n_time), dtype=float )
	for row in file_data :
		if row['integrand'] == 'mtother' :
			age        = row['age_lower']
			time       = row['time_lower']
			age_index  = age_grid.index(age)
			time_index = time_grid.index(time)
			omega_grid[age_index, time_index] = row['meas_value']
			count[age_index, time_index] += 1
	if not (count == 1).all() :
		msg = 'csv2db: the mtother data in ' + data_csv
		msg += ' is not on a rectangular grid'
		sys.exit(msg)
	# -------------------------------------------------------------------------
	# db_file
	file_name = os.path.basename(data_csv)[0:-4] + '.db'
	dir_name  = os.path.dirname(data_csv)
	db_file   = os.path.join(dir_name, file_name)
	# -------------------------------------------------------------------------
	# age_min, age_max
	age_min = + float('inf')
	age_max = - float('inf')
	for row in file_data :
		age_min = min(age_min, row['age_lower'], row['age_upper'])
		age_max = max(age_min, row['age_lower'], row['age_upper'])
	# -------------------------------------------------------------------------
	# time_min, time_max
	time_min = + float('inf')
	time_max = - float('inf')
	for row in file_data :
		time_min = min(time_min, row['time_lower'], row['time_upper'])
		time_max = max(time_min, row['time_lower'], row['time_upper'])
	# -------------------------------------------------------------------------
	# age_list, time_list
	age_list  = copy.copy( age_grid )
	time_list = copy.copy( time_grid )
	#
	if age_min < age_list[0] :
		age_list.insert(0, age_min)
	if time_min < time_list[0] :
		time_list.insert(0, time_min)
	#
	if age_max > age_list[-1] :
		age_list.append(age_max)
	if time_max > time_list[-1] :
		time_list.append(time_max)
	# -------------------------------------------------------------------------
	# integrand_table
	integrand_table = [
		{ 'name': 'Sincidence',     'minimum_meas_cv': 0.0 },
		{ 'name': 'remission',      'minimum_meas_cv': 0.0 },
		{ 'name': 'mtexcess',       'minimum_meas_cv': 0.0 },
		{ 'name': 'mtother',        'minimum_meas_cv': 0.0 },
		{ 'name': 'mtwith',         'minimum_meas_cv': 0.0 },
		{ 'name': 'susceptible',    'minimum_meas_cv': 0.0 },
		{ 'name': 'withC',          'minimum_meas_cv': 0.0 },
		{ 'name': 'prevalence',     'minimum_meas_cv': 0.0 },
		{ 'name': 'Tincidence',     'minimum_meas_cv': 0.0 },
		{ 'name': 'mtspecific',     'minimum_meas_cv': 0.0 },
		{ 'name': 'mtall',          'minimum_meas_cv': 0.0 },
		{ 'name': 'mtstandard',     'minimum_meas_cv': 0.0 },
		{ 'name': 'relrisk',        'minimum_meas_cv': 0.0 }
	]
	# -------------------------------------------------------------------------
	# node_table
	node_table = [ { 'name':'no_name', 'parent': '' } ]
	# -------------------------------------------------------------------------
	# weight_table
	fun          = constant_weight_fun
	weight_table = [
		{ 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
	]
	# -------------------------------------------------------------------------
	# covariate_table
	covariate_table = list()
	# -------------------------------------------------------------------------
	# avgint_table
	rate2integrand = {
		'iota ' : 'Sincidence',
		'rho'   : 'remission',
		'chi'   : 'mtexcess',
		'omega' : 'mtother'
	}
	non_zero_rates = file_option['non_zero_rates'].split()
	avgint_table   = [ 'prevalence' ]
	for rate in non_zero_rates :
		if rate != 'pini' :
			integrand = rate2integrand[rate]
			for age in age_grid :
				for time in time_grid :
					row = {
							'integrand': integrand,
							'node':      'no_name',
							'weight':    'constant',
							'age_lower':  age,
							'age_upper':  age,
							'time_lower': time,
							'time_lower': time
					}
					avgint_table.append(row)
	# -------------------------------------------------------------------------
	# data_table
	data_table = list()
	copy_column  = [
		'integrand',
		'hold_out',
		'age_lower',
		'age_upper',
		'time_lower',
		'time_upper',
		'meas_value',
		'meas_std'
	]
	for row in file_data :
		if row['integrand'] != 'mtother' :
			data_row = dict()
			for field in copy_column :
				data_row[field] = row[field]
		data_row['node']    = 'no_name'
		data_row['weight']  = 'constant'
		data_row['density'] = 'gaussian'
	# -------------------------------------------------------------------------
	# prior_table
	prior = {
		'name': 'uniform01',
		'lower': 0.0,
		'upper': 1.0,
		'density': 'uniform'
	}
	prior_table = [ prior ]
	# -------------------------------------------------------------------------
	# smooth_table
	start = 0
	if age_list[0] != age_grid[0] :
		start = 1
	age_id = range(start, len(age_grid) + 1)
	#
	start = 0
	if time_list[0] != time_grid[0] :
		start = 1
	time_id = range(start, len(time_grid) + 1)
	#
	# omega
	def omega_fun(a, t) :
		return omega_constrant_fun(a, t, age_grid, time_grid, omega_grid)
	smooth_table  = list()
	non_zero_rates = file_option['non_zero_rates'].split()
	for rate in non_zero_rates :
		if rate == 'omega' :
			smooth_table.append( {
				'name':     rate,
				'age_id':   age_id,
				'time_id':  time_id,
				'fun':      omega_fun,
		} )
		elif rate == 'pini' :
			smooth_table.append( {
				'name':     rate,
				'age_id':	[ age_id[0] ],
				'time_id':  time_id,
				'fun':      uniform01_fun
			} )
		else :
			smooth_table.append( {
				'name':     rate,
				'age_id':   age_id,
				'time_id':  time_id,
				'fun':      uniform01_fun,
		} )
	# -------------------------------------------------------------------------
	# rate_table
	non_zero_rates = file_option['non_zero_rates'].split()
	rate_table     = list()
	for rate in [ 'pini', 'iota', 'rho', 'chi', 'omega' ] :
		smooth = None
		for i in len(smooth_table) :
			if smooth_table[i].name == rate :
				smooth = smooth_table[i]
		rate_table.append( {
			'name':          rate,
			'parent_smooth': smooth
		} )
	# -------------------------------------------------------------------------
	# empty tables
	nslist_table = list()
	mulcov_table = list()
	option_table = list()
	# -------------------------------------------------------------------------
	create_database(
	     file_name,
	     age_list,
	     time_list,
	     integrand_table,
	     node_table,
	     weight_table,
	     covariate_table,
	     avgint_table,
	     data_table,
	     prior_table,
	     smooth_table,
	     nslist_table,
	     rate_table,
	     mulcov_table,
	     option_table
	)
# END CSV2DB_SOURCE
# ---------------------------------------------------------------------------
# $begin csv2db_source$$ $newlinech #$$
# $spell
#	csv
# $$
# $section csv2db: Source Code$$
#
# $head Source$$
# $srcfile%
#   example/user/csv2db.py
#   %0%# BEGIN IMPORT%# END IMPORT%0
# %$$
# $srcfile%
#   example/user/csv2db.py
#   %0%# BEGIN CSV2DB_SOURCE%# END CSV2DB_SOURCE%0
# %$$
#
# $end
# ---------------------------------------------------------------------------
# $begin csv2db_example$$ $newlinech #$$
# $spell
#	csv
#	integrands
#	mtexcess
#	mtall
# $$
# $section csv2db: Example and Test$$
#
# $head Discussion$$
# The following describes the mode and data for this example:
#
# $head rate_true$$
# The true value for the rates, used to simulate the data,
# are constant w.r.t age and time and are given by:
# $srcfile%example/user/csv2db.py%0%# BEGIN RATE_TRUE%# END RATE_TRUE%1%$$
#
# $head P$$
# The notation $latex P$$ is used for prevalence.
# The initial prevalence at age zero is zero; i.e. $latex P(0) = 0$$.
# We use $latex S(a)$$ ($latex C(a)$$) for the susceptible
# (with condition) fraction of the initial population.
# The true prevalence $latex P(a) = C(a) / [S(a) + C(a)]$$
# is solved for using the ODE:
# $latex \[
#	\begin{array}{rcl}
#	S(0)    & = & 1 \\
#	C(0)    & = & 0 \\
#	S'(a)   & = & - \iota S(a) + \rho C(a)  - \omega S(a) \\
#	C'(a)   & = & + \iota S(a) - \rho C(a)  - \omega C(a) - \chi C(a)
#	\end{array}
# \] $$
#
# $head Rate Grids$$
# The value of omega is modeled as know and equal to the
# value of
# $cref/mtall/csv2db_function/integrand/mtall/$$
# corresponding to the age-time intervals:
# $srcfile%example/user/csv2db.py%
#	0%# BEGIN INTERVALS%# END INTERVALS%1
# %$$
# The non-zero rates (iota, rho, chi) are modeled as unknown and piecewise
# bilinear with the same grid points.
#
# $head Data$$
# Data is simulated, without any noise, for the following integrands:
# $cref/remission/csv2db_function/integrand/remission/$$,
# $cref/mtexcess/csv2db_function/integrand/mtexcess/$$,
# $cref/prevalence/csv2db_function/integrand/prevalence/$$.
# Note that the model for the noise in the measurement is a 10 percent
# coefficient of variation.
#
# $head mtall$$
# The omega constraints correspond to
# $cref/mtall/csv2db_function/integrand/mtall/$$ data.
# As a check, we include the mtall data with hold_out equal to one.
#
# $head Source$$
# $srcfile%
#   example/user/csv2db.py
#   %0%# BEGIN IMPORT%# END IMPORT%0
# %$$
# $srcfile%
#   example/user/csv2db.py
#   %0%# BEGIN CSV2DB_EXAMPLE%# END CSV2DB_EXAMPLE%1
# %$$
#
# $end
# -------------------------------------------------------------------------------
# BEGIN CSV2DB_EXAMPLE
# check execution is from distribution directory
example = 'example/user/csv2db.py'
if sys.argv[0] != example  or len(sys.argv) != 1 :
	usage  = 'python3 ' + example + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# import get_started_db example
sys.path.append( os.getcwd() + '/example/get_started' )
import get_started_db
#
# change into the build/example/user directory
distutils.dir_util.mkpath('build/example/get_started')
os.chdir('build/example/get_started')
# ----------------------------------------------------------------------------
# BEGIN RATE_TRUE
rate_true = { 'iota':0.001, 'rho':0.1, 'chi':0.1, 'omega':0.01 }
# END RATE_TRUE
# ----------------------------------------------------------------------------
# compute P (prevalence) at integer ages 0, 1, ..., 100
def dSC_da(SC, a) :
	S     = SC[0]
	C     = SC[1]
	iota  = rate_true['iota']
	rho   = rate_true['rho']
	chi   = rate_true['chi']
	omega = rate_true['omega']
	#
	dS_da = - iota * S + rho * C - omega * S
	dC_da = + iota * S - rho * C - omega * C - chi * C
	return numpy.array( [dS_da, dC_da] )
SC0     = numpy.array( [ 1.0, 0.0 ] ) # initial prevalence is zero
age_ode = list( range(101) )
age_ode = numpy.array(age_ode, dtype = float )
SC      = scipy.integrate.odeint(dSC_da, SC0, age_ode)
S       = SC[:,0]
C       = SC[:,1]
P       = C / (S + C)
# ----------------------------------------------------------------------------
# option_csv
file_name  = 'option.csv'
file_ptr   = open(file_name, 'w')
fieldnames = [ 'name', 'value' ]
writer     = csv.DictWriter(file_ptr, fieldnames=fieldnames)
#
writer.writeheader()
row        = { 'name': 'database_name',  'value': 'example.db' }
writer.writerow( row )
row        = { 'name': 'non_zero_rates',  'value': 'iota row chi omega' }
writer.writerow( row )
file_ptr.close()
# ----------------------------------------------------------------------------
# begin data_csv
# ----------------------------------------------------------------------------
# writer
file_name  = 'data.csv'
file_ptr   = open(file_name, 'w')
fieldnames = [
	'integrand',
	'age_lower',
	'age_upper',
	'time_lower',
	'time_upper',
	'meas_value',
	'meas_std',
	'hold_out'
]
writer     = csv.DictWriter(file_ptr, fieldnames=fieldnames)
# ----------------------------------------------------------------------------
# header
writer.writeheader()
# ----------------------------------------------------------------------------
# BEGIN INTERVALS
age_intervals  = [ (0, 10),      (40, 60),     (90, 100)    ]
time_intervals = [ (1985, 1995), (1995, 2005), (2005, 2010) ]
# END INTERVALS
#-----------------------------------------------------------------------------
# write remission, mtexcess, prevalence, mtall data
n_age          = len(age_intervals)
n_time         = len(time_intervals)
mtall_data     = list()
for integrand in [ 'remission', 'mtexcess', 'prevalence', 'mtall' ] :
	for (age_lower, age_upper) in age_intervals :
		# trapoziodal approximation to integral of prevalence w.r.t. age
		P_sum  = (P[age_lower] + P[age_upper]) / 2.0
		P_sum += sum( P[age_lower + 1 : age_upper ] )
		P_avg  = P_sum / (age_upper - age_lower)
		#
		for (time_lower, time_upper) in time_intervals :
			row               = dict()
			row['integrand']  = integrand
			row['age_lower']  = age_lower
			row['age_upper']  = age_upper
			row['time_lower'] = time_lower
			row['time_upper'] = time_upper
			if integrand == 'remission' :
				row['meas_value'] = rate_true['rho']
			elif integrand == 'mtexcess' :
				row['meas_value'] = rate_true['chi']
			elif integrand == 'prevalence':
				row['meas_value'] = P_avg
			else :
				# if the true omega or chi were not constant, we would do
				# a separate integration for mtall.
				row['meas_value'] = \
					rate_true['omega'] + P_avg * rate_true['chi']
			row['meas_std']   = row['meas_value'] / 10.0
			row['hold_out']   = 0
			if integrand == 'mtall' :
				# change so weighted residual is a coefficient of variation
				row['meas_std']   = row['meas_value']
				# hold out because used for omega constaint
				row['hold_out']   = 1
			writer.writerow(row)
			#
			mtall_data.append( row['meas_value'] )
#-----------------------------------------------------------------------------
# mtother data
i_count = 0
for (age_lower, age_upper) in age_intervals :
	for (time_lower, time_upper) in time_intervals :
		row               = dict()
		age               = (age_lower + age_upper) / 2.0
		time              = (time_lower + time_upper) / 2.0
		row['integrand']  = 'mtother'
		row['age_lower']  = age
		row['age_upper']  = age
		row['time_lower'] = time
		row['time_upper'] = time
		row['meas_value'] = mtall_data[i_count]
		row['meas_std']   = row['meas_value'] / 10.0
		row['hold_out']   = 0
		writer.writerow(row)
		i_count           = i_count + 1
file_ptr.close()
# ----------------------------------------------------------------------------
# end data_csv
# ----------------------------------------------------------------------------
# example.db
csv2db( 'option.csv', 'data.csv' )
# ----------------------------------------------------------------------------
program    = '../../devel/dismod_at'
database   = 'example.db'
command    = [ program, database, 'init' ]
print( ' '.join(cmd) )
flag       = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
command    = [ program, database, 'fit' ]
print( ' '.join(cmd) )
flag       = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at fit command failed')
# ---------------------------------------------------------------------------
# connect to database
new        = False
connection = dismod_at.create_connection(file_name, new)
# ---------------------------------------------------------------------------
# get variable and fit_var tables
var_table         = dismod_at.get_table_dict(connection, 'var')
rate_table        = dismod_at.get_table_dict(connection, 'rate')
fit_var_table     = dismod_at.get_table_dict(connection, 'fit_var')
data_table        = dismod_at.get_table_dict(connection, 'data')
data_subset_table = dismod_at.get_table_dict(connection, 'data_subset')
fit_data_subset   = dismod_at.get_table_dict(connection, 'fit_data_subset')
#
for var_id in range( len(var_table) ) :
	var_row        = var_table[var_id]
	fit_row        = fit_var_table[var_id]
	var_type       = var_row['var_type']
	rate_id        = var_row['rate_id']
	rate_name      = rate_table[rate_id]['rate_name']
	fit_var_value  = fit_row['fit_var_value']
	print ( fit_var_value / rate_true[rate_name] - 1.0)
#
# check error in mtall approximation
for data_id in range( len(data_table) ) :
	assert data_id == data_subset_table[data_id]['data_id']
	integrand = data_table[data_id]['integrand']
	if integrand == 'mtall' :
		weighted_residual = fit_data_subset[data_id]['weighted_residual']
		print ( weighted_residual )
# ---------------------------------------------------------------------------
print('csv2db.py: OK')
# END CSV2DB_EXAMPLE
