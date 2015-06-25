// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

/*
$begin devel_pack_info_ctor$$
$spell
	Devel
	mulcov
	CppAD
	struct
	dismod
	var
	const
	integrands
$$

$section Devel Variable Packing Information: Constructor$$

$head Syntax$$
$codei%pack_info %pack_object%(
	%n_integrand%,  %n_child%,
	%smooth_table%, %mulcov_table%, %rate_table%
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
If $cref/mulcov_table/devel_pack_info_ctor/mulcov_table/$$ has size zero,
then $icode n_integrand$$ can be zero (a case used for testing purposes).

$head n_child$$
This argument has prototype
$codei%
	size_t %n_child%
%$$
and is the number of children; i.e., the size of
$cref/child group/node_table/parent/Child Group/$$
corresponding to the
$cref/parent_node/fit_command/parent_node_id/$$.

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
See $cref/pack_info Example/devel_pack_info/Example/$$.

$end
*/

# include <cppad/cppad.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/table_error_exit.hpp>
# include <dismod_at/to_string.hpp>

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
	mulstd_offset_  = offset; offset += 3 * n_smooth_;

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
			size_t n_age  = smooth_table[smooth_id].n_age;
			size_t n_time = smooth_table[smooth_id].n_time;
			size_t n_var  = n_age * n_time;
			rate_info_[rate_id][j].smooth_id = smooth_id;
			rate_info_[rate_id][j].n_var     = n_var;
			rate_info_[rate_id][j].offset    = offset;
			offset += n_var;
			//
			// check_rate_table should already have checked this assumption
			assert( rate_id != pini_enum || n_age == 1 );
		}
	}

	// meas_mean_mulcov_info_ and meas_std_mulcov_info_
	meas_mean_mulcov_info_.resize( n_integrand );
	meas_std_mulcov_info_.resize( n_integrand );
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t mulcov_id;
		for(mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
		{	bool match;
			match  = mulcov_table[mulcov_id].mulcov_type  == meas_mean_enum;
			match |= mulcov_table[mulcov_id].mulcov_type  == meas_std_enum;
			match &= mulcov_table[mulcov_id].integrand_id == int(integrand_id);
			if( match )
			{	size_t covariate_id = size_t(
					mulcov_table[mulcov_id].covariate_id
				);
				string mulcov_type;
				CppAD::vector<subvec_info>* info_vec = nullptr;
				if( mulcov_table[mulcov_id].mulcov_type == meas_mean_enum )
				{	info_vec    = &( meas_mean_mulcov_info_[integrand_id]) ;
					mulcov_type = "'meas_mean'";
				}
				if( mulcov_table[mulcov_id].mulcov_type == meas_std_enum )
				{	info_vec    = &( meas_std_mulcov_info_[integrand_id]) ;
					mulcov_type = "'meas_std'";
				}
				for(size_t j = 0; j < info_vec->size(); j++)
				{	if( (*info_vec)[j].covariate_id == covariate_id )
					{	string msg = "covariate_id appears twice with "
							"mulcov_type equal to";
						msg += mulcov_type;
						string table_name = "mulcov";
						table_error_exit(table_name, mulcov_id, msg);
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

	// rate_mean_mulcov_info_
	rate_mean_mulcov_info_.resize( number_rate_enum );
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t mulcov_id;
		for(mulcov_id = 0; mulcov_id < mulcov_table.size(); mulcov_id++)
		{	bool match;
			match  = mulcov_table[mulcov_id].mulcov_type  == rate_mean_enum;
			match &= mulcov_table[mulcov_id].rate_id == int(rate_id);
			if( match )
			{	size_t covariate_id = size_t(
					mulcov_table[mulcov_id].covariate_id
				);
				CppAD::vector<subvec_info>& info_vec =
					rate_mean_mulcov_info_[rate_id];
				for(size_t j = 0; j < info_vec.size(); j++)
				{	if( info_vec[j].covariate_id == covariate_id )
					{	string msg = "covariate_id appears twice with "
							"mulcov_type equal to 'rate_mean'";
						string table_name = "mulcov";
						table_error_exit(table_name, mulcov_id, msg);
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
$begin devel_pack_info_mulstd$$
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
$icode%offset% = %pack_object%.mulstd_offset(%smooth_id%)%$$

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

$subhead offset$$
The return value has prototype
$codei%
	size_t offset
%$$
and is the offset (index) in the packed variable vector
where the three variables for this smoothing begin.
The three variables for each smoothing are the
value, dage, and dtime standard deviation multipliers
(and should always be used in that order).

$head Example$$
See $cref/pack_info Example/devel_pack_info/Example/$$.

$end

*/
size_t pack_info::mulstd_offset(size_t smooth_id) const
{	assert( smooth_id < n_smooth_ );
	return mulstd_offset_ + 3 * smooth_id;
}
/*
------------------------------------------------------------------------------
$begin devel_pack_info_rate$$
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
$verbatim%include/dismod_at/pack_info.hpp
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

$subhead n_var$$
is the number of packed variables for this $icode rate_id$$
and is the same for each $icode j$$.

$subhead offset$$
is the offset (index) in the packed variable vector for the
specified rates:
If $icode%j% < %n_child%$$,
it is the rate vector for the $th j$$ child node.
If $icode%j% == %n_child%$$,
this is the rate vector for the
$cref/parent_node/fit_command/parent_node_id/$$.

$head Example$$
See $cref/pack_info Example/devel_pack_info/Example/$$.

$end
*/
pack_info::subvec_info pack_info::rate_info(size_t rate_id, size_t j) const
{	assert( j <= n_child_ );
	return rate_info_[rate_id][j];
}

/*
------------------------------------------------------------------------------
$begin devel_pack_info_meas_mulcov$$
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
$icode%n_cov% = %pack_object%.meas_mean_mulcov_n_cov(%integrand_id%)
%$$
$icode%n_cov% = %pack_object%.meas_std_mulcov_n_cov(%integrand_id%)
%$$
$icode%info% = %pack_object%.meas_mean_mulcov_info(%integrand_id%, %j%)
%$$
$icode%info% = %pack_object%.meas_std_mulcov_info(%integrand_id%, %j%)
%$$

$head meas_mean$$
The functions
$code meas_mean_mulcov_n_cov$$ and
$code meas_mean_mulcov_info$$
return information about the measurement mean covariate multipliers.

$head meas_std$$
The functions
$code meas_std_mulcov_n_cov$$ and
$code meas_std_mulcov_info$$
return information about the measurement standard deviation
covariate multipliers.

$head subvec_info$$
The type $code pack_info::subvec_info$$ is defined as follows:
$code
$verbatim%include/dismod_at/pack_info.hpp
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
$th j$$ covariate multiplier fro this $icode integrand_id$$.

$subhead n_var$$
is the number of variables for this covariate multiplier; i.e.
the product of the number are age and time points corresponding to
this $icode smooth_id$$.

$subhead offset$$
is the offset in the packed variable vector where the
$icode n_var$$ variables begin (for this covariate multiplier).

$head Example$$
See $cref/pack_info Example/devel_pack_info/Example/$$.

$end
*/
size_t
pack_info::meas_mean_mulcov_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return meas_mean_mulcov_info_[integrand_id].size();
}
size_t
pack_info::meas_std_mulcov_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return meas_std_mulcov_info_[integrand_id].size();
}
//
pack_info::subvec_info
pack_info::meas_mean_mulcov_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return meas_mean_mulcov_info_[integrand_id][j];
}
pack_info::subvec_info
pack_info::meas_std_mulcov_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return meas_std_mulcov_info_[integrand_id][j];
}

/*
------------------------------------------------------------------------------
$begin devel_pack_info_rate_mulcov$$
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
$icode%n_cov% = %pack_object%.rate_mean_mulcov_n_cov(%rate_id%)
%$$
$icode%info% = %pack_object%.rate_mean_mulcov_info(%rate_id%, %j%)
%$$

$head subvec_info$$
The type $code pack_info::subvec_info$$ is defined as follows:
$code
$verbatim%include/dismod_at/pack_info.hpp
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
$th j$$ covariate multiplier fro this $icode rate_id$$.

$subhead n_var$$
is the number of variables for this covariate multiplier; i.e.
the product of the number are age and time points corresponding to
this $icode smooth_id$$.

$subhead offset$$
is the offset in the packed variable vector where the
$icode n_var$$ variables begin (for this covariate multiplier).

$head Example$$
See $cref/pack_info Example/devel_pack_info/Example/$$.

$end
*/
size_t
pack_info::rate_mean_mulcov_n_cov(size_t rate_id) const
{	assert( rate_id < number_rate_enum );
	return rate_mean_mulcov_info_[rate_id].size();
}
//
pack_info::subvec_info
pack_info::rate_mean_mulcov_info(size_t rate_id, size_t j) const
{	assert( rate_id < number_rate_enum );
	return rate_mean_mulcov_info_[rate_id][j];
}
/*
$begin devel_pack_info_variable_name$$
$spell
	vec
	const
	struct
	CppAD
	std
	covariate
	mulcov
$$

$section Human Readable Name for a Model Variable$$

$head Syntax$$
$icode%name% = %pack_object%.variable_name(
	%index%,
	%parent_node_id%,
	%age_table%,
	%covariate_table%,
	%integrand_table%,
	%mulcov_table%,
	%node_table%
	%smooth_table%,
	%time_table%
	%s_info_vec%,
	%child_object%
)%$$

$head index$$
The argument $icode index$$ has prototype
$codei%
	size_t %index%
%$$
and is its index for this model variable in a packed vector.

$subhead parent_node_id$$
This argument has prototype
$codei%
	size_t %parent_node_id%
%$$
and is the
$cref/parent_node_id/fit_command/parent_node_id/$$.
for the fit command.

$head age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>&  %age_table%
%$$
and is the $cref/age_table/get_age_table/age_table/$$.

$head covariate_table$$
This argument has prototype
$codei%
	const CppAD::vector<covariate_struct>& %covariate_table%
%$$
and is the
$cref/covariate_table/get_covariate_table/covariate_table/$$.

$head integrand_table$$
This argument has prototype
$codei%
	const CppAD::vector<integrand_struct>& %integrand_table%
%$$
and is the
$cref/integrand_table/get_integrand_table/integrand_table/$$.

$head mulcov_table$$
This argument has prototype
$codei%
	const CppAD::vector<mulcov_struct>& %mulcov_table%
%$$
and is the
$cref/mulcov_table/get_mulcov_table/mulcov_table/$$.

$head node_table$$
This argument has prototype
$codei%
	const CppAD::vector<node_struct>& %node_table%
%$$
and is the $cref/node_table/get_node_table/node_table/$$.

$head smooth_table$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_struct>& %smooth_table%
%$$
and is the
$cref/smooth_table/get_smooth_table/smooth_table/$$.

$head time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>&  %time_table%
%$$
and is the $cref/time_table/get_time_table/time_table/$$.

$head s_info_vec$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_info>& %s_info_vec%
%$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%s_info_vec%[ %smooth_id% ]
%$$
is the corresponding $cref smooth_info$$ information.

$head child_object$$
This argument has prototype
$codei%
	const child_info& %child_object%
%$$
and is the $cref child_info$$ information corresponding to
parent node, node table, and data table.

$head name$$
The return value has prototype
$codei%
	std::string %name%
%$$
and is a human readable name for the model variable.

$childtable%example/devel/utility/variable_name_xam.cpp
%$$
$head Example$$
The file $cref variable_name_xam.cpp$$ contains
and example and test of $code pack_info$$.
It returns true for success and false for failure.

$end
*/
std::string
pack_info::variable_name(
	size_t                                 index           ,
	size_t                                 parent_node_id  ,
	const CppAD::vector<double>&           age_table       ,
	const CppAD::vector<covariate_struct>& covariate_table ,
	const CppAD::vector<integrand_struct>& integrand_table ,
	const CppAD::vector<mulcov_struct>&    mulcov_table    ,
	const CppAD::vector<node_struct>&      node_table      ,
	const CppAD::vector<smooth_struct>&    smooth_table    ,
	const CppAD::vector<double>&           time_table      ,
	const CppAD::vector<smooth_info>&      s_info_vec      ,
	const child_info&                      child_object    ) const
{	using std::string;
	std::string name;
	// see get_rate_table.cpp where this table also appears
	const char* rate_id2name[] = {
		"pini", "iota", "rho", "chi", "omega"
	};
	// see get_integrand_table.cpp were this table also appears
	const char* integrand_enum2name[] = {
		"incidence",
		"remission",
		"mtexcess",
		"mtother",
		"mtwith",
		"prevalence",
		"mtspecific",
		"mtall",
		"mtstandard",
		"relrisk"
	};
	//
	size_t base = 0;
	//
	// mulstd case
	size_t n_mulstd = 3 * n_smooth_;
	if( index < base + n_mulstd )
	{	size_t smooth_id = index / 3;
		//
		switch( index % 3 )
		{	case 0:
			name += "value_mulstd(";
			break;

			case 1:
			name += "dage_mulstd(";
			break;

			case 2:
			name += "dtime_mulstd(";
			break;
		}
		name += smooth_table[smooth_id].smooth_name;
		name += ")";
		return name;
	}
	base += n_mulstd;
	//
	// rate case
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	for(size_t j = 0; j <= n_child_; j++)
		{	size_t n_var = rate_info_[rate_id][j].n_var;
			if( index < base + n_var )
			{	name  = rate_id2name[rate_id];
				//
				name += "(";
				size_t node_id;
				if( j == n_child_ )
					node_id = parent_node_id;
				else
					node_id = child_object.child_id2node_id(j);
				name += node_table[node_id].node_name;
				name += ";";
				size_t smooth_id = rate_info_[rate_id][j].smooth_id;
				size_t offset    = rate_info_[rate_id][j].offset;
				size_t n_age     = s_info_vec[smooth_id].age_size();
				size_t n_time    = s_info_vec[smooth_id].time_size();
				assert( n_var == n_age * n_time );
				size_t i         = (index - offset) % n_age;
				size_t k         = (index - offset) / n_age;
				size_t age_id    = s_info_vec[smooth_id].age_id(i);
				size_t time_id   = s_info_vec[smooth_id].time_id(k);
				name            += to_string( age_table[age_id] );
				name            += ";";
				name            += to_string( time_table[time_id] );
				name            +=")";
				//
				return name;
			}
			base += n_var;
		}
	}
	//
	// measurement covariate cases
	size_t n_mulcov    = mulcov_table.size();
	size_t n_integrand = integrand_table.size();
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t mean_count = 0;
		size_t std_count  = 0;
		for(size_t mulcov_id = 0; mulcov_id < n_mulcov; mulcov_id++)
		{	bool match;
			match  = mulcov_table[mulcov_id].mulcov_type  == meas_mean_enum;
			match |= mulcov_table[mulcov_id].mulcov_type  == meas_std_enum;
			match &= mulcov_table[mulcov_id].integrand_id == int(integrand_id);
			if( match )
			{	subvec_info info;
				if( mulcov_table[mulcov_id].mulcov_type == meas_mean_enum )
				{	info = meas_mean_mulcov_info_[integrand_id][mean_count++];
					name = "mean_mulcov(";
				}
				else
				{	info = meas_mean_mulcov_info_[integrand_id][std_count++];
					name = "std_mulcov(";
				}
				size_t n_var  = info.n_var;
				if( index < base + n_var )
				{	size_t covariate_id = info.covariate_id;
					size_t smooth_id    = info.smooth_id;
					size_t offset       = info.offset;
					size_t n_age        = s_info_vec[smooth_id].age_size();
					size_t n_time       = s_info_vec[smooth_id].time_size();
					assert( n_var == n_age * n_time );
					size_t i            = (index - offset) % n_age;
					size_t k            = (index - offset) / n_age;
					name += covariate_table[covariate_id].covariate_name;
					name += ";";
					name += integrand_enum2name[
						integrand_table[integrand_id].integrand
					];
					name += ";";
					size_t age_id    = s_info_vec[smooth_id].age_id(i);
					size_t time_id   = s_info_vec[smooth_id].time_id(k);
					name            += to_string( age_table[age_id] );
					name            += ";";
					name            += to_string( time_table[time_id] );
					name            +=")";
					//
					return name;
				}
				base += n_var;
			}
		}
	}
	name = "";
	return name;
}

} // END DISMOD_AT_NAMESPACE
