# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_binomial.py}
# {xrst_comment_ch #}
#
# Fitting Binomial Data Example
# #############################
#
# Iota
# ****
# There is only one grid point in the parent and child smoothing
# for iota, hence they are constant with respect to age and time.
#
# Parent
# ======
# The value *iota_true*
# is the simulated true rate for *iota*
# for both the parent and two child nodes.
# A uniform prior is used for the parent rate with
# *iota_parent_true* /100 as a lower limit,
# and ``1`` as the upper limit.
#
# Child
# =====
# The *iota*
# :ref:`model_variables@Random Effects, u@Child Rate Effects`
# are simulated with value zero.
# They are fit using
# a Gaussian prior with a mean zero and standard deviation 0.1.
#
# Other Rates
# ***********
# The :ref:`rate_table@parent_smooth_id` and
# :ref:`rate_table@child_smooth_id` are null for the other rates; i.e.,\
# the other rates are zero.
#
# Data
# ****
#
# Integrand
# =========
# All of the data is for the prevalence integrand.
# Since *iota* is constant, and the other rates are zero,
# the true prevalence is :math:`1 - \exp( - iota * age )` .
#
# meas_value
# ==========
# The prevalence data is simulate using a
# binomial distribution divided by its sample size.
# The mean of the binomial distribution
# is the sample size times the true prevalence.
#
# Sample Size
# ===========
# The sample size for each data point is chosen as follows:
# {xrst_literal
#  # BEGIN_SAMPLE_SIZE
#  # END_SAMPLE_SIZE
# }
#
# Density
# =======
# If the count, corresponding to the measured value, is greater than or equal
# a threshold, a Gaussian approximation is used, otherwise the
# :ref:`density_table@density_name@binomial` density is used.
# Note that the approximation seems to work well even when the
# threshold is zero.
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_binomial.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# values used to simulate data
import numpy
iota_true          = 2e-4
n_children         = 2
n_data             = 50
random_seed        = 0
if random_seed == 0 :
   import time
   random_seed = int( time.time() )
numpy.random.seed( int(random_seed) )
#
# sample_size_min, sample_size_max
# prevalence is 1.0 - exp( -iota_true * age )
sample_size_min  = 0.5 / ( 1.0 - numpy.exp( - iota_true * 50.0 ) )
sample_size_max  = 100.0 * sample_size_min
# ------------------------------------------------------------------------
import sys
import os
import copy
test_program  = 'example/user/binomial.py'
check_program = sys.argv[0].replace('\\', '/')
if check_program != test_program  or len(sys.argv) != 1 :
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
# BEGIN_SAMPLE_SIZE
def next_sample_size() :
   sample_size = numpy.random.uniform(
      low  = numpy.log( sample_size_min ) ,
      high = numpy.log( sample_size_max ) ,
   )
   sample_size = numpy.exp( round( sample_size ) )
   return sample_size
