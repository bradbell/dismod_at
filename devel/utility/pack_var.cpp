// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

/*
$begin pack_var_ctor$$
$spell
	pini
	mulcov
	CppAD
	struct
	dismod
	var
	const
	integrands
$$

$section Pack Variables: Constructor$$

$head Syntax$$
$codei%pack_var %var_info%(
	%n_integrand%,  %n_child%,      %pini_smooth_id%,
	%smooth_table%, %mulcov_table%, %rate_table%
)
%$$
$icode%size%  = %var_info%.size()
%$$

$head n_integrand$$
This argument has prototype
$codei%
	size_t %n_integrand%
%$$
and is the number of integrands; i.e., the size of
$cref/integrand_table/get_integrand_table/integrand_table/$$.

$head n_child$$
This argument has prototype
$codei%
	size_t %n_child%
%$$
and is the number of children; i.e., the size of
$cref/child group/node_table/parent/Child Group/$$
corresponding to the
$cref/parent_node/run_table/parent_node_id/$$.
In the special case where there is only one child,
$icode n_child$$ must be zero.
In this case, only the parent rates are estimated.

$head pini_smooth_id$$
This argument has prototype
$codei%
	size_t %pini_smooth_id%
%$$
and is the smoothing id for the
$cref/pini_smooth_id/run_table/pini_smooth_id/$$.

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

$head size$$
This function is $code const$$.
Its return value has prototype
$codei%
	size_t %size%
%$$
and is the total number of variables; i.e.,
the number of elements in the packed variable vector.

$head Example$$
See $cref/pack_var Example/pack_var/Example/$$.

$end
*/

# include <cppad/cppad.hpp>
# include <dismod_at/include/pack_var.hpp>
# include <dismod_at/include/table_error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

