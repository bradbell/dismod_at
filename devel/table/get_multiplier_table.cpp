// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_multiplier_table$$
$spell
	sqlite
	struct
	cpp
	std
	covariate
$$

$section C++: Get the Multiplier Table Information$$
$index get, multiplier table$$
$index multiplier, get table$$
$index table, get multiplier$$

$head Syntax$$
$icode%multiplier_table% = get_multiplier_table(%db%)%$$

$head Purpose$$
To read the $cref multiplier_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head multiplier_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code std::string$$ $cnext $code multiplier_type$$   $pre  $$ $cnext 
	The $cref/multiplier_type/multiplier_table/multiplier_type/$$ 
	for this multiplier
$rnext
$code int$$ $cnext $code rate_id$$     $pre  $$ $cnext 
	The $cref/rate_id/multiplier_table/rate_id/$$ 
	for this multiplier 
$rnext
$code int$$ $cnext $code integrand_id$$     $pre  $$ $cnext 
	The $cref/integrand_id/multiplier_table/integrand_id/$$ 
	for this multiplier
$rnext
$code int$$ $cnext $code covariate_id$$     $pre  $$ $cnext 
	The $cref/covariate_id/multiplier_table/covariate_id/$$ 
	for this multiplier
$rnext
$code int$$ $cnext $code smooth_id$$     $pre  $$ $cnext 
	The $cref/smooth_id/multiplier_table/smooth_id/$$ 
	for this multiplier 
$tend        

$head multiplier_table$$
The return value $icode multiplier_table$$ has prototype
$codei%
	CppAD::vector<multiplier_struct>  %multiplier_table%
%$$
For each $cref/multiplier_id/multiplier_table/multiplier_id/$$,
$codei%
	%multiplier_table%[%multiplier_id%]
%$$
is the information for the corresponding multiplier.

$children%example/devel/table/get_multiplier_table_xam.cpp
%$$
$head Example$$
The file $cref get_multiplier_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cmath>


# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<multiplier_struct> get_multiplier_table(sqlite3* db)
{	using std::string;

	string table_name   = "multiplier";
	size_t n_multiplier = check_table_id(db, table_name);

	string column_name  = "multiplier_type";
	CppAD::vector<string>  multiplier_type;
	get_table_column(db, table_name, column_name, multiplier_type);
	assert( multiplier_type.size() == n_multiplier );

	column_name        =  "rate_id";
	CppAD::vector<int>     rate_id;
	get_table_column(db, table_name, column_name, rate_id);
	assert( rate_id.size() == n_multiplier );

	column_name        =  "integrand_id";
	CppAD::vector<int>     integrand_id;
	get_table_column(db, table_name, column_name, integrand_id);
	assert( integrand_id.size() == n_multiplier );

	column_name        =  "covariate_id";
	CppAD::vector<int>     covariate_id;
	get_table_column(db, table_name, column_name, covariate_id);
	assert( covariate_id.size() == n_multiplier );

	column_name        =  "smooth_id";
	CppAD::vector<int>     smooth_id;
	get_table_column(db, table_name, column_name, smooth_id);
	assert( smooth_id.size() == n_multiplier );

	CppAD::vector<multiplier_struct> multiplier_table(n_multiplier);
	for(size_t i = 0; i < n_multiplier; i++)
	{	multiplier_table[i].multiplier_type = multiplier_type[i];
		multiplier_table[i].rate_id         = rate_id[i];
		multiplier_table[i].integrand_id    = integrand_id[i];
		multiplier_table[i].covariate_id    = covariate_id[i];
		multiplier_table[i].smooth_id       = smooth_id[i];
	}
	return multiplier_table;
}

} // END DISMOD_AT_NAMESPACE
