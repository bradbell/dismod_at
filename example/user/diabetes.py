# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_diabetes.py}
# {xrst_spell
#     bmi
#     bool
#     integrals
#     ms
#     mtspecific
#     rel
#     ts
# }
# {xrst_comment_ch #}
#
# An Example / Speed Test Fitting Simulated Diabetes Data
# #######################################################
#
# Running This example
# ********************
# see :ref:`user_example@Run One Example` .
# The time required to run the program will be printed at then end.
#
# omega
# *****
# The model rate :ref:`rate_table@rate_name@omega`
# is constrained to have the value used during simulation of the data.
#
# rho
# ***
# The model rate :ref:`rate_table@rate_name@rho`
# is constrained to be zero.
#
# Covariates
# **********
#
# Covariate Table
# ===============
# The covariate table has the following values:
#
# .. csv-table::
#     :widths: auto
#
#     covariate_name,reference,max_difference
#     sex,0,0.6
#     bmi,28,null
#     ms_2000,0,null
#
# Data Table
# ==========
# The :ref:`covariate<data_table@Covariates>` columns in the
# data table have the following values:
# *sex* is ``0.5`` for male and ``-0.5`` for female,
# *bmi* is body mass index 20 <= *bmi*  <= 36 ,
# *ms_2000* is ``1.0`` if this is year 2000 market scan data
# and ``0.0`` otherwise.
#
# Multipliers
# ===========
# There are three covariate multipliers, one for each covariate.
# (In general, a covariate can have more than one multiplier.)
# In addition, each covariate multiplier has one grid point; i.e.,
# the multiplier is constant in age and time.
# The value for each multiplier has a uniform distribution
# with the lower and upper limits below:
#
# .. csv-table::
#     :widths: auto
#
#     covariate,affected,lower,upper
#     sex,iota,-2.0,+2.0
#     bmi,iota,-0.1,+0.1
#     ms_2000,prevalence,-1.0,+1.0
#
# Note that the for sex and bmi these are
# :ref:`mulcov_table@mulcov_type@rate_value` multipliers and
# for ms_2000 it is a
# :ref:`mulcov_table@mulcov_type@meas_value` multiplier.
#
# Truth Var Table
# ***************
# The values in the :ref:`truth_var_table-name` are generated using bilinear
# interpolation of the log of values specified points.
#
# Parent Rates
# ============
# We use the notation
# as for age start,
# ae for age end,
# ts for time start, and
# te for time end.
# The following table gives the values used for the parent rates
# (note that the parent rate for pini cannot change with age):
#
# .. list-table::
#     :widths: auto
#
#     * - rate
#       - (as,ts)
#       - (as,te)
#       - (ae,ts)
#       - (ae,te)
#     * - pini
#       - .01
#       - .01
#       - .01
#       - .01
#     * - iota
#       - .001
#       - .002
#       - .01
#       - .02
#     * - omega
#       - .003
#       - .002
#       - .3
#       - .2
#     * - chi
#       - .004
#       - .002
#       - .1
#       - .05
#
# Child Rate Effects
# ==================
# The child rate effects are in log space
# (see :ref:`u_ik<avg_integrand@Rate Functions@Child Rate Effect, u_ik>` ),
# constant in age and time,
# positive for even index children, negative for odd indices, and have the
# following values:
#
# .. csv-table::
#     :widths: auto
#
#     rate,even index,odd index
#     pini,.1,-.1
#     iota,.15,-.15
#     chi,.25,-.25
#
# There is an exception for omega, which is constrained. It is defined
# on the parent age grid and has the following values:
#
# .. list-table::
#     :widths: auto
#
#     * - index
#       - (as,ts)
#       - (as,te)
#       - (ae,ts)
#       - (ae,te)
#     * - even
#       - .1
#       - .02
#       - .02
#       - .03
#     * - odd
#       - -.1
#       - -.02
#       - -.02
#       - -.03
#
# Predict Table
# *************
# The :ref:`predict_command-name` is used to compute the
# :ref:`predict_table@avg_integrand` corresponding to the
# true values for the variables.
# This is then used to create a version of the :ref:`data_table-name`
# with no noise, and with a standard deviation that is modeled using
# a coefficient of variation.
#
# Problem Parameters
# ******************
# The problem parameters below can (and should) be changed to experiment with
# how they affect the results.
#
# mulcov_dict
# ===========
# This is a dictionary that maps each covariate name
# to the true value for the corresponding covariate multiplier.
# These values must satisfy the lower and upper
# :ref:`multiplier<user_diabetes.py@Covariates@Multipliers>` limits above:
# {xrst_spell_off}
# {xrst_code py}
mulcov_dict = { 'sex':0.5, 'bmi':0.02, 'ms_2000':0.25 }
# {xrst_code}
# {xrst_spell_on}
#
# node_list
# =========
# This is a ``list`` with ``str`` elements.
# The first element of this list is the parent node,
# the others are the child nodes. There must be an even number of children;
# i.e., an odd number of elements in this list.
# The case with no child nodes; i.e., one element in the list, is OK:
# {xrst_spell_off}
# {xrst_code py}
node_list = [ 'US', 'Alabama', 'California' ]
# {xrst_code}
# {xrst_spell_on}
#
# integrand_list
# ==============
# This is a ``list`` with ``str`` elements that are
# :ref:`integrand names<integrand_table@integrand_name>`
# that will have measurements in the :ref:`data_table-name`
# and :ref:`data_sim_table-name` .
# As mentioned above, the rates
# :ref:`user_diabetes.py@omega`  and
# :ref:`user_diabetes.py@rho`
# are know during the estimation (fitting) process.
# The integrands must inform the estimation of
# the model rates for
# :ref:`rate_table@rate_name@pini` ,
# :ref:`rate_table@rate_name@iota` , and
# :ref:`rate_table@rate_name@chi` .
# Note that measuring prevalence at age zero should determine pini,
# prevalence at other ages corresponds to integrals of iota, and
# given prevalence, mtspecific should determine chi.
# {xrst_spell_off}
# {xrst_code py}
integrand_table = [
   { 'name':'mtspecific' },
   { 'name':'prevalence' }
]
# {xrst_code}
# {xrst_spell_on}
#
# parent_age_grid
# ===============
# This specifies the age grid used for all the parent rate
# :ref:`smoothings<rate_table@parent_smooth_id>` .
# It is also the age grid used for constraining the child omega rates using
# :ref:`rate_table@child_nslist_id` .
# In addition, it is the set of ages in the :ref:`age_table-name` .
# It is a ``dict`` with ``float`` values
# (except for *number* which is a positive ``int`` ) containing
# the start age, end age, number of age grid points, and
# standard deviation of the log-Gaussian used to smooth the
# :ref:`model_variables@Fixed Effects, theta@Parent Rates`
# age differences.
# (This does not include :ref:`rate_table@rate_name@pini`
# because it only has one age point.)
# The interval between age grid points is the end age, minus the start age,
# divided by the number of grid points minus one.
# {xrst_spell_off}
# {xrst_code py}
parent_age_grid  = { 'start':0.0, 'end':100.0, 'number':6, 'std':0.4 }
# {xrst_code}
# {xrst_spell_on}
#
# child_age_grid
# ==============
# The is the age grid used for all the child rate effect
# :ref:`smoothings<rate_table@child_smooth_id>` except for
# omega (see *parent_age_grid* above).
# It is a ``dict`` with the following values:
# The value of *index* is a list of indices (``int`` ) in the parent
# age grid where there are
# :ref:`random effects<model_variables@Random Effects, u>` .
# Each of these indices must be less than *number* in the age grid.
# The value *std* (a ``float`` ) is the standard deviation in the
# Gaussian used to smooth the child rate effect values.
# (This does not include :ref:`rate_table@rate_name@pini`
# because it only has one age point.)
# {xrst_spell_off}
# {xrst_code py}
child_age_grid  = { 'index':[0], 'std':0.2 }
# {xrst_code}
# {xrst_spell_on}
#
# parent_time_grid
# ================
# This specifies the time grid used for all the parent rate
# :ref:`smoothings<rate_table@parent_smooth_id>` .
# It is also the time grid used for constraining the child omega rates using
# :ref:`rate_table@child_nslist_id` .
# In addition, it is the set of times in the :ref:`time_table-name` .
# :ref:`rate_table@child_nslist_id` .
# It is a ``dict`` with ``float`` values
# (except for *number* which is a positive ``int`` ) containing
# the start time, end time, number of time grid points, and
# standard deviation of the log-Gaussian used to smooth the
# :ref:`model_variables@Fixed Effects, theta@Parent Rates`
# time differences.
# (This includes :ref:`rate_table@rate_name@pini` ).
# This is also the set of times in the :ref:`time_table-name` .
# The interval between time grid points is the end time, minus the start time,
# divided by the number of grid points minus one.
# {xrst_spell_off}
# {xrst_code py}
parent_time_grid = { 'start':1990.0, 'end': 2020, 'number':2, 'std':0.6  }
# {xrst_code}
# {xrst_spell_on}
#
# child_time_grid
# ===============
# The is the time grid used for all the child rate effect
# :ref:`smoothings<rate_table@child_smooth_id>` except for
# omega (see *parent_time_grid* above).
# It is a ``dict`` with the following values:
# The value of *index* is a list of indices (``int`` ) in the parent
# time grid where there are
# :ref:`random effects<model_variables@Random Effects, u>` .
# Each of these indices must be less than *number* in the time grid.
# The value *std* (a ``float`` ) is the standard deviation in the
# Gaussian used to smooth the child rate effect values.
# (This includes :ref:`rate_table@rate_name@pini` ).
# {xrst_spell_off}
# {xrst_code py}
child_time_grid  = { 'index':[0], 'std':0.2 }
# {xrst_code}
# {xrst_spell_on}
#
# ode_step_size
# =============
# This is a ``str`` that specifies the
# :ref:`option_table@Age Average Grid@ode_step_size` .
# It is suggest that this value be less than the intervals in the
# age and time grids:
# {xrst_spell_off}
# {xrst_code py}
ode_step_size = '10.0'
# {xrst_code}
# {xrst_spell_on}
#
# meas_cv
# =======
# This is a ``float`` that specifies the measurement standard deviations
# :ref:`data_table@meas_std` by
#
#     *meas_std* = *meas_cv* * *meas_value*
#
# For this example, the data table column
# :ref:`data_table@meas_value` does not have any noise; i.e.,
# the values in that column are the corresponding
# :ref:`average integrand<avg_integrand@Average Integrand, A_i>` .
# The *meas_std* determines the noise level used by the
# :ref:`simulate_command-name` :
# {xrst_spell_off}
# {xrst_code py}
meas_cv = 0.1
# {xrst_code}
# {xrst_spell_on}
#
# meas_repeat
# ===========
# This is a positive ``int`` that specifies
# the number of times each noiseless measurement is repeated.
# Note that the simulated measurements will be different, because
# the noise for each measurement will be different.
# There are *meas_repeat*
# data points for each integrand in the integrand list,
# each age in the age grid,
# each time in the time grid,
# each node in the node list.
# In addition if an age is not the first age and time is not the first time,
# there is a data point in the middle of the age-time interval that ends
# at that (age, time):
# {xrst_spell_off}
# {xrst_code py}
meas_repeat = 1
# {xrst_code}
# {xrst_spell_on}
#
# fit_with_noise_in_data
# ======================
# This is a ``bool`` that specifies if measurement noise is included
# when fitting the data; i.e., if the column
# :ref:`data_sim_table@data_sim_value` is used to
# fit the :ref:`model_variables-name` .
# Otherwise, the measurements without noise
# are used to fit the model variables; i.e., the column
# :ref:`data_table@meas_value` :
# {xrst_spell_off}
# {xrst_code py}
fit_with_noise_in_data = False
# {xrst_code}
# {xrst_spell_on}
#
# random_seed
# ===========
# This ``str`` must be a non-negative integer and is the
# :ref:`option_table@random_seed` option value.
# This is used to seed the random number generator used to simulate the
# noise in the measurement values.
# The affects the results of the fit when *fit_with_noise_in_data*
# is true
# {xrst_spell_off}
# {xrst_code py}
random_seed = '0'
# {xrst_code}
# {xrst_spell_on}
#
# quasi_fixed
# ===========
# This ``str`` that is either ``true`` or ``false`` and is the
# :ref:`option_table@Optimize Fixed Only@quasi_fixed` option value.
# If it is true, a quasi-Newton method is used.
# This only requires function values and
# first derivatives for the objective and constraints.
# If it is false, a Newton method is used.
# This requires second derivatives in which case initialization
# and function evaluations take longer:
# {xrst_spell_off}
# {xrst_code py}
quasi_fixed = 'false'
# {xrst_code}
# {xrst_spell_on}
#
# tolerance_fixed
# ===============
# This ``str`` contains the
# :ref:`tolerance_fixed<option_table@Optimize Fixed and Random@tolerance>` option value.
# {xrst_spell_off}
# {xrst_code py}
tolerance_fixed = '1e-4'
# {xrst_code}
# {xrst_spell_on}
#
# derivative_test_fixed
# =====================
# This ``str`` is the
# :ref:`option_table@Optimize Fixed and Random@derivative_test`
# option for the fixed effects.
# The choice ``trace-adaptive`` can be used to see if the partial
# derivatives of the objective and constraints after the
# :ref:`scaling<prior_table@eta@Scaling Fixed Effects>` of the fixed effects.
# The choice ``none`` is normal for a working example.
# {xrst_spell_off}
# {xrst_code py}
derivative_test_fixed = 'none'
# {xrst_code}
# {xrst_spell_on}
#
# truth2start
# ===========
# This is a ``float`` that is used to map
#
#     *start_var_value* = *truth2start* * *truth_var_value*
#
# for each model variable that is not constrained to a specific value.
# The notation
# :ref:`truth_var_table@truth_var_value` is the true value
# used to simulate the data and
# :ref:`start_var_table@start_var_value` is the initial
# value of the variable during the fit.
# An error will result if the starting value for a variable is not within
# the upper and lower limits for a variable.
# The starting values are also used for the :ref:`scale_var_table-name` .
# *truth2start* :
# {xrst_spell_off}
# {xrst_code py}
truth2start = 0.3
# {xrst_code}
# {xrst_spell_on}
#
# accept_rel_err
# ==============
# This is a ``float`` that specifies the absolute relative error
# to be accepted as passing the test.
# If the test passes, for each model variable
#
#     *accept_rel_err* >= *fit_var_value* / *truth_var_value* ``- 1.0``
#
# where
# :ref:`truth_var_table@truth_var_value` is the true value
# used to simulate the data and
# :ref:`fit_var_table@fit_var_value` is result of the fit.
# A python assertion is generated if the condition above is not satisfied.
# {xrst_spell_off}
# {xrst_code py}
accept_rel_err = 0.25
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
# {xrst_end user_diabetes.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
import sys
import os
import copy
import math
import time
test_program = 'example/user/diabetes.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
   usage  = 'python3 ' + test_program + '\n'
   usage += 'where python3 is the python 3 program on your system\n'
   usage += 'and working directory is the dismod_at distribution directory\n'
   sys.exit(usage)
