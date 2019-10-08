/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/a1_double.hpp>
# include <dismod_at/fit_model.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/n_random_const.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/remove_const.hpp>
# include <dismod_at/get_var_limits.hpp>
# include <dismod_at/ran_con_rcv.hpp>

namespace dismod_at { // DISMOD_AT_BEGIN_NAMSPACE
/*
$begin fit_model_ctor$$
$spell
	var
	vec
	const
	enum
	stderr
$$

$section Fit Model Constructor$$

$head Syntax$$
$codei%fit_model %fit_object%(
	%db%,
	%simulate_index%,
	%warn_on_stderr%,
	%bound_random%,
	%no_scaling%,
	%pack_object%,
	%var2prior%,
	%start_var%,
	%scale_var%,
	%prior_table%,
	%data_object%,
	%prior_object%,
	%quasi_fixed%,
	%zero_sum_random%
)
%$$

$head fit_object$$
The $code fit_model$$ object being constructed.

$head db$$
This argument is the database connection for
$cref/logging/log_message/$$ errors and warnings.

$head simulate_index$$
Is the $cref/simulate_index/data_sim_table/simulate_index/$$
when simulated data.
This index is $code -1$$ when we are fitting the
$cref/meas_value/data_table/meas_value/$$ column in the data table.

$head warn_on_stderr$$
If true, warnings will be printed on stderr.
(Error messages are always printed on stderr.)

$head bound_random$$
This is the value of the
$cref/bound_random/option_table/Random Only/bound_random/$$
in the option table.

$head no_scaling$$
If this argument is true, no
$cref/scaling/prior_table/eta/Scaling Fixed Effects/$$ is done.

$head pack_object$$
This argument is the $cref pack_info$$ information corresponding to the
$cref model_variables$$.

$head pack_prior$$
This argument is the $cref pack_prior$$ information corresponding to the
$cref model_variables$$.

$head var2prior$$
Mapping from $icode var_id$$ to prior information.

$head start_var$$
This vector is the starting $cref model_variables$$ in the order
specified by $cref pack_info$$.

$head scale_var$$
The object and constraints are scaled using this value for the
$cref model_variables$$.

$head prior_table$$
This argument is the $cref/prior_table/get_prior_table/prior_table/$$.

$head prior_object$$
This object contains the model for the fixed negative log-likelihood;
see $cref prior_model$$.

$head quasi_fixed$$
If this argument is true,
a quasi-Newton method is used when optimizing the fixed effects.
Otherwise a full Newton method is used; see
$cref/quasi_fixed/option_table/Fixed Only/quasi_fixed/$$.

$head zero_sum_random$$
If this vector has size $code number_rate_enum$$.
If $icode%zero_sum_random%[%rate_id%]%$$ is true,
for each age, time,
the sum of the random effects for the corresponding rate
is constrained to be zero.

$head data_object$$
This object contains the model for the data density;
see $cref/data_model/devel_data_model/$$.
It is effectively const.

$head Prototype$$
$srccode%cpp% */
fit_model::fit_model(
	sqlite3*                              db               ,
	int                                   simulate_index   ,
	bool                                  warn_on_stderr   ,
	double                                bound_random     ,
	bool                                  no_scaling       ,
	const pack_info&                      pack_object      ,
	const pack_prior&                     var2prior        ,
	const CppAD::vector<double>&          start_var        ,
	const CppAD::vector<double>&          scale_var        ,
	const CppAD::vector<prior_struct>&    prior_table      ,
	const prior_model&                    prior_object     ,
	bool                                  quasi_fixed      ,
	const CppAD::vector<bool>&            zero_sum_random  ,
	data_model&                           data_object      )
/* %$$
$end
*/
// base class constructor
// (The value of bool_sparsity does not seem to affect speed test results.)
: cppad_mixed(
	// n_fixed
	number_fixed(pack_object),
	// n_random in dismod_at minus number random effects that are constant
	pack_object.random_size() - number_random_const(
		bound_random, pack_object, var2prior, prior_table
	),
	// quasi_fixed
	quasi_fixed,
	// bool_sparsity
	false,
	// A_rcv
	ran_con_rcv(bound_random, zero_sum_random, pack_object)
),
db_            (db)                                 ,
simulate_index_( simulate_index )                   ,
warn_on_stderr_( warn_on_stderr )                   ,
no_scaling_    ( no_scaling )                       ,
n_fixed_       ( number_fixed(pack_object) )        ,
n_random_      ( pack_object.random_size() )        ,
pack_object_   ( pack_object )                      ,
var2prior_     ( var2prior   )                      ,
start_var_     ( start_var   )                      ,
scale_var_     ( scale_var   )                      ,
prior_table_   ( prior_table )                      ,
prior_object_  ( prior_object )                     ,
data_object_   ( data_object )
{	assert( bound_random >= 0.0 );
	double inf = std::numeric_limits<double>::infinity();
	// ----------------------------------------------------------------------
	// n_var
	size_t n_var = n_fixed_ + n_random_;
	assert( pack_object.size() == n_var );
	assert( var2prior_.size() == n_var );
	// ----------------------------------------------------------------------
# ifndef NDEBUG
	for(size_t var_id = 0; var_id < n_var; var_id++)
	{	size_t prior_id    = var2prior_.value_prior_id(var_id);
		double const_value = var2prior_.const_value(var_id);
		double lower       = const_value;
		double upper       = const_value;
		double mean        = const_value;
		if( std::isnan(const_value) )
		{	assert( prior_id != DISMOD_AT_NULL_SIZE_T );
			lower = prior_table_[prior_id].lower;
			upper = prior_table_[prior_id].upper;
			mean  = prior_table_[prior_id].mean;
		}
		assert(lower <= mean && mean <= upper);
		if( (start_var_[var_id] < lower) | (upper < start_var_[var_id]) )
		{	std::string msg;
			msg  = "value = " + CppAD::to_string(start_var_[var_id]);
			msg += "\nnot between lower = " + CppAD::to_string(lower);
			msg += "\nand upper = " + CppAD::to_string(upper);
			error_exit(msg, "start_var", var_id);
		}
	}
# endif
	// ----------------------------------------------------------------------
	// random_lower_, random_upper_
	d_vector var_lower(n_var), var_upper(n_var);
	get_var_limits(
		var_lower, var_upper, bound_random, var2prior_, prior_table_
	);
	random_lower_.resize(n_random_);
	random_upper_.resize(n_random_);
	unpack_random(pack_object_, var_lower, random_lower_);
	unpack_random(pack_object_, var_upper, random_upper_);
	//
	// -----------------------------------------------------------------------
	// n_random_equal_
	remove_const remove_random(random_lower_, random_upper_);
	n_random_equal_ = remove_random.n_const();
	// ----------------------------------------------------------------------
	// diff_prior_
	assert( diff_prior_.size() == 0 );
	for(size_t var_id = 0; var_id < n_var; ++var_id)
	{	size_t prior_id = var2prior_.dage_prior_id(var_id);
		if( prior_id != DISMOD_AT_NULL_SIZE_T )
		{	double lower    = prior_table[prior_id].lower;
			double upper    = prior_table[prior_id].upper;
			if( - inf < lower || upper < + inf )
			{	diff_prior_struct entry;
				entry.direction    = diff_prior_struct::dage_enum;
				entry.plus_var_id  = var2prior_.dage_var_id(var_id);
				entry.minus_var_id = var_id;
				entry.prior_id     = prior_id;
				diff_prior_.push_back( entry );
			}
		}
		prior_id = var2prior_.dtime_prior_id(var_id);
		if( prior_id != DISMOD_AT_NULL_SIZE_T )
		{	double lower    = prior_table[prior_id].lower;
			double upper    = prior_table[prior_id].upper;
			if( - inf < lower || upper < + inf )
			{	diff_prior_struct entry;
				entry.direction    = diff_prior_struct::dtime_enum;
				entry.plus_var_id  = var2prior_.dtime_var_id(var_id);
				entry.minus_var_id = var_id;
				entry.prior_id     = prior_id;
				diff_prior_.push_back( entry );
			}
		}
	}
	// ----------------------------------------------------------------------
	// var_id2fixed_
	var_id2fixed_.resize(n_var);
	for(size_t k = 0; k < n_var; k++)
		var_id2fixed_[k] = n_fixed_;
	CppAD::vector<size_t> pack_index(n_fixed_);
	pack_index = fixed2var_id(pack_object);
	for(size_t j = 0; j < n_fixed_; j++)
		var_id2fixed_[ pack_index[j] ] = j;
	// ----------------------------------------------------------------------
	// fixed_is_scaled_, fixed_scale_eta_
	fixed_is_scaled_.resize(n_fixed_);
	fixed_scale_eta_.resize(n_fixed_);
	for(size_t j = 0; j < n_fixed_; j++)
	{	fixed_is_scaled_[j] = false;
		fixed_scale_eta_[j] = - inf;
	}
	for(size_t var_id = 0; var_id < n_var; var_id++)
	{	size_t fixed_id = var_id2fixed_[var_id];
		if( fixed_id < n_fixed_ )
		{	size_t prior_id  = var2prior_.value_prior_id(var_id);
			if( prior_id != DISMOD_AT_NULL_SIZE_T )
			{	prior_struct prior          = prior_table[prior_id];
				fixed_scale_eta_[fixed_id]  = prior.eta;
				fixed_is_scaled_[fixed_id]  = ! std::isnan( prior.eta );
				fixed_is_scaled_[fixed_id] &= ! no_scaling;
				bool ok = std::isnan(prior.eta);
				ok     |= prior.lower + prior.eta > 0.0;
				if( ! ok  )
				{	std::string	msg = "eta != null, lower + eta <= 0, and\n";
					msg += "this is a value prior for a fixed effect";
					error_exit(msg, "prior", prior_id);
				}
			}
		}
	}
# ifndef NDEBUG
	// check that all the fixed effects scale values have been set
	for(size_t j = 0; j < n_fixed_; j++)
		if( fixed_is_scaled_[j] )
			assert( ! (fixed_scale_eta_[j] == - inf) );
# endif
	// ---------------------------------------------------------------------
	// initialize the cppad_mixed object
	//
	// fixed_vec
	CppAD::vector<double> fixed_vec(n_fixed_);
	unpack_fixed(pack_object, start_var, fixed_vec);
	scale_fixed_effect(fixed_vec, fixed_vec);
	// random_vec
	CppAD::vector<double> random_vec(n_random_);
	unpack_random(pack_object, start_var, random_vec);
	//
	remove_const random_const( random_lower_, random_upper_);
	CppAD::vector<double> cppad_mixed_random_vec =
		random_const.remove( random_vec );
	//
	initialize(fixed_vec, cppad_mixed_random_vec);
}
/*
-----------------------------------------------------------------------------
$begin fit_model_run_fit$$
$spell
	frac
	const
	num
	iter
$$

$section Run optimization to determine the optimal fixed and random effects$$

$head Syntax$$
$icode%fit_object%.run_fit(%random_only%, %option_map%)
%$$

$head fit_object$$
This object must have been constructed with
$cref/no_scaling/fit_model_ctor/no_scaling/$$ equal to $code fit$$.

$head Scaling$$
During optimization the
$cref/fixed effects/model_variables/Fixed Effects, theta/$$,
that have $icode eta$$ not $code null$$ in their value prior, are scaled; see
$cref/scaling/prior_table/eta/Scaling Fixed Effects/$$.

$head random_only$$
If this argument is true,
the fixed effects are set to their starting value and only the
random effects are optimized.
Otherwise, both the fixed and random effects are optimized.

$head option_map$$
This argument is effectively $code const$$ and
must have the following values:
For $icode name$$ equal to
$cref/derivative_test/option_table/Fixed and Random/derivative_test/$$,
$cref/tolerance/option_table/Fixed and Random/tolerance/$$,
$cref/max_num_iter/option_table/Fixed and Random/max_num_iter/$$,
$cref/print_level/option_table/Fixed and Random/print_level/$$,
$cref/accept_after_max_steps
	/option_table
	/Fixed and Random
	/accept_after_max_steps
/$$,
and for $icode fit$$ equal to $code fixed$$ and $code random$$
$codei%
	%option_map%["%name%_%fit%"]
%$$
is the value in the $cref option_table$$ for the corresponding option.
It must also have
$codei%
	%option_map%["bound_frac_fixed"]
	%option_map%["limited_memory_max_history_fixed"]
%$$

$head Prototype$$
$srccode%cpp% */
void fit_model::run_fit(
	bool                                random_only    ,
	std::map<std::string, std::string>& option_map     )
/* %$$
$end
*/
{	assert( ! no_scaling_ );
	double inf = std::numeric_limits<double>::infinity();
	//
	size_t n_var = n_fixed_ + n_random_;
	assert( pack_object_.size() == n_var );
	assert( var2prior_.size() == n_var );
	d_vector pack_vec( n_var );

	// fixed_lower, fixed_upper
	double bound_random = inf; // does not matter for fixed effects
	d_vector var_lower(n_var), var_upper(n_var);
	get_var_limits(
		var_lower, var_upper, bound_random, var2prior_, prior_table_
	);
	d_vector fixed_lower_noscale(n_fixed_), fixed_upper_noscale(n_fixed_);
	unpack_fixed(pack_object_, var_lower, fixed_lower_noscale);
	unpack_fixed(pack_object_, var_upper, fixed_upper_noscale);
	//
	d_vector fixed_lower(n_fixed_), fixed_upper(n_fixed_);
	scale_fixed_effect(fixed_lower_noscale, fixed_lower);
	scale_fixed_effect(fixed_upper_noscale, fixed_upper);
	//
	// fixed_mean
	d_vector fixed_mean(n_fixed_);
	for(size_t i = 0; i < n_var; i++)
	{	size_t prior_id = var2prior_.value_prior_id(i);
		double const_value = var2prior_.const_value(i);
		if( ! std::isnan(const_value) )
			pack_vec[i] = const_value;
		else if( prior_id == DISMOD_AT_NULL_SIZE_T )
			pack_vec[i] = 0.0;
		else
			pack_vec[i] = prior_table_[prior_id].mean;
	}
	unpack_fixed(pack_object_, pack_vec, fixed_mean);
	scale_fixed_effect(fixed_mean, fixed_mean);

	// fix_constraint_lower, fix_constraint_upper
	d_vector fix_constraint_lower, fix_constraint_upper;
	for(size_t k = 0; k < diff_prior_.size(); k++)
	{	// make sure these variable ids correspond to fixed effects
		assert( var_id2fixed_[ diff_prior_[k].plus_var_id ] < n_fixed_ );
		assert( var_id2fixed_[ diff_prior_[k].minus_var_id ] < n_fixed_ );
		//
		size_t prior_id = diff_prior_[k].prior_id;
		double lower    = prior_table_[prior_id].lower;
		double upper    = prior_table_[prior_id].upper;
		fix_constraint_lower.push_back(lower);
		fix_constraint_upper.push_back(upper);
	}

	// fixed_in
	d_vector fixed_in(n_fixed_);
	unpack_fixed(pack_object_, start_var_, fixed_in);
	scale_fixed_effect(fixed_in, fixed_in);

	// Note that scale_fixed_effect scales argument values.
	// The fixed_scale vector is used by cppad_mixed to scale function values.
	d_vector fixed_scale(n_fixed_);
	unpack_fixed(pack_object_, scale_var_, fixed_scale);
	scale_fixed_effect(fixed_scale, fixed_scale);

	// random_in
	d_vector random_in(n_random_);
	unpack_random(pack_object_, start_var_, random_in);

	// Ipopt fixed effects optimization options
	std::string bound_push = CppAD::to_string(
		std::numeric_limits<double>::min()
	);
	std::string options = "";
	options += "String    sb  yes";
	options += "\nNumeric bound_relax_factor 0.0";
	options += "\nNumeric bound_push " + bound_push;
	options += "\nNumeric tol " + option_map["tolerance_fixed"];
	options += "\nNumeric acceptable_tol " + option_map["tolerance_fixed"];
	options += "\nInteger print_level " + option_map["print_level_fixed"];
	options += "\nInteger max_iter " + option_map["max_num_iter_fixed"];
	//
	options += "\nInteger accept_after_max_steps "
		+ option_map["accept_after_max_steps_fixed"];
	options += "\nString derivative_test "
		+ option_map["derivative_test_fixed"];
	//
	options += "\nNumeric bound_frac " + option_map["bound_frac_fixed"];
	options += "\nInteger limited_memory_max_history "
		+ option_map["limited_memory_max_history_fixed"];
	options += "\n";
	//
	std::string fixed_options = options;
	//
	// Ipopt random effects optimization options
	options = "";
	options += "String    sb  yes";
	options += "\nNumeric bound_relax_factor 0.0";
	options += "\nNumeric tol " + option_map["tolerance_random"];
	options += "\nNumeric acceptable_tol " + option_map["tolerance_random"];
	options += "\nInteger print_level " + option_map["print_level_random"];
	options += "\nInteger max_iter " + option_map["max_num_iter_random"];
	options += "\nString evaluation_method " + option_map["method_random"];
	//
	options += "\nInteger accept_after_max_steps "
		+ option_map["accept_after_max_steps_random"];
	options += "\nString derivative_test "
		+ option_map["derivative_test_random"];
	//
	options += "\nString hessian_approximation exact";
	options += "\n";
	//
	std::string random_options = options;
	//
	// convert from dismod_at random effects to cppad_mixed random effects
	remove_const random_const(random_lower_, random_upper_);
	d_vector cppad_mixed_random_lower = random_const.remove( random_lower_ );
	d_vector cppad_mixed_random_upper = random_const.remove( random_upper_ );
	d_vector cppad_mixed_random_in    = random_const.remove( random_in );
	//
	// optimize the fixed effects
	d_vector fixed_opt = fixed_in;
	d_vector fixed_lag(n_fixed_);
	d_vector fixed_con_lag( diff_prior_.size() );
	for(size_t j = 0; j < fixed_lag.size(); j++)
		fixed_lag[j] = 0.0;
	for(size_t j = 0; j < fixed_con_lag.size(); j++)
		fixed_con_lag[j] = 0.0;
	if( ! random_only )
	{	CppAD::mixed::fixed_solution fixed_sol = optimize_fixed(
			fixed_options,
			random_options,
			fixed_lower,
			fixed_upper,
			fix_constraint_lower,
			fix_constraint_upper,
			fixed_scale,
			fixed_in,
			cppad_mixed_random_lower,
			cppad_mixed_random_upper,
			cppad_mixed_random_in
		);
		fixed_opt      = fixed_sol.fixed_opt;
		fixed_lag      = fixed_sol.fixed_lag;
		fixed_con_lag  = fixed_sol.fix_con_lag;
	}
	// optimal random effects
	d_vector random_opt(n_random_);
	if( n_random_ > n_random_equal_ )
	{	d_vector cppad_mixed_random_opt = optimize_random(
			random_options,
			fixed_opt,
			cppad_mixed_random_lower,
			cppad_mixed_random_upper,
			cppad_mixed_random_in
		);
		random_opt = random_cppad_mixed2dismod_at( cppad_mixed_random_opt );
	}
	else
	{	assert( n_random_ == n_random_equal_ );
		for(size_t i = 0; i < n_random_; i++)
			random_opt[i] = random_lower_[i];
	}
	// The optimal solution is scaled, but the Lagrange multilpiers are not
	unscale_fixed_effect(fixed_opt, fixed_opt);
	// make sure round off has not violated bounds
	for(size_t j = 0; j < n_fixed_; ++j)
	{	fixed_opt[j] = std::max(fixed_lower_noscale[j], fixed_opt[j]);
		fixed_opt[j] = std::min(fixed_upper_noscale[j], fixed_opt[j]);
	}
	//
	// size store solution_
	solution_.fit_var_value.resize(n_var);
	solution_.lagrange_value.resize(n_var);
	solution_.lagrange_dage.resize(n_var);
	solution_.lagrange_dtime.resize(n_var);
	//
	// values that are stored by fixed effect index
	pack_fixed(pack_object_, solution_.fit_var_value, fixed_opt);
	pack_fixed(pack_object_, solution_.lagrange_value, fixed_lag);
	//
	// values that are stored by random effect index
	if ( n_random_ > 0 )
	{	d_vector zero(n_random_);
		for(size_t i = 0; i < n_random_; i++)
			zero[i] = 0.0;
		pack_random(pack_object_, solution_.fit_var_value, random_opt);
		pack_random(pack_object_, solution_.lagrange_value, zero);
	}
	//
	// difference constraints
	for(size_t i = 0; i < n_var; i++)
	{	solution_.lagrange_dage[i]  = 0.0;
		solution_.lagrange_dtime[i] = 0.0;
	}
	for(size_t k = 0; k < diff_prior_.size(); k++)
	{	size_t minus_var_id   = diff_prior_[k].minus_var_id;
		assert( var_id2fixed_[minus_var_id] < n_fixed_ );
		if( diff_prior_[k].direction == diff_prior_struct::dage_enum )
			solution_.lagrange_dage[minus_var_id] = fixed_con_lag[k];
		else
			solution_.lagrange_dtime[minus_var_id] = fixed_con_lag[k];
	}
	return;
}
/*
---------------------------------------------------------------------------
$begin fit_model_get_solution$$
$spell
	dage
	dtime
	CppAD
	var
$$

$section Get Solution Corresponding to Previous Fit$$

$head Syntax$$
$codei%%fit_object%.get_solution(
	%fit_var_value%, %lagrange_value%, %lagrange_dage%, %lagrange_dtime%
)%$$

$head fit_object$$
see $cref/fit_object/fit_model_ctor/fit_object/$$.

$head fit_var_value$$
The size of this vector size is zero or
equal to the number of model variables.
The input value of its elements does not matter.
Upon return it is the optimal $cref/variable values/model_variables/$$ in
$cref pack_info$$ order.

$head lagrange_value$$
The size of this vector size is zero or
equal to the number of model variables.
The input value of its elements does not matter.
Upon return it is the Lagrange multipliers for the lower and upper
limits on the corresponding model variables.
If there is no limit, or if a limit is not active, the corresponding
element is zero.

$head lagrange_dage$$
The size of this vector size is zero or
equal to the number of model variables.
The input value of its elements does not matter.
Upon return it is the Lagrange multipliers for the lower and upper
limits on the forward age difference for all the variables.
If a variable does not have a forward age difference,
if there is no limit, or if a limit is not active, the corresponding
element is zero.

$head lagrange_dtime$$
The size of this vector size is zero or
equal to the number of model variables.
The input value of its elements does not matter,
Upon return it is the Lagrange multipliers for the lower and upper
limits on the forward time difference for all the variable.
If a variable does not have a forward time difference,
if there is no limit, or if a limit is not active, the corresponding
element is zero.

$children%example/devel/model/fit_model_xam.cpp
%$$
$head Example$$
The file $cref fit_model_xam.cpp$$ contains an example and test
of using this routine.

$head Prototype$$
$srccode%cpp% */
void fit_model::get_solution(
	CppAD::vector<double>& fit_var_value   ,
	CppAD::vector<double>& lagrange_value  ,
	CppAD::vector<double>& lagrange_dage   ,
	CppAD::vector<double>& lagrange_dtime  )
/* %$$
$end
*/
{	fit_var_value  = solution_.fit_var_value;
	lagrange_value = solution_.lagrange_value;
	lagrange_dage  = solution_.lagrange_dage;
	lagrange_dtime = solution_.lagrange_dtime;
	return;
}
/*
---------------------------------------------------------------------------
$begin fit_model_sample_posterior$$
$spell
	dage
	dtime
	var
$$

$section Sample From Posterior Distribution for a Fit$$

$head Syntax$$
$icode%fit_object%.sample_posterior(
	%sample%,
	%fit_var_value%,
	%lagrange_value%,
	%lagrange_dage%,
	%lagrange_dtime%,
	%option_map%
)%$$

$head fit_object$$
This object must have been constructed with
$cref/no_scaling/fit_model_ctor/no_scaling/$$ equal to $code sample$$.

$head sample$$
This argument is a vector with size equal to the number of samples
$icode n_sample$$ times the number of $cref model_variables$$ $icode n_var$$.
The input value of its elements does not matter.
Upon return, for
$icode%i% = 0 , %...% , %n_sample%-1%$$,
$icode%j% = 0 , %...% , %n_var%-1%$$,
$codei%
	%sample%[ %i% * %n_sample% + %j% ]
%$$
is the $th j$$ component of the $th i$$ sample of the model variables.
These samples are independent for different $icode i$$,
and for fixed $icode i$$ they have the asymptotic covariance
for the model variables.

$head fit_var_value$$
This vector has size equal to the number of model variables.
It is the optimal $cref/variable values/model_variables/$$ in
$cref pack_info$$ order.

$head lagrange_value$$
This vector has size equal to the number of model variables.
It is the Lagrange multipliers for the lower and upper
limits on the corresponding model variables.
If there is no limit, or if a limit is not active, the corresponding
element is zero.

$head lagrange_dage$$
This vector has size equal to the number of model variables.
It is the Lagrange multipliers for the lower and upper
limits on the forward age difference for all the variables.
If a variable does not have a forward age difference,
if there is no limit, or if a limit is not active, the corresponding
element is zero.

$head lagrange_dtime$$
This vector has size equal to the number of model variables.
It is the Lagrange multipliers for the lower and upper
limits on the forward time difference for all the variable.
If a variable does not have a forward time difference,
if there is no limit, or if a limit is not active, the corresponding
element is zero.

$head Prototype$$
$srccode%cpp% */
void fit_model::sample_posterior(
	CppAD::vector<double>&              sample          ,
	const CppAD::vector<double>&        fit_var_value   ,
	const CppAD::vector<double>&        lagrange_value  ,
	const CppAD::vector<double>&        lagrange_dage   ,
	const CppAD::vector<double>&        lagrange_dtime  ,
	std::map<std::string, std::string>& option_map      )
/* %$$
$end
*/
{	double inf = std::numeric_limits<double>::infinity();
	assert( no_scaling_ );
	//
	size_t n_var = n_fixed_ + n_random_;
	assert( sample.size() % n_var == 0     );
	assert( fit_var_value.size() == n_var );
	assert( lagrange_value.size() == n_var );
	assert( lagrange_dage.size()  == n_var );
	assert( lagrange_dtime.size() == n_var );

	// n_sample
	size_t n_sample = sample.size() / n_var;

	// solution.fixed_opt
	CppAD::mixed::fixed_solution solution;
	solution.fixed_opt.resize(n_fixed_);
	unpack_fixed(pack_object_, fit_var_value, solution.fixed_opt);
	//
	// solution.fixed_lag
	solution.fixed_lag.resize(n_fixed_);
	unpack_fixed(pack_object_, lagrange_value, solution.fixed_lag);
	//
	// solution.fix_con_lag
	size_t n_fix_con = diff_prior_.size();
	solution.fix_con_lag.resize(n_fix_con);
	for(size_t k = 0; k < n_fix_con; k++)
	{	size_t minus_var_id   = diff_prior_[k].minus_var_id;
		if( diff_prior_[k].direction==dismod_at::diff_prior_struct::dage_enum )
			solution.fix_con_lag[k] = lagrange_dage[minus_var_id];
		else
			solution.fix_con_lag[k] = lagrange_dtime[minus_var_id];
	}
	//
	// random_opt
	CppAD::vector<double> random_opt(n_random_);
	unpack_random(pack_object_, fit_var_value, random_opt);
	//
	// convert dismod_at random effects to cppad_mixed random effects
	remove_const random_const( random_lower_ , random_upper_ );
	d_vector cppad_mixed_random_opt = random_const.remove( random_opt );
	//
	// information_rcv
	CppAD::mixed::d_sparse_rcv information_rcv = information_mat(
		solution, cppad_mixed_random_opt
	);
	//
	// fixed_lower
	CppAD::vector<double> pack_vec( n_var );
	CppAD::vector<double> fixed_lower(n_fixed_);
	for(size_t i = 0; i < n_var; i++)
	{	size_t prior_id    = var2prior_.value_prior_id(i);
		double const_value = var2prior_.const_value(i);
		if( ! std::isnan(const_value ) )
			pack_vec[i] = const_value;
		else if( prior_id == DISMOD_AT_NULL_SIZE_T )
			pack_vec[i] = -inf;
		else
			pack_vec[i] = prior_table_[prior_id].lower;
	}
	unpack_fixed(pack_object_, pack_vec, fixed_lower);
	scale_fixed_effect(fixed_lower, fixed_lower);

	// fixed_upper
	CppAD::vector<double> fixed_upper(n_fixed_);
	for(size_t i = 0; i < n_var; i++)
	{	size_t prior_id    = var2prior_.value_prior_id(i);
		double const_value = var2prior_.const_value(i);
		if( ! std::isnan(const_value) )
			pack_vec[i] = const_value;
		else if( prior_id == DISMOD_AT_NULL_SIZE_T )
			pack_vec[i] = +inf;
		else
			pack_vec[i] = prior_table_[prior_id].upper;
	}
	unpack_fixed(pack_object_, pack_vec, fixed_upper);
	scale_fixed_effect(fixed_upper, fixed_upper);
	//
	// check diagonal of information matrix is positive
	// (except for bound constrained variables)
	CppAD::vector<size_t> pack_index = fixed2var_id(pack_object_);
	CppAD::vector<bool>   ok(n_fixed_);
	for(size_t j = 0; j < n_fixed_; j++)
	{	ok[j]  = fixed_lower[j] == fixed_upper[j];
		ok[j] |= solution.fixed_lag[j] != 0.0;
	}
	size_t K = information_rcv.nnz();
	for(size_t k = 0; k < K; k++)
	{	size_t i = information_rcv.row()[k];
		size_t j = information_rcv.col()[k];
		double v = information_rcv.val()[k];
		if( i == j )
		{	ok[j] |= v > 0.0;
		}
	}
	std::string msg = "";
	for(size_t j = 0; j < n_fixed_; j++)
	{	if( ! ok[j] )
		{	if( msg == "" )
			{	msg = "Hessian w.r.t following variables not positive\n";
				msg += "and they are not bound constrained: var_id =";
				msg += " " + CppAD::to_string( pack_index[j] );
			}
			else
				msg += "," + CppAD::to_string( pack_index[j] );
		}
	}
	if( msg != "" )
		error_exit(msg);
	//
	// sample_fix
	CppAD::vector<double> sample_fix(n_sample * n_fixed_);
	sample_fixed(
		sample_fix,
		information_rcv,
		solution,
		fixed_lower,
		fixed_upper,
		cppad_mixed_random_opt
	);
	//
	// random_options
	// (same as in run_fit)
	std::string options = "";
	options += "String  sb  yes";
	options += "\nNumeric bound_relax_factor 0.0";
	options += "\nNumeric tol " + option_map["tolerance_random"];
	options += "\nNumeric acceptable_tol " + option_map["tolerance_random"];
	options += "\nInteger max_iter " + option_map["max_num_iter_random"];
	options += "\nInteger print_level " + option_map["print_level_random"];
	options += "\nString derivative_test "
		+ option_map["derivative_test_random"] + "\n";
	std::string random_options = options;
	//
	// random_in
	CppAD::vector<double> random_in(n_random_);
	unpack_random(pack_object_, start_var_, random_in);
	//
	// convert from dismod_at random effects to cppad_mixed random effects
	d_vector cppad_mixed_random_lower = random_const.remove( random_lower_ );
	d_vector cppad_mixed_random_upper = random_const.remove( random_upper_ );
	d_vector cppad_mixed_random_in    = random_const.remove( random_in );
	//
	CppAD::vector<double> one_sample_random(n_random_),
		cppad_mixed_one_sample_random(n_random_ - n_random_equal_);
	CppAD::vector<double> one_sample_fixed(n_fixed_);
	for(size_t i_sample = 0; i_sample < n_sample; i_sample++)
	{	for(size_t j = 0; j < n_fixed_; j++)
		{	one_sample_fixed[j] = sample_fix[ i_sample * n_fixed_ + j];
			//
			if( n_random_ > n_random_equal_ )
			{	sample_random(
					cppad_mixed_one_sample_random,
					random_options,
					one_sample_fixed,
					cppad_mixed_random_lower,
					cppad_mixed_random_upper,
					cppad_mixed_random_in
				);
			}
			one_sample_random = random_cppad_mixed2dismod_at(
				cppad_mixed_one_sample_random
			);
			//
			// pack_vec
			unscale_fixed_effect(one_sample_fixed, one_sample_fixed);
			pack_fixed(pack_object_, pack_vec, one_sample_fixed);
			pack_random(pack_object_, pack_vec, one_sample_random);
			//
			// copy to output vector
			for(size_t i = 0; i < n_var; i++)
				sample[ i_sample * n_var + i ] = pack_vec[i];
		}
	}
	return;
}
// ===========================================================================
// private virtual functions
// ===========================================================================
// ran_likelihood
fit_model::a1_vector fit_model::ran_likelihood(
	const a1_vector& fixed_vec                ,
	const a1_vector& cppad_mixed_random_vec   )
{	//
	// check for case where all random effects are constrained
	assert( n_random_ >= n_random_equal_ );
	if( n_random_ == n_random_equal_ )
		return a1_vector(0);
	//
	// convert from cppad_mixed random effects to dismod_at random effects
	a1_vector random_vec = random_cppad_mixed2dismod_at(
		cppad_mixed_random_vec
	);
	//
	// packed vector
	a1_vector pack_vec( pack_object_.size() );
	//
	// put the fixed and random effects into pack_vec
	a1_vector fixed_tmp(n_fixed_);
	unscale_fixed_effect(fixed_vec, fixed_tmp);
	pack_fixed(pack_object_, pack_vec, fixed_tmp);
	pack_random(pack_object_, pack_vec, random_vec);
	//
	// evaluate the data and prior residuals that depend on the random effects
	CppAD::vector< residual_struct<a1_double> > data_ran, prior_ran;
	bool hold_out       = true;
	bool random_depend  = true;
	data_ran   = data_object_.like_all(hold_out, random_depend, pack_vec);
	prior_ran  = prior_object_.random(pack_vec);
	//
	// number of data and prior residuals
	size_t n_data_ran    = data_ran.size();
	size_t n_prior_ran   = prior_ran.size();
	//
	// check for the case where we return the empty vector
	if( n_data_ran == 0 && n_prior_ran == 0 )
		return a1_vector(0);
	//
# ifndef NDEBUG
	// no absolute value terms
	for(size_t i = 0; i < n_data_ran; i++)
	{	density_enum density = data_ran[i].density;
		assert( ! nonsmooth_density(density) );
	}
	for(size_t i = 0; i < n_prior_ran; i++)
	{	density_enum density = prior_ran[i].density;
		assert( ! nonsmooth_density(density) );
	}
# endif
	// size ran_den
	a1_vector ran_den(1);
	//
	// initialize summation of smooth part
	ran_den[0] = 0.0;
	//
	// data_ran terms
	for(size_t i = 0; i < n_data_ran; i++)
		ran_den[0] += data_ran[i].logden_smooth;
	//
	// random effects prior
	for(size_t i = 0; i < n_prior_ran; i++)
		ran_den[0] += prior_ran[i].logden_smooth;
	//
	// convert from log-density to negative log density
	ran_den[0] = - ran_den[0];
	//
	return ran_den;
}
// ---------------------------------------------------------------------------
// fix_likelihood
fit_model::a1_vector fit_model::fix_likelihood(
	const a1_vector& fixed_vec   )
{	// local vectors
	a1_vector random_vec( pack_object_.random_size() );
	a1_vector a1_pack_vec( pack_object_.size() );
	//
	// set random_vec
	for(size_t i = 0; i < random_vec.size(); i++)
	{	random_vec[i] = CppAD::nan( a1_double(0.0) );
		if( random_lower_[i] == random_upper_[i] )
			random_vec[i] = random_lower_[i];
	}
	//
	// put the fixed and random effects into pack_vec
	a1_vector fixed_tmp(n_fixed_);
	unscale_fixed_effect(fixed_vec, fixed_tmp);
	pack_fixed(pack_object_, a1_pack_vec, fixed_tmp);
	pack_random(pack_object_, a1_pack_vec, random_vec);
	//
	// evaluate the data and prior residuals that only depend on fixed effects
	// and random effects with bounds that are equal
	CppAD::vector< residual_struct<a1_double> > data_fix, prior_fix;
	bool hold_out      = true;
	bool random_depend = false;
	data_fix  = data_object_.like_all(hold_out, random_depend, a1_pack_vec);
	prior_fix = prior_object_.fixed(a1_pack_vec);
# ifndef NDEBUG
	if( n_random_ == n_random_equal_ )
	{	// ran_likelihood returns the empty vector in this case
		// so we need to include rest of the data here.
		for(size_t i = 0; i < n_random_; i++)
			assert( random_lower_[i] == random_upper_[i] );
		random_depend = true;
		CppAD::vector< residual_struct<a1_double> > data_ran =
			data_object_.like_all(hold_out, random_depend, a1_pack_vec);
		assert( data_ran.size() == 0 );
	}
# endif
	//
	// number of data and prior residuals
	size_t n_data_fix    = data_fix.size();
	size_t n_prior_fix   = prior_fix.size();
	//
	// count the number of absolute value terms
	size_t n_abs = 0;
	for(size_t i = 0; i < n_data_fix; i++)
	{	if( ! Constant(data_fix[i].logden_sub_abs) )
			n_abs++;
	}
	for(size_t i = 0; i < n_prior_fix; i++)
	{	if( ! Constant(prior_fix[i].logden_sub_abs) )
			n_abs++;
	}
	// size fix_den
	a1_vector fix_den(1 + n_abs);
	//
	// initialize summation of smooth part
	fix_den[0] = a1_double(0.0);
	//
	// initialize index for non-smooth part
	size_t i_abs = 0;
	//
	// data_fix terms
	for(size_t i = 0; i < n_data_fix; i++)
	{	fix_den[0] += data_fix[i].logden_smooth;
		if( ! Constant(data_fix[i].logden_sub_abs) )
			fix_den[1 + i_abs++] = data_fix[i].logden_sub_abs;
	}
	//
	// fixed effects prior
	for(size_t i = 0; i < n_prior_fix; i++)
	{	fix_den[0] += prior_fix[i].logden_smooth;
		if( ! Constant(prior_fix[i].logden_sub_abs) )
			fix_den[1 + i_abs++] = prior_fix[i].logden_sub_abs;
	}
	//
	// convert from log-density to negative log density
	fix_den[0] = - fix_den[0];
	//
	return fix_den;
}
// --------------------------------------------------------------------------
// fix_constraint
fit_model::a1_vector fit_model::fix_constraint(const a1_vector& fixed_vec)
{	a1_vector ret_val( diff_prior_.size() );
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
{
	std::string msg = "cppad_mixed: " + error_message;
	if( simulate_index_ != -1 )
	{	msg += "\nfitting simulate_index = ";
		msg += CppAD::to_string(simulate_index_);
	}
	// prints on std::cerr, logs in database, generates an assert, then exits
	error_exit(msg);
}
// warning
void fit_model::warning(const std::string& warning_message)
{	std::string msg = "cppad_mixed: " + warning_message;
	if( simulate_index_ != -1 )
	{	msg += "\nfitting simulate_index = ";
		msg += CppAD::to_string(simulate_index_);
	}
	// prints on std::cerr as well as logs in database
	if( warn_on_stderr_ )
		log_message(db_, &std::cerr, "warning", msg);
	else
		log_message(db_, DISMOD_AT_NULL_PTR, "warning", msg);
}
} // DISMOD_AT_END_NAMESPACE
