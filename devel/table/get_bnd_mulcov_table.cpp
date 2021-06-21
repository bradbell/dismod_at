// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_bnd_mulcov_table$$
$spell
	sqlite
	struct
	bnd_mulcov
$$

$section C++: Get the Data Subset Table$$

$head Syntax$$
$icode%bnd_mulcov_table_table% = get_bnd_mulcov_table(%db%)%$$

$head Prototype$$
$srcthisfile%0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1%$$

$head Purpose$$
To read the $cref bnd_mulcov_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head bnd_mulcov_table$$
For each $cref/bnd_mulcov_id/bnd_mulcov_table/bnd_mulcov_id/$$,
$codei%
	%bnd_mulcov_table%[%bnd_mulcov_id%]
%$$
is the information for the corresponding
$icode bnd_mulcov_id$$.

$head bnd_mulcov_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code double$$ $cnext $code max_upper$$ $cnext
	The $cref/max_upper/bnd_mulcov_table/max_upper/$$
$tend

$children%example/devel/table/get_bnd_mulcov_table_xam.cpp
%$$
$head Example$$
The file $cref get_bnd_mulcov_table_xam.cpp$$ contains an example
and test of this function.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_bnd_mulcov_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector<bnd_mulcov_struct> get_bnd_mulcov_table(sqlite3* db)
// END_PROTOTYPE
{	using std::string;

	string table_name     = "bnd_mulcov";
	size_t n_bnd_mulcov   = check_table_id(db, table_name);

	string column_name    =   "max_upper";
	CppAD::vector<double>        max_upper;
	get_table_column(db, table_name, column_name, max_upper);
	assert( max_upper.size() == n_bnd_mulcov );

	CppAD::vector<bnd_mulcov_struct> bnd_mulcov_table(n_bnd_mulcov);
	for(size_t i = 0; i < n_bnd_mulcov; i++)
	{	bnd_mulcov_table[i].max_upper  = max_upper[i];
	}
	return bnd_mulcov_table;
}

} // END DISMOD_AT_NAMESPACE