start_second = time.time()
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
def log_bilinear(grid_value, a, t) :
   # denominator
   da  = parent_age_grid['end'] - parent_age_grid['start']
   dt  = parent_time_grid['end'] - parent_time_grid['start']
   den = da * dt;
   num = 0.0
   #
   # value at start age and start time
   da   = parent_age_grid['end'] - a
   dt   = parent_time_grid['end'] - t
   num += math.log( grid_value['start_age, start_time'] ) * da * dt
   #
   # value at start age and end time
   da   = parent_age_grid['end'] - a
   dt   = t - parent_time_grid['start']
   num += math.log( grid_value['start_age, end_time'] ) * da * dt
   #
   # value at end age and start time
   da   = a - parent_age_grid['start']
   dt   = parent_time_grid['end'] - t
   num += math.log( grid_value['end_age, start_time'] ) * da * dt
   #
   # value at end age and end time
   da   = a - parent_age_grid['start']
   dt   = t - parent_time_grid['start']
   num += math.log( grid_value['end_age, end_time'] ) * da * dt
   #
   return math.exp( num / den )
#
def true_rate(node, rate, a, t) :
   parent_node = node_list[0]
   if node != parent_node :
      even_child = node_list.index(node) % 2 == 0
   # default
   grid_value = dict()
   # -------------------------------------------------------------------------
   if rate == 'pini' :
      if node == parent_node :
         grid_value['start_age, start_time'] = .01
         grid_value['start_age, end_time']   = .01
      elif even_child :
         grid_value['start_age, start_time'] = math.exp(0.1)
         grid_value['start_age, end_time']   = math.exp(0.1)
      else :
         grid_value['start_age, start_time'] = math.exp(-0.1)
         grid_value['start_age, end_time']   = math.exp(-0.1)
      #
      # pini is constant in age
      grid_value['end_age, start_time'] = grid_value['start_age, start_time']
      grid_value['end_age, end_time']   = grid_value['start_age, end_time']
      #
      ret = log_bilinear(grid_value, a, t)
      if node != parent_node :
         ret = math.log(ret)
   # -------------------------------------------------------------------------
   elif rate == 'iota' :
      if node == parent_node :
         grid_value['start_age, start_time'] = .001
         grid_value['start_age, end_time']   = .002
         grid_value['end_age, start_time']   = .01
         grid_value['end_age, end_time']     = .02
      elif even_child :
         grid_value['start_age, start_time'] = math.exp(.15)
         grid_value['start_age, end_time']   = math.exp(.15)
         grid_value['end_age, start_time']   = math.exp(.15)
         grid_value['end_age, end_time']     = math.exp(.15)
      else :
         grid_value['start_age, start_time'] = math.exp(-.15)
         grid_value['start_age, end_time']   = math.exp(-.15)
         grid_value['end_age, start_time']   = math.exp(-.15)
         grid_value['end_age, end_time']     = math.exp(-.15)
      #
      ret = log_bilinear(grid_value, a, t)
      if node != parent_node :
         ret = math.log(ret)
   # -------------------------------------------------------------------------
   # we are constraining omega to its true value so we do a detailed
   # simulation of the random effect for omega; i.e., it is not constant
   elif rate == 'omega' :
      if node == parent_node :
         grid_value['start_age, start_time'] = .003
         grid_value['start_age, end_time']   = .002
         grid_value['end_age, start_time']   = .3
         grid_value['end_age, end_time']     = .2
      elif even_child :
         grid_value['start_age, start_time'] = math.exp(.10)
         grid_value['start_age, end_time']   = math.exp(.20)
         grid_value['end_age, start_time']   = math.exp(.20)
         grid_value['end_age, end_time']     = math.exp(.30)
      else :
         grid_value['start_age, start_time'] = math.exp(-.10)
         grid_value['start_age, end_time']   = math.exp(-.20)
         grid_value['end_age, start_time']   = math.exp(-.20)
         grid_value['end_age, end_time']     = math.exp(-.30)
      #
      ret = log_bilinear(grid_value, a, t)
      if node != parent_node :
         ret = math.log(ret)
   # -------------------------------------------------------------------------
   elif rate == 'chi' :
      if node == parent_node :
         grid_value['start_age, start_time'] = .004
         grid_value['start_age, end_time']   = .002
         grid_value['end_age, start_time']   = .1
         grid_value['end_age, end_time']     = .05
      elif even_child :
         grid_value['start_age, start_time'] = math.exp(.25)
         grid_value['start_age, end_time']   = math.exp(.25)
         grid_value['end_age, start_time']   = math.exp(.25)
         grid_value['end_age, end_time']     = math.exp(.25)
      else :
         grid_value['start_age, start_time'] = math.exp(-.25)
         grid_value['start_age, end_time']   = math.exp(-.25)
         grid_value['end_age, start_time']   = math.exp(-.25)
         grid_value['end_age, end_time']     = math.exp(-.25)
      #
      ret = log_bilinear(grid_value, a, t)
      if node != parent_node :
         ret = math.log(ret)
   # -------------------------------------------------------------------------
   else :
      assert False
   #
   return ret
