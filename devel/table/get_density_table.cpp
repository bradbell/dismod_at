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
$icode%density_table% = get_density_table(%db%)%$$

$head Purpose$$
To read the $cref density_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

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

$head Density Enum Values$$
The $icode density_table$$ can also be addressed
using the $code density_enum$$ values as follows:
$table
$icode density_id$$        $pre  $$ $cnext $icode density_name$$ $rnext
$code uniform_enum$$       $pre  $$ $cnext $code uniform$$       $rnext
$code gaussian_enum$$      $pre  $$ $cnext $code gaussian$$      $rnext
$code laplace_enum$$       $pre  $$ $cnext $code laplace$$       $rnext
$code log_gaussian_enum$$  $pre  $$ $cnext $code log_gaussian$$  $rnext
$code log_laplace_enum$$   $pre  $$ $cnext $code log_laplace$$
$tend
The number of these enum values is $code number_density_enum$$.

$children%example/devel/table/get_density_table_xam.cpp
%$$
$head Example$$
The file $cref get_density_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_density_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/table_error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE


CppAD::vector<density_enum> get_density_table(sqlite3* db)
{	using std::string;

	// density names in same order as enum type in get_density_table.hpp
	// and in th documentationf for density_table.omh
	const char* density_enum2name[] = {
		"uniform",
		"gaussian",
		"laplace",
		"log_gaussian",
		"log_laplace"
	};
	assert( string("uniform")      == density_enum2name[uniform_enum] );
	assert( string("gaussian")     == density_enum2name[gaussian_enum] );
	assert( string("laplace")      == density_enum2name[laplace_enum] );
	assert( string("log_gaussian") == density_enum2name[log_gaussian_enum] );
	assert( string("log_laplace")  == density_enum2name[log_laplace_enum] );
	//
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
	for(size_t density_id = 0; density_id < number_density_enum; density_id++)
	{	if( density_name[density_id] != density_enum2name[density_id] )
		{	string message = "expected density_name to be ";
			message += density_enum2name[density_id];
			message += " but found " + density_name[density_id];
			table_error_exit("density", density_id, message);
		}
			density_table[density_id] = density_enum(density_id);
	}
	return density_table;
}

} // END DISMOD_AT_NAMESPACE
