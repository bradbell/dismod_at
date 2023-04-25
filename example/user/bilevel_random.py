# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_bilevel_random.py}
# {xrst_spell
#     subgraph
# }
# {xrst_comment_ch #}
#
# Example Fitting With Two Levels of Random Effects
# #################################################
#
# Node Table
# **********
# The following is a diagram of the node tree for this example:
# ::
#
#                   n1
#             /-----/\-----\
#           n11            n12
#         /     \        /     \
#       n111   n112    n121   n122
#
# We refer to *n1* as the root node and
# *n111* , *n112* , *n121* , *n122* as the leaf nodes.
#
# Problem Parameters
# ******************
# The following parameters, used in this example, can be changed:
# {xrst_literal
#     begin problem parameters
#     end problem parameters
# }
#
# Model Variables
# ***************
#
# n1
# **
# The values of iota at node n1 at age 0 and age 100 are fixed effects.
#
# n11, n12
# ********
# There is a
# :ref:`child rate effect<model_variables@Random Effects, u@Child Rate Effects>`
# for nodes n11 and node n12.
# This adjusts the value at node n1 to its value for n11 and n12.
#
# n111, n112, n121, 122
# *********************
# There is a
# :ref:`subgroup covariate multiplier<model_variables@Random Effects, u@Subgroup Covariate Multipliers>`
# for nodes n111, n112, n121, and n122.
# The effect (multiplier times covariate) for the target
# nodes n111 and n112 adjusts the value at node n11 to the target node.
# The effect for the target
# nodes n121 and n122 adjusts the value at node n12 to the target node.
#
# Subgroup Table
# **************
# The subgroup table is used for the second level of random effects and
# contains the following subgraph of the node graph:
# ::
#
#           n11            n12
#         /     \        /     \
#       n111   n112    n121   n122
#
# In addition, a special subgroup called ``none`` is
# used for data points that correspond to nodes n1, n11, and n12; i.e.,
# the parent node and the nodes at the first level.
#
# Mulcov Table
# ************
# There are two entries in the mulcov table.
# The first (second) entry is for group n11 (n12) and corresponds to the
# subgroup covariate multipliers for n111, n112 (n121, n122).
#
# Data Table
# **********
# If there was only data for the leaf nodes,
# (n111, n112, n121, n122) there would be an ambiguity in the solution.
# For example, we could shift the estimates for n111 and n112 by
# :math:`+ \Delta` and the estimate for n11 by :math:`- \Delta`
# and get the same fit to the data and prior
# (because we are using a uniform of the priors).
# For this reason, we have included data for n11, n12, and n1.
# If you like, you can think of this data as prior information.
#
# Procedure
# *********
#
# Fit Both
# ========
# Create the database, initialize it, and fit both fixed and random effects.
#
# Compare Fit and Truth
# =====================
# check the fit results and create the truth_var table.
#
# Sample Posterior and Check Coverage
# ===================================
# Sample from the posterior distribution and check for coverage.
# Note that using two levels of random effects (instead of one)
# should give a better value of the uncertainty of the model variables
# (when there are two levels of random effects).
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_bilevel_random.py}
#
# $end
# ----------------------------------------------------------------------------
# BEGIN PYTHON
# begin problem parameters
# True value of iota at node n1
def iota_n1(age) :
   return 0.01 + 0.01 * age / 100.0 # must be non-decreasing with age
data_per_node =  20    # number of simulated data points for each leaf node
meas_cv       =  0.1   # coefficient of variation for each data point
random_seed   = 0      # if zero, seed off the clock
number_sample = 10     # number of posterior samples
#
# True value for random effects
random_effect = dict()
random_effect['n11']  =  0.1
random_effect['n12']  = -random_effect['n11']
random_effect['n111'] =  0.2
random_effect['n112'] = -random_effect['n111']
random_effect['n121'] =  0.3
random_effect['n122'] = -random_effect['n121']
# end problem parameters
# ----------------------------------------------------------------------------
# imports
import sys
import os
import copy
import random
import math
import numpy
import shutil
import time
if random_seed == 0 :
   random_seed = int( time.time() )
# ----------------------------------------------------------------------------
# run in build/example/user using local (not installed) version of dismod_at
test_program = 'example/user/bilevel_random.py'
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
# ----------------------------------------------------------------------------
#
def avg_integrand(age, node) :
   total_effect = 0.0
   if len(node) >= 3 :
      total_effect += random_effect[ node[0:3] ]
   if len(node) == 4 :
      total_effect += random_effect[node]
   return iota_n1(age) * math.exp(total_effect)
