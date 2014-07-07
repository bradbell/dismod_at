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
$begin get_smooth_table$$
$spell
	sqlite
	enum
	cpp
	std
$$

$section C++: Get the Smoothing Table Information$$
$index get, smooth table$$
$index smooth, get table$$
$index table, get smooth$$

$head Syntax$$
$codei%# include <get_smooth_table>
%$$
$icode%smooth_table% = get_smooth_table(%db%)%$$

$head Purpose$$
To read the $cref smooth_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head smooth_table$$
The return value $icode smooth_table$$ has prototype
$codei%
	CppAD::vector<std::string>  %smooth_table%
%$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%smooth_table%[%smooth_id%]
%$$
is the string for the corresponding
$cref/smooth_name/smooth_table/smooth_name/$$.

$comment%example/cpp/get_smooth_grid_xam.cpp in included by smooth_grid.omh
%$$
$head Example$$
The file $cref get_smooth_grid_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/



# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<std::string> get_smooth_table(sqlite3* db)
{	using std::string;

	string table_name  = "smooth";
	string column_name = "smooth_id";
	CppAD::vector<int>    smooth_id;
	get_table_column(db, table_name, column_name, smooth_id);
	size_t n_smooth = smooth_id.size();
	
	column_name        =  "smooth_name";
	CppAD::vector<string>  smooth_name;
	get_table_column(db, table_name, column_name, smooth_name);
	assert( smooth_name.size() == n_smooth );

	for(size_t i = 0; i < n_smooth; i++)
	{	if( smooth_id[i] != i )
		{	string s = "smooth_id must start at zero and increment by one.";
			table_error_exit("smooth", i, s);
		}
	}
	return smooth_name;
}

} // END DISMOD_AT_NAMESPACE
