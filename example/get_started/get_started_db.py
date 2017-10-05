# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin get_started_db.py$$ $newlinech #$$
# $spell
#	avgint
#	dismod
#	covariate
# $$
#
# $section Create get_started Input Tables: Example and Test$$
#
# $head Syntax$$
# $codei%get_started_db.get_started_db()%$$
#
# $head Purpose$$
# The python command above creates the database $code get_started.db$$
# in the current working directory.
# This is a very simple case where:
# $list number$$
# The model is constant in age and time.
# $lnext
# The initial prevalence,
# incidence, remission, and excess mortality are all zero.
# $lnext
# There is only one susceptible measurement (at age 50).
# $lnext
# There is only one node corresponding to the world, and hence
# there are no children (no random effects).
# $lnext
# There is one covariate, income, on the rate of other cause mortality
# $lend
# The other cause mortality $cref/omega/rate_table/rate_name/omega/$$
# is constant in age and time. Hence the susceptible population satisfies the
# following ODE in age $latex a$$:
# $latex \[
#	S(0) = 1 \; S'(t) = - \omega S(t)
# \] $$
# where $latex \omega$$ is the rate of other cause mortality
# (after the covariate effect).
# The solution is $latex S(a) = \exp( - \omega \; a )$$.
#
# $head Reference$$
# See $cref create_database$$.
#
# $head Source Code$$
# $code
# $srcfile%
#	example/get_started/get_started_db.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ---------------------------------------------------------------------------
# note that the (a, t) arguments are not used by these example functions
unknown_omega_world     = 1e-2
known_income_multiplier = -1e-3
#
def constant_one_fun(a, t):
	return 1.0
def fun_zero(a, t):
	return ('prior_zero', 'prior_zero', 'prior_zero')
def fun_one(a, t):
	return ('prior_one', 'prior_one', 'prior_one')
def fun_omega_parent(a, t):
	return ('prior_omega_parent', 'prior_not_used', 'prior_not_used')
def fun_income_multiplier(a, t):
	return ('prior_income_multiplier', 'prior_not_used', 'prior_not_used')
# ------------------------------------------------------------------------
def get_started_db ():
	import sys
	import os
	import copy
	import dismod_at
	from math import exp
	# ----------------------------------------------------------------------
	# age list
	age_list    = [ 0.0, 100.0 ]
	#
	# time list
	time_list   = [ 1995.0, 2015.0 ]
	#
	# integrand table
	integrand_list = [
		'susceptible'
	]
	#
	# node table: is just the world (which has no parent)
	node_table = [
		{ 'name':'world', 'parent':'' }
	]
	#
	# weight table: any constant function can be represented with one
	# (age, time) pair and the corresponding value
	fun = constant_one_fun
	weight_table = [
		{ 'name':'constant_one', 'age_id':[0], 'time_id':[0], 'fun':fun }
	]
	#
	# covariate table: the reference value for income is zero and
	# do not exclude any values becuse they are to far from the reference.
	covariate_table = [
		{ 'name':'income', 'reference':0.0}
	]
	# ---------------------------------------------------------------------
	# avgint table: predict the susceptible fraction for no income at age 100
	avgint_table =  [
		{
			'avgint_info': 'a0',
			'integrand':   'susceptible',
			'node':        'world',
			'weight':      'constant_one',
			'age_lower':   100.0,
			'age_upper':   100.0,
			'time_lower':  2000.0,
			'time_upper':  2000.0,
			'income':      0.0,
		}
	]
	# ---------------------------------------------------------------------
	# data table: note the income for this measurement is 1,000.
	adjusted_omega = unknown_omega_world * exp(known_income_multiplier*1000.0)
	meas_value     = exp( - adjusted_omega * 50.0 )
	meas_std       = meas_value / 20.
	data_table = [
		{
			'data_info':   'd1',
			'integrand':   'susceptible',
			'node':        'world',
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
		{   # prior_not_used:
			# not used because there are no age or time differences
			'name':     'prior_not_used',
			'density':  'uniform',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
		},{ # prior_zero
			'name':     'prior_zero',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    0.0,
			'mean':     0.0,
		},{ # prior_one
			'name':     'prior_one',
			'density':  'uniform',
			'lower':    1.0,
			'upper':    1.0,
			'mean':     1.0,
		},{ # prior_omega_parent
			'name':     'prior_omega_parent',
			'density':  'uniform',
			'lower':    1e-4,
			'upper':    1.0,
			'mean':     1e-1,
		},{ # prior_income_multiplier (constrained to be 1e-3)
			'name':     'prior_income_multiplier',
			'density':  'uniform',
			'lower':    known_income_multiplier,
			'upper':    known_income_multiplier,
			'mean':     known_income_multiplier,
		}
	]
	# ---------------------------------------------------------------------
	# smooth table: omega for the parent is constant in (age, time) and
	# and is a uniform distribution on the interval [1e-4, 1.0]. The prior
	# mean 1e-1, for this case, is only used to initilaize the optimization
	smooth_table = [
		{	# smooth_omega_parent
			'name':                     'smooth_omega_parent',
			'age_id':                   [0],
			'time_id':                  [0],
			'fun':                      fun_omega_parent
		},{	# smooth_income_multiplier
			'name':                     'smooth_income_multiplier',
			'age_id':                   [0],
			'time_id':                  [0],
			'fun':                      fun_income_multiplier
		}
	]
	# ---------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'pini',
		},{
			'name':          'iota',
		},{
			'name':          'rho',
		},{
			'name':          'chi',
		},{
			'name':          'omega',
			'parent_smooth': 'smooth_omega_parent',
		}
	]
	# -------------------------------------------------------------------
	# mulcov table: there is one covariate multiplier for income and it
	# affects the other cause mortality rate:
	mulcov_table = [
		{	'covariate':'income',
			'type':     'rate_value',
			'effected': 'omega',
			'smooth':   'smooth_income_multiplier'
		}
	]
	# -------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'              },
		{ 'name':'ode_step_size',          'value':'10.0'               },
		{ 'name':'rate_case',              'value':'iota_zero_rho_zero' },
		{ 'name':'avgint_extra_columns',   'value':'avgint_info'        },
		{ 'name':'data_extra_columns',     'value':'data_info'          }
	]
	# ---------------------------------------------------------------------
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# create database
	file_name = 'get_started.db'
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_list,
		node_table,
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
