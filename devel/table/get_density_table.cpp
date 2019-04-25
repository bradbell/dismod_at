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
$begin get_density_table$$
$spell
	sqlite
	enum
	cpp
$$

$section C++: Get the Density Table Information$$

$head Syntax$$
$icode%density_table% = get_density_table(%db%)
%$$
$icode%is_log% = log_density(%density%)%$$

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

$head density_enum$$
The $icode density_table$$ can also be addressed
using the $code density_enum$$ values as follows:
$table
$icode density_id$$        $pre  $$ $cnext $icode density_name$$ $rnext
$code uniform_enum$$       $pre  $$ $cnext $code uniform$$       $rnext
$code gaussian_enum$$      $pre  $$ $cnext $code gaussian$$      $rnext
$code laplace_enum$$       $pre  $$ $cnext $code laplace$$       $rnext
$code students_enum$$      $pre  $$ $cnext $code students$$      $rnext
$code log_gaussian_enum$$  $pre  $$ $cnext $code log_gaussian$$  $rnext
$code log_laplace_enum$$   $pre  $$ $cnext $code log_laplace$$
$code log_students_enum$$  $pre  $$ $cnext $code log_students$$
$tend
The number of these enum values is $code number_density_enum$$.

$head log_density$$
The return value $icode is_log$$ is true if the corresponding density is
log_gaussian, log_laplace, or log_students.
It is false otherwise.

$head density_enum2name$$
This is a global variable.
If $icode%density%$$, is an $code density_enum$$ value,
$codei%density_enum2name[%density%]%$$ is the
$icode density_name$$ corresponding to the enum value.


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
# include <dismod_at/error_exit.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// density names in scame order as enum type in get_density.hpp
const char* density_enum2name[] = {
	"uniform",
	"gaussian",
	"laplace",
	"students",
	"log_gaussian",
	"log_laplace",
	"log_students"
};


CppAD::vector<density_enum> get_density_table(sqlite3* db)
{	using std::string;
	using CppAD::to_string;
	//
	// for error messaging
	string message;
	size_t null_id  = DISMOD_AT_NULL_SIZE_T;
	//
	// check density names in same order as enum type in get_density_table.hpp
	assert( string("uniform")      == density_enum2name[uniform_enum] );
	assert( string("gaussian")     == density_enum2name[gaussian_enum] );
	assert( string("laplace")      == density_enum2name[laplace_enum] );
	assert( string("students")     == density_enum2name[students_enum] );
	assert( string("log_gaussian") == density_enum2name[log_gaussian_enum] );
	assert( string("log_laplace")  == density_enum2name[log_laplace_enum] );
	assert( string("log_students") == density_enum2name[log_students_enum] );
	//
	string table_name  = "density";
	size_t n_density   = check_table_id(db, table_name);

	if( n_density != size_t( number_density_enum ) )
	{	message  = "density table does not have ";
		message += to_string( size_t( number_density_enum) ) + " rows.";
		error_exit(message, table_name, null_id);
	}

	string column_name =  "density_name";
	CppAD::vector<string>  density_name;
	get_table_column(db, table_name, column_name, density_name);
	assert( n_density == density_name.size() );

	CppAD::vector<density_enum> density_table(number_density_enum);
	for(size_t density_id = 0; density_id < number_density_enum; density_id++)
	{	string name          = density_name[density_id];
		density_enum density = number_density_enum;
		for(size_t j = 0; j < number_density_enum; ++j)
		{	if( name == density_enum2name[j] )
				density = density_enum(j);
		}
		if( density == number_density_enum )
		{	string msg  = name;
			msg        += " is not a valid choice for a density_name";
			error_exit(msg, table_name, density_id);
		}
		density_table[density_id] = density;
	}
	return density_table;
}
// log_density is a function so that if we add another type of log density
// all the current detection adapts to include the new density.
bool log_density(density_enum density)
{	bool result = density == log_gaussian_enum;
	result     |= density == log_laplace_enum;
	result     |= density == log_students_enum;
	return result;
}

} // END DISMOD_AT_NAMESPACE
