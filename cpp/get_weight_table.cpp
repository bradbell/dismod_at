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
$begin get_weight_table$$
$spell
	sqlite
	enum
	cpp
	std
$$

$section C++: Get the Weight Table Information$$
$index get, weight table$$
$index weight, get table$$
$index table, get weight$$

$head Syntax$$
$codei%# include <get_weight_table>
%$$
$icode%weight_table% = get_weight_table(%db%)%$$

$head Purpose$$
To read the $cref weight_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head weight_table$$
The return value $icode weight_table$$ has prototype
$codei%
	CppAD::vector<std::string>  %weight_table%
%$$
For each $cref/weight_id/weight_table/weight_id/$$,
$codei%
	%weight_table%[%weight_id%]
%$$
is the string for the corresponding
$cref/weight_name/weight_table/weight_name/$$.

$comment%example/cpp/get_weight_grid_xam.cpp in included by weight_grid.omh
%$$
$head Example$$
The file $cref get_weight_grid_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/



# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<std::string> get_weight_table(sqlite3* db)
{	using std::string;

	string table_name  = "weight";
	size_t n_weight    = check_table_id(db, table_name);
	
	string column_name =  "weight_name";
	CppAD::vector<string>  weight_name;
	get_table_column(db, table_name, column_name, weight_name);
	assert( weight_name.size() == n_weight );

	return weight_name;
}

} // END DISMOD_AT_NAMESPACE
