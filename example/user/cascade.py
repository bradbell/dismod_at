# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_cascade.py}
# {xrst_spell
#     misspecification
# }
# {xrst_comment_ch #}
#
# Generating Priors For Next Level Down Node Tree
# ###############################################
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
# Problem
# *******
# Given the information for a fit with *n1* as the parent,
# with corresponding data *y1* ,
# pass down summary information for a fit with *n11* as the parent
# with corresponding data *y11* .
#
# Procedure
# *********
#
# Step 1: Create Database
# =======================
# This first database ``fit_n1.db``
# is for fitting with *n1* as the parent and predicting
# for *n11* .
#
# Step 2: Fit With n1 As Parent
# =============================
# Use :ref:`fit both<fit_command@variables@both>`
# to fit with *n1* as the parent to obtain
# *e1* the corresponding estimate for the :ref:`model_variables-name` .
# This is done using database ``fit_n1.db``
#
# Step 3: Simulate Data
# =====================
# Set the :ref:`truth_var_table-name` equal to the estimate *e1*
# and then use the :ref:`simulate_command-name` to simulate *N* data sets.
# This is done using database ``fit_n1.db``
#
# Step 4: Sample Posterior
# ========================
# Use the sample command with the
# :ref:`sample_command@simulate` method
# to create *N* samples of the model variables.
# Call these samples *s1_1* , ... , *s1_N* .
# This is done using database ``fit_n1.db``
#
# Step 5: Predictions For n11
# ===========================
# Use the predict command with the
# :ref:`predict_command@source@sample`
# to create *N* predictions for the
# model variable corresponding to fit with *n11* as the parent.
# Call these predictions *p11_1* , ... , *p11_N* .
# This is done using database ``fit_n1.db``
#
# Step 6: Priors For n11 As Parent
# ================================
# Use the predictions *p11_1* , ... , *p11_N* to create priors
# for the model variables corresponding to fitting with *n11*
# as the parent and with data *y11* .
# In this process account for the fact that the data *y11* is a subset
# of *y1* which was used to obtain the predictions.
# These priors are written to the database ``fit_n11.db``
# which starts as a copy of the final ``fit_n1.db`` .
# This is done so that the subsequent
# :ref:`init<init_command-name>` and :ref:`fit<fit_command-name>` commands
# do not wipe out the results stored in ``fit_n1.db`` .
#
# Step 7: Fit n11 As Parent
# =========================
# Use :ref:`fit both<fit_command@variables@both>`
# to fit with *n11* as the parent to obtain
# *e11* corresponding estimate for the model variables.
#
# Problem Parameters
# ******************
# The following parameters, used in this example, can be changed:
# {xrst_literal
#     begin problem parameters
#     end problem parameters
# }
#
# Age and Time Values
# *******************
# The time values do not matter for this problem
# because all the functions are constant with respect to time.
# The :ref:`age_table-name` for this problem is given by
# {xrst_literal
#     BEGIN age_table
#     END age_table
# }
# We use *n_age* to denote the length of this table.
#
# Rate Table
# **********
# The only rate in this problem is *iota* . There are *n_age*
# :ref:`parent rate<model_variables@Fixed Effects, theta@Parent Rates>`
# values for *iota* , one for each point in the age table.
# There are two *iota*
# :ref:`model_variables@Random Effects, u@Child Rate Effects` ,
# one for each child node.
# Note that there are two children when fitting *n1* as the parent
# and when fitting *n11* as the parent.
#
# Covariates
# **********
# There are two :ref:`covariates<covariate_table-name>` for this example.
# One covariate has the constant one and reference zero.
# The other covariate is income and uses the average for its reference.
# The average income is different depending on whether
# *n1* or *n11*
# is the parent.
#
# Multipliers
# ***********
# There are two
# :ref:`model_variables@Fixed Effects, theta@Group Covariate Multipliers` .
#
# gamma
# =====
# One multiplier multiples the constant one and models the unknown variation
# in the data (sometimes referred to as model misspecification).
# We call this covariate multiplier
# :ref:`gamma<data_like@Measurement Noise Covariates@gamma_j (a, t)>` .
# We use a uniform prior on this multiplier so that it absorbs
# all the noise due to model misspecification.
# When checking for coverage by the samples *s1_1* , ... , *s1_N* ,
# we expand the sample standard deviation by a factor of
# (1 + *gamma* ) .
# This accounts for the fact that the noise absorbed by *gamma*
# is modeled as independent between data points.
# When fitting with *n1* as the parent, this noise is
# correlated between samples in the same leaf.
#
# alpha
# =====
# The other multiplier multiplies income and affects *iota* .
# We call this covariate multiplier
# :ref:`alpha<avg_integrand@Rate Functions@Group Rate Covariate Multiplier, alpha_jk>` .
# We note that both average income and random effects vary between the nodes.
# When fitting with *n1* as the parent,
# *alpha* tries to absorb the random effects at the leaf level.
# We use a Laplace prior on *alpha* to reduce this effect.
#
# Data Table
# **********
# For this example, all the data is
# :ref:`avg_integrand@Integrand, I_i(a,t)@Sincidence` .
# There are *data_per_leaf* data point for each leaf node.
# Income is varies within each leaf node so the random effect
# can be separated from the income effect.
# Normally there is much more data, so we compensate by using
# a small coefficient of variation for the measurement values
# *meas_cv* .
# The simulation value of *iota* , corresponding to no effect, is
# a function of age and defined by ``iota_no_effect`` ( *age* ) .
# Each data point corresponds to a leaf node.
# The total effect for a data point is
# the random effect for the leaf node,
# plus the random effect for parent of the leaf,
# plus the income effect.
# Each data point is for a specific age and the corresponding mean
# is *iota_no_effect* ( ``age`` ) times the exponential of the total effect.
# The standard deviation of the data is *meas_cv* times its mean.
# A Gaussian with this mean and standard deviation is used to simulate
# each data point.
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_cascade.py}
#
# $end
# ----------------------------------------------------------------------------
# BEGIN PYTHON
# begin problem parameters
def iota_no_effect(age) :
   return 0.01 + 0.01 * age / 100.0 # must be non-decreasing with age
