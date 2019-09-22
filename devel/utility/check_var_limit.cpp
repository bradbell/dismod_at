// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin check_var_limit$$
$spell
	var
$$

$section Check Lower and Upper For Variable Vector$$

$head Syntax$$
$codei%check_rate_limit(
	%table_name%, %var_value%, %var2prior%, %prior_table%
)%$$

$head Prototype$$
$srcfile%devel/utility/check_var_limit.cpp%
	0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1
%$$

$head table_name$$
This is the name of the table that the variable values come from.

$head table_offset$$
This is the offset with in the table values where the vector
$icode var_values$$ begins.

$head var_value$$
This the variable values as they appear in the table.

$head var2prior$$
This maps each variable index to the corresponding prior.

$head prior_table$$
This is the prior table.

$head Error$$
If one of the components of $icode var_value$$
is not within the specified value limits for this variable.
an error message is printed and this routine does not return.

$end
*/
# include <dismod_at/check_var_limit.hpp>
# include <dismod_at/error_exit.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/null_int.hpp>
# include <cppad/utility/nan.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
void check_var_limit(
	const std::string&                        table_name    ,
	size_t                                    table_offset  ,
	const CppAD::vector<double>&              var_value     ,
	const pack_prior&                         var2prior     ,
	const CppAD::vector<prior_struct>&        prior_table   )
// END_PROTOTYPE
{	//
	assert( var_value.size() == var2prior.size() );
	using CppAD::to_string;
	//
	for(size_t var_id = 0; var_id < var_value.size(); ++var_id)
	{	double const_value      = var2prior.const_value(var_id);
		size_t value_prior_id   = var2prior.value_prior_id(var_id);
		double lower = const_value;
		double upper = const_value;
		if( CppAD::isnan(const_value) )
		{	assert( value_prior_id != DISMOD_AT_NULL_SIZE_T );
			lower = prior_table[value_prior_id].lower;
			upper = prior_table[value_prior_id].upper;
		}
		double value = var_value[var_id];
		if( value < lower || upper < value )
		{	std::string message;
			message += "value = " + to_string( var_value[var_id] );
			message += " not within its limits";
			message += ", lower = " + to_string(lower);
			message += ", upper = " + to_string(upper);
			error_exit(message, table_name, table_offset + var_id);
		}
	}
	return;
}

} // END DISMOD_AT_NAMESPACE
