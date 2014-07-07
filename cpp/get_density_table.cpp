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
$begin get_density_table$$
$spell
	sqlite
	enum
	cpp
$$

$section C++: Get the Density Table Information$$
$index get, density table$$
$index density, get table$$
$index table, get density$$

$head Syntax$$
$codei%# include <get_density_table>
%$$
$icode%density_table% = get_density_table(%db%)%$$

$head Purpose$$
To read the $cref density_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head density_enum$$
This is an enum type with the following values:
$table
$icode density$$          $pre  $$ $cnext Corresponding String    $rnext
$code uniform_enum$$      $pre  $$ $cnext $code "gaussian"$$      $rnext
$code gaussian_enum$$     $pre  $$ $cnext $code "gaussian"$$      $rnext
$code laplace_enum$$      $pre  $$ $cnext $code "laplace"$$       $rnext        
$code log_gaussian_enum$$ $pre  $$ $cnext $code "log_gaussian"$$  $rnext
$code log_laplace_enum$$  $pre  $$ $cnext $code "log_laplace"$$
$tend        
The number of these enum values is $code number_density_enum$$.

$head density_enum2name$$
This is a global variable. If $icode%density% < number_density_enum%$$, 
$codei%density_enum2name[%density%]%$$ is the string corresponding
to the $icode density$$ enum value.

$head density_table$$
The return value $icode density_table$$ has prototype
$codei%
	CppAD::vector<density_enum>  %density_table%
%$$
For each $cref/density_id/density_table/density_id/$$,
$codei%
	%density_table%[%density_id%]
%$$
is the enum value for the corresponding
$cref/density_name/density_table/density_name/$$.


$children%example/cpp/get_density_table_xam.cpp
%$$
$head Example$$
The file $cref get_density_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/


# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// density names in same order as enum type in get_density_table.hpp
const char* density_enum2name[] = {
	"uniform",
	"gaussian",
	"laplace",
	"log_gaussian",
	"log_laplace"
};

CppAD::vector<density_enum> get_density_table(sqlite3* db)
{	using std::string;

	string table_name  = "density";
	size_t n_density   = check_table_id(db, table_name);

	if( n_density != size_t( number_density_enum ) )
	{	using std::cerr;
		cerr << "density table does not have ";
		cerr << size_t( number_density_enum) << "rows." << std::endl;
		exit(1);
	}

	string column_name =  "density_name";
	CppAD::vector<string>  density_name;
	get_table_column(db, table_name, column_name, density_name);
	assert( n_density == density_name.size() );

	CppAD::vector<density_enum> density_table(number_density_enum);
	for(size_t i = 0; i < number_density_enum; i++)
	{	bool found = false;
		for( size_t j = 0; j < number_density_enum; j++)
		{	if( density_enum2name[j] == density_name[i] )
			{	density_table[i] = density_enum(j);
				found         = true;
			}
		}
		if( ! found )
		{	string s = density_name[i] + " is not a valid density_name.";
			table_error_exit("density", i, s);
		}
	}
	return density_table;
}

} // END DISMOD_AT_NAMESPACE
