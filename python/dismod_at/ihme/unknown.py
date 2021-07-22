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
# This is a special case where the relative path does not have the structure
# of paths on the IHME cluster. It is meant for testing.
# ----------------------------------------------------------------------------
# (age_grid, time_grid, value_prior, dage_prior, dtime_prior) =
def child_smoothing_fun(
	age_table, time_table, density_name2id, integrand_data
) :
	import copy
	import math
	age_grid      = [0.0]
	time_grid     = [1990]
	density_id    = density_name2id['uniform']
	#
	# default prior
	default_prior = {
		'prior_name' : ''             ,
		'density_id' : density_id     ,
		'lower'      : None           ,
		'upper'      : None           ,
		'mean'       : 0.0            ,
		'std'        : None           ,
		'eta'        : None           ,
		'nu'         : None           ,
	}
	# value_prior
	value_prior = copy.copy( default_prior )
	value_prior['prior_name'] = 'child_smoothing_value_prior'
	value_prior['density_id'] = density_name2id['gaussian']
	value_prior['std']        = 0.1
	#
	# dage_prior
	dage_prior  = default_prior
	#
	# dtime_prior
	dtime_prior = default_prior
	#
	return (age_grid, time_grid, value_prior, dage_prior, dtime_prior)
#
# relative path for original ihme database
relative_path = 'data/unknown/475588-102-3.db'
#
# maximum number of data values per integrand
max_per_integrand = 1000
#
# fixed effects convergence tolerance
tolerance_fixed   = 1e-8
#
# maximum number of iterations when optimizing fixed effects
max_num_iter_fixed = 40
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
#
# Ordered dictionary of child smoothing functions
child_smoothing = collections.OrderedDict()
# child_smoothing['iota']   = child_smoothing_fun
child_smoothing['chi']    = child_smoothing_fun
