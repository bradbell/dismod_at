# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Under Construction
class pack_info :
	# ------------------------------------------------------------------------
	# Constructor
	#	number_rate_enum_
	#	n_smooth_
	#	n_child_
	#	mulstd_offset_
	#	meas_mean_mulcov_info_
	#	meas_std_mulcov_info_
	#	rate_mean_mulcov_info_
	# ------------------------------------------------------------------------
	def __init__(
		self, n_integrand, n_child, smooth_table, mulcov_table, rate_table
	) :
		# initialize offset
		offset = 0;

		# number_rate_enum_ (pini, iota, rho, chi, omega)
		self.number_rate_enum_ = 5

		# n_smooth_
		self.n_smooth_ = len(smooth_table)

		# n_integrand_
		self.n_integrand_ = n_integrand

		# n_child_
		self.n_child_ = n_child

		# mulstd_offset_
		self.mulstd_offset_ = offset
		offset        += 3 * n_smooth_

		# rate_info_
		self.rate_info_ = list()
		for rate_id in range( number_rate_enum_ ) :
			self.rate_info_.append( list() )
			for j in range(n_child + 1) :
				self.rate_info_[rate_id].append( dict() )
				if j < n_child :
					smooth_id = rate_table[rate_id]['child_smooth_id']
				else :
					smooth_id = rate_table[rate_id]['parent_smooth_id']
				n_age  = smooth_table[smooth_id]['n_age']
				n_time = smooth_table[smooth_id]['n_time']
				n_var  = n_age * n_time
				self.rate_info_[rate_id][j]['smooth_id'] = smooth_id
				self.rate_info_[rate_id][j]['n_var']     = n_var
				self.rate_info_[rate_id][j]['offset']    = offset
				offset += n_var
				#
				# check assumption about pini smoothing
				assert rate_id != 0 or n_age == 1

		# meas_mean_mulcov_info_ and meas_std_mulcov_info_
		self.meas_mean_mulcov_info_ = list()
		self.meas_std_mulcov_info_ = list()
		for integrand_id in range(n_integrand) :
			for mulcov_id in range( len(mulcov_table) ) :
				match  = mulcov_table[mulcov_id]['mulcov_type'] == 'meas_mean'
				match |= mulcov_table[mulcov_id]['mulcov_type'] == 'meas_std'
				tmp_id = int( mulcov_table[mulcov_id]['integrand_id'] )
				match &= tmp_id == integrand_id
				if match :
					mulcov_type  = mulcov_table[mulcov_id]['mulcov_type']
					covaraite_id = mulcov_table[mulcov_id]['covariate_id']
					smooth_id    = mulcov_table[mulcov_id]['smooth_id']
					n_age        = smooth_table[smooth_id]['n_age']
					n_time       = smooth_table[smooth_id]['n_time']
					info         = {
						'covaraite_id' : covaraite_id ,
						'smooth_id'    : smooth_id ,
						'n_var'        : n_age * n_time
						'offset'       : offset
					}
					if mulcov_type == 'meas_mean' :
						info_list = meas_mean_mulcov_info_
					elif mulcov_type == 'meas_std' :
						info_list = meas_std_mulcov_info_
					for j in range( len(info) ) :
						if info_list[j].covariate_id == covaraite_id :
							msg  = 'mulcov_table: '
							msg += 'covariate_id appears twice with '
							msg += 'mulcov_type equal to ' + mulcov_type
							sys.exit(msg)
					info_list.append(info)
					offset += info.n_var

		# rate_mean_mulcov_info_
		self.rate_mean_mulcov_info_ = list()
		for integrand_id in range(n_integrand) :
			for mulcov_id in range( len(mulcov_table) ) :
				match  = mulcov_table[mulcov_id]['mulcov_type'] == 'rate_mean'
				tmp_id = int( mulcov_table[mulcov_id]['integrand_id'] )
				match &= tmp_id == integrand_id
				if match :
					covaraite_id = mulcov_table[mulcov_id]['covariate_id']
					smooth_id    = mulcov_table[mulcov_id]['smooth_id']
					n_age        = smooth_table[smooth_id]['n_age']
					n_time       = smooth_table[smooth_id]['n_time']
					info         = {
						'covaraite_id' : covaraite_id ,
						'smooth_id'    : smooth_id ,
						'n_var'        : n_age * n_time
						'offset'       : offset
					}
					info_list = rate_mean_mulcov_info_
					for j in range( len(info) ) :
						if info_list[j].covariate_id == covaraite_id :
							msg  = 'mulcov_table: '
							msg += 'covariate_id appears twice with '
							msg += 'mulcov_type equal to rate_mean'
							sys.exit(msg)
					info_list.append(info)
					offset += info.n_var

		# size is final offset
		size_ = offset
	# ------------------------------------------------------------------------
	def size :
		return size_
	def integrand_size :
		return integrand_size_
	def child_size :
		return child_size_
	def smooth_size :
		return n_smooth_
	# ------------------------------------------------------------------------
	def mulstd_offset(smooth_id)
		assert smooth_id < n_smooth_
		return mulstd_offset_ + 3 * smooth_id
	# ------------------------------------------------------------------------
	def rate_info(rate_id, j)
		assert j < n_child_
		return rate_info_[rate_id][j]
	# ------------------------------------------------------------------------
	def meas_mean_mulcov_n_cov(integrand_id)
		assert integrand_id < n_integrand_
		return len(meas_mean_mulcov_info_)
	def meas_mean_mulcov_info(integrand_id, j)
		assert integrand_id < n_integrand_
		return meas_mean_mulcov_info[integrand_id][j]
	# ------------------------------------------------------------------------
	def meas_std_mulcov_n_cov(integrand_id)
		assert integrand_id < n_integrand_
		return len(meas_std_mulcov_info_)
	def meas_std_mulcov_info(integrand_id, j)
		assert integrand_id < n_integrand_
		return meas_std_mulcov_info[integrand_id][j]
	# ------------------------------------------------------------------------
	def rate_mean_mulcov_n_cov(integrand_id)
		assert integrand_id < n_integrand_
		return len(rate_mean_mulcov_info_)
	def rate_mean_mulcov_info(integrand_id, j)
		assert integrand_id < n_integrand_
		return rate_mean_mulcov_info[integrand_id][j]
# ----------------------------------------------------------------------------