# ------------------------------------------------------------------------
def example_db (file_name) :
   # ------------------------------------------------------------------------
   # functions used for priors in smoothing
   #
   # Note that there are no forward differences for covariate multiplier grids.
   def fun_mulcov_sex(a, t) :
      return('prior_sex',          None, None)
   def fun_mulcov_bmi(a, t) :
      return('prior_bmi',         None, None)
   def fun_mulcov_ms_2000(a, t) :
      return('prior_ms_2000',     None, None)
   #
   # priors used in smoothing for iota
   def fun_iota_parent(a, t) :
      return ( \
         'prior_pos_parent_value', 'prior_parent_age', 'prior_parent_time')
   def fun_iota_child(a, t) :
      return ('prior_N(0,0.5)', 'prior_child_age', 'prior_child_time')
   #
   # priors used in smoothing for chi
   def fun_chi_parent(a, t) :
      return ( \
         'prior_pos_parent_value', 'prior_parent_age', 'prior_parent_time')
   def fun_chi_child(a, t) :
      return ('prior_N(0,0.5)', 'prior_child_age', 'prior_child_time')
   #
   # use const_value to constrain omega to true value
   def fun_omega(node) :
      # Use default argument to bind the value of node at point of
      # definition instead of at point of call.
      def fun_omega_node(a, t, node = node) :
         true_value = true_rate(node, 'omega', a, t)
         return (true_value, 'prior_U(-inf,inf)', 'prior_U(-inf,inf)')
      return fun_omega_node
   #
   # priors used in smoothing for pini
   def fun_pini_parent(a, t) :
      return ('prior_pini_parent_value', None, 'prior_parent_time')
   def fun_pini_child(a, t) :
      return ('prior_N(0,0.5)', None, 'prior_child_time')
   # ----------------------------------------------------------------------
   fun                       = dict()
   fun['mulcov_sex']         = fun_mulcov_sex
   fun['mulcov_bmi']         = fun_mulcov_bmi
   fun['mulcov_ms_2000']     = fun_mulcov_ms_2000
   fun['iota_parent']        = fun_iota_parent
   fun['iota_child']         = fun_iota_child
   fun['chi_parent']         = fun_chi_parent
   fun['chi_child']          = fun_chi_child
   fun['pini_parent']        = fun_pini_parent
   fun['pini_child']         = fun_pini_child
   # ----------------------------------------------------------------------
   # age_list, age_index_rate_parent, age_indexrate__child
   start                     = parent_age_grid['start']
   end                       = parent_age_grid['end']
   number                    = parent_age_grid['number']
   interval                  = (end - start) / (number - 1)
   age_list                  = [ start + j * interval for j in range(number) ]
   age_index_rate_parent     = range(number)
   age_index_rate_child      = child_age_grid['index']
   #
   # ----------------------------------------------------------------------
   # time_list, time_index_rate_parent, time_index_rate_child
   start                  = parent_time_grid['start']
   end                    = parent_time_grid['end']
   number                 = parent_time_grid['number']
   interval               = (end - start) / (number - 1)
   time_list              = [ start + i * interval for i in range(number) ]
   time_index_rate_parent = range(number)
   time_index_rate_child  = child_time_grid['index']
   # ----------------------------------------------------------------------
   # node table:
   parent_node = node_list[0]
   node_table = [ { 'name':parent_node, 'parent':'' } ]
   for i in range(1, len(node_list) ) :
      node_table.append( { 'name':node_list[i], 'parent':parent_node } )
   #
   # ----------------------------------------------------------------------
   # weight table
   weight_table = list()
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      {
         # prior_N(0,0.5)
         'name':     'prior_N(0,0.5)',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      math.sqrt(0.5),
      } , {
         # prior_U(-inf,inf)
         'name':     'prior_U(-inf,inf)',
         'density':  'uniform',
         'mean':     0.0,
      } , {
         # prior_pini_parent_value
         'name':     'prior_pini_parent_value',
         'density':  'uniform',
         'mean':     0.1,  # setting start_var table so mean has no affect
         'lower':    0.0,
         'upper':    1.0,
         'eta':      1e-5, # used for log-scaling during optimization
      } , {
         # prior_pos_parent_value
         'name':     'prior_pos_parent_value',
         'density':  'uniform',
         'mean':     0.1, # setting start_var table so mean has no affect
         'lower':    1e-8,
         'upper':    1.0,
         'eta':      0.0, # used for log-scaling during optimization
      } , {
         # prior_parent_age
         'name':     'prior_parent_age',
         'density':  'log_gaussian',
         'mean':     0.0,
         'std':      parent_age_grid['std'],
         'eta':      1e-5,
      } , {
         # prior_parent_time
         'name':     'prior_parent_time',
         'density':  'log_gaussian',
         'mean':     0.0,
         'std':      parent_time_grid['std'],
         'eta':      1e-5,
      } , {
         # prior_child_age
         'name':     'prior_child_age',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      child_age_grid['std'],
         'eta':      1e-5,
      } , {
         # prior_child_time
         'name':     'prior_child_time',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      child_time_grid['std'],
         'eta':      1e-5,
      } , {
         # prior_sex
         'name':     'prior_sex',
         'density':  'uniform',
         'mean':     0.0,
         'lower':    -2.0,
         'upper':    +2.0,
      } , {
         # prior_bmi
         'name':     'prior_bmi',
         'density':  'uniform',
         'mean':     0.0,
         'lower':    -0.1,
         'upper':    +0.1
      } , {
         # prior_ms_2000
         'name':     'prior_ms_2000',
         'density':  'uniform',
         'mean':     0.0,
         'lower':    -1.0,
         'upper':    +1.0,
         'mean':     0.0,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table and nslist_dict
   smooth_table   = list()
   nslist_dict   = dict()
   #
   # smooth_mulcov
   for cov in [ 'sex', 'bmi', 'ms_2000' ] :
      smooth_table.append( {
         'name':     'smooth_mulcov_' + cov,
         'age_id':   [0],
         'time_id':  [0],
         'fun':      fun['mulcov_' + cov]
      } )
   for rate in [ 'pini', 'iota', 'chi' ] :
      age_index = age_index_rate_parent
      if rate == 'pini' :
         age_index = [0]
      #
      # smooth_rate_parent
      name = rate + '_parent'
      smooth_table.append( {
         'name':        'smooth_' + name        ,
         'age_id':       age_index              ,
         'time_id':      time_index_rate_parent ,
         'fun':          fun[name]
      } )
      #
      age_index = age_index_rate_child
      if rate == 'pini' :
         age_index = [0]
      #
      # smooth_rate_child
      name = rate + '_child'
      smooth_table.append( {
         'name':        'smooth_' + name       ,
         'age_id':       age_index             ,
         'time_id':      time_index_rate_child ,
         'fun':          fun[name]
      } )
   #
   nslist_dict['nslist_omega_child'] = list()
   for node in node_list :
      name                 = 'smooth_omega_' + node
      smoothing            = { 'name': name }
      smoothing['age_id']  = age_index_rate_parent
      smoothing['time_id'] = time_index_rate_parent
      smoothing['fun']     = fun_omega(node)
      smooth_table.append( smoothing )
      #
      if node != parent_node :
         nslist_dict['nslist_omega_child'].append( (node, name) )
   #
   # no standard deviation multipliers
   for dictionary in smooth_table :
      for name in [ 'value' , 'dage', 'dtime' ] :
         key   = 'mulstd_' + name + '_prior_name'
         value = None
         dictionary[key] = value
   # -----------------------------------------------------------------------
   # covariate table:
   covariate_table = [
      {'name':'sex',     'reference':0.0,  'max_difference':0.6  } ,
      {'name':'bmi',     'reference':27.0, 'max_difference':None } ,
      {'name':'ms_2000', 'reference':0.0,  'max_difference':None } ,
   ]
   #
   # mulcov table:
   mulcov_table = [
      {
         # alpha for iota and sex
         'covariate': 'sex',
         'type':      'rate_value',
         'effected':  'iota',
         'group':     'world',
         'smooth':    'smooth_mulcov_sex'
      } , {
         # alpha for iota and bmi
         'covariate': 'bmi',
         'type':      'rate_value',
         'effected':  'iota',
         'group':     'world',
         'smooth':    'smooth_mulcov_bmi'
      } , {
         # beta for prevalence and ms_2000
         'covariate': 'ms_2000',
         'type':      'meas_value',
         'effected':  'prevalence',
         'group':     'world',
         'smooth':    'smooth_mulcov_ms_2000'
      }
   ]
   # ----------------------------------------------------------------------
   # rate table:
   rate_table = [
      {  'name':          'pini',
         'parent_smooth': 'smooth_pini_parent',
         'child_smooth':  'smooth_pini_child',
         'child_nslist':  None,
      } , {
         'name':          'iota',
         'parent_smooth': 'smooth_iota_parent',
         'child_smooth':  'smooth_iota_child',
         'child_nslist':  None,
      } , {
         'name':          'rho',
         'parent_smooth': None,
         'child_smooth':  None,
      } , {
         'name':          'chi',
         'parent_smooth': 'smooth_chi_parent',
         'child_smooth':  'smooth_chi_child',
         'child_nslist':  None,
      } , {
         'name':          'omega',
         'parent_smooth': 'smooth_omega_' + parent_node,
         'child_smooth':  None,
         'child_nslist':  'nslist_omega_child',
      }
   ]
   # ----------------------------------------------------------------------
   # data table, avgint_table:
   data_table   = list()
   avgint_table = list()
   #
   # for each integrand, age, time, node
   n_integrand = len(integrand_table)
   n_age       = len(age_list)
   n_time      = len(time_list)
   n_node      = len(node_table)
   for k1 in range(n_integrand * n_age * n_time * n_node * meas_repeat) :
      den         = n_age * n_time * n_node * meas_repeat
      i_integrand = int( k1 / den )
      k2          =      k1 % den
      #
      den         = n_time * n_node * meas_repeat
      i_age       = int( k2 / den )
      k3          =      k2 % den
      #
      den         = n_node * meas_repeat
      i_time      = int( k3 / den )
      k4          =      k3 % den
      #
      den         = meas_repeat
      i_node      = int( k4 / den )
      #
      age         = age_list[i_age]
      time        = time_list[i_time]
      integrand   = integrand_table[i_integrand]['name']
      node        = node_table[i_node]['name']
      #
      # sex
      if k1 % 2 == 0 :
         sex = -0.5
      else :
         sex = +0.5
      #
      # market scan
      if k1 % 6 < 3 :
         ms_2000 = 1.0
      else :
         ms_2000 = 0.0
      #
      # body mass index
      bmi = 20 + k1 % 17
      #
      row = {
         'node':        node,
         'subgroup':    'world',
         'integrand':   integrand,
         'density':     'log_gaussian',
         'weight':      '',
         'age_lower':    age,
         'age_upper':    age,
         'time_lower':   time,
         'time_upper':   time,
         'sex':          sex,
         'ms_2000':      ms_2000,
         'bmi':          bmi,
         'one':          1,
      }
      avgint_table.append( copy.copy(row) )
      #
      if i_age > 0 and i_time > 0 :
         age  = (age_list[i_age]   + age_list[i_age-1])   / 2.0
         time = (time_list[i_time] + time_list[i_time-1]) / 2.0
      row = {
         'node':        node,
         'subgroup':    'world',
         'integrand':   integrand,
         'density':     'log_gaussian',
         'weight':      '',
         'age_lower':    age,
         'age_upper':    age,
         'time_lower':   time,
         'time_upper':   time,
         'sex':          sex,
         'ms_2000':      ms_2000,
         'bmi':          bmi,
         'one':          1,
      }
      avgint_table.append( copy.copy(row) )
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero'    },
      { 'name':'parent_node_name',       'value':parent_node            },
      { 'name':'ode_step_size',          'value':ode_step_size          },
      { 'name':'random_seed',            'value':random_seed            },
      { 'name':'bound_random',           'value':'1.0'                  },

      { 'name':'quasi_fixed',            'value':quasi_fixed            },
      { 'name':'max_num_iter_fixed',     'value':'300'                  },
      { 'name':'print_level_fixed',      'value':'5'                    },
      { 'name':'tolerance_fixed',        'value':tolerance_fixed        },
      { 'name':'derivative_test_fixed',  'value':derivative_test_fixed  },

      { 'name':'max_num_iter_random',    'value':'50'                   },
      { 'name':'print_level_random',     'value':'0'                    },
      { 'name':'tolerance_random',       'value':'1e-8'                 }
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
   # ----------------------------------------------------------------------
   return
# -----------------------------------------------------------------------
def create_truth_var_table() :
   new             = False
   connection      = dismod_at.create_connection(file_name, new)
   var_table       = dismod_at.get_table_dict(connection, 'var')
   rate_table      = dismod_at.get_table_dict(connection, 'rate')
   covariate_table = dismod_at.get_table_dict(connection, 'covariate')
   integrand_table = dismod_at.get_table_dict(connection, 'integrand')
   node_table      = dismod_at.get_table_dict(connection, 'node')
   time_table      = dismod_at.get_table_dict(connection, 'time')
   age_table       = dismod_at.get_table_dict(connection, 'age')
   # -------------------------------------------------------------------------
   # create truth table
   tbl_name     = 'truth_var'
   col_name     = [ 'truth_var_value' ]
   col_type     = [ 'real' ]
   row_list     = list()
   for var_id in range( len(var_table) ) :
      value = None
      #
      row          = var_table[var_id]
      var_type     = row['var_type']
      age          = age_table[ row['age_id'] ] ['age']
      time         = time_table[ row['time_id'] ] ['time']
      if var_type.startswith('mulcov_') :
         covariate = covariate_table[row['covariate_id' ]]['covariate_name']
         value     = mulcov_dict[covariate]
      elif var_type == 'rate' :
         node  = node_table[ row['node_id'] ] ['node_name']
         rate  = rate_table[ row['rate_id'] ] ['rate_name']
         value = true_rate(node, rate, age, time)
      else :
         assert False
      #
      row_list.append( [ value ] )
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   connection.close()
   return
# ===========================================================================
# Run the init command to create the var table
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
# -----------------------------------------------------------------------
# create truth_var table
create_truth_var_table()
# -----------------------------------------------------------------------------
# create predict table
dismod_at.system_command_prc([ program, file_name, 'predict', 'truth_var' ])
# -----------------------------------------------------------------------------
# add data to data table
new             = False
connection      = dismod_at.create_connection(file_name, new)
predict_table   = dismod_at.get_table_dict(connection, 'predict')
avgint_table    = dismod_at.get_table_dict(connection, 'avgint')
density_table   = dismod_at.get_table_dict(connection, 'density')
#
# get column names and types for data table
tbl_name             = 'data'
(col_name, col_type) = dismod_at.get_name_type(connection, tbl_name)
#
# remove the primary key from col_name and col_type
assert col_name[0] == 'data_id'
del col_name[0]
del col_type[0]
#
# list of row values to place in data table
row_list = list()
#
# density_id for log_gaussian
log_gaussian_id = None
for density_id in range( len( density_table ) ) :
   if density_table[density_id]['density_name'] == 'log_gaussian' :
      log_gaussian_id = density_id
#
for predict_id in range( len(predict_table) ) :
   #
   # get prediction for average integrand
   row = predict_table[predict_id]
   assert row['sample_index'] == None
   assert row['avgint_id']    == predict_id
   avg_integrand = row['avg_integrand']
   #
   # initial row using information in avgint table
   row = copy.copy( avgint_table[predict_id] )
   #
   # add information, that is not in avgint_table, to row
   eta        = 1e-7                     # a very small eta
   meas_value = avg_integrand            # no noise version of meas_value
   meas_std   = meas_cv * avg_integrand # noise in simulated data
   row['density_id'] = log_gaussian_id
   row['hold_out']   = 0
   row['meas_std']   = meas_std
   row['meas_value'] = meas_value
   row['eta']        = eta
   row['nu']         = None
   #
   # values in same order as col_name
   row_value = list()
   for key in col_name :
      if key != 'data_id' :
         row_value.append( row[key] )
   #
   row_list.append( row_value )
#
# drop the old data table
command = 'DROP TABLE data'
dismod_at.sql_command(connection, command)
#
# create the new data table
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list )
# -----------------------------------------------------------------------------
# re-initailize to get data_subset table to correspond to new data
dismod_at.system_command_prc([ program, file_name, 'init' ])
#
# Initializing erases the truth_var table.
# Create a new version of truth_var table that will correspond to fit
create_truth_var_table()
#
# drop the start_var table created by init
new             = False
connection      = dismod_at.create_connection(file_name, new)
command = 'DROP TABLE start_var'
dismod_at.sql_command(connection, command)
#
# create new start_var table
rate_table      = dismod_at.get_table_dict(connection, 'rate')
var_table       = dismod_at.get_table_dict(connection, 'var')
truth_var_table = dismod_at.get_table_dict(connection, 'truth_var')
tbl_name        = 'start_var'
col_name        = [ 'start_var_value' ]
col_type        = [ 'real' ]
row_list        = list()
for var_id in range( len(var_table) ) :
   truth_var_value  = truth_var_table[var_id]['truth_var_value']
   var_type         = var_table[var_id]['var_type']
   rate_id          = var_table[var_id]['rate_id']
   start_var_value  = truth2start * truth_var_value
   if var_type == 'rate' :
         if rate_table[rate_id]['rate_name'] == 'omega' :
            start_var_value  = truth_var_value
   row_list.append( [start_var_value] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
#
# copy start_var table to scale_var table
dismod_at.system_command_prc([ program, file_name, 'set' , 'scale_var', 'start_var' ])
#
# Simulate a data set corresponding to the truth
number_simulate = '1'
dismod_at.system_command_prc([ program, file_name, 'simulate', number_simulate ])
#
# Do a fit with no random effects
cmd            = [ program, file_name, 'fit', 'fixed' ]
if fit_with_noise_in_data :
   simulate_index = '0'
   cmd += [ simulate_index ]
dismod_at.system_command_prc(cmd)
#
# copy fit_var table to start_var table
dismod_at.system_command_prc([ program, file_name, 'set' , 'start_var', 'fit_var' ])
#
# Do a fit with random effects
cmd            = [ program, file_name, 'fit', 'both' ]
if fit_with_noise_in_data :
   simulate_index = '0'
   cmd += [ simulate_index ]
dismod_at.system_command_prc(cmd)
# -----------------------------------------------------------------------------
# compare truth and fit
file_name      = 'example.db'
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
truth_var_table = dismod_at.get_table_dict(connection, 'truth_var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
ok              = True
max_err         = 0.0
for var_id in range( len(var_table) ) :
   truth_var_value = truth_var_table[var_id]['truth_var_value']
   fit_var_value   = fit_var_table[var_id]['fit_var_value']
   if truth_var_value != 0.0 :
      rel_err = fit_var_value / truth_var_value - 1.0
      flag    = abs( rel_err ) <= accept_rel_err
      ok     &= flag
      if not flag :
         print('var_id = ', var_id, ', rel_err = ', rel_err)
      max_err = max( max_err, abs(rel_err) )
assert ok
# -----------------------------------------------------------------------------
print('max_err = ', max_err )
print('elapsed seconds = ', time.time() - start_second)
print('diabetes.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
