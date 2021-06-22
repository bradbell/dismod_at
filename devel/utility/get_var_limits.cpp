// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin variable_limits$$
$spell
	const
	vec
	var
$$

$section Determine Limits in Variable ID Order$$

$head Syntax$$
$icode%get_var_limits(
	%lower_bound%,
	%upper_bound%,
	%bound_random%,
	%var2prior%,
	%prior_table%
)%$$

$head Prototype$$
$srcthisfile%0%// BEGIN PROTOTYPE%// END PROTOTYPE%1%
%$$

$head lower_limit, upper_limit$$
$icode lower_limit$$ ($icode upper_limit$$)
is a vector with size zero or equal to the number of $cref model_variables$$.
The input value of its elements does not matter.
Upon return it contains the lower (upper) limit for each variable.
This include the prior limits and the $cref bnd_mulcov_table$$ limits.


$head bound_random$$
This is the value of the
$cref/bound_random/option_table/Optimize Random Only/bound_random/$$
in the option table.

$head var2prior$$
This argument is the $cref pack_prior$$ information corresponding to the
$cref model_variables$$.

$head prior_table$$
This argument is the $cref/prior_table/get_prior_table/prior_table/$$.


$end
*/
# include <dismod_at/get_var_limits.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at {
	// BEGIN PROTOTYPE
	void get_var_limits(
		CppAD::vector<double>&               lower_limit  ,
		CppAD::vector<double>&               upper_limit  ,
		double                               bound_random ,
	    const pack_prior&                    var2prior    ,
		const CppAD::vector<prior_struct>&   prior_table  )
	// END PROTOTYPE
	{
		double inf   = std::numeric_limits<double>::infinity();
		size_t n_var = var2prior.size();
		//
		assert( bound_random >= 0.0 );
		assert( n_var == lower_limit.size() );
		assert( n_var == upper_limit.size() );
		//
		for(size_t var_id = 0; var_id < n_var; ++var_id)
		{	size_t prior_id    = var2prior.value_prior_id(var_id);
			double const_value = var2prior.const_value(var_id);
			if( ! std::isnan(const_value ) )
			{	lower_limit[var_id] = const_value;
				upper_limit[var_id] = const_value;
			}
			else if( prior_id == DISMOD_AT_NULL_SIZE_T )
			{	if( var2prior.fixed_effect(var_id) )
				{	lower_limit[var_id] = -inf;
					upper_limit[var_id] = +inf;
				}
				else
				{	lower_limit[var_id] = -bound_random;
					upper_limit[var_id] = +bound_random;
				}
			}
			else
			{
				lower_limit[var_id] = prior_table[prior_id].lower;
				upper_limit[var_id] = prior_table[prior_id].upper;
				//
				// random effects case
				if( ! var2prior.fixed_effect(var_id) )
				{	double lower = lower_limit[var_id];
					double upper = upper_limit[var_id];
					if( lower != upper )
					{	assert( lower == -inf );
						assert( upper == +inf );
						lower_limit[var_id] = -bound_random;
						upper_limit[var_id] = +bound_random;
					}
				}
			}
			// enforce the bnd_mulcov table values
			double lower        = lower_limit[var_id];
			double upper        = upper_limit[var_id];
			if( lower < upper )
			{	double max_mulcov   = var2prior.max_mulcov(var_id);
				lower_limit[var_id] = std::max(lower, - max_mulcov);
				upper_limit[var_id] = std::min(upper, max_mulcov);
			}
		}
	}
}