data_per_leaf =  10    # number of simulated data points for each leaf node
meas_cv       =  0.10  # coefficient of variation for each data point
alpha_true    = -0.10  # rate_value covariate multiplier used to simulate data
random_seed   = 0      # if zero, seed off the clock
number_sample =  10    # number of simulated data sets and posterior samples
#
random_effect = dict()
random_effect['n11']  =  0.2
random_effect['n12']  = -random_effect['n11']
random_effect['n111'] =  0.1
random_effect['n112'] = -random_effect['n111']
random_effect['n121'] =  0.1
random_effect['n122'] = -random_effect['n121']
#
avg_income = dict()
avg_income['n111'] = 1.0
avg_income['n112'] = 2.0
avg_income['n121'] = 3.0
avg_income['n122'] = 4.0
avg_income['n11']  = (avg_income['n111'] + avg_income['n112'])/2.0
avg_income['n12']  = (avg_income['n121'] + avg_income['n122'])/2.0
avg_income['n1']   = (avg_income['n11']  + avg_income['n12']) /2.0
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
test_program = 'example/user/cascade.py'
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
# count number of rows in an sql file
def sql_count_rows(connection, table_name) :
   sqlcmd = 'SELECT COUNT(*) FROM ' + table_name
   result = dismod_at.sql_command(connection, sqlcmd)
   n_row  = result[0][0]
   return n_row
#
# average integrand
def avg_integrand(age, income, node) :
   total_effect  = alpha_true * (income - avg_income['n1'])
   if len(node) >= 3 :
      total_effect += random_effect[ node[0:3] ]
   if len(node) == 4 :
      total_effect += random_effect[node]
   return iota_no_effect(age) * math.exp(total_effect)
