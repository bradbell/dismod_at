// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/a2_double.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/log_message.hpp>
/*
$begin fit_model$$
$spell
	cppad
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
$codei%fit_object.run_fit(%option_map%)
%$$
$icode%solution% = %fit_object%.get_solution()%$$

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
It contains the model for the fixed negative log-likelihood; see $cref prior_model$$.

$head run_fit$$
Run the optimization process to determine the optimal fixed and random effects.

$head option_map$$
This argument has prototype
$code
	std::map<std::string, std::string>& %option_map%
%$$
It is effectively $code const$$ and
must have the following values:
For $icode name$$ equal to
$cref/derivative_test/option_table/Optimizer/derivative_test/$$,
$cref/tolerance/option_table/Optimizer/tolerance/$$,
$cref/max_num_iter/option_table/Optimizer/max_num_iter/$$,
$cref/print_level/option_table/Optimizer/print_level/$$,
and for $icode fit$$ equal to $code fixed$$ and $code random$$
$codei%
	%option_map%["%name%_%fit%"]
%$$
is the value in the $cref option_table$$ for the corresponding option.

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
mixed_cppad(
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
void fit_model::run_fit(std::map<std::string, std::string>& option_map)
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

	// Ipopt fixed effects optimization options
	std::string options = "";
	options += "String  sb  yes";
	options += "\nNumeric tol " + option_map["tolerance_fixed"];
	options += "\nInteger max_iter " + option_map["max_num_iter_fixed"];
	options += "\nInteger print_level " + option_map["print_level_fixed"];
	options += "\nString derivative_test "
		+ option_map["derivative_test_fixed"] + "\n";
	std::string fixed_options = options;
	// Ipopt random effects optimization options
	options = "";
	options += "String  sb  yes";
	options += "\nNumeric tol " + option_map["tolerance_random"];
	options += "\nInteger max_iter " + option_map["max_num_iter_random"];
	options += "\nInteger print_level " + option_map["print_level_random"];
	options += "\nString derivative_test "
		+ option_map["derivative_test_random"] + "\n";
	std::string random_options = options;
	//
	// optimal fixed effects
	CppAD::vector<double> optimal_fixed = optimize_fixed(
		fixed_options,
		random_options,
		fixed_lower,
		fixed_upper,
		constraint_lower,
		constraint_upper,
		fixed_in,
		random_in
	);
	// store solution_
	solution_.resize( pack_object_.size() );
	put_fixed_effect(pack_object_, solution_, optimal_fixed);
	if( n_random_ > 0 )
	{	// corresponding optimal random effects
		CppAD::vector<double> optimal_random = optimize_random(
			random_options, optimal_fixed, random_in
		);
		put_random_effect(pack_object_, solution_, optimal_random);
	}
}
// ---------------------------------------------------------------------------
// get_solution
CppAD::vector<double> fit_model::get_solution(void)
{	return solution_; }
// ===========================================================================
// private functions
// ===========================================================================
// ran_like
fit_model::a2d_vector fit_model::ran_like(
	const a2d_vector& fixed_vec   ,
	const a2d_vector& random_vec  )
{	// packed vector
	a2d_vector a2_pack_vec( pack_object_.size() );
	//
	// put the fixed and random effects into pack_vec
	put_fixed_effect(pack_object_, a2_pack_vec, fixed_vec);
	put_random_effect(pack_object_, a2_pack_vec, random_vec);
	//
	// evaluate the data and prior residuals
	CppAD::vector< residual_struct<a2_double> > data_like, prior_ran;
	bool hold_out = true;
	bool parent   = false;
	data_like  = data_object_.like_all(hold_out, parent, a2_pack_vec);
	prior_ran  = prior_object_.random(a2_pack_vec);
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
	// size ran_den
	a2d_vector ran_den(1 + n_abs);
	//
	// initialize summation of smooth part
	ran_den[0] = a2_double(0.0);
	//
	// initialize index for non-smooth part
	size_t i_abs = 0;
	//
	// data_like terms
	for(size_t i = 0; i < n_data_like; i++)
	{	ran_den[0] += data_like[i].logden_smooth;
		density_enum density = data_like[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			ran_den[1 + i_abs++] = data_like[i].logden_sub_abs;
	}
	//
	// random effects prior
	for(size_t i = 0; i < n_prior_ran; i++)
	{	ran_den[0] += prior_ran[i].logden_smooth;
		density_enum density = prior_ran[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			ran_den[1 + i_abs++] = prior_ran[i].logden_sub_abs;
	}
	// convert from log-density to negative log density
	ran_den[0] = - ran_den[0];
	//
	return ran_den;
}
// ---------------------------------------------------------------------------
// fix_like
fit_model::a1d_vector fit_model::fix_like(
	const a1d_vector& fixed_vec   )
{	// local vectors
	a1d_vector random_vec( size_random_effect(pack_object_) );
	a1d_vector a1_pack_vec( pack_object_.size() );
	//
	// set random_vec to nan (not used)
	for(size_t i = 0; i < random_vec.size(); i++)
		random_vec[i] = CppAD::nan( a1_double(0.0) );
	//
	// put the fixed and random effects into pack_vec
	put_fixed_effect(pack_object_, a1_pack_vec, fixed_vec);
	put_random_effect(pack_object_, a1_pack_vec, random_vec);
	//
	// evaluate prior residuals (data residuals empty for now)
	CppAD::vector< residual_struct<a1_double> > data_like, prior_fix;
	bool hold_out = true;
	bool parent   = true;
	data_like     = data_object_.like_all(hold_out, parent, a1_pack_vec);
	prior_fix     = prior_object_.fixed(a1_pack_vec);
	//
	// number of data and prior residuals
	size_t n_data_like   = data_like.size();
	size_t n_prior_fix   = prior_fix.size();
	//
	// count the number of absolute value terms
	size_t n_abs = 0;
	for(size_t i = 0; i < n_data_like; i++)
	{	density_enum density = data_like[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			n_abs++;
	}
	for(size_t i = 0; i < n_prior_fix; i++)
	{	density_enum density = prior_fix[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			n_abs++;
	}
	// size prior_den
	a1d_vector prior_den(1 + n_abs);
	//
	// initialize summation of smooth part
	prior_den[0] = a1_double(0.0);
	//
	// initialize index for non-smooth part
	size_t i_abs = 0;
	//
	// data_like terms
	for(size_t i = 0; i < n_data_like; i++)
	{	prior_den[0] += data_like[i].logden_smooth;
		density_enum density = data_like[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			prior_den[1 + i_abs++] = data_like[i].logden_sub_abs;
	}
	//
	// fixed effects prior
	for(size_t i = 0; i < n_prior_fix; i++)
	{	prior_den[0] += prior_fix[i].logden_smooth;
		density_enum density = prior_fix[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			prior_den[1 + i_abs++] = prior_fix[i].logden_sub_abs;
	}
	//
	// convert from log-density to negative log density
	prior_den[0] = - prior_den[0];
	//
	return prior_den;
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
