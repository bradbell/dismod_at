// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin pack_info_ctor$$
$spell
	mulcov
	CppAD
	struct
	dismod
	var
	const
	integrands
	nslist
$$

$section Variable Pack Info: Constructor$$

$head Syntax$$
$codei%pack_info %pack_object%(
	%n_integrand%,
	%child_id2node_id%,
	%subgroup_table%,
	%smooth_table%,
	%mulcov_table%,
	%rate_table%,
	%nslist_pair%
)
%$$
$codei%pack_info %pack_copy%(%pack_object%)
%$$

$head n_integrand$$
This argument has prototype
$codei%
	size_t %n_integrand%
%$$
and is the number of integrands; i.e., the size of
$cref/integrand_table/get_integrand_table/integrand_table/$$.
If $cref/mulcov_table/pack_info_ctor/mulcov_table/$$ has size zero,
then $icode n_integrand$$ can be zero (a case used for testing purposes).

$head child_id2node_id$$
This argument has prototype
$codei%
	const CppAD::vector<size_t> %child2node%
%$$
and is a mapping from $icode child_id$$ to $icode node_id$$; see
$cref/child_id2node_id/child_info/child_id2node_id/$$.
The size of this vector is the number of children; see
$cref/child_size/child_info/child_size/$$.
If
$codei%
	%rate_table%[%rate_id%].child_nslist_id == DISMOD_AT_NULL_INT
%$$
for all $icode rate_id$$, only the size of this vector matters
(its values are not used).

$head subgroup_table$$
This argument has prototype
$codei%
	const CppAD::vector<subgroup_struct>& %subgroup_table%
%$$
and is the
$cref/subgroup_table/get_subgroup_table/subgroup_table/$$.

$head smooth_table$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_struct>& %smooth_table%
%$$
and is the
$cref/smooth_table/get_smooth_table/smooth_table/$$.
Only the following fields of this table are used:
$code n_age$$, $code n_time$$.

$head mulcov_table$$
This argument has prototype
$codei%
	const CppAD::vector<mulcov_struct>& %mulcov_table%
%$$
and is the
$cref/mulcov_table/get_mulcov_table/mulcov_table/$$.

$head rate_table$$
This argument has prototype
$codei%
	const CppAD::vector<rate_struct>& %rate_table%
%$$
and is the
$cref/rate_table/get_rate_table/rate_table/$$.

$head nslist_pair$$
This argument has prototype
$codei%
	const CppAD::vector<nslist_pair_struct>& %nslist_pair%
%$$
and is the
$cref/nslist_pair/get_nslist_pair/nslist_pair/$$.
If
$codei%
	%rate_table%[%rate_id%].child_nslist_id == DISMOD_AT_NULL_INT
%$$
for all $icode rate_id$$, this table is not used and can be empty; i.e.,
have size zero.


$head pack_copy$$
This object is a copy of the $icode pack_object$$ object
(and acts the same).

$head Age-Time Order$$
When an function of age and time is stored with a specified $icode offset$$
in a packed vector,
it is in time major order; i.e.,
for $icode%i% = 0%, ... , n_age%-1%$$,
for $icode%j% = 0%, ... , n_time%-1%$$,
$codei%
	%offset% + %i% * %n_time% + %j%
%$$
is the index in the packed vector of the corresponding age-time point.


$head Example$$
See $cref/pack_info Example/pack_info/Example/$$.

$end
*/

# include <cppad/cppad.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/error_exit.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