# ----------------------------------------------------------------------------
def example_db (file_name) :
   def fun_iota_n1(a, t) :
      return ('prior_iota_n1_value', 'prior_iota_n1_dage', None)
   def fun_iota_child(a, t) :
      return ('prior_iota_child', None, None)
   def fun_alpha_n1(a, t) :
      return ('prior_alpha_n1', None, None)
   def fun_gamma(a, t) :
      return ('prior_gamma', None, None)
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
   # BEGIN age_table
   age_list = [ 0.0, 20.0, 40.0, 60.0, 80.0, 100.0 ]
   # END age_table
   #
   # time_table
   time_list = [ 1990.0, 2020.0 ]
   # rate_table
   rate_table = [ {
      'name':          'iota',
      'parent_smooth': 'smooth_iota_n1',
      'child_smooth':  'smooth_iota_child',
   } ]
   # covariate_table
   covariate_table = [
      { 'name':'one',    'reference':0.0 },
      { 'name':'income', 'reference':avg_income['n1'] },
   ]
   # mulcov_table
   mulcov_table = [ {
      'covariate': 'one',
      'type':      'meas_noise',
      'effected':  'Sincidence',
      'group':     'world',
      'smooth':    'smooth_gamma'
      },{
      'covariate': 'income',
      'type':      'rate_value',
      'effected':  'iota',
      'group':     'world',
      'smooth':    'smooth_alpha_n1'
   } ]
   # prior_table
   prior_table = [
      {   # prior_iota_n1_value
         'name':    'prior_iota_n1_value',
         'density': 'uniform',
         'lower':   iota_no_effect(0)   / 10.0,
         'upper':   iota_no_effect(100) * 10.0,
         'mean':    iota_no_effect(50)
      },{ # prior_iota_n1_dage
         'name':    'prior_iota_n1_dage',
         'density': 'log_gaussian',
         'mean':     0.0,
         'std':      1.0,
         'eta':      iota_no_effect(0) / 100.0
      },{ # prior_iota_child
         'name':    'prior_iota_child',
         'density': 'gaussian',
         'mean':     0.0,
         'std':      1.0,
      },{ # prior_alpha_n1
         'name':    'prior_alpha_n1',
         'density': 'laplace',
         'mean':     0.0,
         'std':      0.01,
         'lower':   -abs(alpha_true) * 10.0,
         'upper':    abs(alpha_true) * 10.0,
      },{ # prior_gamma
         'name':    'prior_gamma',
         'density': 'uniform',
         'mean':     0.0,
         'std':      1.0,
         'lower':    0.0,
         'upper':   10.0,
      }
   ]
   # smooth_table
   smooth_table = [
      {   # smooth_iota_n1
         'name' :    'smooth_iota_n1',
         'age_id':   range(len(age_list)),
         'time_id':  [0],
         'fun':      fun_iota_n1
      },{ # smooth_iota_child
         'name' :    'smooth_iota_child',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      fun_iota_child
      },{ # smooth_alpha_n1
         'name':     'smooth_alpha_n1',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      fun_alpha_n1
      },{ # smooth_gamma
         'name':     'smooth_gamma',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      fun_gamma
      }
   ]
   # weight table:
   weight_table = list()
   # nslist_table
   nslist_table = dict()
   # option_table
   option_table = [
      { 'name':'parent_node_name',      'value':'n1'},
      { 'name':'rate_case',             'value':'iota_pos_rho_zero'},
      { 'name': 'zero_sum_child_rate',  'value':'iota'},
      { 'name': 'meas_noise_effect',    'value':'add_var_scale_all'},
      { 'name':'quasi_fixed',           'value':'false'},
      { 'name':'max_num_iter_fixed',    'value':'100'},
      { 'name':'print_level_fixed',     'value':'0'},
      { 'name':'tolerance_fixed',       'value':'1e-12'},
   ]
   # integrand_table
   integrand_table = [ {'name':'Sincidence'}, {'name':'mulcov_1'} ]
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
      'subgroup':    'world',
   }
   assert covariate_table[1]['name'] == 'income'
   random.seed(random_seed)
   for age_id in range( len(age_list) ) :
      age       = age_list[age_id]
      for node in [ 'n111', 'n112', 'n121', 'n122' ] :
         for i in range(data_per_leaf) :
            income = i * avg_income[node] * 2.0 / (data_per_leaf - 1)
            iota       = avg_integrand(age, income, node)
            meas_std   = iota * meas_cv
            meas_value = random.gauss(iota, meas_std)
            row['node']       = node
            row['meas_value'] = meas_value
            row['meas_std']   = meas_std
            row['age_lower']  = age
            row['age_upper']  = age
            row['income']     = income
            data_table.append( copy.copy(row) )
   # ------------------------------------------------------------------------
   # avgint_table
   avgint_table = list()
   # values that are the same for all data points
   row = {
      'node':        'n11',
      'subgroup':    'world',
      'weight':      '',
      'hold_out':    False,
      'time_lower':  2000.0,
      'time_upper':  2000.0,
      'income':      avg_income['n11'],
      'one':         1.0,
   }
   for age_id in range( len(age_list) ) :
      age  = age_list[age_id]
      row['integrand']  = 'Sincidence'
      row['age_lower']  = age
      row['age_upper']  = age
      avgint_table.append( copy.copy(row) )
   # alpha is constant w.r.t age and time
   assert mulcov_table[1]['type'] == 'rate_value'
   row['integrand'] = 'mulcov_1'
   row['age_lower'] = 0.0
   row['age_upper'] = 0.0
   avgint_table.append( copy.copy(row) )
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
# ---------------------------------------------------------------------------
# Step 1: Create fit_n1.db
# ---------------------------------------------------------------------------
file_name  = 'fit_n1.db'
example_db(file_name)
#
# init
program = '../../devel/dismod_at'
dismod_at.system_command_prc( [ program, file_name, 'init' ] )
# -----------------------------------------------------------------------------
# Step 2: Fit With n1 As Parent
# -----------------------------------------------------------------------------
dismod_at.system_command_prc( [ program, file_name, 'fit', 'both' ] )
#
# check e1
new              = False
connection       = dismod_at.create_connection(file_name, new)
rate_table       = dismod_at.get_table_dict(connection, 'rate')
node_table       = dismod_at.get_table_dict(connection, 'node')
age_table        = dismod_at.get_table_dict(connection, 'age')
var_table        = dismod_at.get_table_dict(connection, 'var')
fit_var_table    = dismod_at.get_table_dict(connection, 'fit_var')
covariate_table  = dismod_at.get_table_dict(connection, 'covariate')
n_var            = len(var_table)
for var_id in range(n_var) :
   var_type     = var_table[var_id]['var_type']
   age_id       = var_table[var_id]['age_id']
   rate_id      = var_table[var_id]['rate_id']
   node_id      = var_table[var_id]['node_id']
   covariate_id = var_table[var_id]['covariate_id']
   value        = fit_var_table[var_id]['fit_var_value']
   fixed        = True
   if var_type == 'rate' :
      age  = age_table[age_id]['age']
      node = node_table[node_id]['node_name']
      rate = rate_table[rate_id]['rate_name']
      assert rate == 'iota'
      if node == 'n1' :
         truth = iota_no_effect(age)
      else :
         truth = random_effect[node]
         fixed = False
   elif var_type == 'mulcov_rate_value' :
      rate      = rate_table[rate_id]['rate_name']
      covariate = covariate_table[covariate_id]['covariate_name']
      assert rate == 'iota'
      assert covariate == 'income'
      truth = alpha_true
   else :
      covariate = covariate_table[covariate_id]['covariate_name']
      assert var_type == 'mulcov_meas_noise'
      assert covariate == 'one'
      gamma_fit_n1 = value
   if var_type != 'mulcov_meas_noise' :
      rel_err = (1.0 - value / truth)
      fmt = 'fixed={}, truth={:7.4f}, value={:7.4f}, rel_err={:6.3f}'
      # print( fmt.format(fixed, truth, value, rel_err) )
      if abs(rel_err) >= 2e-1 :
         print( fmt.format(fixed, truth, value, rel_err) )
         print("random_seed = ",  random_seed)
         assert False
