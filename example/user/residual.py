# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_residual.py}
# {xrst_comment_ch #}
#
# Weighted Residuals
# ##################
#
# Example Parameters
# ******************
# The following values are used to simulate the data and define the priors:
# {xrst_spell_off}
# {xrst_code py}
omega_true      = [ 0.01, 0.02 ]
omega_mean      = 0.15
minimum_meas_cv = 0.10
# {xrst_code}
# {xrst_spell_on}
#
# Model
# *****
# The only non-zero model variable for this example is
# other cause mortality, :math:`\omega`, for the parent area.
# The parent omega grid has two points,
# one at age zero and the other at age 100.
# The corresponding true values for :math:`\omega` are
# ``omega_true[0]`` at age zero and ``omega_true[1]`` at age 100.
#
# Data
# ****
# The integrand for this data is
# :ref:`avg_integrand@Integrand, I_i(a,t)@mtother` ; i.e.,
# a direct measurement of :math:`\omega`.
# Both ages are included in the data for this example.
# Both the Gaussian and Log-Gaussian :ref:`densities<density_table-name>`
# are included.
# In addition, both the case where the
# :ref:`data_table@meas_std` is above and below the bound specified
# by :ref:`integrand_table@minimum_meas_cv` are included.
#
# Capital Delta
# =============
# We use the notation
# :ref:`Delta<data_like@Notation@Minimum CV Standard Deviation, Delta_i>` for
# the standard deviation adjusted by the minimum measurement cv.
#
# Lower delta
# ===========
# We use the notation
# :ref:`delta<data_like@Transformed Standard Deviation, delta_i(theta)>`
# for the transformed standard deviation.
#
# Gaussian Residuals
# ==================
# There are no measurement noise covariate multipliers, so
# :math:`\delta = \Delta` and the residual is
#
# .. math::
#
#  (y - \mu) / \delta
#
# where :math:`y` is the measured value and :math:`\mu` is the
# model value for the
# :ref:`average integrand<avg_integrand@Average Integrand, A_i>` .
#
# Log-Gaussian Residuals
# ======================
# There are no measurement noise covariate multipliers, so
#
# .. math::
#
#  \delta = \log ( y + \eta + \Delta ) - \log( y + \eta )
#
# where :math:`y` is the measured value
# and :math:`\eta` is the offset in the log transform.
# The residual is
#
# .. math::
#
#  \frac{ \log ( y + \eta ) - \log ( \mu + \eta ) } { \delta }
#
# where :math:`\mu` is the model value for the average integrand.
#
# Priors
# ******
#
# Value Residual
# ==============
# There are two value residuals, one for :math:`\omega` at age zero
# and the other at age 100.
# The density used for the value residuals is Log-Gaussian.
# The mean value used in the prior for the value residuals :math:`\mu` is
# ``omega_mean`` .
# The standard deviation used for the value residuals in
# ``omega_mean * 0.1``
# The log transformed standard deviation is
#
# .. math::
#
#  \delta = \log ( \mu + \eta + \sigma ) - \log( \mu + \eta )
#
# The residual is
#
# .. math::
#
#  \frac{ \log ( y + \eta ) - \log ( \mu + \eta ) } { \delta }
#
# where :math:`y` is the :ref:`fit_var_table@fit_var_value`
# for the model variable.
#
# Difference Residual
# ===================
# There is one difference residuals for the difference of
# :math:`\omega` at age zero and age 100.
# The density used for the value residuals is Log-Gaussian.
# The mean value used in the prior for the difference residual
# :math:`\mu = 0`.
# The standard deviation used for the difference residual is ``0.1`` .
# (This corresponds to a coefficient of variation of :math:`e^{0.1} - 1`.
# which is approximately equal to ``0.1`` ; i.e., 10 percent.)
# The age difference smoothing multiplier prior id
# :ref:`mulstd_dage_prior_id<smooth_table-name>` for this example is null,
# so :math:`\delta` is equal to the standard deviation ``0.1`` .
# The residual is
#
# .. math::
#
#  \frac{ \log ( z + \eta ) - \log ( y + \eta ) - \mu } { \delta }
#
# where :math:`y` (:math:`z`)
# is the :ref:`fit_var_table@fit_var_value`
# at age zero (age 100).
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_residual.py}
#
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import math
import csv
import copy
import numpy
#
test_program  = 'example/user/residual.py'
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
#
def fun_omega_parent(a, t):
   return ('prior_omega_value', 'prior_omega_dage', None)
def fun_omega_eta() :
   return omega_mean * 1e-4
def fun_omega_value_std() :
   return omega_mean * 0.1
def fun_omega_dage_std() :
   return 0.1
