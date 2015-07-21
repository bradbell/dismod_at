// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/a5_double.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/log_message.hpp>
/*
$begin fit_model$$
$spell
	sqlite
	str
	num_iter
	std
	Ipopt
	tol
	vec
	const
	CppAD
	struct
	var
	dismod
$$

$section Fit the Fixed and Random Effects to the Model and Data$$

$head Syntax$$
$codei%fit_model %fit_object%(
	%db%,
	%pack_object%,
	%start_var%,
	%prior_table%,
	%s_info_vec%,
	%data_object%,
	%prior_object%
)
%$$
$codei%fit_object.run_fit(
	%tolerance_str%, %max_num_iter_str%, %print_level_str%
)%$$
$icode%solution% = %fit_object%.get_solution()
%$$

$head fit_object$$
This object has prototype
$codei%
	fit_model %fit_object%
%$$

$head db$$
This argument has prototype
$codei%
	sqlite3* %db%
%$$
and is the database connection for
$cref/logging/log_message/$$ errors and warnings.

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
and is the $cref pack_info$$ information corresponding to the
$cref/model_variables/model_variable/$$.

$head start_var$$
This vector has prototype
$codei%
	const CppAD::vector<double>& %start_var%
%$$
and is the starting $cref/model variables/model_variable/$$ in the order
specified by $cref pack_info$$.

$head prior_table$$
This argument has prototype
$codei%
	const CppAD::vector<prior_struct>&  %prior_table%
%$$
and is the $cref/prior_table/get_prior_table/prior_table/$$.

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

$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
It contains the model for the data density;
see $cref/data_model/devel_data_model/$$.

$head prior_object$$
This object has prototype
$codei%
	const prior_model %prior_object%
%$$
It contains the model for the prior negative log-likelihood; see $cref prior_model$$.

$head run_fit$$
Run the optimization process to determine the optimal fixed and random effects.

$subhead tolerance_str$$
This argument has prototype
$code
	const std::string& %tolerance_str%
%$$
and is the Ipopt relative tolerance $code tol$$ for the fit.

$subhead max_num_iter_str$$
This argument has prototype
$code
	const std::string& %max_num_iter_str%
%$$
and is the Ipopt maximum number of iterations $code max_iter$$ for the fit.

$subhead print_level_str$$
This argument has prototype
$code
	const std::string& %print_level_str%
%$$
and is the Ipopt print level (between zero and 12 inclusive).
Zero is the normal value for $code dismod_at$$ and corresponds to no printing
(five is the default Ipopt value).

$head solution$$
This return value has prototype
$codei%
	CppAD::vector<double> %solution%
%$$
It is the optimal $cref/variable values/model_variable/$$ in
$cref pack_info$$ format.

$children%example/devel/model/fit_model_xam.cpp
%$$
$head Example$$
The file $cref fit_model_xam.cpp$$ contains an example and test
of using this routine.

$end
*/

