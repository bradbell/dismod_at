// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_age_table$$
$spell
	sqlite
	enum
	cpp
	std
$$

$section C++: Get the Age Table Information$$
$index get, age table$$
$index age, get table$$
$index table, get age$$

$head Syntax$$
$icode%age_table% = get_age_table(%db%)%$$

$head Purpose$$
To read the $cref age_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head age_table$$
The return value $icode age_table$$ has prototype
$codei%
	CppAD::vector<double>  %age_table%
%$$
For each $cref/age_id/age_table/age_id/$$,
$codei%
	%age_table%[%age_id%]
%$$
is the age for the corresponding
$cref/age/age_table/age/$$.

$children%example/devel/table/get_age_table_xam.cpp
%$$
$head Example$$
The file $cref get_age_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/



# include <dismod_at/get_age_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/table_error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<double> get_age_table(sqlite3* db)
{	using std::string;

	string table_name  = "age";
	size_t n_age       = check_table_id(db, table_name);

	string column_name =  "age";
	CppAD::vector<double>  age;
	get_table_column(db, table_name, column_name, age);
	assert( age.size() == n_age );

	return age;
}

} // END DISMOD_AT_NAMESPACE
