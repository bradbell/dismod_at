# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_shock_cov.py}
# {xrst_spell
#     exp
# }
# {xrst_comment_ch #}
#
# Example Fitting a Covariate with a Shock
# ########################################
#
# Purpose
# *******
# This example demonstrates fitting a covariate multiplier
# where the covariate has a shock. To be more specific, a spike at
# a certain age, time, node, and sex.
#
# Integrand
# *********
# There is only one integrand in this example,
# :ref:`avg_integrand@Integrand, I_i(a,t)@prevalence` .
#
# Node Tables
# ***********
# The node table for this example is
# ::
#
#                   world
#                  /     \
#     north_america       south_america
#
# Subgroup Table
# **************
# For this example there is only one subgroup (the world).
#
# Covariates
# **********
# There are two covariates in this example, *sex* and *shock* .
# Sex has the following values
# {xrst_code py}
sex_name2value = { 'female' : -0.5, 'both' : 0.0, 'male' : +0.5 }
# {xrst_code}
# Shock is defined by the following function
# {xrst_code py}
def shock_fun(age, time, node_name, sex) :
   shock = 0.0
   if (node_name, sex) == ('north_america', 'male') :
      if 0 <= age and age <= 40 and 1920 <= time and time <= 1960 :
         age_factor  = 1.0 - abs( age - 20.0 ) / 20.0
         time_factor = 1.0 - abs(time - 1940.0 ) / 20.0
         shock       = age_factor * time_factor
   return shock
# {xrst_code}
#
# Covariate Multipliers
# *********************
# There is one covariate multiplier in this example.
# It multiples *shock* and effects the rate iota as follows:
# {xrst_code py}
mulcov_true      = 1.0
iota_no_effect   = 0.01
def iota_true(age, time, node_name, sex) :
   effect = mulcov_true * shock_fun(age, time, node_name, sex)
   return iota_no_effect * math.exp(effect)