namespace dismod_at { // DISMOD_AT_BEGIN_NAMSPACE

// ===========================================================================
// public functions
// ===========================================================================
// constructor
fit_model::fit_model(
	sqlite3*                           db           ,
	const pack_info&                   pack_object  ,
	const CppAD::vector<double>&       start_var    ,
	const CppAD::vector<prior_struct>& prior_table  ,
	const CppAD::vector<smooth_info>&  s_info_vec   ,
	const data_model&                  data_object  ,
	const prior_model&                 prior_object ) :
// base class constructor
approx_mixed(
	size_fixed_effect(pack_object) , // n_fixed
	size_random_effect(pack_object)  // n_random
) ,
db_            (db)                                 ,
n_fixed_       ( size_fixed_effect(pack_object)  )  ,
n_random_      ( size_random_effect(pack_object) )  ,
pack_object_   ( pack_object )                      ,
prior_table_   ( prior_table )                      ,
s_info_vec_    ( s_info_vec  )                      ,
data_object_   ( data_object )                      ,
prior_object_  ( prior_object )
{
	//
	size_t n_var = n_fixed_ + n_random_;
	assert( pack_object_.size() == n_var );
	//
	// value_prior_
	value_prior_ = pack_value_prior(pack_object, s_info_vec);
	assert( value_prior_.size() == n_var );
	//
	// diff_prior_
	CppAD::vector<diff_prior_struct> diff_prior_tmp =
			pack_diff_prior(pack_object, s_info_vec);
	assert( diff_prior_.size() == 0 );
	double inf = std::numeric_limits<double>::infinity();
	for(size_t k = 0; k < diff_prior_tmp.size(); k++)
	{	size_t prior_id = diff_prior_tmp[k].prior_id;
		double lower    = prior_table[prior_id].lower;
		double upper    = prior_table[prior_id].upper;
		if( - inf < lower || upper < + inf )
			diff_prior_.push_back( diff_prior_tmp[k] );
	}
	//
	// var_id2fixed_
	var_id2fixed_.resize(n_var);
	for(size_t k = 0; k < n_var; k++)
		var_id2fixed_[k] = n_fixed_;
	CppAD::vector<size_t> pack_index = fixed2var_id(pack_object);
	for(size_t j = 0; j < n_fixed_; j++)
		var_id2fixed_[ pack_index[j] ] = j;
	// ---------------------------------------------------------------------
	// initialize the aprox_mixed object
	//
	// fixed_vec
	CppAD::vector<double> fixed_vec(n_fixed_);
	get_fixed_effect(pack_object_, start_var, fixed_vec);
	// random_vec
	CppAD::vector<double> random_vec(n_random_);
	get_random_effect(pack_object_, start_var, random_vec);
	//
	initialize(fixed_vec, random_vec);
}
// ---------------------------------------------------------------------------
// run_fit
void fit_model::run_fit(
	const std::string& tolerance_str    ,
	const std::string& max_num_iter_str ,
	const std::string& print_level_str  )
{	size_t n_var = n_fixed_ + n_random_;
	assert( pack_object_.size() == n_var );
	assert( value_prior_.size() == n_var );
	CppAD::vector<double> pack_vec( n_var );

	// fixed_lower
	CppAD::vector<double> fixed_lower(n_fixed_);
	for(size_t i = 0; i < n_var; i++)
		pack_vec[i] = prior_table_[ value_prior_[i] ].lower;
	get_fixed_effect(pack_object_, pack_vec, fixed_lower);

	// fixed_upper
	CppAD::vector<double> fixed_upper(n_fixed_);
	for(size_t i = 0; i < n_var; i++)
		pack_vec[i] = prior_table_[ value_prior_[i] ].upper;
	get_fixed_effect(pack_object_, pack_vec, fixed_upper);

	// constraint_lower, constraint_upper
	CppAD::vector<double> constraint_lower, constraint_upper;
	for(size_t k = 0; k < diff_prior_.size(); k++)
	{	// make sure these variable ids correspond to fixed effects
		assert( var_id2fixed_[ diff_prior_[k].plus_var_id ] < n_fixed_ );
		assert( var_id2fixed_[ diff_prior_[k].minus_var_id ] < n_fixed_ );
		//
		size_t prior_id = diff_prior_[k].prior_id;
		double lower    = prior_table_[prior_id].lower;
		double upper    = prior_table_[prior_id].upper;
		constraint_lower.push_back(lower);
		constraint_upper.push_back(upper);
	}

	// fixed_in
	CppAD::vector<double> fixed_in(n_fixed_);
	for(size_t i = 0; i < n_var; i++)
		pack_vec[i] = prior_table_[ value_prior_[i] ].mean;
	get_fixed_effect(pack_object_, pack_vec, fixed_in);

	// random_in
	CppAD::vector<double> random_in(n_random_);
	for(size_t i = 0; i < n_var; i++)
		pack_vec[i] = prior_table_[ value_prior_[i] ].mean;
	get_random_effect(pack_object_, pack_vec, random_in);

	// Ipopt options (2DO: also set options for random effects optimzation)
	std::string options =
		"String  sb                        yes\n"
		"String  derivative_test           second-order\n"
		"String  derivative_test_print_all yes\n"
	;
	options += "Numeric tol ";
	options += tolerance_str + "\n";
	options += "Integer max_iter ";
	options += max_num_iter_str + "\n";
	options += "Integer print_level ";
	options += print_level_str + "\n";
	//
	// optimal fixed effects
	CppAD::vector<double> optimal_fixed = optimize_fixed(
		options,
		fixed_lower,
		fixed_upper,
		constraint_lower,
		constraint_upper,
		fixed_in,
		random_in
	);
	//
	// optimal random effects
	CppAD::vector<double> optimal_random = optimize_random(
		optimal_fixed, random_in
	);

	// solution_
	solution_.resize( pack_object_.size() );
	put_fixed_effect(pack_object_, solution_, optimal_fixed);
	put_random_effect(pack_object_, solution_, optimal_random);
}
// ---------------------------------------------------------------------------
// get_solution
CppAD::vector<double> fit_model::get_solution(void)
{	return solution_; }
// ===========================================================================
// private functions
// ===========================================================================
// joint_like
fit_model::a5d_vector fit_model::joint_like(
	const a5d_vector& fixed_vec   ,
	const a5d_vector& random_vec  )
{	// packed vector
	a5d_vector a5_pack_vec( pack_object_.size() );
	//
	// put the fixed and random effects into pack_vec
	put_fixed_effect(pack_object_, a5_pack_vec, fixed_vec);
	put_random_effect(pack_object_, a5_pack_vec, random_vec);
	//
	// evaluate the data and prior residuals
	CppAD::vector< residual_struct<a5_double> > data_like, prior_ran;
	bool hold_out = true;
	data_like  = data_object_.like_all(hold_out, a5_pack_vec);
	prior_ran  = prior_object_.random(a5_pack_vec);
	//
	// number of data and prior residuals
	size_t n_data_like  = data_like.size();
	size_t n_prior_ran   = prior_ran.size();
	//
	// count the number of absolute value terms
	size_t n_abs = 0;
	for(size_t i = 0; i < n_data_like; i++)
	{	density_enum density = data_like[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			n_abs++;
	}
	for(size_t i = 0; i < n_prior_ran; i++)
	{	density_enum density = prior_ran[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			n_abs++;
	}
	// size joint_den
	a5d_vector joint_den(1 + n_abs);
	//
	// initialize summation of smooth part
	joint_den[0] = a5_double(0.0);
	//
	// initialize index for non-smooth part
	size_t i_abs = 0;
	//
	// data_like terms
	for(size_t i = 0; i < n_data_like; i++)
	{	joint_den[0] += data_like[i].logden_smooth;
		density_enum density = data_like[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			joint_den[1 + i_abs++] = data_like[i].logden_sub_abs;
	}
	//
	// random effects prior
	for(size_t i = 0; i < n_prior_ran; i++)
	{	joint_den[0] += prior_ran[i].logden_smooth;
		density_enum density = prior_ran[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			joint_den[1 + i_abs++] = prior_ran[i].logden_sub_abs;
	}
	// convert from log-density to negative log density
	joint_den[0] = - joint_den[0];
	//
	return joint_den;
}
// ---------------------------------------------------------------------------
// prior_like
fit_model::a1d_vector fit_model::prior_like(
	const a1d_vector& fixed_vec   )
{	// check if this temporay needs to be sized
	if( random_vec_tmp_.size() == 0 )
		random_vec_tmp_.resize( size_random_effect(pack_object_) );
	if( a1_pack_vec_tmp_.size() == 0 )
		a1_pack_vec_tmp_.resize( pack_object_.size() );
	// if( prior_fix_tmp_.size() == 0 )
	//
	// set random_vec_tmp_ to nan
	for(size_t i = 0; i < random_vec_tmp_.size(); i++)
		random_vec_tmp_[i] = CppAD::nan( a1_double(0.0) );
	//
	// put the fixed and random effects into pack_vec
	put_fixed_effect(pack_object_, a1_pack_vec_tmp_, fixed_vec);
	put_random_effect(pack_object_, a1_pack_vec_tmp_, random_vec_tmp_);
	//
	// evaluate the fixed effects prior
	// (size is determined the the first time this vector is assigned)
	prior_fix_tmp_  = prior_object_.fixed(a1_pack_vec_tmp_);
	//
	// number of data and prior residuals
	size_t n_prior_fix   = prior_fix_tmp_.size();
	//
	// check if this temporary needs to be sized
	if( prior_den_tmp_.size() == 0 )
	{	// count the number of absolute value terms
		size_t n_abs = 0;
		for(size_t i = 0; i < n_prior_fix; i++)
		{	density_enum density = prior_fix_tmp_[i].density;
			if( density == laplace_enum || density == log_laplace_enum )
				n_abs++;
		}
		// size joint_den_tmp_
		prior_den_tmp_.resize(1 + n_abs);
	}
	// initialize summation of smooth part
	prior_den_tmp_[0] = a1_double(0.0);
	//
	// initialize index for non-smooth part
	size_t i_abs = 0;
	//
	// prior_ terms
	for(size_t i = 0; i < n_prior_fix; i++)
	{	prior_den_tmp_[0] += prior_fix_tmp_[i].logden_smooth;
		density_enum density = prior_fix_tmp_[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			prior_den_tmp_[1 + i_abs++] = prior_fix_tmp_[i].logden_sub_abs;
	}
	//
	// convert from log-density to negative log density
	prior_den_tmp_[0] = - prior_den_tmp_[0];
	//
	return prior_den_tmp_;
}
// --------------------------------------------------------------------------
// constraint
fit_model::a1d_vector fit_model::constraint(const a1d_vector& fixed_vec)
{	a1d_vector ret_val( diff_prior_.size() );
	for(size_t k = 0; k < diff_prior_.size(); k++)
	{	size_t plus_var_id    = diff_prior_[k].plus_var_id;
		size_t minus_var_id   = diff_prior_[k].minus_var_id;
		size_t plus_fixed_id  = var_id2fixed_[ plus_var_id ];
		size_t minus_fixed_id = var_id2fixed_[ minus_var_id ];
		ret_val[k] = fixed_vec[plus_fixed_id] - fixed_vec[minus_fixed_id];
	}
	return ret_val;
}
// ---------------------------------------------------------------------------
// fatal_error
void fit_model::fatal_error(const std::string& error_message)
{	error_exit(db_, error_message);
}
// warning
void fit_model::warning(const std::string& warning_message)
{	std::cerr << "Warning: " << warning_message << std::endl;
	log_message(db_, "warning", warning_message);
}
} // DISMOD_AT_END_NAMESPACE
