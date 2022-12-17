# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin user_csv2db.py$$ $newlinech #$$
# $spell
#  csv
#  integrands
#  mtexcess
#  mtall
#  dismodat.py
#  std
# $$
# $nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section csv2db_command: Example and Test$$
#
# $head Using This Example$$
# See $cref/run one example/user_example/Run One Example/$$ for instructions
# on running just this example.
# After doing that, one can run the command
# $codep
#  bin/dismodat.py build/example/user/example.db
# $$
# To generate the csv files corresponding to the example database.
# One can then inspect the csv files in the $code build/example/user$$
# to see all the relevant information.
#
# $head Discussion$$
# The following describes the mode and data for this example:
#
# $head rate_true$$
# The true value for the rates, used to simulate the data,
# are constant w.r.t age and time and are given by:
# $srcthisfile%0%# BEGIN RATE_TRUE%# END RATE_TRUE%1%$$
#
# $head P$$
# The notation $latex P$$ is used for prevalence.
# The initial prevalence at age zero is zero; i.e. $latex P(0) = 0$$.
# We use $latex S(a)$$ ($latex C(a)$$) for the susceptible
# (with condition) fraction of the initial population.
# The true prevalence $latex P(a) = C(a) / [S(a) + C(a)]$$
# is solved for using the ODE:
# $latex \[
#  \begin{array}{rcl}
#  S(0)    & = & 1 \\
#  C(0)    & = & 0 \\
#  S'(a)   & = & - \iota S(a) + \rho C(a)  - \omega S(a) \\
#  C'(a)   & = & + \iota S(a) - \rho C(a)  - \omega C(a) - \chi C(a)
#  \end{array}
# \] $$
#
# $head Rate Grids$$
# The value of omega is modeled as know and equal to the
# value of
# $cref/mtall/csv2db_command/integrand/mtall/$$
# corresponding to the age-time intervals:
# $srcthisfile%
#  0%# BEGIN INTERVALS%# END INTERVALS%1
# %$$
# The non-zero rates (iota, rho, chi) are modeled as unknown and piecewise
# bilinear with the same grid points.
#
# $head Data$$
# The Data is simulated,
# without any noise, for the following integrands:
# $cref/remission/csv2db_command/integrand/remission/$$,
# $cref/mtexcess/csv2db_command/integrand/mtexcess/$$,
# $cref/prevalence/csv2db_command/integrand/prevalence/$$.
# Note that the model for the noise in the measurement
# $cref/meas_std/csv2db_command/meas_std/$$
# is a 10 percent coefficient of variation.
# See the file
# $cref/data.csv/db2csv_command/data.csv/$$ output by the
# db2csv command.
#
# $head Predictions$$
# The $cref/predictions/csv2db_command/Predictions/$$ are in the file
# $cref/predict.csv/db2csv_command/predict.csv/$$ output by the
# db2csv command.
#
# $head mtall$$
# The omega constraints correspond to
# $cref/mtall/csv2db_command/integrand/mtall/$$ data.
# As a check, we include the mtall data with hold_out equal to one.
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1
# %$$
#
# $end
# -----------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import csv
import numpy
import scipy.integrate
#
# dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
   sys.path.insert(0, local_dir)
import dismod_at
#
# check execution is from distribution directory
example = 'example/user/csv2db.py'
if sys.argv[0] != example  or len(sys.argv) != 1 :
   usage  = 'python3 ' + example + '\n'
   usage += 'where python3 is the python 3 program on your system\n'
   usage += 'and working directory is the dismod_at distribution directory\n'
   sys.exit(usage)
#
#
# change into the build/example/user directory
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
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
# configure_csv
file_name  = 'configure.csv'
file_ptr   = open(file_name, 'w')
fieldnames = [ 'name', 'value' ]
writer     = csv.DictWriter(file_ptr, fieldnames=fieldnames)
writer.writeheader()
#
row        = { 'name': 'non_zero_rates',  'value': 'iota rho chi omega' }
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
# ----------------------------------------------------------------------------
# BEGIN INTERVALS
age_intervals  = [ (0, 10),      (40, 60),     (90, 100)    ]
time_intervals = [ (1990, 1994), (1994, 2006), (2006, 2010) ]
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
         #
         row['meas_value'] = round(row['meas_value'], 6)
         row['meas_std']   = row['meas_value'] / 10.0
         row['hold_out']   = 0
         if integrand == 'mtall' :
            # change so weighted residual is a coefficient of variation
            row['meas_std']   = row['meas_value']
            # hold out because used for omega constaint
            row['hold_out']   = 1
            # save a copy for use by omega constraint
            mtall_data.append( row['meas_value'] )
         #
         writer.writerow(row)
         #
#-----------------------------------------------------------------------------
# mtother data
mtall_index = 0
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
      row['meas_value'] = mtall_data[mtall_index]
      row['meas_std']   = row['meas_value'] / 10.0
      row['hold_out']   = 1 # used for constraint, not data
      writer.writerow(row)
      mtall_index      += 1
assert mtall_index == len(mtall_data)
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
   dismod_at.system_command_prc(command)
#
exec_shell_cmd( 'set option quasi_fixed       false' )
exec_shell_cmd( 'set option ode_step_size     5'     )
exec_shell_cmd( 'init' )
exec_shell_cmd( 'fit fixed' )
exec_shell_cmd( 'predict fit_var' )
#
# connect to database
new        = False
connection = dismod_at.create_connection(database, new)
#
# get variable and fit_var tables
var_table         = dismod_at.get_table_dict(connection, 'var')
rate_table        = dismod_at.get_table_dict(connection, 'rate')
integrand_table   = dismod_at.get_table_dict(connection, 'integrand')
fit_var_table     = dismod_at.get_table_dict(connection, 'fit_var')
data_table        = dismod_at.get_table_dict(connection, 'data')
data_subset_table = dismod_at.get_table_dict(connection, 'data_subset')
fit_data_subset   = dismod_at.get_table_dict(connection, 'fit_data_subset')
#
max_abs_err = 0.0
for var_id in range( len(var_table) ) :
   var_row        = var_table[var_id]
   fit_row        = fit_var_table[var_id]
   var_type       = var_row['var_type']
   rate_id        = var_row['rate_id']
   rate_name      = rate_table[rate_id]['rate_name']
   fit_var_value  = fit_row['fit_var_value']
   relative_err   = fit_var_value / rate_true[rate_name]  - 1.0
   max_abs_err    = max(max_abs_err, abs(relative_err) )
if max_abs_err > 0.1 :
   sys.msg('csv2db.py: max_abs_err = ' + str(max_abs_err) )
#
# check error in mtall approximation
max_abs_res = 0.0
for data_id in range( len(data_table) ) :
   assert data_id == data_subset_table[data_id]['data_id']
   integrand_id    = data_table[data_id]['integrand_id']
   integrand_name  = integrand_table[integrand_id]['integrand_name']
   if integrand_name == 'mtall' :
      weighted_residual = fit_data_subset[data_id]['weighted_residual']
      max_abs_res       = max(max_abs_res, abs(weighted_residual) )
if max_abs_res > 0.1 :
   sys.msg('csv2db.py: max_abs_res = ' + str(max_abs_res) )
# ---------------------------------------------------------------------------
# csv representation of database
dismod_at.db2csv_command(database)
# ---------------------------------------------------------------------------
print('csv2db.py: OK')
# END PYTHON