pack_info::pack_info(
	size_t                                    n_integrand      ,
	const CppAD::vector<size_t>               child_id2node_id ,
	const CppAD::vector<subgroup_struct>&     subgroup_table   ,
	const CppAD::vector<smooth_struct>&       smooth_table     ,
	const CppAD::vector<mulcov_struct>&       mulcov_table     ,
	const CppAD::vector<rate_struct>&         rate_table       ,
	const CppAD::vector<nslist_pair_struct>&  nslist_pair
) :
n_smooth_       ( smooth_table.size() )   ,
n_integrand_    ( n_integrand )           ,
n_child_        ( child_id2node_id.size() )
{	using std::string;

	// used to set all fields to null
	subvec_info null_info;
	null_info.covariate_id = DISMOD_AT_NULL_SIZE_T;
	null_info.smooth_id    = DISMOD_AT_NULL_SIZE_T;
	null_info.group_id     = DISMOD_AT_NULL_SIZE_T;
	null_info.n_var        = DISMOD_AT_NULL_SIZE_T;
	null_info.offset       = DISMOD_AT_NULL_SIZE_T;

	// set first_subgroup_id_, subgroup_size_
	size_t previous_group_id  = size_t( subgroup_table[0].group_id );
	assert( previous_group_id == 0 );
	size_t previous_subgroup_id = 0;
	first_subgroup_id_.push_back(previous_subgroup_id);
	size_t end_subgroup = subgroup_table.size();
	for(size_t subgroup_id = 1; subgroup_id < end_subgroup; ++subgroup_id)
	{	size_t group_id = size_t( subgroup_table[subgroup_id].group_id );
		if( group_id != previous_group_id )
		{	assert( group_id == previous_group_id + 1 );
			//
			subgroup_size_.push_back( subgroup_id - previous_subgroup_id );
			first_subgroup_id_.push_back( subgroup_id );
			previous_group_id    = group_id;
			previous_subgroup_id = subgroup_id;
		}
	}
	subgroup_size_.push_back( end_subgroup - previous_subgroup_id );

	// initialize offset
	size_t offset = 0;

	// resize by number of rates
	node_rate_value_info_.resize( number_rate_enum );
	group_rate_value_info_.resize( number_rate_enum );
	subgroup_rate_value_info_.resize( number_rate_enum );

	// resize by number of children
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
		node_rate_value_info_[rate_id].resize(n_child_ + 1);

	// resize by number of integrands
	group_meas_value_info_.resize( n_integrand );
	group_meas_noise_info_.resize( n_integrand );

	// -----------------------------------------------------------------------
	// random effects
	// -----------------------------------------------------------------------

	// node_rate_value_info_
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	for(size_t j = 0;  j < n_child_; j++)
	{	size_t child_nslist_id = rate_table[rate_id].child_nslist_id;
		size_t smooth_id = rate_table[rate_id].child_smooth_id;
		if( child_nslist_id != DISMOD_AT_NULL_SIZE_T )
		{	assert( smooth_id == DISMOD_AT_NULL_SIZE_T );
			//
			// search for the smooth_id for this child
			size_t child_node_id = child_id2node_id[j];
			for(size_t i = 0; i < nslist_pair.size(); i++)
			{	size_t nslist_id = nslist_pair[i].nslist_id;
				size_t node_id   = nslist_pair[i].node_id;
				bool   match     = nslist_id == child_nslist_id;
				match           &= node_id   == child_node_id;
				if( match )
					smooth_id = nslist_pair[i].smooth_id;
			}
			// following should have been checked previously
			assert( smooth_id != DISMOD_AT_NULL_SIZE_T );
		}
		if( smooth_id == DISMOD_AT_NULL_SIZE_T )
			node_rate_value_info_[rate_id][j] = null_info;
		else
		{	size_t n_age  = smooth_table[smooth_id].n_age;
			size_t n_time = smooth_table[smooth_id].n_time;
			size_t n_var  = n_age * n_time;
			subvec_info& info = node_rate_value_info_[rate_id][j];
			info.covariate_id = DISMOD_AT_NULL_SIZE_T;
			info.group_id     = DISMOD_AT_NULL_SIZE_T;
			info.smooth_id    = smooth_id;
			info.n_var        = n_var;
			info.offset       = offset;
			//
			offset += n_var;
			//
			// check_rate_table should have checked this assumption
			assert( rate_id != pini_enum || n_age == 1 );
		}
	}

	// subgroup_rate_value_info_
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	for(size_t mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
	{	const mulcov_struct& mulcov_obj = mulcov_table[mulcov_id];
		bool match;
		match  = mulcov_obj.mulcov_type  == rate_value_enum;
		match &= mulcov_obj.rate_id == int(rate_id);
		match &= mulcov_obj.subgroup_smooth_id != DISMOD_AT_NULL_INT;
		if( match )
		{
			size_t covariate_id = size_t(mulcov_obj.covariate_id);
			size_t group_id     = size_t(mulcov_obj.group_id);
			size_t smooth_id    = mulcov_obj.subgroup_smooth_id;
			size_t n_age        = smooth_table[smooth_id].n_age;
			size_t n_time       = smooth_table[smooth_id].n_time;
			size_t n_subgroup   = subgroup_size_[group_id];
			//
			CppAD::vector<subvec_info> info_vec(n_subgroup);
			for(size_t k = 0; k < n_subgroup; ++k)
			{	info_vec[k].covariate_id = covariate_id;
				info_vec[k].group_id     = size_t(mulcov_obj.group_id);
				info_vec[k].smooth_id    = smooth_id;
				info_vec[k].n_var        = n_age * n_time;
				info_vec[k].offset       = offset;
				offset += info_vec[k].n_var;
			}
			subgroup_rate_value_info_[rate_id].push_back( info_vec );
		}
	}

	// n_random_
	n_random_ = offset;

	// -----------------------------------------------------------------------
	// fixed effects
	// -----------------------------------------------------------------------

	// mulstd_offset_
	mulstd_offset_.resize(3 * n_smooth_);
	for(size_t smooth_id = 0; smooth_id < n_smooth_; smooth_id++)
	{	int prior_id = smooth_table[smooth_id].mulstd_value_prior_id;
		if( prior_id == DISMOD_AT_NULL_INT )
			mulstd_offset_[smooth_id * 3 + 0] = DISMOD_AT_NULL_SIZE_T;
		else
			mulstd_offset_[smooth_id * 3 + 0] = offset++;
		//
		prior_id = smooth_table[smooth_id].mulstd_dage_prior_id;
		if( prior_id == DISMOD_AT_NULL_INT )
			mulstd_offset_[smooth_id * 3 + 1] = DISMOD_AT_NULL_SIZE_T;
		else
			mulstd_offset_[smooth_id * 3 + 1] = offset++;
		//
		prior_id = smooth_table[smooth_id].mulstd_dtime_prior_id;
		if( prior_id == DISMOD_AT_NULL_INT )
			mulstd_offset_[smooth_id * 3 + 2] = DISMOD_AT_NULL_SIZE_T;
		else
			mulstd_offset_[smooth_id * 3 + 2] = offset++;
	}

	// node_rate_value_info_
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t smooth_id  = rate_table[rate_id].parent_smooth_id;
		subvec_info& info = node_rate_value_info_[rate_id][n_child_];

		if( smooth_id == DISMOD_AT_NULL_SIZE_T )
			info = null_info;
		else
		{	size_t n_age  = smooth_table[smooth_id].n_age;
			size_t n_time = smooth_table[smooth_id].n_time;
			size_t n_var  = n_age * n_time;
			info.covariate_id = DISMOD_AT_NULL_SIZE_T;
			info.group_id     = DISMOD_AT_NULL_SIZE_T;
			info.smooth_id    = smooth_id;
			info.n_var        = n_var;
			info.offset       = offset;
			//
			offset += n_var;
			//
			// check_rate_table should have checked this assumption
			assert( rate_id != pini_enum || n_age == 1 );
		}
	}

	// group_meas_value_info_ and group_meas_noise_info_
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	for(size_t mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
	{	const mulcov_struct& mulcov_obj = mulcov_table[mulcov_id];
		bool match;
		match  = mulcov_obj.mulcov_type  == meas_value_enum;
		match |= mulcov_obj.mulcov_type  == meas_noise_enum;
		match &= mulcov_obj.integrand_id == int(integrand_id);
		match &= mulcov_obj.group_smooth_id != DISMOD_AT_NULL_INT;
		if( match )
		{	size_t covariate_id = size_t(mulcov_obj.covariate_id);
			string mulcov_type;
			CppAD::vector<subvec_info>* info_vec = DISMOD_AT_NULL_PTR;
			if( mulcov_obj.mulcov_type == meas_value_enum )
			{	info_vec    = &( group_meas_value_info_[integrand_id]) ;
				mulcov_type = "'meas_value'";
			}
			if( mulcov_obj.mulcov_type == meas_noise_enum )
			{	info_vec    = &( group_meas_noise_info_[integrand_id]) ;
				mulcov_type = "'meas_noise'";
			}
			for(size_t j = 0; j < info_vec->size(); j++)
			{	if( (*info_vec)[j].covariate_id == covariate_id )
				{	string msg = "covariate_id appears twice with "
						"mulcov_type equal to";
					msg += mulcov_type;
					string table_name = "mulcov";
					error_exit(msg, table_name, mulcov_id);
				}
			}
			size_t smooth_id = mulcov_obj.group_smooth_id;
			size_t n_age     = smooth_table[smooth_id].n_age;
			size_t n_time    = smooth_table[smooth_id].n_time;
			//
			subvec_info info;
			info.covariate_id = covariate_id;
			info.group_id     = size_t(mulcov_obj.group_id);
			info.smooth_id    = smooth_id;
			info.n_var        = n_age * n_time;
			info.offset       = offset;
			info_vec->push_back(info);
			//
			offset           += info.n_var;
		}
	}

	// group_rate_value_info_
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	for(size_t mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
	{	const mulcov_struct& mulcov_obj = mulcov_table[mulcov_id];
		bool match;
		match  = mulcov_obj.mulcov_type  == rate_value_enum;
		match &= mulcov_obj.rate_id == int(rate_id);
		match &= mulcov_obj.group_smooth_id != DISMOD_AT_NULL_INT;
		if( match )
		{	size_t covariate_id = size_t(mulcov_obj.covariate_id);
			CppAD::vector<subvec_info>& info_vec =
				group_rate_value_info_[rate_id];
			for(size_t j = 0; j < info_vec.size(); j++)
			{	if( info_vec[j].covariate_id == covariate_id )
				{	string msg = "covariate_id appears twice with "
						"mulcov_type equal to 'rate_value'";
					string table_name = "mulcov";
					error_exit(msg, table_name, mulcov_id);
				}
			}
			size_t smooth_id = mulcov_obj.group_smooth_id;
			size_t n_age     = smooth_table[smooth_id].n_age;
			size_t n_time    = smooth_table[smooth_id].n_time;
			//
			subvec_info info;
			info.covariate_id = covariate_id;
			info.group_id     = size_t(mulcov_obj.group_id);
			info.smooth_id    = smooth_id;
			info.n_var        = n_age * n_time;
			info.offset       = offset;
			info_vec.push_back(info);
			//
			offset           += info.n_var;
		}
	}

	// size is final offset
	size_ = offset;
}
// use default copy constructor
/*
-------------------------------------------------------------------------------
$begin pack_info_sizes$$
$spell
	const
$$

$section Variable Pack Info: Sizes$$

$head Syntax$$
$icode%size%              = %pack_object%.size()
%$$
$icode%integrand_size%    = %pack_object%.integrand_size()
%$$
$icode%child_size%        = %pack_object%.child_size()
%$$
$icode%smooth_size%       = %pack_object%.smooth_size()
%$$
$icode%random_size%       = %pack_object%.random_size()
%$$
$icode%group_size%        = %pack_object%.group_size()
%$$
$icode%subgroup_size%     = %pack_object%.subgroup_size(%group_id%)
%$$
$icode%first_subgroup_id% = %pack_object%.first_subgroup_id(%group_id%)
%$$

$head pack_object$$
This object has prototype
$codei%
	const pack_info %pack_object%
%$$
i.e., these member functions to not modify $icode pack_object$$.

$head size$$
Its return value has prototype
$codei%
	size_t %size%
%$$
and is the total number of variables; i.e.,
the number of elements in the packed variable vector.

$head integrand_size$$
This return value has prototype
$codei%
	size_t %integrand_size%
%$$
and is the size of the
$cref/integrand_table/get_integrand_table/integrand_table/$$.

$head child_size$$
This return value has prototype
$codei%
	size_t %child_size%
%$$
and is the number of children.

$head smooth_size$$
This return value has prototype
$codei%
	size_t %smooth_size%
%$$
and is the size of
$cref/smooth_table/get_smooth_table/smooth_table/$$.

$head random_size$$
This return value has prototype
$codei%
	size_t %random_size%
%$$
and is the number of
$cref/random effects/model_variables/Random Effects, u/$$ in the model
(counting those that are constrained with equal upper and lower limits).

$head group_size$$
This return value has prototype
$codei%
	size_t %group_size%
%$$
and is the number of groups in the $cref subgroup_table$$.

$head group_id$$
This argument has prototype
$codei%
	size_t %group_id%
%$$
and must be less than $icode group_size$$.

$head subgroup_size$$
This return value has prototype
$codei%
	size_t %subgroup_size%
%$$
and is the number of subgroups in the specified group.

$head first_subgroup_id$$
This return value has prototype
$codei%
	size_t %first_subgroup_id%
%$$
and is the index of the first subgroup in the specified group.

$end
*/


// size
size_t pack_info::size(void) const
{	return size_; }

// integrand_size
size_t pack_info::integrand_size(void) const
{	return n_integrand_; }

// child_size
size_t pack_info::child_size(void) const
{	return n_child_; }

// smooth_size
size_t pack_info::smooth_size(void) const
{	return n_smooth_; }

// random_size
size_t pack_info::random_size(void) const
{	return n_random_; }

// group_size
size_t pack_info::group_size(void) const
{	return subgroup_size_.size(); }

// subgroup_size
size_t pack_info::subgroup_size(size_t group_id) const
{	return subgroup_size_[group_id]; }

// first_subgroup_id
size_t pack_info::first_subgroup_id(size_t group_id) const
{	return first_subgroup_id_[group_id]; }


/*
------------------------------------------------------------------------------
$begin pack_info_mulstd$$
$spell
	mulstd
	dage
	dtime
	const
	dismod
$$

$section Variable Pack Info: Standard Deviation Multipliers$$

$head Syntax$$
$icode%offset% = %pack_object%.mulstd_offset(%smooth_id%, %k%)
%$$

$head pack_object$$
This object has prototype
$codei%
	const pack_info %pack_object%
%$$

$head smooth_id$$
This argument has prototype
$codei%
	size_t %smooth_id%
%$$
and is the $cref/smooth_id/smooth_table/smooth_id/$$
for the smoothing that this multiplier effects.

$head k$$
This argument has prototype
$codei%
	size_t %k%
%$$
It specifies which type of prior standard deviations that get multiplied
by this variable as follows:
$table
$icode k$$ $pre  $$ $cnext Type of prior $rnext
$code 0$$  $pre  $$ $cnext value priors for this smoothing $rnext
$code 1$$  $pre  $$ $cnext age difference priors for this smoothing $rnext
$code 2$$  $pre  $$ $cnext time difference priors for this smoothing
$tend

$subhead offset$$
The return value has prototype
$codei%
	size_t offset
%$$
and is the offset (index) in the packed variable vector
where this multiplier is located.
If $icode offset$$  has
$codei%
	DISMOD_AT_NULL_SIZE_T
%$$
for $icode%k% = 0%$$, then it has the same value for
$icode%k% = 1, 2%$$.
In this case, the multiplier has value one and is not a variable.

$head Example$$
See $cref/pack_info Example/pack_info/Example/$$.

$end

*/
size_t pack_info::mulstd_offset(size_t smooth_id, size_t k) const
{	assert( smooth_id < n_smooth_ );
	assert( k < 3 );
	return mulstd_offset_[3 * smooth_id + k];
}
/*
------------------------------------------------------------------------------
$begin pack_info_node_rate$$
$spell
	std
	cov
	var
	mulcov
	dismod
	const
	covariate
	subvec
$$

$section Variable Pack Info: Node Rates$$

$head Syntax$$
$icode%info% = %pack_object%.node_rate_value_info(%rate_id%, %j%)
%$$

$head subvec_info$$
the type $code pack_info::subvec_info$$ is defined as follows:
$srcfile%include/dismod_at/pack_info.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$

$head pack_object$$
This object has prototype
$codei%
	const pack_info %pack_object%
%$$

$head rate_id$$
This argument has prototype
$codei%
	size_t %rate_id%
%$$
and it specifies the
$cref/rate_id/rate_table/rate_id/$$ the rate values.

$head j$$
This argument has prototype
$codei%
	size_t %j%
%$$
and $icode%j% <= %n_child%$$.

$head info$$
The return value  has prototype
$codei%
	pack_info::subvec_info %info%
%$$

$subhead covariate_id$$
This field is set to null.

$subhead group_id$$
This field is set to null.

$subhead smooth_id$$
is the $cref/group_smooth_id/mulcov_table/group_smooth_id/$$ for this rate.
If $icode%j% == %n_child%$$,
this smoothing corresponds to the parent rates.
Otherwise it corresponds to the child rate effects and is the same
for all children.
If
$codei%
	%smooth_id% == DISMOD_AT_NULL_SIZE_T
%$$
then this rate is identically zero and there are no corresponding variables.

$subhead n_var$$
is the number of packed variables for this $icode rate_id$$; i.e.
the product of the number of age and number of time points in the smoothing
for this rate.

$subhead offset$$
is the offset (index) in the packed variable vector for the
specified rates:
If $icode%j% < %n_child%$$,
it is the rate vector for the $th j$$ child node.
If $icode%j% == %n_child%$$,
this is the rate vector for the
$cref/parent_node/option_table/parent_node_id/$$.
(Not specified when $icode smooth_id$$ corresponds to $code null$$).

$head Example$$
See $cref/pack_info Example/pack_info/Example/$$.

$end
*/
pack_info::subvec_info pack_info::node_rate_value_info(size_t rate_id, size_t j) const
{	assert( j <= n_child_ );
	return node_rate_value_info_[rate_id][j];
}

/*
------------------------------------------------------------------------------
$begin pack_info_group_meas$$
$spell
	std
	cov
	var
	mulcov
	dismod
	const
	covariate
	subvec
$$

$section Variable Pack Info: Group Measurement Covariate Multipliers$$

$head Syntax$$
$icode%n_cov% = %pack_object%.group_meas_value_n_cov(%integrand_id%)
%$$
$icode%n_cov% = %pack_object%.group_meas_noise_n_cov(%integrand_id%)
%$$
$icode%info% = %pack_object%.group_meas_value_info(%integrand_id%, %j%)
%$$
$icode%info% = %pack_object%.group_meas_noise_info(%integrand_id%, %j%)
%$$

$head meas_value$$
The functions
$code group_meas_value_n_cov$$ and
$code group_meas_value_info$$
return information about the measurement mean covariate multipliers.

$head meas_noise$$
The functions
$code group_meas_noise_n_cov$$ and
$code group_meas_noise_info$$
return information about the measurement noise covariate multipliers.

$head subvec_info$$
The type $code pack_info::subvec_info$$ is defined as follows:
$srcfile%include/dismod_at/pack_info.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$

$head pack_object$$
This object has prototype
$codei%
	const pack_info %pack_object%
%$$

$head integrand_id$$
This argument has prototype
$codei%
	size_t %integrand_id%
%$$
and it specifies the
$cref/integrand_id/integrand_table/integrand_id/$$ for the covariate
multipliers.

$head n_cov$$
This return value has prototype
$codei%
	size_t %n_cov%
%$$
and is the number of covariate multipliers
(rows in $cref mulcov_table$$) for the specified $icode integrand_id$$.
This is referred to as $codei%n_cov(%integrand_id%)%$$ below.

$head j$$
This argument has prototype
$codei%
	size_t %j%
%$$
and $icode%j% < n_cov(%integrand_id%)%$$.
For each fixed $icode integrand_id$$, the
$cref/mulcov_id/mulcov_table/mulcov_id/$$ index corresponding to $icode j$$
is monotone increasing with $icode j$$.

$head info$$
this return value has prototype
$codei%
	pack_info::subvec_info %info%
%$$

$subhead covariate_id$$
is the $cref/covariate_id/covariate_table/covariate_id/$$ for the
$th j$$ covariate multiplier for this $icode integrand_id$$.

$subhead group_id$$
is the $cref/group_id/mulcov_table/group_id/$$ for the
$th j$$ covariate multiplier for this $icode integrand_id$$.

$subhead smooth_id$$
is the $cref/group_smooth_id/mulcov_table/group_smooth_id/$$ for the
$th j$$ covariate multiplier for this $icode integrand_id$$.

$subhead n_var$$
is the number of variables for this covariate multiplier; i.e.
the product of the number of age and time points corresponding to
this $icode smooth_id$$.

$subhead offset$$
is the offset in the packed variable vector where the
$icode n_var$$ variables begin (for this covariate multiplier).

$head Example$$
See $cref/pack_info Example/pack_info/Example/$$.

$end
*/
size_t pack_info::group_meas_value_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return group_meas_value_info_[integrand_id].size();
}
size_t pack_info::group_meas_noise_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return group_meas_noise_info_[integrand_id].size();
}
//
pack_info::subvec_info
pack_info::group_meas_value_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return group_meas_value_info_[integrand_id][j];
}
pack_info::subvec_info
pack_info::group_meas_noise_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return group_meas_noise_info_[integrand_id][j];
}

