#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_covid_19.py$$ $newlinech #$$
#
# $section Model The Covid-19 Epidemic$$
#
# $head csv_file_data$$
# This example simulates reading a CSV file with the following columns:
# $table
# $icode day$$      $cnext day that this row of data corresponds to $rnext
# $icode death$$    $cnext cummulative covid_19 death as fraction of population $rnext
# $icode mobility$$ $cnext mobility shifted and scaled to be in [-1, 0.]   $rnext
# $icode testing$$  $cnext testing shifted and scaled to be in [0., 1]
# $icode
#
#
# $end
# ------------------------------------------------------------------------------------
# BEGIN_PYTHON
import sys
import os
import csv
import distutils.dir_util
import copy
import matplotlib.pyplot
test_program = 'example/user/covid_19.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
	usage  = 'python3 ' + test_program + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
print(test_program)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/example/user directory
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
#
# ------------------------------------------------------------------------------------
csv_file_data = '''day,death,mobility,testing
0,1.5159e-07,0,0.012738
1,4.0424e-07,-0.099623,0
2,8.59e-07,-0.20332,0.017105
3,1.2127e-06,-0.30759,0.06067
4,2.0717e-06,-0.40911,0.14986
5,3.2844e-06,-0.50501,0.21134
6,4.9014e-06,-0.59301,0.26117
7,6.9226e-06,-0.67147,0.34118
8,8.8932e-06,-0.73938,0.40973
9,1.1015e-05,-0.79633,0.4094
10,1.5917e-05,-0.84249,0.37192
11,2.1475e-05,-0.87867,0.38522
12,2.7842e-05,-0.90616,0.47688
13,3.724e-05,-0.92649,0.44716
14,5.3561e-05,-0.94127,0.42764
15,6.6042e-05,-0.95196,0.35542
16,8.4132e-05,-0.95975,0.44191
17,0.00010793,-0.96562,0.43481
18,0.00013663,-0.9703,0.47483
19,0.00016124,-0.97439,0.4991
20,0.00020601,-0.97832,0.57172
21,0.00025144,-0.98238,0.578
22,0.0002858,-0.98666,0.54818
23,0.00031389,-0.99101,0.51412
24,0.00036765,-0.99503,0.5705
25,0.00042076,-0.99821,0.64551
26,0.00047392,-1,0.71982
27,0.00051854,-0.99992,0.69938
28,0.00056846,-0.99765,0.64184
29,0.00061707,-0.993,0.54799
30,0.00066199,-0.98593,0.52809
31,0.00071307,-0.97664,0.57946
32,0.00076133,-0.9656,0.66222
33,0.00079933,-0.95358,0.68223
34,0.00083738,-0.94159,0.64932
35,0.00087133,-0.93087,0.615
36,0.00090342,-0.92271,0.54825
37,0.0009345,-0.9183,0.47447
38,0.00096795,-0.91827,0.47153
39,0.0009973,-0.9223,0.55527
40,0.0010307,-0.92956,0.73615
41,0.0010602,-0.93907,0.97671
42,0.0010865,-0.94973,1
43,0.0011096,-0.96031,0.83756
44,0.0011353,-0.9694,0.61103
45,0.0011592,-0.97554,0.60876
46,0.0011882,-0.97761,0.6771
47,0.0012051,-0.97497,0.73262
48,0.0012176,-0.96751,0.79038
49,0.0012257,-0.95556,0.80307
50,0.0012519,-0.93992,0.81576
51,0.0012667,-0.92161,0.82845
52,0.0012797,-0.90996,0.84115
53,0.0013138,-0.90255,0.85384
54,0.0013249,-0.89783,0.86653
'''
# ====================================================================================
def example_db(file_name) :
	def fun_iota_prior (a, t) :
		return ('prior_iota_value', None, 'prior_iota_dtime')
	def fun_pini_prior (a, t) :
		return ('prior_pini_value', None, None)
	def fun_rho_prior (a, t) :
		return ('prior_rho_value', None, None)
	def fun_chi_prior (a, t) :
		return ('prior_chi_value', None, None)
	def fun_covariate_prior (a, t) :
		return ('prior_covariate_value', None, None)
	#
	# file_data
	file_data  = list()
	reader    = csv.DictReader( csv_file_data.splitlines() )
	for row in reader :
		for key in row :
			row[key] = float( row[key] )
		file_data.append( copy.copy( row ) )
	#
	# age_list
	# This analysis has no age variation
	age_list = [0.0, 100.0]
	#
	# time_list
	time_list = list()
	for row in file_data :
		time_list.append( row['day'] )
	#
	# integrand_table
	integrand_table = [ { 'name' : 'mtspecific' } ]
	#
	# node_table
	node_table = [ { 'name': 'world', 'parent': '' } ]
	#
	# subgroup_table
	subgroup_table = [ {'subgroup' : 'world', 'group' : 'world' } ]
	#
	# weight_table
	weight_table = list()
	#
	# covariate_table
	covariate_table = [ {
		'name'      : 'mobility',
		'reference' : 0.0,
		},{
		'name'      : 'testing',
		'reference' : 0.0,
	} ]
	#
	# avgint_table
	avgint_table = list()
	#
	# data_table
	data_table = list()
	row_out = {
		'hold_out'  : False       ,
		'integrand' : 'mtspecific',
		'node'      : 'world'     ,
		'subgroup'  : 'world'     ,
		'weight'    : ''          ,
		'age_lower' : 50.0        ,
		'age_upper' : 50.0        ,
		'density'   : 'gaussian'  ,
	}
	data_cv = 0.25
	n_data  = len(file_data) - 1
	for i in range(n_data) :
		row_avg = dict()
		for key in [ 'day', 'mobility', 'testing' ] :
			row_avg[key] = ( file_data[i][key] + file_data[i+1][key] ) / 2.0
		row_out['time_lower'] = row_avg['day']
		row_out['time_upper'] = row_avg['day']
		row_out['mobility']   = row_avg['mobility']
		row_out['testing']    = row_avg['testing']
		death_difference      = file_data[i+1]['death'] - file_data[i]['death']
		day_difference        = file_data[i+1]['day'] - file_data[i]['day']
		row_out['meas_value'] = death_difference / day_difference
		row_out['meas_std']   = data_cv * row_out['meas_value']
		data_table.append( copy.copy(row_out) )
	#
	# prior_table
	prior_table = [ {
		'name'    : 'prior_iota_value',
		'lower'   : 1e-6,
		'upper'   : 1e0,
		'mean'    : 1e-3,
		'density' : 'uniform',
	},{
		'name'    : 'prior_iota_dtime',
		'density' : 'log_gaussian',
		'mean'    : 0.0,
		'std'     : 0.1,
		'eta'     : 1e-5,
	},{
		'name'    : 'prior_rho_value',
		'density' : 'uniform',
		'mean'    : 0.1,
		'lower'   : 0.1,
		'upper'   : 0.1,
	},{
		'name'    : 'prior_chi_value',
		'density' : 'uniform',
		'mean'    : 0.01,
		'lower'   : 0.01,
		'upper'   : 0.01,
	},{
		'name'    : 'prior_pini_value',
		'density' : 'uniform',
		'lower'   : 1e-7,
		'upper'   : 1e-3,
		'mean'    : 1e-5,
	},{
		'name'    : 'prior_covariate_value',
		'density' : 'uniform',
		'mean'    : 0.0,
		'lower'   : - 1.0,
		'upper'   : + 1.0,
	} ]
	#
	# smooth_table
	time_id = list( range( len( time_list ) ) )
	smooth_table = [ {
		'name'    : 'smooth_pini',
		'age_id'  : [ 0 ],
		'time_id' : [ 0 ],
		'fun'     : fun_pini_prior,
	},{
		'name'    : 'smooth_iota',
		'age_id'  : [ 0 ],
		'time_id' : time_id,
		'fun'     : fun_iota_prior,
	},{
		'name'    : 'smooth_rho',
		'name'    : 'smooth_rho',
		'age_id'  : [ 0 ],
		'time_id' : [ 0 ],
		'fun'     : fun_rho_prior,
	},{
		'name'    : 'smooth_chi',
		'age_id'  : [ 0 ],
		'time_id' : [ 0 ],
		'fun'     : fun_chi_prior,
	},{
		'name'    : 'smooth_covariate',
		'age_id'  : [ 0 ],
		'time_id' : [ 0 ],
		'fun'     : fun_covariate_prior,

	} ]
	#
	# nslist_table
	nslist_table = dict()
	#
	# rate_table
	rate_table = [ {
		'name'          : 'pini',
		'parent_smooth' : 'smooth_pini',
	},{
		'name'          : 'iota',
		'parent_smooth' : 'smooth_iota',
	},{
		'name'          : 'rho',
		'parent_smooth' : 'smooth_rho',
	},{
		'name'          : 'chi',
		'parent_smooth' : 'smooth_chi',
	} ]
	#
	# mulcov_table
	mulcov_table = [ {
		'covariate' : 'mobility',
		'type':       'rate_value',
		'effected':   'iota',
		'group':      'world',
		'smooth':     'smooth_covariate',
	},{
		'covariate' : 'testing',
		'type':       'rate_value',
		'effected':   'iota',
		'group':      'world',
		'smooth':     'smooth_covariate',
	} ]
	#
	# option_tabel
	option_table = [
		{ 'name'  : 'parent_node_name',   'value' : 'world'            },
		{ 'name'  : 'rate_case',          'value' : 'iota_pos_rho_pos' },
		{ 'name'  : 'print_level_fixed',  'value' : '5'                },
		{ 'name'  : 'quasi_fixed',        'value' : 'false'            },
	]
	# ----------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_table,
		node_table,
		subgroup_table,
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
	return
# ===========================================================================
# create the database
file_name  = 'example.db'
example_db(file_name)
#
# fit data
program_cpp = '../../devel/dismod_at'
program_py  = '../../../bin/dismodat.py'
dismod_at.system_command_prc([ program_cpp, file_name, 'init' ])
dismod_at.system_command_prc([ program_cpp, file_name, 'fit', 'fixed' ])
dismod_at.system_command_prc([ program_py,  file_name, 'db2csv' ])
#
# plot result
file_in = 'variable.csv'
file_in = open(file_in, 'r')
reader  = csv.DictReader(file_in)
time    = list()
iota    = list()
for row in reader :
	if row['var_type'] == 'rate' and row['rate'] == 'iota' :
		time.append( float(row['time']) )
		iota.append( float(row['fit_value']) )
#
fig, ax = matplotlib.pyplot.subplots()
ax.plot(time, iota)
ax.set(xlabel = 'day')
ax.set(ylabel = 'iota(t)' )
ax.set(title = 'beta(t) / I(t)' )
matplotlib.pyplot.show()

#
print('covid_19.py: OK')
sys.exit(0)
# END_PYTHON