# ------------------------------------------------------------------------
def example_db ():
   # ----------------------------------------------------------------------
   # age list
   age_list    = [ 0.0, 100.0 ]
   #
   # time list
   time_list   = [ 1995.0, 2015.0 ]
   #
   # only one integrand in this example
   integrand_table = [
       { 'name':'mtother', 'minimum_meas_cv':minimum_meas_cv }
   ]
   #
   # just the world (which has no parent)
   node_table = [ { 'name':'world', 'parent':'' } ]
   #
   # ---------------------------------------------------------------------
   weight_table    = list()
   covariate_table = list()
   avgint_table    = list()
   mulcov_table    = list()
   nslist_dict    = dict()
   # ---------------------------------------------------------------------
   # data table
   row = {
      'integrand':   'mtother',
      'node':        'world',
      'subgroup':    'world',
      'weight':      '',
      'time_lower':  2000.0,
      'time_upper':  2000.0,
      'hold_out':    False,
      'eta':         fun_omega_eta(),
      'nu':          4.0
   }
   data_table = list()
   for age_id in range( len(age_list) ) :
      age               = age_list[age_id]
      row['age_lower']  = age
      row['age_upper']  = age
      meas_value        = omega_true[age_id]
      row['meas_value'] = meas_value
      for density in [ 'gaussian', 'log_gaussian' ] :
         row['density']  = density
         #
         meas_std        = meas_value * minimum_meas_cv / 2.0
         row['meas_std'] = meas_std
         data_table.append( copy.copy(row) )
         #
         meas_std        = meas_value * minimum_meas_cv * 2.0
         row['meas_std'] = meas_std
         data_table.append( copy.copy(row) )
   # ---------------------------------------------------------------------
   # prior_table
   prior_table = [
      {  # prior_omega_value
         'name':     'prior_omega_value',
         'density':  'log_gaussian',
         'lower':    0.0,
         'upper':    1.0,
         'mean':     omega_mean,
         'std':      fun_omega_value_std(),
         'eta':      fun_omega_eta()
      },{
         # prior_omega_dage
         'name':     'prior_omega_dage',
         'density':  'log_gaussian',
         'mean':     0.0,
         'std':      fun_omega_dage_std(),
         'eta':      fun_omega_eta()
      }
   ]
   # ---------------------------------------------------------------------
   # smooth table:
   smooth_table = [
      {  # smooth_omega_parent
         'name':                     'smooth_omega_parent',
         'age_id':                   [0, 1],
         'time_id':                  [0],
         'fun':                      fun_omega_parent
      }
   ]
   # ---------------------------------------------------------------------
   # rate table
   rate_table = [
      {  'name':          'omega',
         'parent_smooth': 'smooth_omega_parent',
      }
   ]
   # -------------------------------------------------------------------
   # option_table
   # maximum number of iterations is -1 so compute residuals at start value
   option_table = [
      {'name':'parent_node_name',     'value':'world'             },
      {'name':'rate_case',            'value':'iota_zero_rho_zero'}
   ]
   # -------------------------------------------------------------------
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   # -------------------------------------------------------------------
   # create database
   file_name = 'example.db'
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
example_db()
file_name = 'example.db'
program   = '../../devel/dismod_at'
#
dismod_at.system_command_prc([ program, file_name, 'init'] )
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed'] )
# -----------------------------------------------------------------------
# connect to database
connection      = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
#
# get variable and fit_var tables
data_table            = dismod_at.get_table_dict(connection, 'data')
data_subset_table     = dismod_at.get_table_dict(connection, 'data_subset')
fit_data_subset_table = dismod_at.get_table_dict(connection, 'fit_data_subset')
density_table         = dismod_at.get_table_dict(connection, 'density')
var_table             = dismod_at.get_table_dict(connection, 'var')
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
smooth_gird_table     = dismod_at.get_table_dict(connection, 'smooth_grid')
#
# check data residuals
eps99    = 99.0 * numpy.finfo(float).eps
n_subset = len(data_subset_table)
assert n_subset == 8
for data_subset_id in range(n_subset) :
   data_id        = data_subset_table[data_subset_id]['data_id']
   data_row       = data_table[data_id]
   fit_row        = fit_data_subset_table[data_subset_id]
   avg_integrand  = fit_row['avg_integrand']
   residual       = fit_row['weighted_residual']
   meas_value     = data_row['meas_value']
   meas_std       = data_row['meas_std']
   density_id     = data_row['density_id']
   eta            = data_row['eta']
   density        = density_table[density_id]['density_name']
   Delta          = max( minimum_meas_cv * meas_value, meas_std)
   check          = None
   if density == 'gaussian' :
      check = (meas_value - avg_integrand) / Delta
   if density == 'log_gaussian' :
      log_y_eta_plus  = math.log(meas_value + eta + Delta)
      log_y_eta       = math.log(meas_value + eta)
      log_mu_eta      = math.log(avg_integrand + eta)
      delta           = log_y_eta_plus - log_y_eta
      check           = (log_y_eta - log_mu_eta) / delta
   relerr = residual / check - 1.0
   assert abs(relerr) <= eps99
#
# check variable value residuals
n_var = len(var_table)
assert( n_var == 2)
omega_id         = 4
age_id2var_value = n_var * [0.0]
for var_id in range(n_var) :
   var_row = var_table[var_id]
   assert( var_row['var_type'] == 'rate' )
   assert( var_row['rate_id'] == omega_id )
   fit_row   = fit_var_table[var_id]
   value     = fit_row['fit_var_value']
   residual  = fit_row['residual_value']
   mu        = omega_mean
   sigma     = fun_omega_value_std()
   delta     = math.log(mu + eta + sigma) - math.log(mu + eta)
   check     = (math.log(value + eta) - math.log(mu + eta)) / delta
   relerr    = residual / check - 1.0
   assert abs(relerr) <= eps99
   #
   # mapping from age_id to fit_var_value
   age_id = var_row['age_id']
   age_id2var_value[age_id] = value
   #
   # residual_dage
   if age_id == 0 :
      residual_dage = fit_row['residual_dage']
#
# check dage residual
z        = age_id2var_value[1]
y        = age_id2var_value[0]
mu       = 0.0
delta    = fun_omega_dage_std()
check    = (math.log(z + eta) - math.log(y + eta) - mu) / delta
relerr   = residual_dage / check - 1.0
assert( abs(relerr) <= eps99 )
# -----------------------------------------------------------------------
print('residual.py: OK')
# END PYTHON
