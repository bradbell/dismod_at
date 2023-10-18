# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_sample_asy.py}
# {xrst_comment_ch #}
#
# Sample from Asymptotic Distribution for Model Variables
# #######################################################
#
# Purpose
# *******
# The command
#
# | |tab| ``dismod_at`` *database* ``sample asymptotic both`` *number_sample*
# | |tab| ``dismod_at`` *database* ``sample asymptotic fixed`` *number_sample*
#
# samples form an asymptotic approximation for the posterior distribution
# of the :ref:`model_variables-name` .
#
# Notation
# ********
#
# .. csv-table::
#     :widths: auto
#
#     :math:`\iota_n`,model incidence for ``north_america``
#     *u_m*,incidence random effect for ``mexico``
#     *u_c*,incidence random effect for ``canada``
#     *y_n*,measured incidence for ``north_america``
#     *y_m*,measured incidence for ``mexico``
#     *y_c*,measured incidence for ``canada``
#     *s_n*,standard deviation for *y_n*
#     *s_m*,standard deviation for *y_m*
#     *s_c*,standard deviation for *y_c*
#     *s_r*,standard deviation for random effects
#
# The only fixed effect in this model is :math:`\iota_n`.
# The random effects are :math:`u_m` and :math:`u_c`.
#
# Likelihood
# **********
# We define :math:`h( y, \mu , \delta )`
# to be the log-density for a :math:`\B{N}( \mu, \delta^2 )` distribution;
# i.e.,
#
# .. math::
#
#  h( y, \mu, \delta ) =
#     - \frac{ ( y - \mu )^2 }{ \delta^2 }
#     - \log \left( \delta \sqrt{ 2 \pi } \right)
#
# The total log-likelihood for
# :ref:`fit fixed<fit_command@variables@fixed>` is
#
# .. math::
#
#  h[ y_n, \iota_n, s_n ] +
#  h[ y_m, \iota_n, s_m ] +
#  h[ y_c, \iota_n, s_c ]
#
# The total log-likelihood for
# :ref:`fit both<fit_command@variables@both>` is
#
# .. math::
#
#  h[ y_n, \iota_n, s_n ] +
#  h[ y_m, \exp( u_m ) \iota_n, s_m ] +
#  h[ y_c, \exp( u_c ) \iota_n, s_c ] +
#  h( u_m, 0, s_r ) + h( u_c , 0 , s_r )
#
# Problem Parameters
# ******************
# {xrst_spell_off}
# {xrst_code py}
measure = {
   'north_america' : 1.0e-2 , # y_n
   'mexico'        : 2.0e-2 , # y_m
   'canada'        : 0.5e-2   # y_c
}
standard = {
   'north_america' : 1.0e-3 , # s_n
   'mexico'        : 2.0e-3 , # s_m
   'canada'        : 0.5e-3   # s_c
}
standard_random_effect = 1.0   # s_r
number_sample          = 500   # number of posterior samples to generate
number_metropolis      = 10 * number_sample
# {xrst_code}
# {xrst_spell_on}
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_sample_asy.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import time
import math
import numpy
import sys
import os
import copy
test_program = 'example/user/sample_asy.py'
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
# ---------------------------------------------------------------------------
random_seed = int( time.time() )
# ---------------------------------------------------------------------------
#
# no need to include sqrt{2 \pi} term (it does not depend on model variables)
def h(y, mu, delta ) :
   if delta <= 0.0 :
      return - float("inf")
   res = (y - mu ) / delta
   return - res * res - math.log( delta )
#
def log_f_fixed(x) :
   iota_n = x[0]
   #
   y_n    = measure['north_america']
   y_m    = measure['mexico']
   y_c    = measure['canada']
   #
   s_n    = standard['north_america']
   s_m    = standard['mexico']
   s_c    = standard['canada']
   #
   ret    = h(y_n, iota_n, s_n)
   ret   += h(y_m, iota_n, s_m )
   ret   += h(y_c, iota_n, s_c )
   return ret
#
def log_f_both(x) :
   iota_n = x[0]
   u_m    = x[1]
   u_c    = x[2]
   #
   y_n    = measure['north_america']
   y_m    = measure['mexico']
   y_c    = measure['canada']
   #
   s_n    = standard['north_america']
   s_m    = standard['mexico']
   s_c    = standard['canada']
   s_r    = standard_random_effect
   #
   ret    = h(y_n, iota_n, s_n)
   ret   += h(y_m, math.exp(u_m) * iota_n, s_m )
   ret   += h(y_c, math.exp(u_c) * iota_n, s_c )
   ret   += h(u_m, 0.0, s_r ) + h(u_c, 0.0, s_r)
   return ret
