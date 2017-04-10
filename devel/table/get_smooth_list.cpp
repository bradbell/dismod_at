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
$begin get_smooth_list_table$$
$spell
	sqlite
	struct
	cpp
	std
	smoothings
$$

$section C++: Get the List of Smoothing Table$$

$head Syntax$$
$icode%smooth_list_table% = get_smooth_list_table(%db%)%$$

$head Purpose$$
To read the $cref smooth_list_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head smooth_list_table$$
The return value $icode smooth_list_table$$ has prototype
$codei%
	CppAD::vector<smooth_list_struct> %smooth_list_table%
%$$
For each $cref/smooth_list_id/smooth_list_table/smooth_list_id/$$,
$codei%
	%smooth_list_table%[%smooth_list_id%]
%$$
is the information for one entry in one smoothing list.

$head smooth_list_struct$$
This is a structure with the following fields
$table
$code int$$ $cnext $code density_id$$  $cnext
	$cref/smooth_list_index/smooth_list_table/smooth_list_index/$$
	identifies this list of smoothings.
$rnext
$code int$$ $cnext $code node_id$$ $cnext
	$cref/node_id/node_table/node_id/$$ is the node that this smoothing
	(in this list) applies to.
$rnext
$code int$$ $cnext $code smooth_id$$ $cnext
	$cref/smooth_id/smooth_table/smooth_id/$$ is the smoothing
	that is applied to this node (for this list).
$tend

$children%example/devel/table/get_smooth_list_xam.cpp
%$$
$head Example$$
The file $cref get_smooth_list_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/
# include <cmath>
# include <dismod_at/get_smooth_list.hpp>
# include <dismod_at/get_table_column.hpp>
# include<dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<smooth_list_struct> get_smooth_list(sqlite3* db)
{	using std::string;

	// user for error messaging
	string msg, table_name, column_name;

	table_name         = "smooth_list";
	size_t n_smooth_list  = check_table_id(db, table_name);

	column_name        =  "smooth_list_index";
	CppAD::vector<int>     smooth_list_index;
	get_table_column(db, table_name, column_name, smooth_list_index);
	assert( smooth_list_index.size() == n_smooth_list );

	column_name        =  "node_id";
	CppAD::vector<int>     node_id;
	get_table_column(db, table_name, column_name, node_id);
	assert( node_id.size() == n_smooth_list );

	column_name        =  "smooth_id";
	CppAD::vector<int>     smooth_id;
	get_table_column(db, table_name, column_name, smooth_id);
	assert( smooth_id.size() == n_smooth_list );

	CppAD::vector<smooth_list_struct> smooth_list_table(n_smooth_list);
	for(size_t i = 0; i < n_smooth_list; i++)
	{	smooth_list_table[i].smooth_list_index  = smooth_list_index[i];
		smooth_list_table[i].node_id            = node_id[i];
		smooth_list_table[i].smooth_id          = smooth_id[i];
	}
	return smooth_list_table;
}

} // END DISMOD_AT_NAMESPACE
