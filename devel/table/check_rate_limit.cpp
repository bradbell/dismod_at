// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin check_rate_limit$$
$spell
	const
	CppAD
	struct
	pini
	sqlite
$$

$section Check Lower and Upper Iota and Rho Limits$$

$head syntax$$
$codei%check_rate_limit(
	%db%, %rate_case%, %rate_table%, %prior_table%, %smooth_grid%
)%$$

$head db$$
This argument has prototype
$codei%
	sqlite3* %db%
%$$
and is the database connection for $cref/logging/log_message/$$ errors.

$head rate_case$$
This is the option table information about the limits for
$icode iota$$ and $icode rho$$; see
$cref/rate_case/option_table/rate_case/$$.

$head rate_table$$
This argument has prototype
$codei%
	const CppAD::vector<rate_struct>& %rate_table%
%$$
and it is the
$cref/rate_table/get_rate_table/rate_table/$$.
For this table,
only the $code parent_smooth_id$$ column is used.

$head prior_table$$
This argument has prototype
$codei%
	const CppAD::vector<prior_struct>& %prior_table%
%$$
and it is the
$cref/prior_table/get_prior_table/prior_table/$$.
For this table, only the $code lower$$ and $code upper$$ fields are used.

$head smooth_grid$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_grid_struct>& %smooth_grid%
%$$
and it is the
$cref/smooth_grid_table/get_smooth_grid/smooth_grid/$$.
For this table, only the $code value_prior_id$$ field is used.

$end
*/
# include <dismod_at/check_rate_limit.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/to_string.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void check_rate_limit(
	sqlite3*                                  db            ,
	const std::string&                        rate_case     ,
	const CppAD::vector<rate_struct>&         rate_table    ,
	const CppAD::vector<prior_struct>&        prior_table   ,
	const CppAD::vector<smooth_grid_struct>&  smooth_grid   )
{	assert( rate_table.size()   == number_rate_enum );
	//
	CppAD::vector<bool> rate_zero(number_rate_enum);
	if( rate_case == "iota_zero_rho_zero" )
	{	rate_zero[iota_enum] = true;
		rate_zero[rho_enum]  = true;
	}
	else if( rate_case == "iota_pos_rho_zero" )
	{	rate_zero[iota_enum] = false;
		rate_zero[rho_enum]  = true;
	}
	else if( rate_case == "iota_zero_rho_pos" )
	{	rate_zero[iota_enum] = true;
		rate_zero[rho_enum]  = false;
	}
	else if( rate_case == "iota_pos_rho_pos" )
	{	rate_zero[iota_enum] = false;
		rate_zero[rho_enum]  = false;
	}
	size_t n_grid = smooth_grid.size();
	size_t error_rate_id    = number_rate_enum;
	size_t error_smooth_id  = 0;
	size_t error_grid_id    = 0;
	size_t error_prior_id   = 0;
	bool   error_zero       = false;
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	if( rate_enum(rate_id) == iota_enum || rate_enum(rate_id) == rho_enum )
	{	size_t smooth_id = rate_table[rate_id].parent_smooth_id;
		for(size_t grid_id = 0; grid_id < n_grid; grid_id++)
		if( size_t( smooth_grid[grid_id].smooth_id ) == smooth_id )
		{	size_t prior_id = smooth_grid[grid_id].value_prior_id;
			double lower    = prior_table[prior_id].lower;
			double upper    = prior_table[prior_id].upper;
			bool error;
			if( rate_zero[rate_id] )
				error = upper != 0.0;
			else
				error = lower <= 0.0;
			if( error )
			{
				error_rate_id   = rate_id;
				error_smooth_id = smooth_id;
				error_grid_id   = grid_id;
				error_prior_id  = prior_id;
				error_zero     = rate_zero[rate_id];
			}
		}
	}
	if( error_rate_id < number_rate_enum )
	{
		std::string message;
		if( error_zero)
			message = "expected prior upper limit to be == 0.0 for";
		else
			message = "expected prior lower limit to be > 0.0 for";
		if( rate_enum(error_rate_id) == iota_enum )
			message += " iota\n";
		else
			message += " rho\n";
		message += "rate_case = " + rate_case;
		message += ", parent_smooth_id = " + to_string(error_smooth_id);
		message += "\nsmooth_grid_id = " + to_string(error_grid_id);
		message += ", value_prior_id = " + to_string(error_prior_id);
		std::string table_name = "rate";
		error_exit(db, message, table_name, error_rate_id);
	}
}

} // END DISMOD_AT_NAMESPACE
