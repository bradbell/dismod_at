# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
import time
import sys
import os
import subprocess
import random
import math
#
# import dismod_at
path_to_example = 'example/user/diabetes.py'
if sys.argv[0] != path_to_example or not os.path.isdir( 'python/dismod_at' ) :
   usage  = 'python3 ' + path_to_example + '\n'
   usage += 'where python3 is the python 3 program on your system\n'
   usage += 'and working directory is the dismod_at distribution directory\n'
   sys.exit(usage)
sys.path.insert(0, os.getcwd() + '/python')
import dismod_at
#
# build/example/user
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
# ----------------------------------------------------------------------------
# {xrst_begin user_diabetes.py}
# {xrst_spell
#     mtspecific
#     bmi
#     def
#     exp
#     elif
#     rel
# }
# {xrst_comment_ch #}
#
# An Example Fitting Simulated Diabetes Data
# ##########################################
# This example fits iota and chi using only prevalence and mtspecific data.
# It is designed so that you are changed the setting documented below.
# See :ref:`user_example@Run One Example` for instruction on running this
# example.
#
# random_seed
# ***********
# This integer is used to seed the random number generator.
# If it is zero, the current number of seconds on the system clock is used.
# {xrst_code py}
random_seed = 0
# {xrst_code}
#
# age_list, time_list
# *******************
# These are the ages and times at which we simulate the data.
# They are also the ages and times where we model iota and chi.
# {xrst_code py}
age_list  = [ 0.0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 ]
time_list = [ 1980, 1990, 2000, 2010, 2020 ]
# {xrst_code}
#
# number_nodes
# ************
# This integer is the number of nodes.
# If it is one, there are no child nodes (hence no random effects).
# Otherwise, there are *number_nodes* - 1 child nodes.
# {xrst_code py}
number_nodes = 10
# {xrst_code}
#
# sim_std_random_effect
# *********************
# If there are child nodes, the random effect for each node is simulated
# using a Gaussian with this standard deviation.
# For each child node, the same random effect is used for both iota and chi.
# {xrst_code py}
sim_std_random_effect = 0.2
# {xrst_code}
#
# sim_mulcov
# **********
# This python dictionary contains the covariate multiplier values used
# to simulate the data.
# In this example, there is one covariate multiplier for each covariate,
# so we use the covariate names to identify the multipliers:
# {xrst_code py}
sim_mulcov = { 'sex':0.4, 'bmi':0.02, 'ms_2000':0.3  }
# {xrst_code}
#
# .. csv-table::
#  :header-rows: 1
#
#  Name,    Covariate Value,                Affects
#  sex,     (female=-0.5) (male=0.5),       chi
#  bmi,     body mass index,                iota
#  ms_2000, 1 (0) if year is (is not) 2000, prevalence
#
# sim_no_effect_rate
# ******************
# This is the value used to simulate the no effect rates; i.e.,
# the rates before the random effects and covariate effects are included.
# The dismod_at model for iota, chi and omega are constant with respect to
# age and time outside the grid limits for the corresponding rate.
# The last age is not included in the iota age grid because there
# is no prevalence data after it. Hence iota is constant for ages after
# the next to last age.
# The first age is not included in the chi age grid because prevalence
# is zero at that age (and mtspecific does not inform chi).
# Hence chi is constant for ages before the second age.
# {xrst_code py}
def sim_no_effect_rate(rate_name, age, time) :
   time     = min( time_list[-1], max(time_list[0], age) )
   age_exp  = - abs(age - 50.0) / 100.00
   time_exp = - abs(time - 2000.0) / 40.0
   if rate_name == 'iota' :
      age  = min( age_list[-2], max(age_list[0], age) )
      rate = 0.001 * ( 1.0 + math.exp(age_exp) + math.exp(time_exp) )
   elif rate_name == 'chi' :
      # chi is currently constant so age does not matter, but we set it here
      # incase you change that.
      age  = min( age_list[-1], max(age_list[1], age) )
      rate = 0.02
   elif rate_name == 'omega' :
      # For this example, omega must be constant for all age and time
      # (the value of omega does not affect prevalence or mtspecific).
      rate = 0.01
   else :
      rate = None
      assert False
   return rate
