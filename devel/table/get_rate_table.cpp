// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_rate_table$$
$spell
	struct
	sqlite
	enum
	cpp
	pini
	nslist
$$

$section C++: Get the Rate Table Information$$
$index get, rate table$$
$index rate, get table$$
$index table, get rate$$

$head Syntax$$
$icode%rate_table% = get_rate_table(%db%)%$$

$head Purpose$$
To read the $cref rate_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head rate_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code rate_enum$$ $cnext $code rate$$  $cnext
	enum corresponding to $cref/rate_name/rate_table/rate_name/$$
$rnext
$code int$$ $cnext $code parent_smooth_id$$  $cnext
	The $cref/parent_smooth_id/rate_table/parent_smooth_id/$$
$rnext
$code int$$ $cnext $code child_smooth_id$$  $cnext
	The $cref/child_smooth_id/rate_table/child_smooth_id/$$
$rnext
$code int$$ $cnext $code child_nslist_id$$  $cnext
	The $cref/child_nslist_id/rate_table/child_nslist_id/$$
$tend

$head rate_table$$
The return value $icode rate_table$$ has prototype
$codei%
	CppAD::vector<rate_struct>  %rate_table%
%$$
For each $cref/rate_id/rate_table/rate_id/$$,
$codei%
	%rate_table%[%rate_id%]
%$$
is the information value for the corresponding
$cref/rate_id/rate_table/rate_id/$$.

$head rate_enum$$
The $icode rate_table$$ can also be addressed
using the $code rate_enum$$ values as follows:
$table
$icode rate_id$$    $pre  $$ $cnext $icode rate_name$$     $rnext
$code pini_enum$$   $pre  $$ $cnext $code pini$$           $rnext
$code iota_enum$$   $pre  $$ $cnext $code iota$$           $rnext
$code rho_enum$$    $pre  $$ $cnext $code rho$$            $rnext
$code chi_enum$$    $pre  $$ $cnext $code chi$$            $rnext
$code omega_enum$$  $pre  $$ $cnext $code omega$$
$tend
The number of these enum values is $code number_rate_enum$$.


$children%example/devel/table/get_rate_table_xam.cpp
%$$
$head Example$$
The file $cref get_rate_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/


# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<rate_struct> get_rate_table(sqlite3* db)
{	using std::string;
	using CppAD::to_string;
	//
	// for error messaging
	string msg;
	size_t null_id  = DISMOD_AT_NULL_SIZE_T;
	//
	// rate names in same order as enum type in get_rate_table.hpp
	// and in the documentation for rate_table.omh
	const char* rate_enum2name[] = {
		"pini",
		"iota",
		"rho",
		"chi",
		"omega"
	};
	assert( string("pini")  == rate_enum2name[pini_enum] );
	assert( string("iota")  == rate_enum2name[iota_enum] );
	assert( string("rho")   == rate_enum2name[rho_enum] );
	assert( string("chi")   == rate_enum2name[chi_enum] );
	assert( string("omega") == rate_enum2name[omega_enum] );


	string table_name   = "rate";
	size_t n_rate       = check_table_id(db, table_name);
	if( n_rate != size_t( number_rate_enum ) )
	{	msg  = "rate table does not have ";
		msg += to_string( size_t( number_rate_enum) )  + "rows.";
		error_exit(msg, table_name, null_id);
	}

	string column_name  = "rate_name";
	CppAD::vector<string>  rate_name;
	get_table_column(db, table_name, column_name, rate_name);
	assert( n_rate == rate_name.size() );

	column_name         = "parent_smooth_id";
	CppAD::vector<int>     parent_smooth_id;
	get_table_column(db, table_name, column_name, parent_smooth_id);
	assert( n_rate == parent_smooth_id.size() );

	column_name         = "child_smooth_id";
	CppAD::vector<int>     child_smooth_id;
	get_table_column(db, table_name, column_name, child_smooth_id);
	assert( n_rate == child_smooth_id.size() );

	column_name         = "child_nslist_id";
	CppAD::vector<int>     child_nslist_id;
	get_table_column(db, table_name, column_name, child_nslist_id);
	assert( n_rate == child_nslist_id.size() );

	CppAD::vector<rate_struct> rate_table(number_rate_enum);
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	if( rate_name[rate_id] != rate_enum2name[rate_id] )
		{	msg  = "expected rate_name to be ";
			msg += rate_enum2name[rate_id];
			msg += " but found " + rate_name[rate_id];
			error_exit(msg, table_name, rate_id);
		}
		if( child_smooth_id[rate_id] != DISMOD_AT_NULL_INT
		&&  child_nslist_id[rate_id] != DISMOD_AT_NULL_INT )
		{	msg = "both child_smooth_id and child_nslist_id are not null";
			error_exit(msg, table_name, rate_id);
		}
		rate_table[rate_id].rate             = rate_enum(rate_id);
		rate_table[rate_id].parent_smooth_id = parent_smooth_id[rate_id];
		rate_table[rate_id].child_smooth_id  = child_smooth_id[rate_id];
		rate_table[rate_id].child_nslist_id  = DISMOD_AT_NULL_INT;
		rate_table[rate_id].child_nslist_id  = child_nslist_id[rate_id];
	}

	return rate_table;
}

} // END DISMOD_AT_NAMESPACE