# ----------------------------------------------------------------------------
# example.db function
def example_db (file_name) :
   def fun_iota_n1(a, t) :
      return ('prior_iota_n1_value', None, None)
   def fun_iota_child(a, t) :
      return ('prior_iota_child', None, None)
   def fun_iota_subgroup(a, t) :
      return ('prior_iota_subgroup', None, None)
   #
   # node_table
   node_table = [
      { 'name':'n1',      'parent':''    },
      { 'name':'n11',     'parent':'n1'  },
      { 'name':'n12',     'parent':'n1'  },
      { 'name':'n111',    'parent':'n11' },
      { 'name':'n112',    'parent':'n11' },
      { 'name':'n121',    'parent':'n12' },
      { 'name':'n122',    'parent':'n12' },
   ]
   # age_table
   age_list = [ 0.0, 100.0 ]
   # time_table
   time_list = [ 1990.0, 2020.0 ]
   # rate_table
   rate_table = [ {
      'name':          'iota',
      'parent_smooth': 'smooth_iota_n1',
      'child_smooth':  'smooth_iota_child',
   } ]
   #
   # subgroup_table
   subgroup_table = [
      { 'subgroup':'none',  'group':'none' },
      { 'subgroup':'n111',  'group':'n11'  },
      { 'subgroup':'n112',  'group':'n11'  },
      { 'subgroup':'n121',  'group':'n12'  },
      { 'subgroup':'n122',  'group':'n12'  },
   ]
   # covariate_table
   covariate_table = [
      { 'name':'one',    'reference':0.0 }
   ]
   # mulcov_table
   mulcov_table = [
      {
      'covariate': 'one',
      'type':      'rate_value',
      'effected':  'iota',
      'group':     'n11',
      'smooth':    None,
      'subsmooth': 'smooth_iota_subgroup',
      },{
      'covariate': 'one',
      'type':      'rate_value',
      'effected':  'iota',
      'group':     'n12',
      'smooth':    None,
      'subsmooth': 'smooth_iota_subgroup',
      }
   ]
   # prior_table
   prior_table = [
      {   # prior_iota_n1_value
         'name':    'prior_iota_n1_value',
         'density': 'uniform',
         'lower':   iota_n1(0)   / 100.0,
         'upper':   iota_n1(100) * 100.0,
         'mean':    iota_n1(50) / 2.0,
      },{ # prior_iota_child
         'name':    'prior_iota_child',
         'density': 'uniform',
         'mean':     0.0,
      },{ # prior_iota_subgroup
         'name':    'prior_iota_subgroup',
         'density': 'uniform',
         'mean':     0.0,
      }
   ]
   # smooth_table
   smooth_table = [
      {   # smooth_iota_n1
         'name' :    'smooth_iota_n1',
         'age_id':   [0, 1],
         'time_id':  [0],
         'fun':      fun_iota_n1
      },{ # smooth_iota_child
         'name' :    'smooth_iota_child',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      fun_iota_child
      },{ # smooth_iota_subgroup
         'name' :    'smooth_iota_subgroup',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      fun_iota_subgroup
      }
   ]
   # weight table:
   weight_table = list()
   # nslist_dict
   nslist_dict = dict()
   # option_table
   option_table = [
      { 'name':'parent_node_name',      'value':'n1'},
      { 'name':'rate_case',             'value':'iota_pos_rho_zero'},
      { 'name': 'zero_sum_child_rate',  'value':'iota'},
      { 'name':'quasi_fixed',           'value':'false'},
      { 'name':'max_num_iter_fixed',    'value':'100'},
      { 'name':'print_level_fixed',     'value':'0'},
      { 'name':'tolerance_fixed',       'value':'1e-12'},
   ]
   # integrand_table
   integrand_table = [ {'name':'Sincidence'} ]
   # ------------------------------------------------------------------------
   # data_table
   data_table = list()
   # values that are the same for all data points
   row = {
      'integrand':   'Sincidence',
      'density':     'gaussian',
      'weight':      '',
      'hold_out':    False,
      'time_lower':  2000.0,
      'time_upper':  2000.0,
      'one':         1.0,
   }
   random.seed(random_seed)
   for age_id in range( len(age_list) ) :
      age       = age_list[age_id]
      for node in [ 'n1', 'n11', 'n12', 'n111', 'n112', 'n121', 'n122' ] :
         for i in range(data_per_node) :
            iota       = avg_integrand(age, node)
            meas_std   = iota * meas_cv
            meas_value = random.gauss(iota, meas_std)
            if node in [ 'n1', 'n11', 'n12' ] :
               row['subgroup'] = 'none'
            else :
               row['subgroup']   = node
            row['node']       = node
            row['meas_value'] = meas_value
            row['meas_std']   = meas_std
            row['age_lower']  = age
            row['age_upper']  = age
            data_table.append( copy.copy(row) )
   # ------------------------------------------------------------------------
   # avgint_table
   avgint_table = list()
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
# ---------------------------------------------------------------------------
# Fit Both
# ---------------------------------------------------------------------------
#
# create the database
file_name  = 'example.db'
example_db(file_name)
#
# initialize the database
program = '../../devel/dismod_at'
dismod_at.system_command_prc( [ program, file_name, 'init' ] )
#
# fit both the fixed and random effects
dismod_at.system_command_prc( [ program, file_name, 'fit', 'both' ] )
# ---------------------------------------------------------------------------
# Compare Fit and Truth
# ---------------------------------------------------------------------------
#
# read var table and supporting information including the fit
connection       = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
subgroup_table   = dismod_at.get_table_dict(connection, 'subgroup')
node_table       = dismod_at.get_table_dict(connection, 'node')
age_table        = dismod_at.get_table_dict(connection, 'age')
var_table        = dismod_at.get_table_dict(connection, 'var')
fit_var_table    = dismod_at.get_table_dict(connection, 'fit_var')
n_var            = len(var_table)
#
# check fit results and create the truth_var table
tbl_name         = 'truth_var'
col_name         = [ 'truth_var_value' ]
col_type         = [ 'real' ]
row_list         = list()
var_id2node_name = list()
for var_id in range(n_var) :
   var_type     = var_table[var_id]['var_type']
   age_id       = var_table[var_id]['age_id']
   if var_type == 'rate' :
      node_id   = var_table[var_id]['node_id']
      node_name = node_table[node_id]['node_name']
   else :
      assert var_type == 'mulcov_rate_value'
      subgroup_id  = var_table[var_id]['subgroup_id']
      node_name    = subgroup_table[subgroup_id]['subgroup_name']
   if node_name == 'n1' :
      age             = age_table[age_id]['age']
      truth_var_value = iota_n1(age)
   else :
      truth_var_value = random_effect[node_name]
   row_list.append( [ truth_var_value ] )
   var_id2node_name.append(node_name)
   #
   fit_var_value = fit_var_table[var_id]['fit_var_value']
   relerr           = 1.0 - fit_var_value / truth_var_value
   if abs(relerr) > 3.0 * meas_cv :
      print(node_name, truth_var_value, fit_var_value, relerr)
      assert False