# {xrst_code}
#
# model_meas_cv
# *************
# The measurements are simulated without noise, but the model
# has this level of measurement nose (express as a coefficient of variation):
# {xrst_code py}
model_meas_cv = 0.1
# {xrst_code}
#
# minimum_prevalence_std
# **********************
# In this model, initial prevalence is zero. Hence a coefficient of variation
# model for the measurement noise does not work at age zero.
# We use a minimum prevalence standard deviation to avoid this problem:
# {xrst_code py}
minimum_prevalence_std = 1e-4
# {xrst_code}
#
# ode_step_size
# *************
# This is the option table :ref:`option_table@Age Average Grid@ode_step_size`
# value.
# {xrst_code py}
ode_step_size = 5.0
# {xrst_code}
#
# hold_out_max_fit
# ****************
# This is the hold out command :ref:`hold_out_command@max_fit` value.
# It is used to hold out prevalence and mtspecific data.
# {xrst_code py}
hold_out_max_fit = 250
# {xrst_code}
#
# fixed_effect_rel_error_bnd
# **************************
# This is a bound on the relative error in the fixed effects estimates.
# If the relative error is greater that this for a fixed effect,
# a message that identifies the fixed effect and the relative error is
# printed and this program will exit with a non-zero error flag.
# {xrst_code py}
fixed_effect_rel_error_bnd = 0.1
# {xrst_code}
#
# Priors
# ******
#
# parent_rate_value_prior
# =======================
# This prior is used for the value of iota and chi, for the parent node,
# at each of the grid points in the corresponding smoothings.
# Note that omega is constrained to the value used for it during simulation.
# Also note that eta does not affect the density but it does affect the
# :ref:`prior_table@eta@Scaling Fixed Effects` .
# {xrst_code py}
parent_rate_value_prior = {
   'density': 'gaussian',
   'mean':    0.01,
   'std':     1.0,
   'lower':   1e-6,
   'upper':   1.0,
   'eta':     1e-6,
}
# {xrst_code}
#
# child_rate_value_prior
# ======================
# This prior is used for the log of iota and chi for the child nodes.
# These are random effects and are constant w.r.t. age, time; i.e.,
# there is only one grid point (one variable)
# for each node and each rate (iota and chi) .
# {xrst_code py}
child_rate_value_prior = {
   'density': 'gaussian',
   'mean':    0.0,
   'std':     0.1,
}
# {xrst_code}
#
# difference_prior
# ================
# This is the prior used for the forward difference (w.r.t. age and time)
# of the value of iota and chi between grid points.
# {xrst_code py}
difference_prior = {
   'density': 'log_gaussian',
   'mean':    0.0,
   'std':     0.4,
   'eta':     1e-6,
}
# {xrst_code}
#
# mulcov_prior
# ============
# This is the value prior used for each of the covariate multipliers.
# These are fixed effects and are constant w.r.t age, time.
# The mean does not affect the density and
# is used as a starting point for the first optimization.
# (The first optimization is just w.r.t the fixed effects and is used to get
# a starting point for optimization w.r.t. both the fixed and random effects.)
# {xrst_code py}
mulcov_prior = {
   'density': 'uniform',
   'mean':    0.0,
}
# {xrst_code}
#
# Source Code
# ***********
# {xrst_literal
#     # BEGIN_SOURCE_CODE
#     # END_SOURCE_CODE
# }
#
# {xrst_end user_diabetes.py}
# ---------------------------------------------------------------------------
# BEGIN_SOURCE_CODE
#
# random_seed
if random_seed == 0 :
   random_seed = int( time.time() )
#
# random_effect_sim
# In this example, the random effects are the same iota and chi.
# Note that index zero corresponds to the parent node and has no random effect.
random_effect_sim = [ 0.0 ]
for i in range(number_nodes-1) :
   random_effect = sim_std_random_effect * random.gauss(mu = 0.0, sigma = 1.0)
   random_effect_sim.append( random_effect )
