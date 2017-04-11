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
$begin get_nslist_table$$
$spell
	sqlite
	enum
	cpp
	std
	nslist
$$

$section C++: Get the nslist Table Information$$

$head Syntax$$
$icode%nslist_table% = get_nslist_table(%db%)%$$

$head Purpose$$
To read the $cref nslist_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head nslist_table$$
The return value $icode nslist_table$$ has prototype
$codei%
	CppAD::vector<std::string>  %nslist_table%
%$$
For each $cref/nslist_id/nslist_table/nslist_id/$$,
$codei%
	%nslist_table%[%nslist_id%]
%$$
is the name for the corresponding
$cref/nslist_id/nslist_pair_table/nslist_id/$$.

$children%example/devel/table/get_nslist_table_xam.cpp
%$$
$head Example$$
The file $cref get_nslist_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/



# include <dismod_at/get_nslist_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<std::string> get_nslist_table(sqlite3* db)
{	using std::string;

	string table_name  = "nslist";
# ifndef NDEBUG
	size_t n_nslist       = check_table_id(db, table_name);
# else
	check_table_id(db, table_name);
# endif

	string column_name = "nslist_name";
	CppAD::vector<std::string>  nslist;
	get_table_column(db, table_name, column_name, nslist);
	assert( nslist.size() == n_nslist );

	return nslist;
}

} // END DISMOD_AT_NAMESPACE
