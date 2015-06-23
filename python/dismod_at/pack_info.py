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
#	%n_integrand%,
#	%parent_node_id%,
#	%node_dict%,
#	%smooth_dict%,
#	%mulcov_dict%,
#	%rate_dict%
# )%$$
# $icode%size% = %pack_object%.size()
# %$$
# $icode%n_child% = %pack_object%.n_child()%$$
#
# $head See Also$$
# see $cref get_table_dict$$.
#
# $head n_integrand$$
# This is a non-negative $code int$$
# that is equal to the number of rows in the
# $cref integrand_table$$.
# If the $cref mulcov_table$$ has size zero,
# then $icode n_integrand$$ can be zero (a case used for testing purposes).
#
# $head parent_node_id$$
# is an $code in$$ corresponding to the
# $cref/parent_node_id/fit_command/parent_node_id/$$ for the fit command.
#
# $head node_dict$$
# This is a list of dictionaries.
# For each primary key value $icode node_id$$
# and column name $icode col_name$$ in the $cref node_table$$,
# $icode%node_dict%[%node_id%][%col_name%]%$$
# is the corresponding value.
# Only the $cref/parent/node_table/parent/$$ column is used.
#
# $head smooth_dict$$
# This is a list of dictionaries.
# For each primary key value $icode smooth_id$$
# and column name $icode col_name$$ in the $cref smooth_table$$,
# $icode%smooth_dict%[%smooth_id%][%col_name%]%$$
# is the corresponding value.
# Only the $cref/n_age/smooth_table/n_age/$$
# and $cref/n_time/smooth_table/n_time/$$ columns are used.
#
# $head mulcov_dict$$
# This is a list of dictionaries.
# For each primary key value $icode mulcov_id$$
# and column name $icode col_name$$ in the $cref mulcov_table$$,
# $icode%mulcov_dict%[%mulcov_id%][%col_name%]%$$
# is the corresponding value.
# The primary key column $icode%col_name% == mulcov_id%$$ is not used.
#
# $head rate_dict$$
# This is a list of dictionaries.
# For each primary key value $icode rate_id$$
# and column name $icode col_name$$ in the $cref rate_table$$,
# $icode%rate_dict%[%rate_id%][%col_name%]%$$
# is the corresponding value.
# The primary key column $icode%col_name% == rate_id%$$ is not used.
#
# $head size$$
# this return value is a $code int$$ equal to the number of
# $cref/model_variables/model_variable/$$.
#
# $head n_child$$
# This is a non-negative $code int$$
# that is the number of children; i.e., the size of
# $cref/child group/node_table/parent/Child Group/$$
# corresponding to $icode parent_node_id$$.
#
# $end
# ----------------------------------------------------------------------------
# $begin pack_info_mulstd$$ $newlinech #$$
# $spell
#	var
#	mulstd
#	dage
#	dtime
#	dismod
# $$
#
# $section Pack Variables: Standard Deviations Multipliers$$
#
# $head Syntax$$
# $icode%offset% = %pack_object%.mulstd_offset(%smooth_id%)%$$
#
# $head pack_object$$
# This object was constructed using $cref pack_info_ctor$$.
#
# $head smooth_id$$
# This is an $code int$$ that specifies the
# $cref/smooth_id/smooth_table/smooth_id/$$ for this multiplier.
#
# $head offset$$
# The return value is the $code int$$
# offset (index) in the packed variable list
# where the three variables for this smoothing begin.
# The three variables for each smoothing are the
# value, dage, and dtime standard deviation multipliers
# (and should always be used in that order).
#
# $head Example$$
# See $cref/pack_info Example/pack_info/Example/$$.
#
# $end
# ----------------------------------------------------------------------------
# $begin pack_info_rate$$ $newlinech #$$
# $spell
#	var
# $$
#
# $section Pack Variables: Rates$$
#
# $head Syntax$$
# $icode%info% = %pack_object%.rate_info(%rate_id%, %child_id%)
# %$$
#
# $head pack_object$$
# This object was constructed using $cref pack_info_ctor$$.
#
# $head rate_id$$
# This is an $code int$$ that specifies the
# $cref/rate_id/rate_table/rate_id/$$ for the rate values.
#
# $head child_id$$
# This is an $code int$$.
# If $icode%child_id% < %n_child%$$, these rates are for the corresponding
# child node.
# If $icode%child_id% == %n_child%$$, these rates are for the
# parent node.
#
# $head info$$
# The return value is the $code info$$
# is a dictionary with the following keys:
#
# $subhead node_id$$
# $icode%info%['node_id']%$$ is an $code int$$ equal to the
# $cref/node_id/node_table/node_id/$$ for this $icode child_id$$.
# Note that if $icode%child_id% == %n_child%$$,
# $icode%info%['node_id']%$$ is
# $cref/parent_node_id/pack_info_ctor/parent_node_id/$$.
#
# $subhead smooth_id$$
# $icode%info%['smooth_id']%$$ is an $code int$$ equal to the
# $cref/smooth_id/smooth_table/smooth_id/$$ for the rate.
# Note that the smoothing is the same for all child rates.
#
# $subhead n_var$$
# $icode%info%['n_var']%$$ is an $code int$$ equal to the
# number of variables for this $icode rate_id$$ and index $icode j$$.
#
# $subhead offset$$
# $icode%info%['offset']%$$ is an $code int$$ equal to the
# index in the packed variable list where the variables begin.
#
# $head node_id$$
# Is the $cref node_table$$ primary key value for this $icode child_id$$.
# Note that if $icode child_id$$ is equal to
# $cref/n_child/pack_info_ctor/n_child/$$,
# $icode node_id$$ is equal to
# $cref/parent_node_id/pack_info_ctor/parent_node_id/$$.
# Otherwise,
# $codei%
#	%parent_node_id% = %node_dict%[%node_id%]['parent']
# %$$
#
# $head Example$$
# See $cref/pack_info Example/pack_info/Example/$$.
#
# $end
# ----------------------------------------------------------------------------
# $begin pack_info_meas_mulcov$$ $newlinech #$$
# $spell
# 	std
# 	cov
# 	var
# 	mulcov
# 	dismod
# 	covariate
# $$
#
# $section Pack Variables: Measurement Covariate Multipliers$$
#
# $head Syntax$$
# $icode%n_cov% = %pack_object%.meas_mean_mulcov_n_cov(%integrand_id%)
# %$$
# $icode%n_cov% = %pack_object%.meas_std_mulcov_n_cov(%integrand_id%)
# %$$
# $icode%info% = %pack_object%.meas_mean_mulcov_info(%integrand_id%, %j%)
# %$$
# $icode%info% = %pack_object%.meas_std_mulcov_info(%integrand_id%, %j%)
# %$$
#
# $head meas_mean$$
# The functions
# $code meas_mean_mulcov_n_cov$$ and
# $code meas_mean_mulcov_info$$
# return information about the measurement mean covariate multipliers.
#
# $head meas_std$$
# The functions
# $code meas_std_mulcov_n_cov$$ and
# $code meas_std_mulcov_info$$
# return information about the measurement standard deviation
# covariate multipliers.
#
# $head pack_object$$
# This object was constructed using $cref pack_info_ctor$$.
#
# $head integrand_id$$
# This is an $code int$$ that specifies the
# $cref/integrand_id/integrand_table/integrand_id/$$ for the
# covariate multipliers.
#
# $head n_cov$$
# This $code int$$
# is the number of covariate multipliers for the specified
# $icode integrand_id$$.
# It is referred to as $codei%n_cov(%integrand_id%)%$$ below.
#
# $head j$$
# This in an $code int$$
# and $icode%j% < n_cov(%integrand_id%)%$$.
#
# $head info$$
# The return value is the $code info$$
# is a dictionary with the following keys:
#
# $subhead covariate_id$$
# $icode%info%['covariate_id']%$$ is an $code int$$ that specifies the
# $cref/covariate_id/covariate_table/covariate_id/$$ for the
# $th j$$ covariate multiplier for this $icode integrand_id$$.
#
# $subhead smooth_id$$
# $icode%info%['smooth_id']%$$ is an $code int$$ that specifies the
# $cref/smooth_id/smooth_table/smooth_id/$$ for the
# $th j$$ covariate multiplier for this $icode integrand_id$$.
#
# $subhead n_var$$
# $icode%info%['n_var']%$$ is an $code int$$ equal to the
# number of variables for this covariate multiplier.
#
# $subhead offset$$
# $icode%info%['offset']%$$ is an $code int$$ equal to the
# index in the packed variable list where the variables begin.
#
#
# $head Example$$
# See $cref/pack_info Example/pack_info/Example/$$.
#
# $end
# ----------------------------------------------------------------------------
# $begin pack_info_rate_mulcov$$ $newlinech #$$
# $spell
# 	std
# 	cov
# 	var
# 	mulcov
# 	dismod
# 	covariate
# $$
#
# $section Pack Variables: Rate Covariate Multipliers$$
#
# $head Syntax$$
# $icode%n_cov% = %pack_object%.rate_mean_mulcov_n_cov(%integrand_id%)
# %$$
# $icode%info% = %pack_object%.rate_mean_mulcov_info(%integrand_id%, %j%)
# %$$
#
# $head pack_object$$
# This object was constructed using $cref pack_info_ctor$$.
#
# $head rate_id$$
# This is an $code int$$ that specifies the
# $cref/rate_id/rate_table/rate_id/$$ for the
# covariate multipliers.
#
# $head n_cov$$
# This $code int$$
# is the number of covariate multipliers for the specified
# $icode rate_id$$.
# It is referred to as $codei%n_cov(%rate_id%)%$$ below.
#
# $head j$$
# This in an $code int$$
# and $icode%j% < n_cov(%rate_id%)%$$.
#
# $head info$$
# The return value is the $code info$$
# is a dictionary with the following keys:
#
# $subhead covariate_id$$
# $icode%info%['covariate_id']%$$ is an $code int$$ that specifies the
# $cref/covariate_id/covariate_table/covariate_id/$$ for the
# $th j$$ covariate multiplier for this $icode rate_id$$.
#
# $subhead smooth_id$$
# $icode%info%['smooth_id']%$$ is an $code int$$ that specifies the
# $cref/smooth_id/smooth_table/smooth_id/$$ for the
# $th j$$ covariate multiplier for this $icode rate_id$$.
#
# $subhead n_var$$
# $icode%info%['n_var']%$$ is an $code int$$ equal to the
# number of variables for this covariate multiplier.
#
# $subhead offset$$
# $icode%info%['offset']%$$ is an $code int$$ equal to the
# index in the packed variable list where the variables begin.
#
# $head Example$$
# See $cref/pack_info Example/pack_info/Example/$$.
#
# $end
# ----------------------------------------------------------------------------
import pdb
class pack_info :
	# ------------------------------------------------------------------------
	def __init__(
		self,
		n_integrand,
		parent_node_id,
		node_dict,
		smooth_dict,
		mulcov_dict,
		rate_dict
	) :
		# initialize offset
		offset = 0;

		# number of rates (pini, iota, rho, chi, omega)
		self.n_rate_ = 5

		# n_smooth_
		self.n_smooth_ = len(smooth_dict)

		# n_integrand_
		self.n_integrand_ = n_integrand

		# child2node_
		self.child2node_ = list()
		for node_id in range( len(node_dict) ) :
			if node_dict[node_id]['parent'] == parent_node_id :
				self.child2node_.append( node_id )
		self.child2node_.append( parent_node_id )

		# n_child_
		self.n_child_ = len( self.child2node_ ) - 1

		# mulstd_offset_
		self.mulstd_offset_ = offset
		offset        += 3 * self.n_smooth_

		# rate_info_
		self.rate_info_ = list()
		for rate_id in range( self.n_rate_ ) :
			self.rate_info_.append( list() )
			for child_id in range(self.n_child_ + 1) :
				self.rate_info_[rate_id].append( dict() )
				if child_id < self.n_child_ :
					smooth_id = rate_dict[rate_id]['child_smooth_id']
				else :
					smooth_id = rate_dict[rate_id]['parent_smooth_id']
				node_id = self.child2node_[child_id]
				n_age   = smooth_dict[smooth_id]['n_age']
				n_time  = smooth_dict[smooth_id]['n_time']
				n_var   = n_age * n_time
				self.rate_info_[rate_id][child_id]['node_id']   = node_id
				self.rate_info_[rate_id][child_id]['smooth_id'] = smooth_id
				self.rate_info_[rate_id][child_id]['n_var']     = n_var
				self.rate_info_[rate_id][child_id]['offset']    = offset
				offset += n_var
				#
				# check assumption about pini smoothing
				assert rate_id != 0 or n_age == 1

		# meas_mean_mulcov_info_ and meas_std_mulcov_info_
		self.meas_mean_mulcov_info_ = list()
		self.meas_std_mulcov_info_  = list()
		for integrand_id in range(n_integrand) :
			self.meas_mean_mulcov_info_.append( list() )
			self.meas_std_mulcov_info_.append( list() )
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
						'covariate_id' : covaraite_id ,
						'smooth_id'    : smooth_id ,
						'n_var'        : n_age * n_time,
						'offset'       : offset
					}
					if mulcov_type == 'meas_mean' :
						info_list = self.meas_mean_mulcov_info_[integrand_id]
					elif mulcov_type == 'meas_std' :
						info_list = self.meas_std_mulcov_info_[integrand_id]
					for j in range( len(info_list) ) :
						if info_list[j]['covariate_id'] == covaraite_id :
							msg  = 'mulcov_dict: '
							msg += 'covariate_id appears twice with '
							msg += 'mulcov_type equal to ' + mulcov_type
							sys.exit(msg)
					info_list.append(info)
					offset += info['n_var']

		# rate_mean_mulcov_info_
		self.rate_mean_mulcov_info_ = list()
		for rate_id in range(self.n_rate_) :
			self.rate_mean_mulcov_info_.append( list() )
			for mulcov_id in range( len(mulcov_dict) ) :
				match  = mulcov_dict[mulcov_id]['mulcov_type'] == 'rate_mean'
				tmp_id = int( mulcov_dict[mulcov_id]['rate_id'] )
				match &= tmp_id == rate_id
				if match :
					covaraite_id = mulcov_dict[mulcov_id]['covariate_id']
					smooth_id    = mulcov_dict[mulcov_id]['smooth_id']
					n_age        = smooth_dict[smooth_id]['n_age']
					n_time       = smooth_dict[smooth_id]['n_time']
					info         = {
						'covariate_id' : covaraite_id ,
						'smooth_id'    : smooth_id ,
						'n_var'        : n_age * n_time,
						'offset'       : offset
					}
					info_list = self.rate_mean_mulcov_info_[rate_id]
					for j in range( len(info_list) ) :
						if info_list[j].covariate_id == covaraite_id :
							msg  = 'mulcov_dict: '
							msg += 'covariate_id appears twice with '
							msg += 'mulcov_type equal to rate_mean'
							sys.exit(msg)
					info_list.append(info)
					offset += info['n_var']

		# size is final offset
		self.size_ = offset
	# ------------------------------------------------------------------------
	def size(self) :
		return self.size_
	def n_child(self) :
		return self.n_child_
	# ------------------------------------------------------------------------
	def mulstd_offset(self, smooth_id) :
		assert smooth_id < self.n_smooth_
		return self.mulstd_offset_ + 3 * smooth_id
	# ------------------------------------------------------------------------
	def rate_info(self, rate_id, child_id) :
		return self.rate_info_[rate_id][child_id]
	# ------------------------------------------------------------------------
	def meas_mean_mulcov_n_cov(self, integrand_id) :
		return len(self.meas_mean_mulcov_info_[integrand_id])
	def meas_mean_mulcov_info(self, integrand_id, j) :
		return self.meas_mean_mulcov_info_[integrand_id][j]
	# ------------------------------------------------------------------------
	def meas_std_mulcov_n_cov(self, integrand_id) :
		return len(self.meas_std_mulcov_info_[integrand_id])
	def meas_std_mulcov_info(self, integrand_id, j) :
		return self.meas_std_mulcov_info_[integrand_id][j]
	# ------------------------------------------------------------------------
	def rate_mean_mulcov_n_cov(self, rate_id) :
		return len(self.rate_mean_mulcov_info_[rate_id])
	def rate_mean_mulcov_info(self, rate_id, j) :
		return self.rate_mean_mulcov_info_[rate_id][j]
# ----------------------------------------------------------------------------



