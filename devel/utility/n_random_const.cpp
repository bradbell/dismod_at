// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin number_random_const$$
$spell
	const
	vec
	var
$$

$section Determine Number of Random Effects that are Constant$$

$head Syntax$$
$icode%n_random_const% = number_random_const(
	%bound_random%, %pack_object%, %var2prior%, %prior_table%
)%$$

$head bound_random$$
This is the value of the
$cref/bound_random/option_table/Random Only/bound_random/$$
in the option table.

$head pack_object$$
This argument is the $cref pack_info$$ information corresponding to the
$cref model_variables$$.

$head var2prior$$
This argument is the $cref pack_prior$$ information corresponding to the
$cref model_variables$$.

$head prior_table$$
This argument is the $cref/prior_table/get_prior_table/prior_table/$$.

$head Prototype$$
$srcfile%devel/utility/n_random_const.cpp
	%0%// BEGIN PROTOTYPE%// END PROTOTYPE%1%
%$$

$end
*/
# include <dismod_at/n_random_const.hpp>
# include <dismod_at/random_effect.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/pack_prior.hpp>

namespace dismod_at {
	// BEGIN PROTOTYPE
	size_t number_random_const(
		double                               bound_random ,
		const pack_info&                     pack_object  ,
	    const pack_prior&                    var2prior    ,
		const CppAD::vector<prior_struct>&   prior_table  )
	// END PROTOTYPE
	{	assert( bound_random >= 0.0 );
		//
		// mapping from random effects to all variables
		CppAD::vector<size_t> pack_index = random2var_id(pack_object);
		assert( pack_index.size() == pack_object.random_size() );
		//
		if( bound_random == 0.0 )
		{	// all random effects are the constant zero
			return pack_index.size();
		}
		//
		// count how many random effects are constant
		size_t n_random_const = 0;
		for(size_t j = 0; j < pack_index.size(); j++)
		{	size_t var_id      = pack_index[j];
			size_t prior_id    = var2prior.value_prior_id(var_id);
			double const_value = var2prior.const_value(var_id);
			if( ! std::isnan(const_value) )
				++n_random_const;
			else if( prior_id != DISMOD_AT_NULL_SIZE_T )
			{	double lower = prior_table[prior_id].lower;
				double upper = prior_table[prior_id].upper;
				if( lower ==  upper )
					++n_random_const;
			}
		}
		return n_random_const;
	}
}
