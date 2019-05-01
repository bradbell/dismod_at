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
	cen
	nonsmooth
$$

$section C++: Get the Density Table Information$$

$head Syntax$$
$icode%density_table% = get_density_table(%db%)
%$$
$icode%is_log% = log_density(%density%)
%$$
$icode%is_nonsmooth% = nonsmooth_density(%density%)
%$$

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
This enum type has the following values:
$align right$$
$table
$icode value$$             $pre  $$ $cnext $icode density_name$$ $rnext
$code uniform_enum$$       $pre  $$ $cnext $code uniform$$       $rnext
$code gaussian_enum$$      $pre  $$ $cnext $code gaussian$$      $rnext
$code cen_gaussian_enum$$  $pre  $$ $cnext $code c_gaussian$$    $rnext
$code log_gaussian_enum$$  $pre  $$ $cnext $code log_gaussian$$  $rnext
$code laplace_enum$$       $pre  $$ $cnext $code laplace$$       $rnext
$code cen_laplace_enum$$   $pre  $$ $cnext $code laplace$$       $rnext
$code log_laplace_enum$$   $pre  $$ $cnext $code log_laplace$$   $rnext
$code students_enum$$      $pre  $$ $cnext $code students$$      $rnext
$code log_students_enum$$  $pre  $$ $cnext $code log_students$$
$tend
The number of these enum values is $code number_density_enum$$.

$head log_density$$
The return value $icode is_log$$ is true if the corresponding density is
$cref/log scaled/density_table/Notation/Log Scaled/$$.
It is false otherwise.

$head nonsmooth_density$$
The return value $icode is_nonsmooth$$ is true if the corresponding density is
$cref/nonsmooth/density_table/Notation/Nonsmooth/$$.
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
	"cen_gaussian",
	"log_gaussian",
	"laplace",
	"cen_laplace",
	"log_laplace",
	"students",
	"log_students"
};


CppAD::vector<density_enum> get_density_table(sqlite3* db)
{	using std::string;
	using CppAD::to_string;
	using CppAD::vector;
	//
	// for error messaging
	string message;
	//
	// check density names in same order as enum type in get_density_table.hpp
# ifndef NDEBUG
	const char** enum2name = density_enum2name;
	assert( string("uniform")          == enum2name[uniform_enum] );
	assert( string("gaussian")         == enum2name[gaussian_enum] );
	assert( string("cen_gaussian")     == enum2name[cen_gaussian_enum] );
	assert( string("log_gaussian")     == enum2name[log_gaussian_enum] );
	assert( string("cen_laplace")      == enum2name[cen_laplace_enum] );
	assert( string("laplace")          == enum2name[laplace_enum] );
	assert( string("log_laplace")      == enum2name[log_laplace_enum] );
	assert( string("students")         == enum2name[students_enum] );
	assert( string("log_students")     == enum2name[log_students_enum] );
# endif
	//
	vector<bool> found(number_density_enum);
	for(size_t i = 0; i < size_t(number_density_enum); i++)
		found[i] = false;
	//
	string table_name  = "density";
	size_t n_density   = check_table_id(db, table_name);
	//
	string column_name =  "density_name";
	CppAD::vector<string>  density_name;
	get_table_column(db, table_name, column_name, density_name);
	assert( n_density == density_name.size() );
	//
	CppAD::vector<density_enum> density_table(n_density);
	for(size_t density_id = 0; density_id < n_density; density_id++)
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
		if( found[density] )
		{	string msg = "The density_name " + name;
			msg += " appears more than once";
			error_exit(msg, table_name, density_id);
		}
		found[density] = true;
		density_table[density_id] = density;
	}
	return density_table;
}
bool log_density(density_enum density)
{	bool result = density == log_gaussian_enum;
	result     |= density == log_laplace_enum;
	result     |= density == log_students_enum;
	return result;
}
bool nonsmooth_density(density_enum density)
{	bool result = density == laplace_enum;
	result     |= density == cen_laplace_enum;
	result     |= density == log_laplace_enum;
	return result;
}

} // END DISMOD_AT_NAMESPACE
