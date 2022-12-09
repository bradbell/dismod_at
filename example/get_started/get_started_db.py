# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin get_started_db.py}
# {xrst_comment_ch #}
#
# Create get_started Input Tables: Example and Test
# #################################################
#
# Syntax
# ******
# ``get_started_db.get_started_db`` ()
#
# Discussion
# **********
# The python command above creates the database ``get_started.db``
# in the current working directory.
# This is a very simple case where:
#
# #. All of the rates are zero except for
#    :ref:`rate_table@rate_name@omega` .
# #. There is only one node corresponding to the world, and hence there are no
#    :ref:`option_table@Parent Node@Children` or
#    :ref:`random effects<model_variables@Random Effects, u>` .
# #. There is only one measurement and it is for the
#    :ref:`integrand_table@integrand_name`
#    :ref:`avg_integrand@Integrand, I_i(a,t)@susceptible`
#    and at age 50 and year 2000.
# #. The data table has a
#    :ref:`comment column<database@Comment Columns and Tables>`
#    named ``c_data_info`` that is used as a
#    :ref:`data_extra_column<option_table@Extra Columns@data_extra_columns>` .
# #. There is one
#    :ref:`covariate<covariate_table-name>` income and a corresponding
#    covariate multiplier on the
#    :ref:`rate value<mulcov_table@mulcov_type@rate_value>` for
#    :ref:`omega<avg_integrand@Rate Functions@omega_i(a,t)>` .
# #. The model for other cause mortality
#    :ref:`rate_table@rate_name@omega` ,
#    and the model for the rate covariate multiplier
#    :ref:`alpha<avg_integrand@Rate Functions@Group Rate Covariate Multiplier, alpha_jk>` ,
#    are constant in age and time.
#
# Because other cause mortality and the covariate multiplier are
# constant in age and time,
# the susceptible population satisfies the
# following ODE in age :math:`a`:
#
# .. math::
#
#  S(0) = 1 \W{\R{and}} S'(a) = - \exp( \alpha x ) \omega S(a)
#
# where :math:`\omega` is the other cause mortality rate
# before the covariate effect,
# :math:`x` is the value of the income for this measurement,
# :math:`\alpha` is the covariate multiplier, and
# :math:`\exp ( \alpha x ) \omega`
# is the other cause mortality after the covariate effect;
# see :ref:`r_ik<avg_integrand@Rate Functions@Adjusted Rate, r_ik>` .
# The solution is
#
# .. math::
#
#  S(a) = \exp \left[ - \exp( \alpha x ) \omega \; a \right]
#
# Reference
# *********
# See :ref:`create_database-name` .
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end get_started_db.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ---------------------------------------------------------------------------
# note that the (a, t) arguments are not used by these example functions
unknown_omega_world     = 1e-2
known_income_multiplier = -1e-3
#
def constant_one_fun(a, t):
   return 1.0
def fun_omega_parent(a, t):
   return ('prior_omega_parent', 'prior_not_used', 'prior_not_used')
def fun_income_multiplier(a, t):
   return ('prior_income_multiplier', 'prior_not_used', 'prior_not_used')
# ------------------------------------------------------------------------
def get_started_db ():
   import dismod_at
   from math import exp
   # ----------------------------------------------------------------------
   # age list
   age_list    = [ 0.0, 100.0 ]
   #
   # time list
   time_list   = [ 1995.0, 2015.0 ]
   #
   # only one integrand in this example
   integrand_table = [
       { 'name':'susceptible' }
   ]
   #
   # just the world (which has no parent)
   node_table = [ { 'name':'world', 'parent':'' } ]
   #
   # weight table is represented by one (age, time) point with value 1.0
   fun = constant_one_fun
   weight_table = [
      { 'name':'constant_one', 'age_id':[0], 'time_id':[0], 'fun':fun }
   ]
   #
   # covariate table: the reference value for income is zero
   covariate_table = [ { 'name':'income', 'reference':0.0} ]
   # ---------------------------------------------------------------------
   # avgint table: predict the susceptible fraction for no income at age 100
   avgint_table =  [
      {
         'avgint_info': 'a0',
         'integrand':   'susceptible',
         'node':        'world',
         'subgroup':    'world',
         'weight':      'constant_one',
         'age_lower':   100.0,
         'age_upper':   100.0,
         'time_lower':  2000.0,
         'time_upper':  2000.0,
         'income':      0.0,
      }
   ]
   # ---------------------------------------------------------------------
   # data table: measure the susceptible fraction for income 1000 at age 50
   # (no noise in this simulated data, but modeled as having noise)
   adjusted_omega = unknown_omega_world * exp(known_income_multiplier * 1000.0)
   meas_value     = exp( - adjusted_omega * 50.0 )
   meas_std       = meas_value / 20.
   data_table = [
      {
         'c_data_info':  'd1',
         'integrand':   'susceptible',
         'node':        'world',
         'subgroup':    'world',
         'weight':      'constant_one',
         'age_lower':   50.0,
         'age_upper':   50.0,
         'time_lower':  2000.0,
         'time_upper':  2000.0,
         'income':      1000.0,
         'density':     'gaussian',
         'hold_out':    False,
         'meas_value':  meas_value,
         'meas_std':    meas_std,
      }
   ]
   # ---------------------------------------------------------------------
   # prior_table
   prior_table = [
      {   # prior_not_used
         # (not used because there are no age or time differences)
         'name':     'prior_not_used',
         'density':  'uniform',
         'mean':     0.0,
      },{ # prior_omega_parent
         # omega for the parent is constant in (age, time) and
         # and is a uniform distribution on the interval [1e-4, 1.0].
         # Becasue the prior is uniform, its mean 1e-1 is only used to
         # start the optimization
         'name':     'prior_omega_parent',
         'density':  'uniform',
         'lower':    1e-4,
         'upper':    1.0,
         'mean':     1e-1,
      },{ # prior_income_multiplier
         # (constrained to be equal to know_income_multiplier)
         'name':     'prior_income_multiplier',
         'density':  'uniform',
         'lower':    known_income_multiplier,
         'upper':    known_income_multiplier,
         'mean':     known_income_multiplier,
      }
   ]
   # ---------------------------------------------------------------------
   # smooth table:
   smooth_table = [
      {  # smooth_omega_parent
         'name':                     'smooth_omega_parent',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_omega_parent
      },{  # smooth_income_multiplier
         'name':                     'smooth_income_multiplier',
         'age_id':                   [0],
         'time_id':                  [0],
         'fun':                      fun_income_multiplier
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
   # mulcov table: there is one covariate multiplier for income and it
   # affects the other cause mortality rate:
   mulcov_table = [
      {  'covariate':'income',
         'type':     'rate_value',
         'effected': 'omega',
         'group':    'world',
         'smooth':   'smooth_income_multiplier'
      }
   ]
   # -------------------------------------------------------------------
   # option_table
   option_table = [
      {'name':'parent_node_name',     'value':'world'             },
      {'name':'ode_step_size',        'value':'10.0'              },
      {'name':'age_avg_split',        'value':'5.0'               },
      {'name':'rate_case',            'value':'iota_zero_rho_zero'},
      {'name':'avgint_extra_columns', 'value':'avgint_info'       },
      {'name':'data_extra_columns',   'value':'c_data_info'       }
   ]
   # ---------------------------------------------------------------------
   # nslist_table:
   nslist_table = dict()
   # ----------------------------------------------------------------------
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   # ----------------------------------------------------------------------
   # create database
   file_name = 'get_started.db'
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
# END PYTHON
