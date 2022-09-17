# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
import sys
import os
import csv
import subprocess
#
# dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# check execution is from distribution directory
test_program = 'test/user/csv2db.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
	usage  = 'python3 ' + test_program + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
print(test_program)
#
#
# change into the build/test/user directory
if not os.path.exists('build/test/user') :
    os.makedirs('build/test/user')
os.chdir('build/test/user')
# ============================================================================
# Test case where age and time grids are all the age and time points
# ============================================================================
# configure_csv
file_name  = 'configure.csv'
file_ptr   = open(file_name, 'w')
fieldnames = [ 'name', 'value' ]
writer     = csv.DictWriter(file_ptr, fieldnames=fieldnames)
writer.writeheader()
#
row        = { 'name': 'non_zero_rates',  'value': 'omega' }
writer.writerow( row )
#
file_ptr.close()
# ----------------------------------------------------------------------------
# begin measure_csv
# ----------------------------------------------------------------------------
# writer
file_name  = 'measure.csv'
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
#
# mtother data
row               = dict()
time              = 00.
row['integrand']  = 'mtother'
row['age_lower']  = 0.0
row['age_upper']  = 0.0
row['time_lower'] = time
row['time_upper'] = time
row['meas_value'] = 0.01
row['meas_std']   = row['meas_value'] / 10.0
row['hold_out']   = 1 # used for constraint, not data
writer.writerow(row)
row['age_lower']  = 100.0
row['age_upper']  = 100.0
writer.writerow(row)
file_ptr.close()
# ----------------------------------------------------------------------------
# end measure_csv
# ----------------------------------------------------------------------------
# create example.db
database      = 'example.db'
configure_csv = 'configure.csv'
measure_csv   = 'measure.csv'
dismod_at.csv2db_command(database, configure_csv, measure_csv)
#
# run dismod_at commands
def exec_shell_cmd(cmd) :
	program    = '../../devel/dismod_at'
	command    = [ program, database ] + cmd.split()
	print( ' '.join(command) )
	flag       = subprocess.call( command )
	if flag != 0 :
		sys.exit('The dismod_at init command failed')
#
exec_shell_cmd( 'init' )
#
# connect to database
new        = False
connection = dismod_at.create_connection(database, new)
#
# get variable and fit_var tables
var_table         = dismod_at.get_table_dict(connection, 'var')
rate_table        = dismod_at.get_table_dict(connection, 'rate')
integrand_table   = dismod_at.get_table_dict(connection, 'integrand')
#
assert len(var_table) == 2
for i in range(2) :
	row  = var_table[i]
	assert row['var_type']     == 'rate'
	assert row['age_id']       == i
	assert row['time_id']      == 0
	assert row['node_id']      == 0
	assert row['integrand_id'] == None
	assert row['covariate_id'] == None
rate_row            = rate_table[ row['rate_id'] ]
assert rate_row['rate_name'] == 'omega'
# ============================================================================
print('csv2db.py: OK')
