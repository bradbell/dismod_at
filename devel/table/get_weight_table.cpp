// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-22 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_weight_table$$
$spell
	struct
	sqlite
	enum
	cpp
	std
$$

$section C++: Get the Weight Table Information$$

$head Syntax$$
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
	CppAD::vector<weight_struct>  %weight_table%
%$$
For each $cref/weight_id/weight_table/weight_id/$$,
$codei%
	%weight_table%[%weight_id%]
%$$
is the information for the corresponding
$cref/weight_id/weight_table/weight_id/$$.

$head weight_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code std::string$$ $cnext $code weight_name$$ $cnext
	The $cref/weight_name/weight_table/weight_name/$$ for this weighting.
$rnext
$code int$$ $cnext $code n_age$$ $cnext
	The $cref/n_age/weight_table/n_age/$$ for this weighting.
$rnext
$code int$$ $cnext $code n_time$$ $cnext
	The $cref/n_time/weight_table/n_time/$$ for this weighting.
$tend

$comment
   example/devel/table/get_weight_grid_xam.cpp is included by weight_grid.omh
%$$
$head Example$$
The file $cref get_weight_grid_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/get_weight_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<weight_struct> get_weight_table(sqlite3* db)
{	using std::string;

	string table_name  = "weight";
	size_t n_weight    = check_table_id(db, table_name);

	string column_name =  "weight_name";
	CppAD::vector<string>  weight_name;
	get_table_column(db, table_name, column_name, weight_name);
	assert( weight_name.size() == n_weight );

	column_name        =  "n_age";
	CppAD::vector<int>     n_age;
	get_table_column(db, table_name, column_name, n_age);
	assert( n_age.size() == n_weight );

	column_name        =  "n_time";
	CppAD::vector<int>     n_time;
	get_table_column(db, table_name, column_name, n_time);
	assert( n_time.size() == n_weight );

	CppAD::vector<weight_struct> weight_table(n_weight);
	for(size_t i = 0; i < n_weight; i++)
	{	weight_table[i].weight_name   = weight_name[i];
		weight_table[i].n_age         = n_age[i];
		weight_table[i].n_time        = n_time[i];
	}
	return weight_table;
}

} // END DISMOD_AT_NAMESPACE