#
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# ---------------------------------------------------------------------------
# Sample Posterior and Check Coverage
# ---------------------------------------------------------------------------
#
# sample from the posterior distribution
n_str = str(number_sample)
dismod_at.system_command_prc( [program, file_name, 'simulate', n_str] )
dismod_at.system_command_prc(
   [program, file_name, 'sample',  'simulate', 'both', n_str]
)
#
# compute sample standard deviation and check for coverate
connection   = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
sample_table    = dismod_at.get_table_dict(connection, 'sample')
sample_array    = numpy.zeros( (number_sample, n_var), dtype = numpy.double )
for sample_id in range( len(sample_table) ) :
   sample_index = sample_table[sample_id]['sample_index']
   var_id       = sample_table[sample_id]['var_id']
   var_value    = sample_table[sample_id]['var_value']
   assert sample_id == sample_index * n_var + var_id
   sample_array[sample_index, var_id] = var_value
sample_std  = numpy.std(sample_array, axis=0, ddof = 1)
#
# check for covarage
truth_var_table = dismod_at.get_table_dict(connection, 'truth_var')
for var_id in range(n_var) :
   var_type        = var_table[var_id]['var_type']
   age_id          = var_table[var_id]['age_id']
   node_name       = var_id2node_name[var_id]
   fit             = fit_var_table[var_id]['fit_var_value']
   truth           = truth_var_table[var_id]['truth_var_value']
   std             = sample_std[var_id]
   std_factor      = (fit - truth) / std
   if abs(std_factor) > 3.0 :
      print(node_name, ", std_factor = ", std_factor)
      print("std = ", std, ", fit = ", fit, ", truth = ", truth)
      assert False
#
# ----------------------------------------------------------------------------
print('bilevel_random.py: OK')
# END PYTHON
