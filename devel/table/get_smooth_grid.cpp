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
$begin get_smooth_grid$$
$spell
	sqlite
	struct
	cpp
	std
	dage
	dtime
$$

$section C++: Get the Smoothing Grid Information$$
$index get, smooth grid$$
$index smooth, get grid$$
$index grid, get smooth$$

$head Syntax$$
$icode%smooth_grid_table% = get_smooth_grid(%db%)%$$

$head Purpose$$
To read the $cref smooth_grid_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head smooth_grid$$
The return value $icode smooth_grid$$ has prototype
$codei%
	CppAD::vector<smooth_grid_struct>  %smooth_grid%
%$$
For each $cref/smooth_grid_id/smooth_grid_table/smooth_grid_id/$$,
$codei%
	%smooth_grid%[%smooth_grid_id%]
%$$
is the information for a specific smoothing and specific age and time.

$head smooth_grid_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code int$$ $cnext $code smooth_id$$   $pre  $$ $cnext 
	The $cref/smooth_id/smooth_grid_table/smooth_id/$$ for this smoothing. 
$rnext
$code int$$ $cnext $code age_id$$      $pre  $$ $cnext 
	The $cref/age_id/smooth_grid_table/age_id/$$ for this grid point
$rnext
$code int$$ $cnext $code time_id$$      $pre  $$ $cnext 
	The $cref/time_id/smooth_grid_table/time_id/$$ for this grid point
$rnext
$code int$$ $cnext $code value_prior_id$$    $pre  $$ $cnext 
	The $cref/value_prior_id/smooth_grid_table/value_prior_id/$$
	for this smoothing, age, and time.
$rnext
$code int$$ $cnext $code dage_prior_id$$    $pre  $$ $cnext 
	The $cref/dage_prior_id/smooth_grid_table/dage_prior_id/$$
	for this smoothing, age, and time.
$rnext
$code int$$ $cnext $code dtime_prior_id$$    $pre  $$ $cnext 
	The $cref/dtime_prior_id/smooth_grid_table/dtime_prior_id/$$
	for this smoothing, age, and time.
$tend        

$children%example/devel/table/get_smooth_grid_xam.cpp
%$$
$head Example$$
The file $cref get_smooth_grid_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cmath>


# include <dismod_at/include/get_smooth_grid.hpp>
# include <dismod_at/include/get_table_column.hpp>
# include <dismod_at/include/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<smooth_grid_struct> get_smooth_grid(sqlite3* db)
{	using std::string;

	string table_name  = "smooth_grid";
	size_t n_smooth    = check_table_id(db, table_name);

	string column_name = "smooth_id";
	CppAD::vector<int>    smooth_id;
	get_table_column(db, table_name, column_name, smooth_id);
	assert( smooth_id.size() == n_smooth );

	column_name        =  "age_id";
	CppAD::vector<int>     age_id;
	get_table_column(db, table_name, column_name, age_id);
	assert( age_id.size() == n_smooth );

	column_name        =  "time_id";
	CppAD::vector<int>     time_id;
	get_table_column(db, table_name, column_name, time_id);
	assert( time_id.size() == n_smooth );

	column_name        =  "value_prior_id";
	CppAD::vector<int>     value_prior_id;
	get_table_column(db, table_name, column_name, value_prior_id);
	assert( value_prior_id.size() == n_smooth );


	column_name        =  "dage_prior_id";
	CppAD::vector<int>     dage_prior_id;
	get_table_column(db, table_name, column_name, dage_prior_id);
	assert( dage_prior_id.size() == n_smooth );

	column_name        =  "dtime_prior_id";
	CppAD::vector<int>     dtime_prior_id;
	get_table_column(db, table_name, column_name, dtime_prior_id);
	assert( dtime_prior_id.size() == n_smooth );

	CppAD::vector<smooth_grid_struct> smooth_grid(n_smooth);
	for(size_t i = 0; i < n_smooth; i++)
	{	smooth_grid[i].smooth_id      = smooth_id[i];
		smooth_grid[i].age_id         = age_id[i];
		smooth_grid[i].time_id        = time_id[i];
		smooth_grid[i].value_prior_id = value_prior_id[i];
		smooth_grid[i].dage_prior_id  = dage_prior_id[i];
		smooth_grid[i].dtime_prior_id = dtime_prior_id[i];
	}
	return smooth_grid;
}

} // END DISMOD_AT_NAMESPACE
