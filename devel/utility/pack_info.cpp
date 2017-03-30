// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

/*
$begin pack_info_ctor$$
$spell
	Devel
	mulcov
	CppAD
	struct
	dismod
	var
	const
	integrands
	sqlite
$$

$section Devel Variable Packing Information: Constructor$$

$head Syntax$$
$codei%pack_info %pack_object%(
	%n_integrand%,  %n_child%, %smooth_table%, %mulcov_table%, %rate_table%
)
%$$
$codei%pack_info %pack_copy%(%pack_object%)
%$$
$icode%size%            = %pack_object%.size()
%$$
$icode%integrand_size%  = %pack_object%.integrand_size()
%$$
$icode%child_size%      = %pack_object%.child_size()
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

$head n_child$$
This argument has prototype
$codei%
	size_t %n_child%
%$$
and is the number of children; i.e., the size of
$cref/child group/node_table/parent/Child Group/$$
corresponding to the
$cref/parent_node/option_table/parent_node_id/$$.

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
Only the following fields of this table are used:
$code parent_smooth_id$$, $code child_smooth_id$$.

$head pack_copy$$
This object is a copy of the $icode pack_object$$ object
(and acts the same).

$head size$$
This function is $code const$$.
Its return value has prototype
$codei%
	size_t %size%
%$$
and is the total number of variables; i.e.,
the number of elements in the packed variable vector.

$head integrand_size$$
This function is $code const$$.
Its return value has prototype
$codei%
	size_t %integrand_size%
%$$
and is the value of $icode n_integrand$$ in the constructor.

$head child_size$$
This function is $code const$$.
Its return value has prototype
$codei%
	size_t %child_size%
%$$
and is the value of $icode n_child$$ in the constructor.

$head smooth_size$$
This function is $code const$$.
Its return value has prototype
$codei%
	size_t %smooth_size%
%$$
and is the value is $icode%smooth_table%.size()%$$ in the constructor.

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
	size_t                               n_integrand    ,
	size_t                               n_child        ,
	const CppAD::vector<smooth_struct>&  smooth_table   ,
	const CppAD::vector<mulcov_struct>&  mulcov_table   ,
	const CppAD::vector<rate_struct>&    rate_table
) :
n_smooth_       ( smooth_table.size() ) ,
n_integrand_    ( n_integrand )         ,
n_child_        ( n_child )
{	using std::string;

	// initialize offset
	size_t offset = 0;

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

	// rate_info_
	rate_info_.resize( number_rate_enum );
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	rate_info_[rate_id].resize(n_child + 1);
		for(size_t j = 0;  j <= n_child; j++)
		{	size_t smooth_id;
			if(j < n_child )
				smooth_id = rate_table[rate_id].child_smooth_id;
			else
				smooth_id = rate_table[rate_id].parent_smooth_id;
			rate_info_[rate_id][j].smooth_id = smooth_id;
			if( smooth_id == DISMOD_AT_NULL_SIZE_T )
			{	rate_info_[rate_id][j].n_var  = DISMOD_AT_NULL_SIZE_T;
				rate_info_[rate_id][j].offset = DISMOD_AT_NULL_SIZE_T;
			}
			else
			{	size_t n_age  = smooth_table[smooth_id].n_age;
				size_t n_time = smooth_table[smooth_id].n_time;
				size_t n_var  = n_age * n_time;
				rate_info_[rate_id][j].n_var     = n_var;
				rate_info_[rate_id][j].offset    = offset;
				offset += n_var;
				// check_rate_table should have checked this assumption
				assert( rate_id != pini_enum || n_age == 1 );
			}
		}
	}

	// mulcov_meas_value_info_ and mulcov_meas_std_info_
	mulcov_meas_value_info_.resize( n_integrand );
	mulcov_meas_std_info_.resize( n_integrand );
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t mulcov_id;
		for(mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
		{	bool match;
			match  = mulcov_table[mulcov_id].mulcov_type  == meas_value_enum;
			match |= mulcov_table[mulcov_id].mulcov_type  == meas_std_enum;
			match &= mulcov_table[mulcov_id].integrand_id == int(integrand_id);
			match &= mulcov_table[mulcov_id].smooth_id!=DISMOD_AT_NULL_INT;
			if( match )
			{	size_t covariate_id = size_t(
					mulcov_table[mulcov_id].covariate_id
				);
				string mulcov_type;
				CppAD::vector<subvec_info>* info_vec = DISMOD_AT_NULL_PTR;
				if( mulcov_table[mulcov_id].mulcov_type == meas_value_enum )
				{	info_vec    = &( mulcov_meas_value_info_[integrand_id]) ;
					mulcov_type = "'meas_value'";
				}
				if( mulcov_table[mulcov_id].mulcov_type == meas_std_enum )
				{	info_vec    = &( mulcov_meas_std_info_[integrand_id]) ;
					mulcov_type = "'meas_std'";
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
				size_t smooth_id = mulcov_table[mulcov_id].smooth_id;
				size_t n_age     = smooth_table[smooth_id].n_age;
				size_t n_time    = smooth_table[smooth_id].n_time;
				//
				subvec_info info;
				info.covariate_id = covariate_id;
				info.smooth_id    = smooth_id;
				info.n_var        = n_age * n_time;
				info.offset       = offset;
				info_vec->push_back(info);
				//
				offset           += info.n_var;
			}
		}
	}

	// mulcov_rate_value_info_
	mulcov_rate_value_info_.resize( number_rate_enum );
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t mulcov_id;
		for(mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
		{	bool match;
			match  = mulcov_table[mulcov_id].mulcov_type  == rate_value_enum;
			match &= mulcov_table[mulcov_id].rate_id == int(rate_id);
			match &= mulcov_table[mulcov_id].smooth_id!=DISMOD_AT_NULL_INT;
			if( match )
			{	size_t covariate_id = size_t(
					mulcov_table[mulcov_id].covariate_id
				);
				CppAD::vector<subvec_info>& info_vec =
					mulcov_rate_value_info_[rate_id];
				for(size_t j = 0; j < info_vec.size(); j++)
				{	if( info_vec[j].covariate_id == covariate_id )
					{	string msg = "covariate_id appears twice with "
							"mulcov_type equal to 'rate_value'";
						string table_name = "mulcov";
						error_exit(msg, table_name, mulcov_id);
					}
				}
				size_t smooth_id = mulcov_table[mulcov_id].smooth_id;
				size_t n_age     = smooth_table[smooth_id].n_age;
				size_t n_time    = smooth_table[smooth_id].n_time;
				//
				subvec_info info;
				info.covariate_id = covariate_id;
				info.smooth_id    = smooth_id;
				info.n_var        = n_age * n_time;
				info.offset       = offset;
				info_vec.push_back(info);
				//
				offset           += info.n_var;
			}
		}
	}

	// size is final offset
	size_ = offset;
};

// use default copy constructor

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

/*
------------------------------------------------------------------------------
$begin pack_info_mulstd$$
$spell
	Devel
	var
	mulstd
	dage
	dtime
	const
	dismod
$$

$section Devel Pack Variables: Standard Deviations Multipliers$$

$head Syntax$$
$icode%offset% = %pack_object%.mulstd_offset(%smooth_id%, %k%)
%$$
$index mulstd_offset$$

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
and is the
$cref/smooth_id/smooth_table/smooth_id/$$
for this multiplier.

$head k$$
This argument has prototype
$codei%
	size_t %k%
%$$
It specifies which type of priors standard deviations that get multiplied
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
If it has value
$codei%
	DISMOD_AT_NULL_SIZE_T
%$$
This multiplier has value one and is not a variable.

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
$begin pack_info_rate_info$$
$spell
	Devel
	std
	cov
	var
	mulcov
	dismod
	const
	covariate
	subvec
$$

$section Devel Pack Variables: Rates$$

$head Syntax$$
$icode%info% = %pack_object%.rate_info(%rate_id%, %j%)
%$$

$head subvec_info$$
the type $code pack_info::subvec_info$$ is defined as follows:
$code
$srcfile%include/dismod_at/pack_info.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$
$$

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
This field is not used or set by $code rate_info$$.

$subhead smooth_id$$
is the $cref/smooth_id/smooth_table/smooth_id/$$ for the rate.
If $icode%j% == %n_child%$$,
this smoothing corresponds to the parent rates.
Otherwise it corresponds to the child rates and is the same
for all children.
If
$codei%
	%smooth_id% == DISMOD_AT_NULL_SIZE_T
%$$
then this rate is identically zero and there are no corresponding variables.

$subhead n_var$$
is the number of packed variables for this $icode rate_id$$
and is the same for each $icode j$$.
(Not specified when $icode smooth_id$$ corresponds to $code null$$).

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
pack_info::subvec_info pack_info::rate_info(size_t rate_id, size_t j) const
{	assert( j <= n_child_ );
	return rate_info_[rate_id][j];
}

/*
------------------------------------------------------------------------------
$begin pack_info_mulcov_meas$$
$spell
	Devel
	std
	cov
	var
	mulcov
	dismod
	const
	covariate
	subvec
$$

$section Devel Pack Variables: Measurement Covariate Multipliers$$

$head Syntax$$
$icode%n_cov% = %pack_object%.mulcov_meas_value_n_cov(%integrand_id%)
%$$
$icode%n_cov% = %pack_object%.mulcov_meas_std_n_cov(%integrand_id%)
%$$
$icode%info% = %pack_object%.mulcov_meas_value_info(%integrand_id%, %j%)
%$$
$icode%info% = %pack_object%.mulcov_meas_std_info(%integrand_id%, %j%)
%$$

$head meas_value$$
The functions
$code mulcov_meas_value_n_cov$$ and
$code mulcov_meas_value_info$$
return information about the measurement mean covariate multipliers.

$head meas_std$$
The functions
$code mulcov_meas_std_n_cov$$ and
$code mulcov_meas_std_info$$
return information about the measurement standard deviation
covariate multipliers.

$head subvec_info$$
The type $code pack_info::subvec_info$$ is defined as follows:
$code
$srcfile%include/dismod_at/pack_info.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$
$$

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
and is the number of covariate multipliers for the specified
$icode integrand_id$$.
This is referred to as $codei%n_cov(%integrand_id%)%$$ below.

$head j$$
This argument has prototype
$codei%
	size_t %j%
%$$
and $icode%j% < n_cov(%integrand_id%)%$$.

$head info$$
this return value has prototype
$codei%
	pack_info::subvec_info %info%
%$$

$subhead covariate_id$$
is the $cref/covariate_id/covariate_table/covariate_id/$$ for the
$th j$$ covariate multiplier for this $icode integrand_id$$.

$subhead smooth_id$$
is the $cref/smooth_id/smooth_table/smooth_id/$$ for the
$th j$$ covariate multiplier for this $icode integrand_id$$.

$subhead n_var$$
is the number of variables for this covariate multiplier; i.e.
the product of the number are age and time points corresponding to
this $icode smooth_id$$.

$subhead offset$$
is the offset in the packed variable vector where the
$icode n_var$$ variables begin (for this covariate multiplier).

$head Example$$
See $cref/pack_info Example/pack_info/Example/$$.

$end
*/
size_t
pack_info::mulcov_meas_value_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return mulcov_meas_value_info_[integrand_id].size();
}
size_t
pack_info::mulcov_meas_std_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return mulcov_meas_std_info_[integrand_id].size();
}
//
pack_info::subvec_info
pack_info::mulcov_meas_value_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return mulcov_meas_value_info_[integrand_id][j];
}
pack_info::subvec_info
pack_info::mulcov_meas_std_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return mulcov_meas_std_info_[integrand_id][j];
}