# -----------------------------------------------------------------------------
# Step 3: Simulate Data
# Step 4: Sample Posterior
# -----------------------------------------------------------------------------
# obtain s1_1, ... , s1_N
N_str = str(number_sample)
dismod_at.system_command_prc(
   [ program, file_name, 'set', 'truth_var', 'fit_var' ]
)
dismod_at.system_command_prc(
   [ program, file_name, 'set', 'start_var', 'fit_var' ]
)
dismod_at.system_command_prc(
   [ program, file_name, 'set', 'scale_var', 'fit_var' ]
)
dismod_at.system_command_prc([ program, file_name, 'simulate', N_str ])
dismod_at.system_command_prc(
   [ program, file_name, 'sample', 'simulate', 'both', N_str ]
)
#
# check coverage of true values by posterior samples standard deviation
connection.close()
connection   = dismod_at.create_connection(file_name, new)
sample_table = dismod_at.get_table_dict(connection, 'sample')
sample_array = numpy.zeros( (number_sample, n_var), dtype = numpy.double )
for sample_id in range( len(sample_table) ) :
   sample_index = sample_table[sample_id]['sample_index']
   var_id       = sample_table[sample_id]['var_id']
   var_value    = sample_table[sample_id]['var_value']
   assert sample_id == sample_index * n_var + var_id
   sample_array[sample_index, var_id] = var_value
