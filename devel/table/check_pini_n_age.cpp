// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin check_pini_n_age$$
$spell
	const
	CppAD
	struct
	pini
	sqlite
$$

$section Check Initial Prevalence Grid Has One Age$$

$head syntax$$
$codei%check_pini_n_age(%db%, %rate_table%, %smooth_table%)%$$

$head db$$
This argument has prototype
$codei%
	sqlite3* %db%
%$$
and is the database connection for $cref/logging/log_message/$$ errors.

$head rate_table$$
This argument has prototype
$codei%
	const CppAD::vector<rate_struct>& %rate_table%
%$$
and it is the
$cref/rate_table/get_rate_table/rate_table/$$.
For this table,
only the fields $code parent_smooth_id$$ and $code child_smooth_id$$
are used.

$head smooth_table$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_struct>& %smooth_table%
%$$
and it is the
$cref/smooth_table/get_smooth_table/smooth_table/$$.
For this table, only the $code n_age$$ field is used.

$end
*/
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_smooth_table.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void check_pini_n_age(
	sqlite3*                            db            ,
	const CppAD::vector<rate_struct>&   rate_table    ,
	const CppAD::vector<smooth_struct>& smooth_table  )
{	assert( rate_table.size()   == number_rate_enum );
	std::string message;
	std::string table_name = "rate";
	//
	size_t rate_id = size_t( pini_enum );
	size_t parent_smooth_id = rate_table[rate_id].parent_smooth_id;
	if( parent_smooth_id != DISMOD_AT_NULL_SIZE_T )
	{	size_t n_age = smooth_table[parent_smooth_id].n_age;
		if( n_age != 1 )
		{	message = "parent_smooth_id, for pini, corresponds to a smoothing"
				" with n_age not equal to one";
		}
	}
	size_t child_smooth_id  = rate_table[rate_id].child_smooth_id;
	if( child_smooth_id != DISMOD_AT_NULL_SIZE_T )
	{	size_t n_age = smooth_table[child_smooth_id].n_age;
		if( n_age != 1 )
		{	message = "child_smooth_id, for pini, corresponds to a smoothing"
				" with n_age not equal to one";
		}
	}
	if( message != "" )
		error_exit(message, table_name, rate_id);
}

} // END DISMOD_AT_NAMESPACE
