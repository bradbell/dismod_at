# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
test_name     = '22906'
program_name  = test_name + '.py'
database_name = test_name + '.db'
output_dir    = 'build/test/ihme/' + test_name
# ---------------------------------------------------------------------------
# true values used to simulate data
import sys
import os
import distutils.dir_util
import subprocess
import pdb
test_program = 'test/ihme/' + test_name + '.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
	usage  = 'python3 ' + test_program + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# import dismod_at
sys.path.append( os.getcwd() + '/python' )
import dismod_at
#
# create output dir
distutils.dir_util.mkpath(output_dir)
# ---------------------------------------------------------------------------
# option.csv
text='''name,value
cascade_path,/home/bradbell/cascade_ode.git/build/''' + test_name + '''
parent_node_name,usa
ode_step_size,1.0
mtall2mtother,true
chi_zero,true
quasi_fixed,true
rate_case,iota_pos_rho_zero
parent_dtime_std,0.2
child_value_std,0.2
child_dtime_std,0.2
xi_factor,0.2
zeta_factor,2.0
random_bound,
age_grid,0 10 20 30 31 40 50 60 70 80 90 100
time_grid,1970 2015
include_covariates,a_local
include_integrands,incidence
'''
file_name  = output_dir + '/option.csv'
file_ptr   = open(file_name, 'w')
file_ptr.write(text)
file_ptr.close()
# ---------------------------------------------------------------------------
program    = 'bin/import_cascade.py'
file_name  = output_dir + '/option.csv'
cmd        = [ program, file_name ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit(program_name + ';: import failed')
# ---------------------------------------------------------------------------
file_name      = output_dir + '/' + database_name
program        = 'build/devel/dismod_at'
for command in [ 'init', 'start', 'fit' ] :
	cmd = [ program, file_name, command ]
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit(program_name + ': dismod_at ' + command + ' command failed')
# ---------------------------------------------------------------------------
program    = 'bin/database2csv.py'
file_name  = output_dir + '/' + database_name
cmd        = [ program, file_name ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit(program_anme + ': database2csv failed')
# ---------------------------------------------------------------------------
print(program_name + ': OK')
sys.exit(0)