sample_std  = numpy.std(sample_array, axis=0, ddof = 1)
#
for var_id in range(n_var) :
   var_type     = var_table[var_id]['var_type']
   age_id       = var_table[var_id]['age_id']
   node_id      = var_table[var_id]['node_id']
   covariate_id = var_table[var_id]['covariate_id']
   fit          = fit_var_table[var_id]['fit_var_value']
   std          = sample_std[var_id] * (1.0 + gamma_fit_n1)
   if var_type == 'rate' :
      age  = age_table[age_id]['age']
      node = node_table[node_id]['node_name']
      if node == 'n1' :
         truth = iota_no_effect(age)
      else :
         truth = random_effect[node]
   elif var_type == 'mulcov_rate_value' :
      truth = alpha_true
   else :
      assert var_type == 'mulcov_meas_noise'
   if var_type != 'mulcov_meas_noise' :
      std_factor = abs( (fit - truth) ) / std
      fmt = 'truth={:7.4f}, fit={:7.4f}, std_factor={:6.3f}'
      # print( fmt.format(truth, fit, std_factor) )
      if std_factor > 3.0 :
         print( fmt.format(truth, fit, std_factor) )
         print("random_seed = ",  random_seed)
         assert False
# ----------------------------------------------------------------------------
# Step 5: Predictions For n11
# ----------------------------------------------------------------------------
# obtain p11_1, p_11_2, ...
# and add prior_n11_age values to data base
dismod_at.system_command_prc([ program, file_name, 'predict', 'sample' ])
avgint_table    = dismod_at.get_table_dict(connection, 'avgint')
predict_table   = dismod_at.get_table_dict(connection, 'predict')
n_avgint        = len( avgint_table )
n_predict       = len( predict_table )
n_subset        = int( n_predict / number_sample )
assert n_predict == n_subset * number_sample
predict_array = numpy.zeros( (number_sample, n_avgint), dtype = numpy.double )
predict_found = n_avgint * [False]
for predict_id in range( n_predict ) :
   sample_index  = predict_table[predict_id]['sample_index']
   avgint_id     = predict_table[predict_id]['avgint_id']
   value         = predict_table[predict_id]['avg_integrand']
   predict_array[sample_index, avgint_id] = value
   predict_found[avgint_id] = True