/*
------------------------------------------------------------------------------
$begin pack_info_mulcov_rate$$
$spell
	Devel
	std
	cov
	var
	mulcov
	dismod
	const
	covariate
	subvec
$$

$section Devel Pack Variables: Rate Covariate Multipliers$$

$head Syntax$$
$icode%n_cov% = %pack_object%.mulcov_rate_value_n_cov(%rate_id%)
%$$
$icode%info% = %pack_object%.mulcov_rate_value_info(%rate_id%, %j%)
%$$

$head subvec_info$$
The type $code pack_info::subvec_info$$ is defined as follows:
$code
$srcfile%include/dismod_at/pack_info.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$
$$

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
and is the number of covariate multipliers for the specified
$icode rate_id$$.
This is referred to as $codei%n_cov(%rate_id%)%$$ below.

$head j$$
This argument has prototype
$codei%
	size_t %j%
%$$
and $icode%j% < n_cov(%rate_id%)%$$.

$head info$$
this return value has prototype
$codei%
	pack_info::subvec_info %info%
%$$

$subhead covariate_id$$
is the $cref/covariate_id/covariate_table/covariate_id/$$ for the
$th j$$ covariate multiplier for this $icode rate_id$$.

$subhead smooth_id$$
is the $cref/smooth_id/smooth_table/smooth_id/$$ for the
$th j$$ covariate multiplier for this $icode rate_id$$.

$subhead n_var$$
is the number of variables for this covariate multiplier; i.e.
the product of the number are age and time points corresponding to
this $icode smooth_id$$.

$subhead offset$$
is the offset in the packed variable vector where the
$icode n_var$$ variables begin (for this covariate multiplier).

$head Example$$
See $cref/pack_info Example/pack_info/Example/$$.

$end
*/
size_t
pack_info::mulcov_rate_value_n_cov(size_t rate_id) const
{	assert( rate_id < number_rate_enum );
	return mulcov_rate_value_info_[rate_id].size();
}
//
pack_info::subvec_info
pack_info::mulcov_rate_value_info(size_t rate_id, size_t j) const
{	assert( rate_id < number_rate_enum );
	return mulcov_rate_value_info_[rate_id][j];
}

} // END DISMOD_AT_NAMESPACE