/*
------------------------------------------------------------------------------
$begin pack_info_group_rate$$
$spell
	std
	cov
	var
	mulcov
	dismod
	const
	covariate
	subvec
$$

$section Variable Pack Info: Group Rate Covariate Multipliers$$

$head Syntax$$
$icode%n_cov% = %pack_object%.group_rate_value_n_cov(%rate_id%)
%$$
$icode%info% = %pack_object%.group_rate_value_info(%rate_id%, %j%)
%$$

$head subvec_info$$
The type $code pack_info::subvec_info$$ is defined as follows:
$srcfile%include/dismod_at/pack_info.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$

$head pack_object$$
This object has prototype
$codei%
	const pack_info %pack_object%
%$$

$head rate_id$$
This argument has prototype
$codei%
	size_t %rate_id%
%$$
and it specifies the
$cref/rate_id/rate_table/rate_id/$$ for the covariate
multipliers.

$head n_cov$$
This return value has prototype
$codei%
	size_t %n_cov%
%$$
and is the number of covariate multipliers
(rows in $cref mulcov_table$$) for the specified $icode rate_id$$.
This is referred to as $codei%n_cov(%rate_id%)%$$ below.

$head j$$
This argument has prototype
$codei%
	size_t %j%
%$$
and $icode%j% < n_cov(%rate_id%)%$$.
For each fixed $icode rate_id$$, the
$cref/mulcov_id/mulcov_table/mulcov_id/$$ index corresponding to $icode j$$
is monotone increasing with $icode j$$.

$head info$$
this return value has prototype
$codei%
	pack_info::subvec_info %info%
%$$

$subhead covariate_id$$
is the $cref/covariate_id/covariate_table/covariate_id/$$
for the $th j$$ covariate multiplier for this $icode rate_id$$.

$subhead group_id$$
is the $cref/group_id/mulcov_table/group_id/$$
for the $th j$$ covariate multiplier for this $icode rate_id$$.

$subhead smooth_id$$
is the $cref/group_smooth_id/mulcov_table/group_smooth_id/$$ for the
$th j$$ covariate multiplier for this $icode rate_id$$.

$subhead n_var$$
is the number of variables for this covariate multiplier; i.e.
the product of the number of age and time points corresponding to
this $icode smooth_id$$.

$subhead offset$$
is the offset in the packed variable vector where the
$icode n_var$$ variables begin (for this covariate multiplier).

$head Example$$
See $cref/pack_info Example/pack_info/Example/$$.

$end
*/
size_t pack_info::group_rate_value_n_cov(size_t rate_id) const
{	assert( rate_id < number_rate_enum );
	return group_rate_value_info_[rate_id].size();
}
//
pack_info::subvec_info
pack_info::group_rate_value_info(size_t rate_id, size_t j) const
{	assert( rate_id < number_rate_enum );
	return group_rate_value_info_[rate_id][j];
}
/*
------------------------------------------------------------------------------
$begin pack_info_subgroup_rate$$
$spell
	std
	cov
	var
	mulcov
	dismod
	const
	covariate
	subvec
$$

$section Variable Pack Info: Subgroup Rate Covariate Multipliers$$

$head Syntax$$
$icode%n_cov% = %pack_object%.subgroup_rate_value_n_cov(%rate_id%)
%$$
$icode%n_sub% = %pack_object%.subgroup_rate_value_n_subgroup(%rate_id%, %j%)
%$$
$icode%info% = %pack_object%.subgroup_rate_value_info(%rate_id%, %j%, %k%)
%$$

$head subvec_info$$
The type $code pack_info::subvec_info$$ is defined as follows:
$srcfile%include/dismod_at/pack_info.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$

$head pack_object$$
This object has prototype
$codei%
	const pack_info %pack_object%
%$$

$head rate_id$$
This argument has prototype
$codei%
	size_t %rate_id%
%$$
and it specifies the
$cref/rate_id/rate_table/rate_id/$$
for the covariate multipliers.

$head n_cov$$
This return value has prototype
$codei%
	size_t %n_cov%
%$$
and is the number of covariate multipliers
(rows in $cref mulcov_table$$) for the specified $icode rate_id$$.
This is referred to as $codei%n_cov(%rate_id%)%$$ below.

$head n_sub$$
This return value has prototype
$codei%
	size_t %n_sub%
%$$
and is the number of subgroups corresponding to the
$cref/group/mulcov_table/group_id/$$ for this covariate multiplier.
This is referred to as $codei%n_sub(%rate_id%, %j%)%$$ below.

$head j$$
This argument has prototype
$codei%
	size_t %j%
%$$
and $icode%j% < n_cov(%rate_id%)%$$.
For each fixed $icode rate_id$$, the
$cref/mulcov_id/mulcov_table/mulcov_id/$$ index corresponding to $icode j$$
is monotone increasing with $icode j$$.

$head k$$
This argument has prototype
$codei%
	size_t %k%
%$$
and $icode%k% < n_sub(%rate_id%, %j%)%$$.
For each fixed $icode rate_id$$ and $icode j$$, the
$cref/subgroup_id/subgroup_table/subgroup_id/$$ index corresponding to
$icode k$$ is monotone increasing with $icode k$$.

$head info$$
this return value has prototype
$codei%
	pack_info::subvec_info %info%
%$$

$subhead covariate_id$$
is the $cref/covariate_id/covariate_table/covariate_id/$$ for the
$th j$$ covariate multiplier for this $icode rate_id$$.

$subhead smooth_id$$
is the $cref/subgroup_smooth_id/mulcov_table/subgroup_smooth_id/$$ for the
$th j$$ covariate multiplier, and this $icode rate_id$$.
Note that the smoothing does not depend on $icode k$$; see
$cref/subgroup_smooth_id/mulcov_table/subgroup_smooth_id/$$.

$subhead n_var$$
is the number of variables for this covariate multiplier; i.e.
the product of the number of age and time points corresponding to
this $icode smooth_id$$.

$subhead offset$$
is the offset in the packed variable vector where the
$icode n_var$$ variables begin (for this covariate multiplier and subgroup).

$head Example$$
See $cref/pack_info Example/pack_info/Example/$$.

$end
*/
size_t pack_info::subgroup_rate_value_n_cov(size_t rate_id) const
{	assert( rate_id < number_rate_enum );
	return subgroup_rate_value_info_[rate_id].size();
}
size_t pack_info::subgroup_rate_value_n_sub(size_t rate_id, size_t j) const
{	assert( rate_id < number_rate_enum );
	return subgroup_rate_value_info_[rate_id][j].size();
}
//
pack_info::subvec_info
pack_info::subgroup_rate_value_info(size_t rate_id, size_t j, size_t k) const
{	assert( rate_id < number_rate_enum );
	return subgroup_rate_value_info_[rate_id][j][k];
}

} // END DISMOD_AT_NAMESPACE