predict_mean = numpy.mean(predict_array, axis=0)
predict_std  = numpy.std(predict_array, axis=0, ddof = 1)
# -----------------------------------------------------------------------------
# Step 6: Priors for n11 as Parent
# -----------------------------------------------------------------------------
# create fit_n11.db starting from fit_n1.db
shutil.copyfile(file_name, 'fit_n11.db')
file_name = 'fit_n11.db'
connection.close()
connection       = dismod_at.create_connection(file_name, new)
#
# get last id from certain tables
sqlcmd           = 'SELECT COUNT(prior_id) FROM prior'
result           = dismod_at.sql_command(connection, sqlcmd)
prior_id         = sql_count_rows(connection, 'prior') - 1
smooth_id        = sql_count_rows(connection, 'smooth') - 1
smooth_grid_id   = sql_count_rows(connection, 'smooth_grid') - 1
#
sqlcmd      = 'SELECT density_id FROM density WHERE density_name=="uniform"'
result      = dismod_at.sql_command(connection, sqlcmd)
uniform_id  = result[0][0]
#
sqlcmd      = 'SELECT density_id FROM density WHERE density_name=="gaussian"'
result      = dismod_at.sql_command(connection, sqlcmd)
gaussian_id = result[0][0]
#
# add smooth_iota_n11
smooth_name = 'smooth_iota_n11'
n_age       = len( age_table )
smooth_id   = smooth_id + 1
sqlcmd  = 'INSERT INTO smooth \n'
sqlcmd += '(smooth_id, smooth_name, n_age, n_time) \n'
sqlcmd += 'VALUES (' + str(smooth_id) + ','
sqlcmd += '"' + smooth_name + '",'
sqlcmd += str( n_age ) + ','
sqlcmd += '1)'
dismod_at.sql_command(connection, sqlcmd)
iota_smooth_id = smooth_id
#
# add smooth_alpha_n11
smooth_name = 'smooth_alpha_n11'
smooth_id   = smooth_id + 1
sqlcmd  = 'INSERT INTO smooth \n'
sqlcmd += '(smooth_id, smooth_name, n_age, n_time) \n'
sqlcmd += 'VALUES (' + str(smooth_id) + ','
sqlcmd += '"' + smooth_name + '",'
sqlcmd += '1,'
sqlcmd += '1)'
dismod_at.sql_command(connection, sqlcmd)
alpha_smooth_id = smooth_id
#
# add prior_none
prior_name = 'prior_none'
prior_id   = prior_id + 1
sqlcmd     = 'INSERT INTO prior \n'
sqlcmd    += '(prior_id, prior_name, density_id, mean) \n'
sqlcmd    += 'VALUES (' + str(prior_id) + ','
sqlcmd    += '"' + prior_name + '",'
sqlcmd    += str(uniform_id) + ','
sqlcmd    += '0)'
dismod_at.sql_command(connection, sqlcmd)
none_prior_id = smooth_id
#
# add new entries in prior and smooth_grid tables
assert len(age_table) == n_avgint - 1
for avgint_id in range( n_avgint ) :
   assert predict_found[avgint_id]
   age        = avgint_table[avgint_id]['age_lower']
   mean       = predict_mean[avgint_id]
   std        = (1.0 + gamma_fit_n1) * predict_std[avgint_id]
   #
   # entry in prior table for alpha
   prior_id   = prior_id + 1
   prior_name = 'prior_alpha_n11'
   lower      = 'null'
   upper      = 'null'
   if avgint_id < len(age_table) :
      # entry in prior table for iota
      prior_name = 'prior_iota_n11_' + str(int(age))
      lower      = str( iota_no_effect(0) / 10.0 )
      upper      = str( iota_no_effect(100) * 10.0 )
   sqlcmd  = 'INSERT INTO prior \n'
   sqlcmd += '(prior_id, prior_name, density_id, mean, std, lower, upper)\n'
   sqlcmd += 'VALUES (' + str(prior_id) + ','
   sqlcmd += '"' + prior_name + '",'
   sqlcmd += str(gaussian_id) + ','
   sqlcmd += str( round(mean, 4) ) + ','
   sqlcmd += str( round(std, 5) ) + ','
   sqlcmd += lower + ','
   sqlcmd += upper + ')'
   dismod_at.sql_command(connection, sqlcmd)
   #
   # entry in smooth_grid table
   age_id          = 0
   time_id         = 0
   value_prior_id  = prior_id
   dage_prior_id   = none_prior_id
   tmp_smooth_id   = alpha_smooth_id
   if avgint_id < len(age_table) :
      assert age == age_table[avgint_id]['age']
      age_id        = avgint_id
      tmp_smooth_id = iota_smooth_id
   smooth_grid_id   = smooth_grid_id + 1
   sqlcmd  = 'INSERT INTO smooth_grid \n'
   sqlcmd += '(smooth_grid_id, smooth_id, age_id, time_id, '
   sqlcmd += 'value_prior_id, dage_prior_id) \n'
   sqlcmd += 'VALUES (' + str(smooth_grid_id) + ','
   sqlcmd += str(tmp_smooth_id) + ','
   sqlcmd += str(age_id) + ','
   sqlcmd += str(time_id) + ','
   sqlcmd += str(value_prior_id) + ','
   sqlcmd += str(dage_prior_id) + ')'
   dismod_at.sql_command(connection, sqlcmd)
