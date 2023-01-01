# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin user_change_grid.py$$ $newlinech #$$
#
# $nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section Remove an Age or Time From a Smoothing$$
#
# $head Purpose$$
# This examples uses SQL commands to modify a data base.
# To be specific, it removes one of the age points from a smoothing grid.
#
# $head Age Table$$
# The $cref age_table$$ for this example is specified by the code
# $codep
#  age_list = [0, 1, 20, 30, 40, 50, 60, 70, 80, 90, 100 ]
# $$
#
# $head Time Table$$
# The $cref time_table$$ is specified by
# $codep
#  time_list = [ 1960, 1995 ]
# $$
#
# $head Parent Smoothing$$
# The original parent smoothing table contains all of the age and time points.
#
# $subhead Value Prior$$
# The value prior at each (age, time) pair is log-Gaussian with
# lower limit $codei%1e-2*%iota_mean%$$,
# upper limit $icode%1e+2*%iota_mean%$$,
# mean $icode iota mean$$,
# standard deviation $icode iota_mean$$,
# and the offset in the log transform $codei%1e-3*%iota_mean%$$.
#
# $subhead Difference Prior$$
# The forward difference prior at each (age, time) pair is log-Gaussian with
# no lower limit,
# no upper limit,
# mean zero,
# standard deviation $icode 1.0$$,
# and the offset in the log transform $code 1e-6$$.
#
# $head Change Parent Smoothing$$
# Note that the original age grid has 11 points.
# The last age point, and all the corresponding (age, time) pairs
# are removed from the smoothing grid with the following SQL commands:
# $codep
#  UPDATE smooth SET n_age = 10 WHERE smooth_id == 0;
#  UPDATE smooth_grid SET smooth_id = WHERE smooth_id == 0 AND age_id == 10;
# $$
# We refer to this as the modified smoothing.
#
# $head Fit$$
# The only variables in this example are the parent rates for $icode iota$$.
# Hence the number of variables in the $cref var_table$$ should be the number
# of grid points in the modified smoothing; i.e., 20.
# There is no data for this example.
# Hence the optimal fit is to have $icode iota$$ equal to $icode iota_mean$$
# for all the (age, time) paris in the modified smoothing.
#
# $head Source Code$$
# $srcthisfile%0% BEGIN PYTHON%# END PYTHON%1%$$
#
# $end
# ------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
iota_mean = 0.01
# ------------------------------------------------------------------------
import sys
import os
test_program = 'example/user/change_grid.py'
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
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
def example_db (file_name) :
   # note that the a, t values are not used for this case
   def fun_iota_parent(a, t) :
      return ('prior_value', 'prior_diff', 'prior_diff')
   import copy
   import dismod_at
   import math
   # ----------------------------------------------------------------------
   # age table
   age_list    = [0, 1, 20, 30, 40, 50, 60, 70, 80, 90, 100 ]
   #
   # time table
   time_list   = [ 1960, 1995 ]
   #
   # integrand table
   integrand_table = [
      { 'name':'Sincidence' }
   ]
   # node table
   node_table = [
      { 'name':'world',         'parent':'' }
   ]
   #
   # weight table:
   weight_table = list()
   #
   # covariate table:
   covariate_table = list()
   #
   # mulcov table
   mulcov_table = list()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_diff
         'name':     'prior_diff',
         'density':  'log_gaussian',
         'lower':    None,
         'upper':    None,
         'mean':     0.0,
         'std':      1.0,
         'eta':      1e-6
      },{ # prior_value
         'name':     'prior_value',
         'density':  'log_gaussian',
         'lower':    1e-2 * iota_mean,
         'upper':    1e+2 * iota_mean,
         'mean':     iota_mean,
         'std':      iota_mean,
         'eta':      1e-3 * iota_mean
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   age_id_list = range( len( age_list ) )
   time_id_list = range( len( time_list ) )
   smooth_table = [
      { # smooth_rate_parent
         'name':                     'smooth_rate_parent',
         'age_id':                   age_id_list,
         'time_id':                  time_id_list,
         'mulstd_value_prior_name':  None,
         'mulstd_dage_prior_name':   None,
         'mulstd_dtime_prior_name':  None,
         'fun':                      fun_iota_parent
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [ {
         'name':          'iota',
         'parent_smooth': 'smooth_rate_parent'
   } ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'world'             },
      { 'name':'ode_step_size',          'value':'10.0'              },
      { 'name':'random_seed',            'value':'0'                 },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },

      { 'name':'quasi_fixed',            'value':'true'              },
      { 'name':'derivative_test_fixed',  'value':'none'              },
      { 'name':'max_num_iter_fixed',     'value':'50'                },
      { 'name':'print_level_fixed',      'value':'0'                 },
      { 'name':'tolerance_fixed',        'value':'1e-10'             },

      { 'name':'derivative_test_random', 'value':'second-order'      },
      { 'name':'max_num_iter_random',    'value':'50'                },
      { 'name':'print_level_random',     'value':'0'                 },
      { 'name':'tolerance_random',       'value':'1e-10'             }
   ]
   # ----------------------------------------------------------------------
   # avgint table: empty
   avgint_table = list()
   # ----------------------------------------------------------------------
   # nslist_table:
   nslist_table = dict()
   # ----------------------------------------------------------------------
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
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
   # ----------------------------------------------------------------------
#
file_name   = 'example.db'
example_db(file_name)
# ===========================================================================
# remove one age from the original smoothing
# ===========================================================================
#
new         = False
connection  = dismod_at.create_connection(file_name, new)
age_table   = dismod_at.get_table_dict(connection, 'age')
n_age       = len( age_table )
#
# change number of ages in the smooth table
command  = 'UPDATE smooth SET n_age = ' + str(n_age - 1) + ' '
command += 'WHERE smooth_id == 0;'
dismod_at.sql_command(connection, command)
#
# Remove the last grid point. Note that there is a new last age and the
# corresponding dage priors are not used (even though they are not null).
remove_index = int( n_age - 1 )
command  = 'UPDATE smooth_grid SET smooth_id = null '
command += 'WHERE smooth_id == 0 AND age_id == ' + str(remove_index) + ';'
dismod_at.sql_command(connection, command)
#
# ===========================================================================
# do a fit and check results
# ===========================================================================
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
#
# Results for fitting with no noise
age_table     = dismod_at.get_table_dict(connection, 'age')
time_table    = dismod_at.get_table_dict(connection, 'time')
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
#
n_age  = len( age_table )
n_time = len( time_table )
n_var  = len( var_table )
assert n_var == (n_age - 1 ) * n_time
#
parent_node_id = 0
eps            = 1e-4
#
# check parent rates values
iota_rate_id      = 1
max_err           = 0.0;
age_id_list       = range( n_age )
for var_id in range( n_var ) :
   row    = var_table[var_id]
   value  = fit_var_table[var_id]['fit_var_value']
   assert row['var_type'] == 'rate'
   assert row['node_id'] == parent_node_id
   assert row['rate_id'] == iota_rate_id
   assert row['age_id'] in age_id_list
   assert row['age_id'] != remove_index
   max_err = max(max_err, abs( value / iota_mean - 1.0 ) )
assert abs(value / iota_mean - 1.0 ) <= 1e-2
# -----------------------------------------------------------------------------
print('change_grid.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