# ---------------------------------------------------------------------------
# Note that the a, t values are not used for this example
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
   integrand_table = [
      { 'name':'Sincidence' }
   ]
   #
   # node table: world -> north_america
   #             north_america -> (mexico, canada)
   node_table = [
      { 'name':'world',         'parent':''              },
      { 'name':'north_america', 'parent':'world'         },
      { 'name':'mexico',        'parent':'north_america' },
      { 'name':'canada',        'parent':'north_america' }
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
   # nslist_dict:
   nslist_dict = dict()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   row = {
      'node':        'north_america',
      'subgroup':    'world',
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'time_lower':   2000.0,
      'time_upper':   2000.0,
      'age_lower':    0.0,
      'age_upper':    100.0,
      'integrand':   'Sincidence',
      'meas_value':   measure['north_america'],
      'meas_std':     standard['north_america'],
   }
   data_table.append( copy.copy(row) )
   #
   row['node']       = 'mexico';
   row['meas_value'] = measure['mexico']
   row['meas_std']   = standard['mexico']
   data_table.append( copy.copy(row) )
   #
   row['node']       = 'canada';
   row['meas_value'] = measure['canada']
   row['meas_std']   = standard['canada']
   data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_rate_parent
         'name':     'prior_rate_parent',
         'density':  'uniform',
         'lower':    1e-2 * measure['north_america'],
         'upper':    1e+2 * measure['north_america'],
         'mean':     measure['north_america'] / 3.0,
      },{ # prior_gauss_zero
         'name':     'prior_gauss_zero',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      standard_random_effect,
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
      { 'name':'parent_node_name',       'value':'north_america'      },
      { 'name':'ode_step_size',          'value':'10.0'               },
      { 'name':'random_seed',            'value':str(random_seed)     },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero'  },

      { 'name':'quasi_fixed',            'value':'true'         },
      { 'name':'derivative_test_fixed',  'value':'first-order'  },
      { 'name':'max_num_iter_fixed',     'value':'100'          },
      { 'name':'print_level_fixed',      'value':'0'            },
      { 'name':'tolerance_fixed',        'value':'1e-12'        },

      { 'name':'derivative_test_random', 'value':'second-order' },
      { 'name':'max_num_iter_random',    'value':'100'          },
      { 'name':'print_level_random',     'value':'0'            },
      { 'name':'tolerance_random',       'value':'1e-12'        }
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
      nslist_dict,
      rate_table,
      mulcov_table,
      option_table
   )
# ===========================================================================
file_name  = 'example.db'
example_db(file_name)
#
program   = '../../devel/dismod_at'
ns_string = str(number_sample)
dismod_at.system_command_prc([ program, file_name, 'init' ])
#
# Tables that are the same for both types of sampling
connection    = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
var_table     = dismod_at.get_table_dict(connection, 'var')
node_table    = dismod_at.get_table_dict(connection, 'node')
rate_table    = dismod_at.get_table_dict(connection, 'rate')
connection.close()
assert len(var_table) == 3
#
# map from node name to variable id
node_name2var_id = dict()
for var_id in range(len(var_table) ) :
   row = var_table[var_id]
   assert row['var_type'] == 'rate'
   assert rate_table[row['rate_id']]['rate_name']  == 'iota'
   node_name = node_table[row['node_id']]['node_name']
   node_name2var_id[node_name] = var_id
#
for variables in [ 'fixed' , 'both' ] :
   dismod_at.system_command_prc([ program, file_name, 'fit', variables ])
   dismod_at.system_command_prc(
      [ program, file_name, 'sample', 'asymptotic', variables, ns_string ]
   )
   # -----------------------------------------------------------------------
   # sample_table
   connection      = dismod_at.create_connection(
      file_name, new = False, readonly = True
   )
   sample_table  = dismod_at.get_table_dict(connection, 'sample')
   #
   # var_avg, var_std
   sample_array = numpy.zeros( (number_sample, 3), dtype = float )
   for row in sample_table :
      var_id                              = row['var_id']
      sample_index                        = row['sample_index']
      sample_array[sample_index, var_id ] = row['var_value']
   #
   # compute statistics
   var_avg = numpy.average(sample_array, axis=0);
   var_std = numpy.std(sample_array, axis=0, ddof=1);
   # -----------------------------------------------------------------------
   # now use MCMC to calculate the same values
   print('mcmc')
   numpy.random.seed( seed = random_seed )
   if variables == 'fixed' :
      x0         = numpy.array( [ 1e-2 ] )
      s          = numpy.array( [ 1e-3] )
      log_f      = log_f_fixed
   else :
      x0         = numpy.array( [ 1e-2, 0.0, 0.0 ] )
      s          = numpy.array( [ 1e-3, 1e-1, 1e-1] )
      log_f      = log_f_both
   mcmc_order = [ 'north_america', 'mexico', 'canada' ]
   (a, c)     = dismod_at.metropolis(log_f, number_metropolis, x0, s)
   burn_in    = int( 0.1 * number_metropolis )
   c          = c[burn_in :, :]
   x_avg_mcmc = numpy.average(c, axis=0)
   x_std_mcmc = numpy.std(c, axis=0, ddof=1)
   # -----------------------------------------------------------------------
   # now check values
   for i in range( len(var_table) ) :
      node_name = mcmc_order[i]
      var_id    = node_name2var_id[node_name]
      value     = var_avg[var_id]
      if node_name == 'north_america' or variables == 'both' :
         mcmc      = x_avg_mcmc[i]
         err   = value / mcmc - 1.0
      else :
         err   = value
      if abs( err ) > 0.05 :
         print(node_name, '_avg (value, mcmc, err) = ',
            value, mcmc, err
         )
         print('random_seed = ', random_seed )
         assert(False)
      value     = var_std[var_id]
      if node_name == 'north_america' or variables == 'both' :
         mcmc = x_std_mcmc[i]
         err  = value / mcmc  - 1.0
      else :
         err  = value
      if abs(err) > 0.6 :
         print(node_name, '_std (value, mcmc, err) = ',
            value, mcmc, err
         )
         print('random_seed = ', random_seed )
         assert(False)
print('sample_asy.py: OK')
# END PYTHON
