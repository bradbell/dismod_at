// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/get_prior_mean.hpp>
# include <dismod_at/null_int.hpp>
/*
$begin get_prior_mean$$
$spell
	var
$$

$section Get Prior Mean For Each Variable$$

$head Syntax$$
$icode%prior_mean% = get_prior_mean(
	%prior_table%, %pack_prior%, %var2prior%
)%$$

$head Prototype$$
$srcfile%devel/utility/get_prior_mean.cpp%
	0%// BEGIN PROTOTYPE%// END PROTOTYPE%1
%$$

$head prior_table$$
Is the $cref/prior_table/get_prior_table/prior_table/$$.

$head var2prior$$
Is the prior information as a mapping from variable index to
prior table index.

$head prior_mean$$
The return value $icode prior_main$$
is the prior mean for each variable by
$cref/var_id/var_table/var_id/$$.

$end
*/
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN PROTOTYPE
CppAD::vector<double> get_prior_mean(
	const CppAD::vector<dismod_at::prior_struct>& prior_table ,
	const dismod_at::pack_prior&                  var2prior   )
// END PROTOTYPE
{
	// put means in return value
	size_t n_var = var2prior.size();
	CppAD::vector<double> result(n_var);
	for(size_t var_id = 0; var_id < n_var; var_id++)
	{	double var_value = var2prior.const_value(var_id);
		if( std::isnan(var_value) )
		{	size_t prior_id = var2prior.value_prior_id(var_id);
			assert( prior_id != DISMOD_AT_NULL_SIZE_T );
			var_value = prior_table[prior_id].mean;
		}
		result[var_id] = var_value;
	}
	return result;
}

} // END_DISMOD_AT_NAMESPACE
