# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin user_hes_random.py$$ $newlinech #$$
# $spell
#  cppad
# $$
#
# $section Check the Hessian of the Random Effects Objective$$
#
# $head Purpose$$
# This example checks the computation of the Hessian of the
# random effects objective used by the
# $cref/asymptotic/sample_command/asymptotic/$$ sampling method.
#
# $head Reference$$
# See the
# $href%https://bradbell.github.io/cppad_mixed/doc/theory.htm%theory%$$
# section of the $code cppad_mixed$$ documentation.
#
# $head Notation$$
# $table
# $latex \theta$$ $cnext model incidence for the parent region $rnext
# $latex u_0$$    $cnext incidence random effect for first child    $rnext
# $latex u_1$$    $cnext incidence random effect for second child   $rnext
# $latex y_0$$    $cnext measured incidence for the first child $rnext
# $latex y_1$$    $cnext measured incidence for the second child $rnext
# $latex s$$      $cnext standard deviation for data and random effects $rnext
# $tend
# The only fixed effect in this model is $latex \theta$$
# (sometimes written $icode theta$$) the incidence level for the world.
# The random effects are $latex u_0$$ and $latex u_1$$.
#
# $head Problem Parameters$$
# $srccode%py%
import csv
import math
import time
theta_true     = 0.5
u_true         = [ 0.5, -0.5 ]
y_0            = theta_true * math.exp( u_true[0] )
y_1            = theta_true * math.exp( u_true[1] )
standard_dev   = theta_true / 10.0 # the standard deviation s
random_seed    = int( time.time() )
number_sample  = 4000
# %$$
#
# $head Random Likelihood$$
# The negative log-likelihood for this example, ignoring the constant
# of integration, is
# $latex \[
#  f( \theta, u )
#  = \frac{1}{2 s^2} \left(
#     [ y_0 - \theta \exp( u_0 ) ]^2 +
#     [ y_1 - \theta \exp( u_1 ) ]^2 +
#     u_0^2 +
#     u_1^2
#  \right)
# \] $$
#
# $head Gradient w.r.t. Random Effects$$
# $latex \[
#  f_u ( \theta, u )
#  =
#  \frac{1}{s^2} \left(
#  \begin{array}{c}
#     \theta^2 \exp( 2 u_0 ) - y_0 \theta \exp( u_0 )  + u_0
#     \\
#     \theta^2 \exp( 2 u_1 ) - y_1 \theta \exp( u_1 )  + u_1
#  \end{array}
#  \right)
# \] $$
#
# $head Hessian w.r.t. Random Effects$$
# $latex \[
#  f_{u,u} ( \theta, u )
#  =
#  \frac{1}{s^2} \left(
#  \begin{array}{cc}
#     2 \theta^2 \exp( 2 u_0 ) - y_0 \theta \exp( u_0 ) + 1   & 0
#     \\
#     0   & 2 \theta^2 \exp( 2 u_1 ) - y_1 \theta \exp( u_1 ) + 1
#  \end{array}
#  \right)
# \] $$
#
# $head Asymptotic Statistics$$
# The asymptotic posterior distribution for random effects is normal
# with mean $latex \hat{u} ( \theta )$$
# and variance $latex f_{u,u} [ \theta , \hat{u} ( \theta ) ]^{-1}$$
# where $latex \hat{u} ( \theta )$$ minimizes the random effects objective
# $latex f( \theta , \cdot )$$.
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import numpy
import sys
import os
import copy
test_program = 'example/user/hes_random.py'
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
#
def example_db (file_name) :
   def fun_rate_child(a, t) :
      return ('prior_gauss_zero', None, None)
   def fun_rate_parent(a, t) :
      return ('prior_rate_parent', None, None)
   # ----------------------------------------------------------------------
   # age table
   age_list    = [    0.0,   100.0 ]
   #
   # time table
   time_list   = [ 1995.0,  2015.0 ]
   #
   # integrand table
   integrand_table = [ { 'name':'Sincidence' } ]
   #
   node_table = [
      { 'name':'world',    'parent':''       },
      { 'name':'child_0',  'parent':'world'  },
      { 'name':'child_1',  'parent':'world'  },
   ]
   #
   # weight table:
   weight_table = list()
   #
   # covariate table: no covriates
   covariate_table = list()
   #
   # mulcov table
   mulcov_table = list()
   #
   # avgint table:
   avgint_table = list()
   #
   # nslist_table:
   nslist_table = dict()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   row = {
      'node':        'child_0',
      'subgroup':    'world',
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'time_lower':   2000.0,
      'time_upper':   2000.0,
      'age_lower':    50.0,
      'age_upper':    50.0,
      'integrand':   'Sincidence',
      'meas_value':   y_0,
      'meas_std':     standard_dev,
   }
   data_table.append( copy.copy(row) )
   #
   row['node']       = 'child_1'
   row['meas_value'] = y_1
   data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_rate_parent
         'name':     'prior_rate_parent',
         'density':  'uniform',
         'lower':    1e-2 * theta_true,
         'upper':    1e+2 * theta_true,
         'mean':     theta_true / 3.0, # only used for start and scale
      },{ # prior_gauss_zero
         'name':     'prior_gauss_zero',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      standard_dev,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      { # smooth_rate_parent
         'name':                     'smooth_rate_parent',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_rate_parent
      }, { # smooth_rate_child
         'name':                     'smooth_rate_child',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_rate_child
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'iota',
         'parent_smooth': 'smooth_rate_parent',
         'child_smooth':  'smooth_rate_child',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'world'              },
      { 'name':'ode_step_size',          'value':'10.0'               },
      { 'name':'random_seed',            'value':str(random_seed)     },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero'  },

      { 'name':'quasi_fixed',            'value':'true'               },
      { 'name':'derivative_test_fixed',  'value':'none'               },
      { 'name':'max_num_iter_fixed',     'value':'100'                },
      { 'name':'print_level_fixed',      'value':'0'                  },
      { 'name':'tolerance_fixed',        'value':'1e-12'              },

      { 'name':'derivative_test_random', 'value':'none'               },
      { 'name':'max_num_iter_random',    'value':'100'                },
      { 'name':'print_level_random',     'value':'0'                  },
      { 'name':'tolerance_random',       'value':'1e-12'              }
   ]
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
# ===========================================================================
file_name  = 'example.db'
example_db(file_name)
#
program   = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
dismod_at.system_command_prc(
   [ program, file_name, 'sample', 'asymptotic', 'both', str(number_sample) ]
)
# -----------------------------------------------------------------------
# get tables
new               = False
connection        = dismod_at.create_connection(file_name, new)
var_table         = dismod_at.get_table_dict(connection, 'var')
node_table        = dismod_at.get_table_dict(connection, 'node')
rate_table        = dismod_at.get_table_dict(connection, 'rate')
fit_var_table     = dismod_at.get_table_dict(connection, 'fit_var')
hes_random_before = dismod_at.get_table_dict(connection, 'hes_random')
sample_table     = dismod_at.get_table_dict(connection, 'sample')
hes_random_after = dismod_at.get_table_dict(connection, 'hes_random')
connection.close()
dismod_at.db2csv_command(file_name)
# -----------------------------------------------------------------------
# var_id2name
var_id2node_name = dict()
assert len(var_table) == 3
for var_id in range(len(var_table) ) :
   assert var_id < 3
   row = var_table[var_id]
   assert row['var_type'] == 'rate'
   assert rate_table[row['rate_id']]['rate_name']  == 'iota'
   node_name = node_table[row['node_id']]['node_name']
   var_id2node_name[var_id] = node_name
   if node_name == 'world' :
      theta = fit_var_table[var_id]['fit_var_value']
