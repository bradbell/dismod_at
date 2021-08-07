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
def pini_parent_smoothing(
	age_table, time_table, density_name2id, integrand_data
) :
	age_grid     = [ 0.0 ]
	time_grid    = [ 0.0 ]
	density_id   = density_name2id['uniform']
	value_prior = {
		'prior_name' : 'parent_smoothing_pini_value_prior' ,
		'density_id' : density_id      ,
		'lower'      : 0.0             ,
		'upper'      : 1e-3            ,
		'mean'       : 0.0             ,
		'std'        : 1.0             ,
		'eta'        : None            ,
		'nu'         : None            ,
	}
	dage_prior = {
		'prior_name' : 'parent_smoothing_pini_dage_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : None           ,
		'eta'        : None           ,
		'nu'         : None           ,
	}
	dtime_prior = {
		'prior_name' : 'parent_smooting_pini_dtime_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : None           ,
		'eta'        : None           ,
		'nu'         : None           ,
	}
	return (age_grid, time_grid, value_prior, dage_prior, dtime_prior)
# --------------------------------------------------------------------------
def iota_parent_smoothing(
	age_table, time_table, density_name2id, integrand_data
) :
	age_grid     = [ float(age)  for age in range(0, 110, 10) ]
	time_grid    = [ 1990.0, 1995.0, 2015.0 ]
	density_id   = density_name2id['log_gaussian']
	value_prior = {
		'prior_name' : 'parent_smoothing_iota_value_prior' ,
		'density_id' : density_id      ,
		'lower'      : 1e-6           ,
		'upper'      : 1e-2            ,
		'mean'       : 1e-4            ,
		'std'        : 10.0            ,
		'eta'        : 1e-8            ,
		'nu'         : None            ,
	}
	dage_prior = {
		'prior_name' : 'parent_smoothing_iota_dage_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.5            ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	dtime_prior = {
		'prior_name' : 'parent_smooting_iota_dtime_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.1            ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	return (age_grid, time_grid, value_prior, dage_prior, dtime_prior)
# --------------------------------------------------------------------------
def rho_parent_smoothing(
	age_table, time_table, density_name2id, integrand_data
) :
	age_grid     = [ float(age) for age in range(0, 60, 10) ]
	age_grid    += [ float(age) for age in range(60, 105, 5) ]
	time_grid    = [ 1990.0, 2015.0 ]
	density_id   = density_name2id['log_gaussian']
	value_prior = {
		'prior_name' : 'parent_smoothing_rho_value_prior' ,
		'density_id' : density_id      ,
		'lower'      : 1e-4           ,
		'upper'      : 2.0             ,
		'mean'       : 1e-1            ,
		'std'        : 10.0            ,
		'eta'        : 1e-8            ,
		'nu'         : None            ,
	}
	dage_prior = {
		'prior_name' : 'parent_smoothing_rho_dage_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.5            ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	dtime_prior = {
		'prior_name' : 'parent_smooting_rho_dtime_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.1            ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	return (age_grid, time_grid, value_prior, dage_prior, dtime_prior)
# --------------------------------------------------------------------------
def chi_parent_smoothing(
	age_table, time_table, density_name2id, integrand_data
) :
	age_grid     = [ float(age) for age in range(0, 110, 10) ]
	time_grid    = [ 1990.0, 2015.0 ]
	density_id   = density_name2id['log_gaussian']
	value_prior = {
		'prior_name' : 'parent_smoothing_chi_value_prior' ,
		'density_id' : density_id      ,
		'lower'      : 1e-3           ,
		'upper'      : 5.0             ,
		'mean'       : 1e-1            ,
		'std'        : 10.0            ,
		'eta'        : 1e-8            ,
		'nu'         : None            ,
	}
	dage_prior = {
		'prior_name' : 'parent_smoothing_chi_dage_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 0.1            ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	dtime_prior = {
		'prior_name' : 'parent_smooting_chi_dtime_prior',
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : 1.0            ,
		'eta'        : 1e-8           ,
		'nu'         : None           ,
	}
	return (age_grid, time_grid, value_prior, dage_prior, dtime_prior)
# ----------------------------------------------------------------------------
# relative path for original ihme database
relative_path = 'data/475527/dbs/1/2/dismod.db'  # Global
relative_path = 'data/475527/dbs/96/2/dismod.db' # Australasia
#
# meta regression noise level for the variance of the log of the data
log_meta_noise_level = 0.1
#
# maximum number of data values per integrand
max_per_integrand = 1000
#
# fixed effects convergence tolerance
tolerance_fixed   = 1e-8
#
# maximum number of iterations when optimizing fixed effects
max_num_iter_fixed = 50
#
# list of integrand that are in fitting without ode but not with ode
ode_hold_out_list = []
#
# Maximum absolute covariate effect = multiplier * (covariate - referece).
# Note that exp(effect) multiplies a model value to get the model value for
# this covariate value. (Noise covariate multipliers are not included.)
max_covariate_effect = 2.0
#
# Covariate multipliers that are set to a specific value:
# This is a list with each entrry a list consisting of
# covariate_name, rate or integrand, and value we are setting multilpier to
set_mulcov_value = []
#
# Ordered dictionary of parent smoothing functions
import collections
parent_smoothing = collections.OrderedDict()
parent_smoothing['pini']   = None
parent_smoothing['iota']   = iota_parent_smoothing
parent_smoothing['rho']    = rho_parent_smoothing
parent_smoothing['chi']    = chi_parent_smoothing
#
# Ordered dictionary of child smoothing functions
child_smoothing = collections.OrderedDict()
# child_smoothing['pini']   = None # Note necessary because parent is None
child_smoothing['chi']    = None
