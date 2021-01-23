# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# (age_grid, time_grid, value_prior, dage_prior, dtime_prior) =
def pini_parent_smoothing(
	age_table, time_table, density_name2id, integrand_data
) :
	import numpy
	age_grid     = [ 0.0 ]
	time_grid    = [ float(time) for time in range(2000, 2020, 5) ]
	median       = numpy.median( integrand_data['prevalence'] )
	density_id   = density_name2id['uniform']
	value_prior = {
		'prior_name' : 'parent_smoothing_pini_value_prior' ,
		'density_id' : density_id      ,
		'lower'      : 0.0             ,
		'upper'      : 1.0             ,
		'mean'       : 0.0             ,
		'std'        : None            ,
		'eta'        : None            ,
		'nu'         : None            ,
	}
	density_id   = density_name2id['log_gaussian']
	dage_prior = {
		'prior_name' : 'parent_smoothing_pini_dage_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 1.0            ,
		'eta'        : 1e-9           ,
		'nu'         : None           ,
	}
	dtime_prior = {
		'prior_name' : 'parent_smooting_pini_dtime_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 1.0            ,
		'eta'        : 1e-9           ,
		'nu'         : None           ,
	}
	return (age_grid, time_grid, value_prior, dage_prior, dtime_prior)
#
# ----------------------------------------------------------------------------
# (age_grid, time_grid, value_prior, dage_prior, dtime_prior) =
def iota_parent_smoothing(
	age_table, time_table, density_name2id, integrand_data
) :
	age_grid     = [ float(age)  for age in range(30, 90, 10) ]
	age_grid     = [10.0, 15.0, 20.0, 25.0] + age_grid
	time_grid    = [ float(time) for time in range(1990, 2020, 5) ]
	density_id   = density_name2id['log_gaussian']
	value_prior = {
		'prior_name' : 'parent_smoothing_iota_value_prior' ,
		'density_id' : density_id      ,
		'lower'      : 1e-19           ,
		'upper'      : 0.1             ,
		'mean'       : 1e-5            ,
		'std'        : 5.0             ,
		'eta'        : 1e-6            ,
		'nu'         : None            ,
	}
	dage_prior = {
		'prior_name' : 'parent_smoothing_iota_dage_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.05           ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	dtime_prior = {
		'prior_name' : 'parent_smooting_iota_dtime_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.02           ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	return (age_grid, time_grid, value_prior, dage_prior, dtime_prior)
# ----------------------------------------------------------------------------
# (age_grid, time_grid, value_prior, dage_prior, dtime_prior) =
def chi_parent_smoothing(
	age_table, time_table, density_name2id, integrand_data
) :
	age_grid     = [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 10.0, 15.0, 20.0 ]
	age_grid    += [ float(age)  for age in range(30, 110, 10) ]
	time_grid    = [ float(time) for time in range(1990, 2020, 5) ]
	density_id   = density_name2id['log_gaussian']
	value_prior = {
		'prior_name' : 'parent_smoothing_chi_value_prior' ,
		'density_id' : density_id      ,
		'lower'      : 1e-19           ,
		'upper'      : 1.0             ,
		'mean'       : 1e-3            ,
		'std'        : 5.0             ,
		'eta'        : 1e-6            ,
		'nu'         : None            ,
	}
	dage_prior = {
		'prior_name' : 'parent_smoothing_chi_dage_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.2            ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	dtime_prior = {
		'prior_name' : 'parent_smooting_chi_dtime_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.02           ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	return (age_grid, time_grid, value_prior, dage_prior, dtime_prior)
# --------------------------------------------------------------------------
# relative path for original ihme database
relative_path =  'data/475588/dbs/100/3/dismod.db'
#
# maximum number or sampels per integrand
max_sample = 1000
#
# maximum number of iterations when optimizing fixed effects
max_num_iter_fixed = 50
#
# list of integrand that are in fitting without ode but not with ode
ode_hold_out_list = ['mtexcess']
#
# Maximum absolute covariate effect = multiplier * (covariate - referece).
# Note that exp(effect) multiplies a model value to get the model value for
# this covariate value. (Noise covariate multipliers are not included.)
max_covariate_effect = 2.0
#
# Ordered dictionary of parent smoothing functions
import collections
parent_smoothing = collections.OrderedDict()
parent_smoothing['pini'] = pini_parent_smoothing
parent_smoothing['iota'] = iota_parent_smoothing
parent_smoothing['chi']  = chi_parent_smoothing
#
# Ordered dictionary of child smoothing functions
child_smoothing = dict()