#
# change parent to be n11
sqlcmd  = 'UPDATE option SET option_value = "n11"'
sqlcmd += ' WHERE option_name == "parent_node_name"'
dismod_at.sql_command(connection, sqlcmd)
#
# use smooth_iota_n11 for parent smoothing of iota
sqlcmd  = 'UPDATE rate SET parent_smooth_id = ' + str(iota_smooth_id)
sqlcmd += ' WHERE rate_name == "iota"'
dismod_at.sql_command(connection, sqlcmd)
#
# use smooth_alpha_n11 for smoothing alpha
sqlcmd  = 'UPDATE mulcov SET group_smooth_id = ' + str(alpha_smooth_id)
sqlcmd += ' WHERE mulcov_type == "rate_value"'
dismod_at.sql_command(connection, sqlcmd)
#
# change reference income
sqlcmd  = 'UPDATE covariate SET reference = ' + str(avg_income['n11'])
sqlcmd += ' WHERE covariate_name == "income"'
dismod_at.sql_command(connection, sqlcmd)
# ----------------------------------------------------------------------------
# Step 7: Fit With n11 as Parent
# ----------------------------------------------------------------------------
# obtain e11, estimate of model variables with n11 as the parent node
dismod_at.system_command_prc( [ program, file_name, 'init' ] )
dismod_at.system_command_prc( [ program, file_name, 'fit', 'both' ] )
#
# check e11
var_table        = dismod_at.get_table_dict(connection, 'var')
fit_var_table    = dismod_at.get_table_dict(connection, 'fit_var')
n_var            = len(var_table)
for var_id in range(n_var) :
   var_type     = var_table[var_id]['var_type']
   age_id       = var_table[var_id]['age_id']
   rate_id      = var_table[var_id]['rate_id']
   node_id      = var_table[var_id]['node_id']
   covariate_id = var_table[var_id]['covariate_id']
   value        = fit_var_table[var_id]['fit_var_value']
   fixed        = True
   if var_type == 'rate' :
      age  = age_table[age_id]['age']
      node = node_table[node_id]['node_name']
      rate = rate_table[rate_id]['rate_name']
      assert rate == 'iota'
      if node == 'n11' :
         truth = avg_integrand(age, avg_income[node], node)
      else :
         truth = random_effect[node]
         fixed = False
   elif var_type == 'mulcov_rate_value' :
      truth = alpha_true
   if var_type != 'mulcov_meas_noise' :
      rel_err = (1.0 - value / truth)
      fmt     = 'fixed={}, truth={:7.4f}, value={:7.4f}, rel_err={:6.3f}'
      # print( fmt.format(fixed, truth, value, rel_err) )
      if abs(rel_err) >= 2e-1 :
         print( fmt.format(fixed, truth, value, rel_err) )
         print("random_seed = ",  random_seed)
         assert False
# ----------------------------------------------------------------------------
print('cascade.py: OK')
# END PYTHON
