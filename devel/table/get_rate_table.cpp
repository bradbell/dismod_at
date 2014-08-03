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
$begin get_rate_table$$
$spell
	struct
	sqlite
	enum
	cpp
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
$code rate_enum$$ $cnext $code rate_name$$  $cnext 
	The $cref/rate_name/rate_table/rate_name/$$ for this rate  
$rnext
$code int$$ $cnext $code parent_smooth_id$$  $cnext 
	The $cref/parent_smooth_id/rate_table/parent_smooth_id/$$ 
$rnext
$code int$$ $cnext $code child_smooth_id$$  $cnext 
	The $cref/child_smooth_id/rate_table/child_smooth_id/$$ 
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
This is an enum type with the following values:
$table
$icode rate$$        $pre  $$ $cnext Corresponding String    $rnext
$code iota_enum$$    $pre  $$ $cnext $code "iota"$$          $rnext        
$code rho_enum$$     $pre  $$ $cnext $code "rho"$$           $rnext        
$code chi_enum$$     $pre  $$ $cnext $code "chi"$$           $rnext        
$code omega_enum$$   $pre  $$ $cnext $code "omega"$$
$tend        
The number of these enum values is $code number_rate_enum$$.

$head rate_enum2name$$
This is a global variable. If $icode%rate% < number_rate_enum%$$, 
$codei%rate_enum2name[%rate%]%$$ is the string corresponding
to the $icode rate$$ enum value.


$children%example/devel/table/get_rate_table_xam.cpp
%$$
$head Example$$
The file $cref get_rate_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/


# include <dismod_at/include/get_rate_table.hpp>
# include <dismod_at/include/get_table_column.hpp>
# include <dismod_at/include/check_table_id.hpp>
# include <dismod_at/include/table_error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// rate names in same order as enum type in get_rate_table.hpp
const char* rate_enum2name[] = {
	"iota",
	"rho",
	"chi",
	"omega"
};

CppAD::vector<rate_struct> get_rate_table(sqlite3* db)
{	using std::string;

	string table_name   = "rate";
	size_t n_rate       = check_table_id(db, table_name);
	if( n_rate != size_t( number_rate_enum ) )
	{	using std::cerr;
		cerr << "rate table does not have ";
		cerr << size_t( number_rate_enum) << "rows." << std::endl;
		exit(1);
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

	CppAD::vector<rate_struct> rate_table(number_rate_enum);
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	bool found = false;
		for( size_t j = 0; j < number_rate_enum; j++)
		{	if( rate_enum2name[j] == rate_name[rate_id] )
			{	rate_table[rate_id].rate_name = rate_enum(j);
				found                         = true;
			}
		}
		if( ! found )
		{	string message = "rate_name is not iota, rho, chi, or omega.";
			table_error_exit("rate", rate_id, message);
		}
		rate_table[rate_id].parent_smooth_id = parent_smooth_id[rate_id];
		rate_table[rate_id].child_smooth_id  = child_smooth_id[rate_id];
	}
	return rate_table;
}

} // END DISMOD_AT_NAMESPACE
