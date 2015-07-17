// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin approx_mixed_optimize_fixed$$
$spell
	ipopt
	xam
	vec
	const
	CppAD
	std
$$

$section Optimize Fixed Effects$$

$head Syntax$$
$icode%fixed_out% =%$$
$icode%approx_object%.optimize_fixed(
	%options%,
	%fixed_lower%,
	%fixed_upper%,
	%fixed_in%,
	%random_in%
)%$$

$head Purpose$$
This routine maximizes the Laplace approximation for the
negative log-likelihood
$cref/L(theta)/approx_mixed_theory/Objective/L(theta)/$$
$latex \[
L( \theta ) \approx - \log \left[
	\int \B{p} ( y | \theta , u) \B{p} ( u | \theta ) \B{p} ( \theta ) \R{d} u
\right]
\] $$

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head options$$
The argument $icode options$$ has prototype
$codei%
	const std::string %options%
%$$
It contains a list of options.
Each option, including the last option,
is terminated by the $code '\n'$$ character.
Each line consists of three tokens separated by one or more spaces.

$subhead String$$
You can set any Ipopt string option using a line with the following syntax:
$codei%
	String %name% %value%
%$$
Here $icode name$$ is any valid Ipopt string option
and $icode value$$ is its setting.

$subhead Integer$$
You can set any Ipopt integer option using a line with the following syntax:
$codei%
	Integer %name% %value%
%$$
Here $icode name$$ is any valid Ipopt integer option
and $icode value$$ is its setting.

$subhead Numeric$$
You can set any Ipopt numeric option using a line with the following syntax:
$codei%
	Numeric %name% %value%
%$$
Here $icode name$$ is any valid Ipopt numeric option
and $icode value$$ is its setting.


$head fixed_lower$$
specifies the lower limits for the
$cref/fixed_effects/model_variable/Fixed Effects, theta/$$.
Note that
$code%
	- std::numeric_limits<double>::infinity()
%$$
is used for minus infinity; i.e., no lower limit.

$head fixed_upper$$
specifies the upper limits for the fixed effects.
Note that
$code%
	std::numeric_limits<double>::infinity()
%$$
is used for plus infinity; i.e., no upper limit.

$head constraint_lower$$
specifies the lower limits for the
$cref/constraints/approx_mixed_constraint/$$.
Note that
$code%
	- std::numeric_limits<double>::infinity()
%$$
is used for minus infinity; i.e., no lower limit.

$head constraint_upper$$
specifies the upper limits for the constraints.
Note that
$code%
	std::numeric_limits<double>::infinity()
%$$
is used for plus infinity; i.e., no upper limit.

$head fixed_in$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_in%
%$$
It specifies the initial value for the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ during the optimization process.
It must hold for each $icode j$$ that
$codei%
	%fixed_lower%[%j%] <= %fixed_in%[%j%] <= %fixed_upper%[%j%]
%$$

$head random_in$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_in%
%$$
It specifies the initial value used for the optimization of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$.

$head fixed_out$$
The return value has prototype
$codei%
	CppAD::vector<double> %fixed_out%
%$$
It is the final value (obtained by optimization) of the
fixed effects vector.
This vector satisfies its bounds; i.e.,
$codei%
	%fixed_lower%[%j%] <= %fixed_out%[%j%] <= %fixed_upper%[%j%]
%$$
for $icode%j% = 0 , %...%, %n_fixed_%-1%$$.

$head Laplace Approximation$$
The $cref/theory/approx_mixed_theory/$$ for the
Laplace approximation optimization only includes the case where
the $cref/joint negative log-likelihood/approx_mixed_joint_like/$$ is smooth.

$children%example/devel/approx_mixed/optimize_fixed_xam.cpp
	%example/devel/approx_mixed/approx_constraint_xam.cpp
	%include/dismod_at/ipopt_fixed.hpp
%$$

$head Example$$
The file $cref optimize_fixed_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$head ipopt_fixed$$
The  class $cref ipopt_fixed$$ is used by $code optimize_fixed$$
to optimize the fixed effects.
It's specifications are not part of the $cref approx_mixed$$ public interface.

$end
------------------------------------------------------------------------------
*/
# include <coin/IpIpoptApplication.hpp>
# include <dismod_at/approx_mixed.hpp>
# include <dismod_at/ipopt_fixed.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

