// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_mulcov_table$$
$spell
	mulcov
	sqlite
	struct
	cpp
	std
	covariate
	enum
$$

$section C++: Get the Covariate Multiplier Table Information$$

$head Syntax$$
$icode%mulcov_table% = get_mulcov_table(%db%)%$$

$head Purpose$$
To read the $cref mulcov_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head mulcov_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code mulcov_type_enum$$ $pre  $$ $cnext $code mulcov_type$$   $pre  $$ $cnext
	See the description of $code mulcov_type$$ below.
$rnext
$code int$$ $cnext $code rate_id$$     $pre  $$ $cnext
	The $cref/rate_id/mulcov_table/rate_id/$$
	for this multiplier
$rnext
$code int$$ $cnext $code integrand_id$$     $pre  $$ $cnext
	The $cref/integrand_id/mulcov_table/integrand_id/$$
	for this multiplier
$rnext
$code int$$ $cnext $code covariate_id$$     $pre  $$ $cnext
	The $cref/covariate_id/mulcov_table/covariate_id/$$
	for this multiplier
$rnext
$code int$$ $cnext $code smooth_id$$     $pre  $$ $cnext
	The $cref/smooth_id/mulcov_table/smooth_id/$$
	for this multiplier
$tend

$subhead mulcov_type_enum$$
The $code mulcov_type_enum$$ possible values are
$code rate_value_enum$$,
$code meas_value_enum$$, and
$code meas_noise_enum$$.
These correspond to the values in the
$cref/mulcov_type/mulcov_table/mulcov_type/$$ column of the
$code mulcov_type$$ table.

$head mulcov_table$$
The return value $icode mulcov_table$$ has prototype
$codei%
	CppAD::vector<mulcov_struct>  %mulcov_table%
%$$
For each $cref/mulcov_id/mulcov_table/mulcov_id/$$,
$codei%
	%mulcov_table%[%mulcov_id%]
%$$
is the information for the corresponding multiplier.

$children%example/devel/table/get_mulcov_table_xam.cpp
%$$
$head Example$$
The file $cref get_mulcov_table_xam.cpp$$ contains an example that uses
this function.

$head found_meas_std_in_mulcov_table_$$
If this is true, get_mulcov_table found the deprecated name
$cref/meas_std/mulcov_table/mulcov_type/meas_std: Deprecated 2019-04-07/$$
in the mulcov_table.

$end
-----------------------------------------------------------------------------
*/

# include <cmath>


# include <dismod_at/get_mulcov_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/log_message.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// initialize this flag as false
bool found_meas_std_in_mulcov_table_ = false;

CppAD::vector<mulcov_struct> get_mulcov_table(sqlite3* db)
{	using std::string;

	// so we can check if both names are there
	bool found_meas_noise = false;

	string table_name   = "mulcov";
	size_t n_mulcov     = check_table_id(db, table_name);

	string column_name  = "mulcov_type";
	CppAD::vector<string>  mulcov_type;
	get_table_column(db, table_name, column_name, mulcov_type);
	assert( mulcov_type.size() == n_mulcov );

	column_name        =  "rate_id";
	CppAD::vector<int>     rate_id;
	get_table_column(db, table_name, column_name, rate_id);
	assert( rate_id.size() == n_mulcov );

	column_name        =  "integrand_id";
	CppAD::vector<int>     integrand_id;
	get_table_column(db, table_name, column_name, integrand_id);
	assert( integrand_id.size() == n_mulcov );

	column_name        =  "covariate_id";
	CppAD::vector<int>     covariate_id;
	get_table_column(db, table_name, column_name, covariate_id);
	assert( covariate_id.size() == n_mulcov );

	column_name        =  "smooth_id";
	CppAD::vector<int>     smooth_id;
	get_table_column(db, table_name, column_name, smooth_id);
	assert( smooth_id.size() == n_mulcov );

	CppAD::vector<mulcov_struct>     mulcov_table(n_mulcov);
	for(size_t i = 0; i < n_mulcov; i++)
	{
		mulcov_table[i].rate_id         = rate_id[i];
		mulcov_table[i].integrand_id    = integrand_id[i];
		mulcov_table[i].covariate_id    = covariate_id[i];
		mulcov_table[i].smooth_id       = smooth_id[i];
		//
		if( mulcov_type[i] == "rate_value" )
			mulcov_table[i].mulcov_type = rate_value_enum;
		else if( mulcov_type[i] == "meas_value" )
			mulcov_table[i].mulcov_type = meas_value_enum;
		else if( mulcov_type[i] == "meas_noise" )
		{	found_meas_noise = true;
			mulcov_table[i].mulcov_type = meas_noise_enum;
		}
		else if( mulcov_type[i] == "meas_std" )
		{	string msg  =
			"The mulcov_type meas_std was deprecated on 2019-04-07\n"
			"and may not work in the future. "
			"It should be changed to meas_noise.";
			if( ! found_meas_std_in_mulcov_table_ ) log_message(
				db, &std::cout, "warning", msg, table_name, i
			);
			found_meas_std_in_mulcov_table_ = true;
			mulcov_table[i].mulcov_type = meas_noise_enum;
		}
		else
		{	string message = "mulcov_type = '" + mulcov_type[i] + "'";
			message += " is not one of the following:\n"
				"'rate_value', 'meas_value', 'meas_noise'.";
			table_name = "mulcov";
			error_exit(message, table_name, i);
		}
	}
	if( found_meas_noise && found_meas_std_in_mulcov_table_ )
	{	string msg = "Found both meas_noise and meas_std in mulcov_table.\n";
		msg       += "Must changen remaining meas_std to meas_noise.";
		error_exit(msg, table_name);
	}
	return mulcov_table;
}

} // END DISMOD_AT_NAMESPACE
