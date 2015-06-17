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
# $begin pack_info_ctor$$ $newlinech #$$
# $spell
#	dismod
#	mulcov
# $$
#
#
# $section Variable Packing Information: Python Constructor$$
#
# $head Syntax$$
# $icode%pack_object% = dismod_at.pack_info(
#	%n_integrand%, %n_child%, %smooth_table%, %mulcov_table%, %rate_table%
# %$$
#
# $head n_integrand$$
# This is a non-negative $code int$$
# that is equal to the number of rows in the
# $cref integrand_table$$.
# If the $cref mulcov_table$$ has size zero,
# then $icode n_integrand$$ can be zero (a case used for testing purposes).
#
# $head n_child$$
# This is a non-negative $code int$$
# that is the number of children; i.e., the size of
# $cref/child group/node_table/parent/Child Group/$$
# corresponding to the
# $cref/parent_node/fit_table/parent_node_id/$$.
#
# $head smooth_dict$$
# This is a list of dictionaries.
# For each primary key value $icode smooth_id$$
# and column name $icode col_name$$ in the $cref smooth_table$$,
# $icode%smooth_dict%[%smooth_id%][%col_name%]%$$
# is the corresponding value. Note that
# $codei%
#	%smooth_dict%[%smooth_id%][smooth_id]% == %smooth_id%
# %$$
#
# $head mulcov_dict$$
# This is a list of dictionaries.
# For each primary key value $icode mulcov_id$$
# and column name $icode col_name$$ in the $cref mulcov_table$$,
# $icode%mulcov_dict%[%mulcov_id%][%col_name%]%$$
# is the corresponding value. Note that
# $codei%
#	%mulcov_dict%[%mulcov_id%][mulcov_id]% == %mulcov_id%
# %$$
#
# $head rate_dict$$
# This is a list of dictionaries.
# For each primary key value $icode rate_id$$
# and column name $icode col_name$$ in the $cref rate_table$$,
# $icode%rate_dict%[%rate_id%][%col_name%]%$$
# is the corresponding value. Note that
# $codei%
#	%rate_dict%[%rate_id%][rate_id]% == %rate_id%
# %$$
#
# $end
# ----------------------------------------------------------------------------
class pack_info :
	# ------------------------------------------------------------------------
	def __init__(
		self, n_integrand, n_child, smooth_dict, mulcov_dict, rate_dict
	) :
		# initialize offset
		offset = 0;

		# number_rate_enum_ (pini, iota, rho, chi, omega)
		self.number_rate_enum_ = 5

		# n_smooth_
		self.n_smooth_ = len(smooth_dict)

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
					smooth_id = rate_dict[rate_id]['child_smooth_id']
				else :
					smooth_id = rate_dict[rate_id]['parent_smooth_id']
				n_age  = smooth_dict[smooth_id]['n_age']
				n_time = smooth_dict[smooth_id]['n_time']
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
			for mulcov_id in range( len(mulcov_dict) ) :
				match  = mulcov_dict[mulcov_id]['mulcov_type'] == 'meas_mean'
				match |= mulcov_dict[mulcov_id]['mulcov_type'] == 'meas_std'
				tmp_id = int( mulcov_dict[mulcov_id]['integrand_id'] )
				match &= tmp_id == integrand_id
				if match :
					mulcov_type  = mulcov_dict[mulcov_id]['mulcov_type']
					covaraite_id = mulcov_dict[mulcov_id]['covariate_id']
					smooth_id    = mulcov_dict[mulcov_id]['smooth_id']
					n_age        = smooth_dict[smooth_id]['n_age']
					n_time       = smooth_dict[smooth_id]['n_time']
					info         = {
						'covaraite_id' : covaraite_id ,
						'smooth_id'    : smooth_id ,
						'n_var'        : n_age * n_time,
						'offset'       : offset
					}
					if mulcov_type == 'meas_mean' :
						info_list = meas_mean_mulcov_info_
					elif mulcov_type == 'meas_std' :
						info_list = meas_std_mulcov_info_
					for j in range( len(info) ) :
						if info_list[j].covariate_id == covaraite_id :
							msg  = 'mulcov_dict: '
							msg += 'covariate_id appears twice with '
							msg += 'mulcov_type equal to ' + mulcov_type
							sys.exit(msg)
					info_list.append(info)
					offset += info.n_var

		# rate_mean_mulcov_info_
		self.rate_mean_mulcov_info_ = list()
		for integrand_id in range(n_integrand) :
			for mulcov_id in range( len(mulcov_dict) ) :
				match  = mulcov_dict[mulcov_id]['mulcov_type'] == 'rate_mean'
				tmp_id = int( mulcov_dict[mulcov_id]['integrand_id'] )
				match &= tmp_id == integrand_id
				if match :
					covaraite_id = mulcov_dict[mulcov_id]['covariate_id']
					smooth_id    = mulcov_dict[mulcov_id]['smooth_id']
					n_age        = smooth_dict[smooth_id]['n_age']
					n_time       = smooth_dict[smooth_id]['n_time']
					info         = {
						'covaraite_id' : covaraite_id ,
						'smooth_id'    : smooth_id ,
						'n_var'        : n_age * n_time,
						'offset'       : offset
					}
					info_list = rate_mean_mulcov_info_
					for j in range( len(info) ) :
						if info_list[j].covariate_id == covaraite_id :
							msg  = 'mulcov_dict: '
							msg += 'covariate_id appears twice with '
							msg += 'mulcov_type equal to rate_mean'
							sys.exit(msg)
					info_list.append(info)
					offset += info.n_var

		# size is final offset
		size_ = offset
	# ------------------------------------------------------------------------
	def size() :
		return size_
	# ------------------------------------------------------------------------
	def mulstd_offset(smooth_id) :
		assert smooth_id < n_smooth_
		return mulstd_offset_ + 3 * smooth_id
	# ------------------------------------------------------------------------
	def rate_info(rate_id, j) :
		assert j < n_child_
		return rate_info_[rate_id][j]
	# ------------------------------------------------------------------------
	def meas_mean_mulcov_n_cov(integrand_id) :
		assert integrand_id < n_integrand_
		return len(meas_mean_mulcov_info_)
	def meas_mean_mulcov_info(integrand_id, j) :
		assert integrand_id < n_integrand_
		return meas_mean_mulcov_info[integrand_id][j]
	# ------------------------------------------------------------------------
	def meas_std_mulcov_n_cov(integrand_id) :
		assert integrand_id < n_integrand_
		return len(meas_std_mulcov_info_)
	def meas_std_mulcov_info(integrand_id, j) :
		assert integrand_id < n_integrand_
		return meas_std_mulcov_info[integrand_id][j]
	# ------------------------------------------------------------------------
	def rate_mean_mulcov_n_cov(integrand_id) :
		assert integrand_id < n_integrand_
		return len(rate_mean_mulcov_info_)
	def rate_mean_mulcov_info(integrand_id, j) :
		assert integrand_id < n_integrand_
		return rate_mean_mulcov_info[integrand_id][j]
# ----------------------------------------------------------------------------



