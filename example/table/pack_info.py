# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin pack_info.py$$ $newlinech #$$
#
# $section pack_info: Example and Test$$
#
# $code
# $verbatim%example/table/pack_info.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
def pack_info() :
	import dismod_at
	# -------------------------------------------------------------------------
	# constructor
	n_integrand = 4
	n_child     = 2
	smooth_dict =  [
		{ 'n_age':2, 'n_time':3 },
		{ 'n_age':2, 'n_time':3 },
		{ 'n_age':1, 'n_time':1 },
		{ 'n_age':3, 'n_time':3 }
	]
	mulcov_dict = [ {
			'mulcov_type':'meas_mean',
			'rate_id':     -1,
			'integrand_id': 0,
			'covariate_id': 0,
			'smooth_id':    0
		},{
			'mulcov_type':'meas_mean',
			'rate_id':     -1,
			'integrand_id': 1,
			'covariate_id': 1,
			'smooth_id':    1
		},{
			'mulcov_type':'meas_std',
			'rate_id':     -1,
			'integrand_id': 2,
			'covariate_id': 2,
			'smooth_id':    2
		},{
			'mulcov_type':'rate_mean',
			'rate_id':      3,
			'integrand_id':-1,
			'covariate_id': 3,
			'smooth_id':    3
	} ]
	rate_dict = [
		{ 'rate_name':'pini',  'parent_smooth_id':2, 'child_smooth_id':2 },
		{ 'rate_name':'iota',  'parent_smooth_id':0, 'child_smooth_id':1 },
		{ 'rate_name':'rho',   'parent_smooth_id':0, 'child_smooth_id':1 },
		{ 'rate_name':'chi',   'parent_smooth_id':0, 'child_smooth_id':1 },
		{ 'rate_name':'omega', 'parent_smooth_id':0, 'child_smooth_id':1 }
	]
	pack_object = dismod_at.pack_info(
		n_integrand, n_child, smooth_dict, mulcov_dict, rate_dict
	)
	# -------------------------------------------------------------------------
	# packed list
	size      = pack_object.size()
	pack_list =  size * [0.0]
	count     = 0
	# -------------------------------------------------------------------------
	# set mulstd
	n_smooth = len(smooth_dict)
	for smooth_id in range(n_smooth) :
		offset = pack_object.mulstd_offset(smooth_id)
		pack_list[offset + 0] = float(smooth_id + 0) # value multiplier
		pack_list[offset + 1] = float(smooth_id + 1) # dage  multiplier
		pack_list[offset + 2] = float(smooth_id + 2) # dtime multiplier
		count                += 3
	#
	# set rates
	n_rate = 5
	for rate_id in range(n_rate) :
		for j in range(n_child) :
			info = pack_object.rate_info(rate_id, j)
			for k in range( info['n_var'] ) :
				pack_list[info['offset'] + k] = float(rate_id + 3 + j + k)
				count += 1
	#
	# set meas_mean_mulcov
	for integrand_id in range(n_integrand) :
		n_cov = pack_object.meas_mean_mulcov_n_cov(integrand_id)
		for j in range(n_cov) :
			info = pack_object.meas_mean_mulcov_info(integrand_id, j)
			for k in range( info['n_var'] ) :
				pack_list[info['offset'] + k] = float(integrand_id + 4 + j + k)
				count += 1
	#
	# set meas_std_mulcov
	for integrand_id in range(n_integrand) :
		n_cov = pack_object.meas_std_mulcov_n_cov(integrand_id)
		for j in range(n_cov) :
			info = pack_object.meas_std_mulcov_info(integrand_id, j)
			for k in range( info['n_var'] ) :
				pack_list[info['offset'] + k] = float(integrand_id + 5 + j + k)
				count += 1
	#
	# set rate_mean_mulcov
	for rate_id in range(n_rate) :
		n_cov = pack_object.rate_mean_mulcov_n_cov(integrand_id)
		for j in range(n_cov) :
			info = pack_object.rate_mean_mulcov_info(integrand_id, j)
			for k in range( info['n_var'] ) :
				pack_list[info['offset'] + k] = float(integrand_id + 6 + j + k)
				count += 1
	# -------------------------------------------------------------------------
	# check mulstd
	for smooth_id in range(n_smooth) :
		offset = pack_object.mulstd_offset(smooth_id)
		assert pack_list[offset + 0] == float(smooth_id + 0)
		assert pack_list[offset + 1] == float(smooth_id + 1)
		assert pack_list[offset + 2] == float(smooth_id + 2)
	#
	# check rates
	n_rate = 5
	for rate_id in range(n_rate) :
		for j in range(n_child) :
			info   = pack_object.rate_info(rate_id, j)
			offset = info['offset']
			for k in range( info['n_var'] ) :
				assert pack_list[offset + k] == float(rate_id + 3 + j + k)
	#
	# check meas_mean_mulcov
	for integrand_id in range(n_integrand) :
		n_cov = pack_object.meas_mean_mulcov_n_cov(integrand_id)
		for j in range(n_cov) :
			info   = pack_object.meas_mean_mulcov_info(integrand_id, j)
			offset = info['offset']
			for k in range( info['n_var'] ) :
				assert pack_list[offset + k] == float(integrand_id + 4 + j + k)
	#
	# check meas_std_mulcov
	for integrand_id in range(n_integrand) :
		n_cov = pack_object.meas_std_mulcov_n_cov(integrand_id)
		for j in range(n_cov) :
			info   = pack_object.meas_std_mulcov_info(integrand_id, j)
			offset = info['offset']
			for k in range( info['n_var'] ) :
				assert pack_list[offset + k] == float(integrand_id + 5 + j + k)
	#
	# check rate_mean_mulcov
	for rate_id in range(n_rate) :
		n_cov = pack_object.rate_mean_mulcov_n_cov(integrand_id)
		for j in range(n_cov) :
			info   = pack_object.rate_mean_mulcov_info(integrand_id, j)
			offset = info['offset']
			for k in range( info['n_var'] ) :
				assert pack_list[offset + k] == float(integrand_id + 6 + j + k)
# END PYTHON