# -----------------------------------------------------------------------
# check the Hessian of the random effects objective
#
# The Hessian of the random effects objective it diagonal
# and there are two random effects
assert hes_random_before == hes_random_after
assert len(hes_random_after) == 2
covariance = numpy.zeros((2, 2), dtype = float)
uhat    = numpy.zeros(2, dtype = float)
for row in hes_random_after :
   assert row['row_var_id'] == row['col_var_id']
   var_id           = row['row_var_id']
   hes_random_value = row['hes_random_value']
   u                = fit_var_table[var_id]['fit_var_value']
   node_name        = var_id2node_name[var_id]
   if node_name == 'child_0' :
      uhat[0]      = u
      covariance[0,0] = 1.0 /hes_random_value
      y = y_0
   elif node_name == 'child_1' :
      uhat[1]      = u
      covariance[1,1] = 1.0 / hes_random_value
      y = y_1
   else :
      assert False
   exp_2u = math.exp( 2.0 * u )
   exp_u  = math.exp( u )
   check   = 2.0 * theta * theta * exp_2u - y * theta * exp_u  + 1.0
   check  /= standard_dev * standard_dev
   rel_error = hes_random_value / check - 1.0
   assert abs(rel_error) < 1e-15
#
# compute sample statistics
assert  len(sample_table) == number_sample * 3
sample_array  = numpy.zeros( (2, number_sample), dtype = float)
for row in sample_table :
   var_id           = row['var_id']
   sample_index     = row['sample_index']
   node_name        = var_id2node_name[var_id]
   if node_name == 'child_0' :
      sample_array[0, sample_index] = row['var_value']
   elif node_name == 'child_1' :
      sample_array[1, sample_index] = row['var_value']
   else :
      assert node_name == 'world'
#
# check sample averages
sample_avg = numpy.average(sample_array, axis=1)
sample_cov = numpy.cov(sample_array, ddof=1)
rel_error  = sample_avg / uhat - 1.0
assert all( abs(rel_error)  < 2e-2 )
#
# check sample covariance
scale = math.sqrt( covariance[0,0] * covariance[1,1] )
for i in range(2) :
   for j in range(2) :
      if i == j :
         rel_error = sample_cov[i,i] / covariance[i,i] - 1.0
      else :
         rel_error = sample_cov[i,j] / scale
   assert abs(rel_error) < 1e-1
#
# check db2csv output of hes_random.csv
# (assumes database is in current working directory)
file_ptr  = open('hes_random.csv', 'r')
hes_random_csv = list()
reader    = csv.DictReader(file_ptr)
for row in reader :
   hes_random_csv.append(row)
file_ptr.close()
#
for hes_random_id in range( len(hes_random_after) ) :
   row_table    = hes_random_after[hes_random_id]
   row_csv      = hes_random_csv[hes_random_id]
   #
   value_table  = row_table['row_var_id']
   value_csv    = int( row_csv['row_var_id'] )
   assert value_table == value_csv
   #
   value_table  = row_table['col_var_id']
   value_csv    = int( row_csv['col_var_id'] )
   assert value_table == value_csv
   #
   value_table  = row_table['hes_random_value']
   value_csv    = float( row_csv['hes_random_value'] )
   rel_error    = value_csv / value_table - 1.0
   eps9         = 9.0 * numpy.finfo(float).eps
   assert abs(rel_error) < eps9
print('hes_random.py: OK')
# END PYTHON