# END_SAMPLE_SIZE
# ------------------------------------------------------------------------
def binomial(sample_size, success_rate) :
   count      = numpy.random.binomial(n = sample_size, p = success_rate)
   return count
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
   # note that the a, t values are not used for this case
   def fun_rate_child(a, t) :
      return ('prior_iota_child', None, None)
   def fun_iota_parent(a, t) :
      return ('prior_iota_parent', None, None)
   # ----------------------------------------------------------------------
   # age table:
   age_list    = [ 0.0, 50.0, 100.0 ]
   #
   # time table:
   time_list   = [ 1980.0, 2000.0, 2200.0 ]
   #
   # integrand table:
   integrand_table = [ { 'name':'prevalence' } ]
   #
   # node table:
   node_table = [ { 'name':'world', 'parent':'' } ]
   for i in range(n_children) :
      name = 'child_' + str(i + 1)
      node_table.append( { 'name':name, 'parent':'world' } )
   #
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   #
   # values that are the same for all data rows
   row = {
      'integrand':  'prevalence',
      'weight':      '',
      'hold_out':     False,
      'time_lower':   2000.,
      'time_upper':   2000.,
   }
   # values that change between rows:
   for data_id in range( n_data ) :
      row['node']         = 'child_' + str( (data_id % n_children) + 1 )
      fraction            = (data_id + 1) / float(n_data)
      age                 = age_list[0] + (age_list[-1] - age_list[0])*fraction
      row['age_lower']    = age
      row['age_upper']    = age
      meas_mean           = 1.0 - numpy.exp( -iota_true * age )
      assert 0.0 < meas_mean and meas_mean < 1.0
      sample_size         = next_sample_size()
      count               = binomial(sample_size, meas_mean)
      meas_value          = count / sample_size
      row['sample_size']  = sample_size
      row['meas_value']   = meas_value
      threshold = 5
      if threshold <= count :
         row['density']  = 'gaussian'
         k               = count
         n               = sample_size
         meas_std        = numpy.sqrt( (k + 1)*(n + 1 - k) / n ) / (n + 2)
         row['meas_std'] = meas_std
      else :
         row['density']  = 'binomial'
         row['meas_std'] = None
      data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      {  # prior_iota_child
         'name':     'prior_iota_child',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      0.1,
      },{ # prior_iota_parent
         'name':     'prior_iota_parent',
         'density':  'uniform',
         'lower':    iota_true / 10.,
         'upper':    1.0,
         'mean':     0.1,
      }
   ]
   #----------------------------------------------------------------------
   # smooth table
   name           = 'smooth_rate_child'
   fun            = fun_rate_child
   age_id         = int( len( age_list ) / 2 )
   time_id        = int( len( time_list ) / 2 )
   smooth_table = [
      {'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
   ]
   name = 'smooth_iota_parent'
   fun  = fun_iota_parent
   smooth_table.append(
      {'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
   )
   # no standard deviation multipliers
   for dictionary in smooth_table :
      for name in [ 'value' , 'dage', 'dtime' ] :
         key   = 'mulstd_' + name + '_prior_name'
         value = None
         dictionary[key] = value
   # ----------------------------------------------------------------------
   # rate table:
   rate_table = [
      {  'name':          'iota',
         'parent_smooth': 'smooth_iota_parent',
         'child_smooth':  'smooth_rate_child',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },
      { 'name':'random_seed',            'value':str(random_seed)    },
      { 'name':'parent_node_name',       'value':'world'             },
      { 'name':'ode_step_size',          'value':'10.0'              },
      { 'name':'meas_noise_effect',      'value':'add_std_scale_all' },

      { 'name':'quasi_fixed',            'value':'false'        },
      { 'name':'derivative_test_fixed',  'value':'first-order'  },
      { 'name':'max_num_iter_fixed',     'value':'100'          },
      { 'name':'print_level_fixed',      'value':'0'            },
      { 'name':'tolerance_fixed',        'value':'1e-10'        },

      { 'name':'derivative_test_random', 'value':'second-order' },
      { 'name':'max_num_iter_random',    'value':'100'          },
      { 'name':'print_level_random',     'value':'0'            },
      { 'name':'tolerance_random',       'value':'1e-10'        }
   ]
   # ----------------------------------------------------------------------
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   # ----------------------------------------------------------------------
   # create database
   dismod_at.create_database(
      file_name        = file_name,
      age_list         = age_list,
      time_list        = time_list,
      integrand_table  = integrand_table,
      node_table       = node_table,
      subgroup_table   = subgroup_table,
      weight_table     = list(),
      covariate_table  = list(),
      avgint_table     = list(),
      data_table       = data_table,
      prior_table      = prior_table,
      smooth_table     = smooth_table,
      nslist_dict      = dict(),
      rate_table       = rate_table,
      mulcov_table     = list(),
      option_table     = option_table,
   )
   # ----------------------------------------------------------------------
   return
# ===========================================================================
# Run the init command to create the var table
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit' , 'both' ])
# -----------------------------------------------------------------------
# read database
connection      = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
var_table        = dismod_at.get_table_dict(connection, 'var')
rate_table       = dismod_at.get_table_dict(connection, 'rate')
node_table       = dismod_at.get_table_dict(connection, 'node')
rate_table       = dismod_at.get_table_dict(connection, 'rate')
fit_var_table    = dismod_at.get_table_dict(connection, 'fit_var')
# -----------------------------------------------------------------------------
count = 0
for (var_id, var_row) in enumerate(var_table) :
   var_type  = var_row['var_type']
   node_id   = var_row['node_id']
   node_name = None
   if node_id != None :
      node_name = node_table[node_id]['node_name']
   if var_type == 'rate' and node_name == 'world' :
      count += 1
      assert count == 1
      #
      rate_id   = var_row['rate_id']
      rate_name = rate_table[rate_id]['rate_name']
      assert rate_name == 'iota'
      #
      fit_var_value = fit_var_table[var_id]['fit_var_value']
      rel_error     = 1. - fit_var_value / iota_true
      if abs(rel_error) >= 0.1 :
         print( f'binomial.py: rel_error = {rel_error}')
      assert abs(rel_error) < 0.1
connection.close()
print('binomial.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
