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

$head rate_table$$
The return value $icode rate_table$$ has prototype
$codei%
	CppAD::vector<rate_enum>  %rate_table%
%$$
For each $cref/rate_id/rate_table/rate_id/$$,
$codei%
	%rate_table%[%rate_id%]
%$$
is the enum value for the corresponding
$cref/rate_name/rate_table/rate_name/$$.


$children%example/cpp/get_rate_table_xam.cpp
%$$
$head Example$$
The file $cref get_rate_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/


# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// rate names in same order as enum type in get_rate_table.hpp
const char* rate_enum2name[] = {
	"iota",
	"rho",
	"chi",
	"omega"
};

CppAD::vector<rate_enum> get_rate_table(sqlite3* db)
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

	CppAD::vector<rate_enum> rate_table(number_rate_enum);
	for(size_t i = 0; i < number_rate_enum; i++)
	{	bool found = false;
		for( size_t j = 0; j < number_rate_enum; j++)
		{	if( rate_enum2name[j] == rate_name[i] )
			{	rate_table[i] = rate_enum(j);
				found         = true;
			}
		}
		if( ! found )
		{	string s = "rate_name is not iota, rho, chi, or omega.";
			table_error_exit("rate", i, s);
		}
	}
	return rate_table;
}

} // END DISMOD_AT_NAMESPACE