#
# rate_true
# Assume all covariates are relative to their referece values
def rate_true(rate_name, age, time, node_index, sex, bmi) :
   rate   = sim_no_effect_rate(rate_name, age, time)
   effect = random_effect_sim[node_index]
   if rate_name == 'iota' :
      effect += sim_mulcov['bmi'] * bmi
   elif rate_name == 'chi' :
      effect += sim_mulcov['sex'] * sex
   elif rate_name == 'omega' :
      effect += 0.0
   else :
      assert False
   rate = rate * math.exp( effect )
   return rate
#
# eample_db
def example_db(file_name) :
   #
   # sex_dict
   sex_dict = { 'female' : -0.5 , 'male' : +0.5 }
   #
   # integrand_table
   integrand_table = integrand_table = [
      { 'name':'mtspecific' },
      { 'name':'prevalence' }
   ]
   #
   # node_table
   node_table = list()
   for i in range( number_nodes ) :
      name = f'n{i}'
      parent = '' if i == 0 else 'n0'
      node_table.append( { 'name' : name, 'parent' : parent } )
   #
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   #
   # bmi_lower, bmi_upper, bmi_average
   bmi_lower   = 17.0
   bmi_upper   = 35.0
   bmi_average = (bmi_lower + bmi_upper) / 2.0
   #
   # covariate table:
   covariate_table = [
      {'name':'sex',     'reference':0.0,         'max_difference':0.6  } ,
      {'name':'bmi',     'reference':bmi_average, 'max_difference':None } ,
      {'name':'ms_2000', 'reference':0.0,         'max_difference':None } ,
   ]
   #
   # integrand_list
   integrand_list = list()
   for row in integrand_table :
      integrand_list.append( row['name'] )
   #
   # random_effect
   node_list = list()
   for row in node_table :
      node_list.append( row['name'] )
   #
   # data_table
   data_table   = list()
   #
   # integrand_name, age, time, node_name, sex_name
   # Skip first age because prevalence and mtspecific are zero for that age.
   for integrand_name in integrand_list :
      for age in age_list[1:] :
         for time in time_list :
            for node_index in range(number_nodes) :
               for sex_name in sex_dict :
                  #
                  # node_name
                  node_name = node_list[node_index]
                  #
                  # sex
                  sex = sex_dict[sex_name]
                  #
                  # ms_2000
                  ms_2000 = 1 if time == 2000 else 0
                  #
                  # bmi
                  bmi = random.uniform(bmi_lower, bmi_upper)
                  #
                  # rate_fun
                  rate_fun  = dict()
                  relative_bmi = bmi - bmi_average
                  rate_fun['iota'] = lambda age, time : rate_true(
                     'iota', age, time, node_index, sex, relative_bmi
                  )
                  rate_fun['chi'] = lambda age, time : rate_true(
                     'chi', age, time, node_index, sex, relative_bmi
                  )
                  rate_fun['omega'] = lambda age, time : rate_true(
                     'omega', age, time, node_index, sex, relative_bmi
                  )
                  #
                  # meas_mean
                  if age == 0.0 :
                     meas_mean = 0
                  else :
                     #
                     # grid
                     grid         = dict()
                     grid['age']  = [ age ]
                     grid['time'] = [ time ]
                     #
                     # abs_tol
                     abs_tol = 1e-7
                     #
                     # meas_mean
                     meas_mean = dismod_at.average_integrand(
                        rate_fun, integrand_name, grid, abs_tol
                     )
                     if integrand_name == 'prevalence' :
                        effect    = ms_2000 * sim_mulcov['ms_2000']
                        meas_mean = meas_mean * math.exp(effect)
                  #
                  # meas_std
                  meas_std = meas_mean * model_meas_cv
                  min_std  = minimum_prevalence_std
                  if integrand_name == 'mtspecific' :
                     age_mid  = (age_list[0] + age_list[-1]) / 2.0
                     time_mid = (time_list[0] + time_list[-1]) / 2.0
                     chi_mid  = sim_no_effect_rate('chi', age_mid, time_mid)
                     min_std  = minimum_prevalence_std * chi_mid
                  meas_std = max(meas_std, min_std)
                  #
                  # data_table
                  row = {
                     # avgint columns
                     'integrand':   integrand_name,
                     'node':        node_name,
                     'subgroup':    'world',
                     'density':     'gaussian',
                     'weight':      '',
                     'age_lower':    age,
                     'age_upper':    age,
                     'time_lower':   time,
                     'time_upper':   time,
                     'sex':          sex,
                     'ms_2000':      ms_2000,
                     'bmi':          bmi,
                     #
                     # other columns
                     'hold_out':      0,
                     'density':       'gaussian',
                     'meas_value':    meas_mean,
                     'meas_std':      meas_std,
                  }
                  data_table.append(row)
   #
   # prior_table
   parent_rate_value_prior['name'] = 'parent_rate_value_prior'
   child_rate_value_prior['name']  = 'child_rate_value_prior'
   difference_prior['name']        = 'difference_prior'
   mulcov_prior['name']            = 'mulcov_prior'
   prior_table = list()
   prior_table.append( parent_rate_value_prior )
   prior_table.append( child_rate_value_prior )
   prior_table.append( difference_prior )
   prior_table.append( mulcov_prior )
   #
   # prior_fun
   omega_0_0 = sim_no_effect_rate('omega', 0.0, 0.0)
   prior_fun = dict()
   prior_fun['parent'] = lambda age, time : \
      ('parent_rate_value_prior', 'difference_prior', 'difference_prior')
   prior_fun['child'] = lambda age, time : \
      ('child_rate_value_prior', None, None)
   prior_fun['mulcov'] = lambda age, time : \
      ('mulcov_prior', None, None)
   prior_fun['omega'] = lambda age, time : \
      (omega_0_0, None, None)
   #
   # age_id_list, time_id_list
   age_id_list  = list( range( len(age_list) ) )
   time_id_list = list( range( len(time_list) ) )
   #
   # smooth_table
   smooth_table = [ {
         # smooth_omega
         'name':     'smooth_omega',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      prior_fun['omega']
      },{
         # smooth_parent_iota
         'name':     'smooth_parent_iota',
         'age_id':   age_id_list[: -1],
         'time_id':  time_id_list,
         'fun':      prior_fun['parent']
      },{
         # smooth_parent_chi
         'name':     'smooth_parent_chi',
         'age_id':   age_id_list[1 :],
         'time_id':  time_id_list,
         'fun':      prior_fun['parent']
      },{
         # smooth_child
         'name':     'smooth_child',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      prior_fun['child']
      },{
         # smooth_mulcov
         'name':     'smooth_mulcov',
         'age_id':   [0],
         'time_id':  [0],
         'fun':      prior_fun['mulcov']
   } ]
   #
   # rate_table
   rate_table = [ {
         'name':           'iota',
         'parent_smooth':  'smooth_parent_iota',
         'child_smooth':   'smooth_child',
         'child_nslist':   None,
      },{
         'name':           'chi',
         'parent_smooth':  'smooth_parent_chi',
         'child_smooth':   'smooth_child',
         'child_nslist':   None,
      },{
         'name':           'omega',
         'parent_smooth':  'smooth_omega',
         'child_smooth':   None,
         'child_nslist':   None,
   } ]
   #
   # mulcov_table
   mulcov_table = [ {
      'covariate':     'sex',
      'type':          'rate_value',
      'effected':      'chi',
      'group':         'world',
      'smooth':        'smooth_mulcov',
   },{
      'covariate':     'bmi',
      'type':          'rate_value',
      'effected':      'iota',
      'group':         'world',
      'smooth':        'smooth_mulcov',
   },{
      'covariate':     'ms_2000',
      'type':          'meas_value',
      'effected':      'prevalence',
      'group':         'world',
      'smooth':        'smooth_mulcov',
   } ]
   #
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero'    },
      { 'name':'parent_node_name',       'value':'n0'                   },
      { 'name':'ode_step_size',          'value':str(ode_step_size)     },
      { 'name':'random_seed',            'value':str(random_seed)       },
      { 'name':'bound_random',           'value':'1.0'                  },

      { 'name':'quasi_fixed',            'value':'false'                },
      { 'name':'max_num_iter_fixed',     'value':'50'                   },
      { 'name':'print_level_fixed',      'value':'5'                    },
      { 'name':'tolerance_fixed',        'value':'1e-4'                 },

      { 'name':'max_num_iter_random',    'value':'50'                   },
      { 'name':'print_level_random',     'value':'0'                    },
      { 'name':'tolerance_random',       'value':'1e-8'                 },
   ]
   #
   # create_database
   dismod_at.create_database(
      file_name       = file_name,
      age_list        = age_list,
      time_list       = time_list,
      integrand_table = integrand_table,
      node_table      = node_table,
      subgroup_table  = subgroup_table,
      weight_table    = list(),
      covariate_table = covariate_table,
      avgint_table    = list(),
      data_table      = data_table,
      prior_table     = prior_table,
      smooth_table    = smooth_table,
      nslist_dict     = dict(),
      rate_table      = rate_table,
      mulcov_table    = mulcov_table,
      option_table    = option_table,
      rate_eff_cov_table  = list(),
   )