CppAD::vector<double> approx_mixed::optimize_fixed(
	const std::string& options           ,
	const d_vector&    fixed_lower       ,
	const d_vector&    fixed_upper       ,
	const d_vector&    constraint_lower  ,
	const d_vector&    constraint_upper  ,
	const d_vector&    fixed_in          ,
	const d_vector&    random_in   )
{	bool ok = true;
	using Ipopt::SmartPtr;
	// make sure initialize has been called
	if( ! constraint_initialized_ )
	{	std::string error_message =
		"approx_mixed::initialize was not called before optimize_fixed";
		fatal_error(error_message);
	}

# ifndef NDEBUG
	assert( fixed_lower.size() == fixed_in.size() );
	assert( fixed_lower.size() == fixed_upper.size() );
	for(size_t j = 0; j < fixed_lower.size(); j++)
	{	assert( fixed_lower[j] <= fixed_in[j] );
		assert( fixed_in[j]    <= fixed_upper[j] );
	}
# endif

	// create a reference to this object
	approx_mixed& approx_object(*this);

	// Create an instance of an IpoptApplication
	SmartPtr<Ipopt::IpoptApplication> app = IpoptApplicationFactory();

	// Set options
	double ipopt_tolerance = 1e-8; // default
	size_t begin_1, end_1, begin_2, end_2, begin_3, end_3;
	begin_1     = 0;
	while( options[begin_1] == ' ')
		begin_1++;
	while( begin_1 < options.size() )
	{	// split this line into tokens
		end_1   = options.find_first_of(" \n", begin_1);
		begin_2 = end_1;
		while( options[begin_2] == ' ')
			begin_2++;
		end_2   = options.find_first_of(" \n", begin_2);
		begin_3 = end_2;
		while( options[begin_3] == ' ')
			begin_3++;
		end_3   = options.find_first_of(" \n", begin_3);

		// check for errors
		assert( end_3 != std::string::npos );
		assert( begin_1 < end_1 && end_1 <= begin_2 );
		assert( begin_2 < end_2 && end_2 <= begin_3 );
		assert( begin_3 < end_3 );

		// get the three tokens
		std::string tok_1 = options.substr(begin_1, end_1 - begin_1);
		std::string tok_2 = options.substr(begin_2, end_2 - begin_2);
		std::string tok_3 = options.substr(begin_3, end_3 - begin_3);

		// switch on option type
		if ( tok_1 == "String" )
			app->Options()->SetStringValue(tok_2.c_str(), tok_3.c_str());
		else if ( tok_1 == "Numeric" )
		{	Ipopt::Number value = std::atof( tok_3.c_str() );
			app->Options()->SetNumericValue(tok_2.c_str(), value);
			if( tok_2 == "tol" )
				ipopt_tolerance = value;
		}
		else if ( tok_1 == "Integer" )
		{	Ipopt::Index value = std::atoi( tok_3.c_str() );
			app->Options()->SetIntegerValue(tok_2.c_str(), value);
		}
		else assert(false);
		//
		// next line
		begin_1 = end_3;
		while( options[begin_1] == ' ' || options[begin_1] == '\n' )
			begin_1++;
	}

	// object that is used to evalutate objective and constraints
	SmartPtr<ipopt_fixed> fixed_nlp = new ipopt_fixed(
		ipopt_tolerance,
		fixed_lower,
		fixed_upper,
		constraint_lower,
		constraint_upper,
		fixed_in,
		random_in,
		approx_object
	);

	// Set values used for minus and plus infinity
	app->Options()->SetNumericValue(
		"nlp_lower_bound_inf", fixed_nlp->nlp_lower_bound_inf()
	);

	// variable to hold status values returned by app
	Ipopt::ApplicationReturnStatus status;

	// initialize app
	status = app->Initialize();
	ok    &= status == Ipopt::Solve_Succeeded;
	if( ! ok )
	{	fatal_error("optimize_fixed: initalization failed");
	}

	// solve the problem
	status = app->OptimizeTNLP(fixed_nlp);
	if( status != Ipopt::Solve_Succeeded )
	{	warning("optimize_fixed: ipopt failed to converge");
	}
	if( ! fixed_nlp->finalize_solution_ok_ )
	{	warning("optimize_fixed: solution check failed");
	}
	//
	// must call finalize_solution before getting fixed_opt
	return fixed_nlp->fixed_opt();
}

} // END_DISMOD_AT_NAMESPACE