pack_var::pack_var(
	size_t                               n_integrand    ,
	size_t                               n_child        ,
	size_t                               pini_smooth_id ,
	const CppAD::vector<smooth_struct>&  smooth_table   ,
	const CppAD::vector<mulcov_struct>&  mulcov_table   ,
	const CppAD::vector<rate_struct>&    rate_table 
) :
n_smooth_       ( smooth_table.size() ) ,
n_integrand_    ( n_integrand )         ,
n_child_        ( n_child )
{	using std::string;

	// check assumption
	assert( n_child != 1 );

	// initialize offset
	size_t offset = 0;

	// pini_info_
	if( smooth_table[pini_smooth_id].n_age != 1 )
	{	string message = "run table: the smoothing corresponding to initial"
		"\nprevalence does not have n_age equal to 1";
		std::cerr << message << std::endl;
		exit(1);
	}
	pini_info_.smooth_id = pini_smooth_id;
	pini_info_.n_var    = smooth_table[pini_smooth_id].n_time;
	pini_info_.offset   = offset; offset += pini_info_.n_var;

	// mulstd_offset_
	mulstd_offset_  = offset; offset += 3 * n_smooth_;

	// rate_offset
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
			if( j < n_child || n_child == 0 )
			{	rate_info_[rate_id][j].offset  = offset;
				offset += n_var;
			}
			else
			{	// set to a value that shouldi not be used
				rate_info_[rate_id][j].offset  = size_t(-1);
			}
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

// size
size_t pack_var::size(void) const
{	return size_; }
/*
------------------------------------------------------------------------------
$begin pack_var_pini$$
$spell
	var
	pini
	dage
	dtime
	const
	dismod
	pinitial
$$

$section Pack Variables: Initial Prevalence Information$$
$spell
	subvec
	covariate
$$

$head Syntax$$
$icode%info% = %var_info%.pini_info()%$$

$head subvec_info$$
the type $code pack_var::subvec_info$$ is defined as follows:
$code
$verbatim%dismod_at/include/pack_var.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$
$$

$head var_info$$
This object has prototype
$codei%
	const pack_var %var_info%
%$$

$head info$$
The return value  has prototype
$codei%
	subvec_info %info%
%$$

$subhead covariate_id$$
This field is not used or set by $code pini_info$$.

$subhead smooth_id$$
is the $cref/smooth_id/smooth_table/smooth_id/$$ for the
initial prevalence variables;  i.e.,
$cref/pini_smooth_id/run_table/pini_smooth_id/$$.

$subhead n_var$$
is the number of initial prevalence variables; i.e.
the number of time points corresponding to the
$cref/pini_smooth_id/run_table/pini_smooth_id/$$ smoothing
(the number of age points for this smoothing is one).

$subhead offset$$
The return value has prototype
$codei%
	size_t offset
%$$
and is the offset (index) in the packed variable vector
where the variables for the initial prevalence begin.

$head Example$$
See $cref/pack_var Example/pack_var/Example/$$.

$end

*/
pack_var::subvec_info pack_var::pini_info(void) const
{	return pini_info_; }

/*
------------------------------------------------------------------------------
$begin pack_var_mulstd$$
$spell
	var
	mulstd
	dage
	dtime
	const
	dismod
$$

$section Pack Variables: Standard Deviations Multipliers$$

$head Syntax$$
$icode%offset% = %var_info%.mulstd_offset(%smooth_id%)
%$$

$head var_info$$
This object has prototype
$codei%
	const pack_var %var_info%
%$$

$head smooth_id$$
This argument has prototype
$codei%
	size_t %smooth_id%
%$$
and is the 
$cref/smooth_id/smooth_table/smooth_id/$$.

$subhead offset$$
The return value has prototype
$codei%
	size_t offset
%$$
and is the offset (index) in the packed variable vector
where the three variables for this smoothing begin.
The three variables for each smoothing are the
value, dage, and dtime standard deviation multipliers.

$head Example$$
See $cref/pack_var Example/pack_var/Example/$$.

$end

*/
size_t pack_var::mulstd_offset(size_t smooth_id) const
{	assert( smooth_id < n_smooth_ );
	return mulstd_offset_ + 3 * smooth_id;
}
/*
------------------------------------------------------------------------------
$begin pack_var_rate$$
$spell
	std
	cov
	var
	mulcov
	dismod
	const
	covariate
$$

$section Pack Variables: Rates$$
$spell
	subvec
$$

$head Syntax$$
$icode%info% = %var_info%.rate_info(%rate_id%, %j%)
%$$

$head subvec_info$$
the type $code pack_var::subvec_info$$ is defined as follows:
$code
$verbatim%dismod_at/include/pack_var.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$
$$

$head var_info$$
This object has prototype
$codei%
	const pack_var %var_info%
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
	subvec_info %info%
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
If $icode%j% == %n_child% == 0%$$,
this is the rate vector for the
$cref/parent_node/run_table/parent_node_id/$$.
If $icode%j% == %n_child% > 0%$$,
this offset is not defined (and should not be used).

$head Example$$
See $cref/pack_var Example/pack_var/Example/$$.

$end
*/
pack_var::subvec_info pack_var::rate_info(size_t rate_id, size_t j) const
{	assert( j <= n_child_ );
	return rate_info_[rate_id][j];
}

/*
------------------------------------------------------------------------------
$begin pack_var_meas_mulcov$$
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

$section Pack Variables: Measurement Multipliers$$

$head Syntax$$
$icode%n_cov% = %var_info%.meas_mean_mulcov_n_cov(%integrand_id%)
%$$
$icode%n_cov% = %var_info%.meas_std_mulcov_n_cov(%integrand_id%)
%$$
$icode%info% = %var_info%.meas_mean_mulcov_info(%integrand_id%, %j%)
%$$
$icode%info% = %var_info%.meas_std_mulcov_info(%integrand_id%, %j%)
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
The type $code pack_var::subvec_info$$ is defined as follows:
$code
$verbatim%dismod_at/include/pack_var.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$
$$

$head var_info$$
This object has prototype
$codei%
	const pack_var %var_info%
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
	subvec_info %info%
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
See $cref/pack_var Example/pack_var/Example/$$.

$end
*/
size_t 
pack_var::meas_mean_mulcov_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return meas_mean_mulcov_info_[integrand_id].size();
}
size_t 
pack_var::meas_std_mulcov_n_cov(size_t integrand_id) const
{	assert( integrand_id < n_integrand_ );
	return meas_std_mulcov_info_[integrand_id].size();
}
//
pack_var::subvec_info 
pack_var::meas_mean_mulcov_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return meas_mean_mulcov_info_[integrand_id][j];
}
pack_var::subvec_info 
pack_var::meas_std_mulcov_info(size_t integrand_id, size_t j) const
{	assert( integrand_id < n_integrand_ );
	return meas_std_mulcov_info_[integrand_id][j];
}

/*
------------------------------------------------------------------------------
$begin pack_var_rate_mulcov$$
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

$section Pack Variables: Rate Multipliers$$

$head Syntax$$
$icode%n_cov% = %var_info%.rate_mean_mulcov_n_cov(%rate_id%)
%$$
$icode%info% = %var_info%.rate_mean_mulcov_info(%rate_id%, %j%)
%$$

$head subvec_info$$
The type $code pack_var::subvec_info$$ is defined as follows:
$code
$verbatim%dismod_at/include/pack_var.hpp
%5%// BEGIN SUBVEC_INFO%// END SUBVEC_INFO%$$
$$

$head var_info$$
This object has prototype
$codei%
	const pack_var %var_info%
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
	subvec_info %info%
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
See $cref/pack_var Example/pack_var/Example/$$.

$end
*/
size_t 
pack_var::rate_mean_mulcov_n_cov(size_t rate_id) const
{	assert( rate_id < number_rate_enum );
	return rate_mean_mulcov_info_[rate_id].size();
}
//
pack_var::subvec_info 
pack_var::rate_mean_mulcov_info(size_t rate_id, size_t j) const
{	assert( rate_id < number_rate_enum );
	return rate_mean_mulcov_info_[rate_id][j];
}


} // END DISMOD_AT_NAMESPACE