# -----------------------------------------------------------------------------
#
# file_nanme
file_name = 'example.db'
example_db(file_name)
#
# program
program = '../../devel/dismod_at'
#
# init
dismod_at.system_command_prc([ program, file_name, 'init' ])
#
# hold_out
for integrand_name in [ 'prevalence', 'mtspecific' ] :
   str_max_fit = str( hold_out_max_fit )
   dismod_at.system_command_prc(
      [ program, file_name, 'hold_out', integrand_name, str_max_fit ]
   )
#
if True :
   #
   # fit fixed
   dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
   #
   # start_var table
   dismod_at.system_command_prc(
      [ program, file_name, 'set', 'start_var', 'fit_var' ]
   )
#
# fit both
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
#
# rate_table, var_table, fit_var_table, covariate_table
connection = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
rate_table       = dismod_at.get_table_dict(connection, 'rate')
var_table        = dismod_at.get_table_dict(connection, 'var')
fit_var_table    = dismod_at.get_table_dict(connection, 'fit_var')
covariate_table  = dismod_at.get_table_dict(connection, 'covariate')
connection.close()
#
# max_rel_error, truth_var_list
max_rel_error = 0.0
truth_var_list  = list()
#
# var_id, row
for (var_id, row) in enumerate(var_table) :
   #
   # fixed_effect, sim_var_value, rel_error
   fit_var_value = fit_var_table[var_id]['fit_var_value']
   var_type      = row['var_type']
   if var_type == 'rate' :
      rate_name     = rate_table[ row['rate_id'] ]['rate_name']
      node_id       = row['node_id']
      age           = age_list[ row['age_id'] ]
      time          = time_list[ row['time_id'] ]
      if node_id == 0 :
         fixed_effect  = True
         sim_var_value = sim_no_effect_rate(rate_name, age, time)
         rel_error     = 1.0 - fit_var_value / sim_var_value
         if abs(rel_error) > fixed_effect_rel_error_bnd :
            msg  = f'rate = {rate_name}, age = {age}, time = {time}'
            msg += f', rel_error = {rel_error}'
            print(msg)
      else :
         fixed_effect  = False
         sim_var_value = random_effect_sim[node_id]
         rel_error     = 1.0 - fit_var_value / sim_var_value
   elif var_type in [ 'mulcov_rate_value' , 'mulcov_meas_value' ] :
      fixed_effect   = True
      covariate_name = covariate_table[ row['covariate_id'] ]['covariate_name']
      sim_var_value  = sim_mulcov[covariate_name]
      rel_error     = 1.0 - fit_var_value / sim_var_value
      if abs(rel_error) > fixed_effect_rel_error_bnd :
         msg = f'covariate = {covariate_name}, rel_error = {rel_error}'
         print(msg)
   else :
      assert False
   #
   # max_rel_error
   if fixed_effect :
      max_rel_error = max(max_rel_error, abs(rel_error) )
   #
   # truth_var_list
   truth_var_list.append( [ sim_var_value , rel_error] )
#
# truth_var_table
connection = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
tbl_name = 'truth_var'
col_name = [ 'truth_var_value' , 'rel_error' ]
col_type = [ 'real', 'real' ]
row_list = truth_var_list
dismod_at.create_table(
   connection, tbl_name, col_name, col_type, row_list
)
connection.close()
#
# max_rel_error
print( f'maximum fixed effect relative error = {max_rel_error}' )
assert max_rel_error <= fixed_effect_rel_error_bnd
print('diabetes.py: OK')
# END_SOURCE_CODE
