// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/approx_mixed.hpp>

/*
$begin approx_mixed_prior_eval$$
$spell
	eval
	vec
	const
	Cpp
$$

$section approx_mixed: Evaluate Prior for Fixed Effects$$

$head Syntax$$
$icode%vec% = %approx_object%.prior_eval(%fixed_vec%)%$$

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ at which $latex g( \theta )$$ is evaluated.

$head vec$$
The return value has prototype
$codei%
	CppAD::vector<double> %vec%
%$$
and is a
$cref/negative log-density vector/approx_mixed/Negative Log-Density Vector/$$
corresponding to the prior for the fixed effects
$cref/g(theta)/approx_mixed_theory/Prior Density, g(theta)/$$.
To be specific;
$pre
	$$
$latex g( \theta ) = $$
$icode%vec%[0] + CppAD::abs(%vec%[1]) + %...% CppAD::abs(%vec%[%s%-1])
%$$
where $icode%s% = %vec%.size()%$$.

$children%
	example/devel/approx_mixed/prior_eval_xam.cpp
%$$
$head Example$$
The file $cref prior_eval_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

CppAD::vector<double> approx_mixed::prior_eval(const d_vector& fixed_vec)
{	assert( prior_density_.Domain() == n_fixed_ );
	// make sure initialize has been called
	if( grad_ran_.size_var() == 0 )
	{	std::string error_message =
		"approx_mixed::initialize was not called before prior_eval";
		fatal_error(error_message);
	}
	return prior_density_.Forward(0, fixed_vec);
}


} // END_DISMOD_AT_NAMESPACE