# {xrst_code}
#
# Simulated Data
# **************
# The data is simulated using the true value for the variables,
# and the covariate effects mentioned above. No noise is added to the data,
# but it is modeled as having a ten percent coefficient of variation.
#
# Rate Variables
# **************
# There is one non-zero rate for this example iota
# and the no effect model for iota is constant and equal to
# ``iota_no_effect`` .
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_shock_cov.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
#
import sys
import os
import copy
import math
test_program = 'example/user/shock_cov.py'
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
   def fun_no_effect_iota(a, t) :
      return (iota_no_effect,  None, None)
   def fun_mulcov(a, t) :
      return ('mulcov_value_prior', None, None)
   def fun_weight_north_america_male(a, t) :
      return shock_fun(a, t, 'north_america', 'male')
   def fun_weight_other(a, t) :
      return shock_fun(a, t, '', '')
   # ----------------------------------------------------------------------
   # age table
   age_list    = list( range(0, 101, 20) )
   #
   # time table
   time_list   = list( range(1920, 2021, 20) )
   #
   #
   # integrand table
   integrand_table = [ { 'name':'prevalence' } ]
   #
   # node table: world -> (north_america, south_america)
   node_table = [
      { 'name':'world',         'parent':'' },
      { 'name':'north_america', 'parent':'world' },
      { 'name':'south_america', 'parent':'world' },
   ]
   #
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   #
   # weight table:
   weight_table = [ {
      'name'    : 'shock_north_america_male',
      'age_id'  : range( len( age_list ) ),
      'time_id' : range( len( time_list ) ),
      'fun'     : fun_weight_north_america_male,
      },{
      'name'    : 'shock_other',
      'age_id'  : range( len( age_list ) ),
      'time_id' : range( len( time_list ) ),
      'fun'     : fun_weight_other,
   } ]
   #
   # covariate table:
   covariate_table = [
      {'name':'shock', 'reference':0.0},
      {'name':'sex',   'reference':0.0},
   ]
   #
   # mulcov table
   # income has been scaled the same as sex so man use same smoothing
   mulcov_table = [
      {  # income effects north american incidence
         'covariate': 'shock',
         'type':      'rate_value',
         'effected':  'iota',
         'group':     'world',
         'smooth':    'smooth_mulcov'
      }
   ]
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   for node_name in [ 'north_america' ] :
      for sex in [ 'male' ] :
         for age in age_list :
            for time in time_list :
               fun  = lambda age, time : \
                  iota_true(age, time, node_name, sex)
               meas_value = dismod_at.average_integrand(
                  rate           = { 'iota' : fun },
                  integrand_name = 'prevalence',
                  grid           =  { 'age': [age], 'time' : [time] },
                  abs_tol        = 1e-6,
               )
               row = {
                  'node':              node_name,
                  'subgroup':          'world',
                  'density':           'gaussian',
                  'weight':            '',
                  'hold_out':           False,
                  'time_lower':         time,
                  'time_upper':         time,
                  'age_lower':          age,
                  'age_upper':          age,
                  'shock':              shock_fun(age, time, node_name, sex),
                  'sex':                sex_name2value[sex],
                  'integrand':          'prevalence',
                  'meas_value':         meas_value,
                  'meas_std':           1e-3,
                  'density':            'gaussian',
               }
               data_table.append( row )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      {  # mulcov_value_prior
         'name':     'mulcov_value_prior',
         'density':  'uniform',
         'mean':     0.0,
         'lower':    -2.0,
         'upper':    +2.0,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      { # smooth_iota
         'name':                     'smooth_iota',
         'age_id':                   range( len( age_list ) ) ,
         'time_id':                  range( len( age_list ) ) ,
         'fun':                      fun_no_effect_iota,
      },{ # smooth_mulcov
         'name':                     'smooth_mulcov',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_mulcov
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [ {
         'name':          'iota',
         'parent_smooth': 'smooth_iota',
         'child_smooth':  None,
   } ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'splitting_covariate',    'value':'sex'          },
      #
      { 'name':'parent_node_name',       'value':'world'        },
      { 'name':'ode_step_size',          'value':'0.5'          },
      { 'name':'random_seed',            'value':'0'            },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },
      #
      { 'name':'quasi_fixed',            'value':'true'         },
      { 'name':'derivative_test_fixed',  'value':'first-order'  },
      { 'name':'max_num_iter_fixed',     'value':'100'          },
      { 'name':'print_level_fixed',      'value':'0'            },
      { 'name':'tolerance_fixed',        'value':'1e-8'         },
      #
      { 'name':'derivative_test_random', 'value':'second-order' },
      { 'name':'max_num_iter_random',    'value':'100'          },
      { 'name':'print_level_random',     'value':'0'            },
      { 'name':'tolerance_random',       'value':'1e-10'        },
   ]
   # ----------------------------------------------------------------------
   rate_eff_cov_table = list()
   for node_id in range( len( node_table ) ) :
      for sex_name in sex_name2value :
         node_name      = node_table[node_id]['name']
         split_value    = sex_name2value[sex_name]
         if node_name == 'north_america' and sex_name == 'male' :
            weight_name = 'shock_north_america_male'
         else :
            weight_name = 'shock_other'
         row = {
            'node_name'      : node_name,
            'covariate_name' : 'shock',
            'split_value'    : split_value,
            'weight_name'    : weight_name,
         }
         rate_eff_cov_table.append( row )
   # ----------------------------------------------------------------------
   # create database
   dismod_at.create_database(
      file_name       = file_name        ,
      age_list        = age_list         ,
      time_list       = time_list        ,
      integrand_table = integrand_table  ,
      node_table      = node_table       ,
      subgroup_table  = subgroup_table   ,
      weight_table    = weight_table     ,
      covariate_table = covariate_table  ,
      data_table      = data_table       ,
      prior_table     = prior_table      ,
      smooth_table    = smooth_table     ,
      rate_table      = rate_table       ,
      mulcov_table    = mulcov_table     ,
      option_table    = option_table     ,
      rate_eff_cov_table  = rate_eff_cov_table   ,
   )
# ===========================================================================
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
dismod_at.db2csv_command( file_name )
#
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
for (var_id, row_var) in enumerate(var_table) :
   row_fit     = fit_var_table[var_id]
   var_type    = row_var['var_type']
   fit_value   = row_fit['fit_var_value']
   if var_type == 'rate' :
      assert fit_value == iota_no_effect
   else :
      assert var_type == 'mulcov_rate_value'
      rel_error = fit_value / mulcov_true - 1.0
      if abs(rel_error) > 1e-3 :
         print('rel_error = ' , rel_error)
         assert False
#
# -----------------------------------------------------------------------------
print('shock_cov.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
