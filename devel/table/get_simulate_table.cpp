// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_simulate_table$$
$spell
	sqlite
	CppAD
	struct
	std
$$

$section C++: Get the Simulate Table$$

$head Syntax$$
$icode%simulate_table% = get_simulate_table(%db%)%$$

$head Purpose$$
To read the $cref simulate_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head simulate_table$$
The return value $icode simulate_table$$ has prototype
$codei%
	CppAD::vector<simulate_struct>  %simulate_table%
%$$
For each $cref/simulate_id/simulate_table/simulate_id/$$,
$codei%
	%simulate_table%[%simulate_id%]
%$$
is the information for the corresponding
$cref/simulate_id/simulate_table/simulate_id/$$.

$head simulate_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code int$$ $cnext $code simulate_index$$ $cnext
	The $cref/simulate_index/simulate_table/simulate_index/$$
	for this simulated measurement.
$rnext
$code int$$ $cnext $code data_subset_id$$ $cnext
	The $cref/data_subset_id/data_subset_table/data_subset_id/$$
	for this simulated measurement.
$rnext
$code double$$ $cnext $code simulate_value$$ $cnext
	The $cref/meas_value/data_table/meas_value/$$
	for this simulated measurement.
$rnext
$code double$$ $cnext $code simulate_delta$$ $cnext
	The $cref/adjusted standard deviation
	/data_like
	/Adjusted Standard Deviation, delta_i
	/$$
	for this simulated measurement.
$tend

$children%example/devel/table/get_simulate_table_xam.cpp
%$$
$head Example$$
The file $cref get_simulate_table_xam.cpp$$ contains an example
and test of this function.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_simulate_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<simulate_struct> get_simulate_table(sqlite3* db)
{	using std::string;

	string table_name  = "simulate";
	size_t n_simulate = check_table_id(db, table_name);

	std::string column_name =  "simulate_index";
	CppAD::vector<int>          simulate_index;
	get_table_column(db, table_name, column_name, simulate_index);
	assert( simulate_index.size() == n_simulate );

	column_name             =  "data_subset_id";
	CppAD::vector<int>          data_subset_id;
	get_table_column(db, table_name, column_name, data_subset_id);
	assert( data_subset_id.size() == n_simulate );

	column_name             =  "simulate_value";
	CppAD::vector<double>       simulate_value;
	get_table_column(db, table_name, column_name, simulate_value);
	assert( simulate_value.size() == n_simulate );

	column_name             =  "simulate_delta";
	CppAD::vector<double>       simulate_delta;
	get_table_column(db, table_name, column_name, simulate_delta);
	assert( simulate_delta.size() == n_simulate );

	CppAD::vector<simulate_struct> simulate_table(n_simulate);
	for(size_t i = 0; i < n_simulate; i++)
	{	simulate_table[i].simulate_index   = simulate_index[i];
		simulate_table[i].data_subset_id   = data_subset_id[i];
		simulate_table[i].simulate_value   = simulate_value[i];
		simulate_table[i].simulate_delta   = simulate_delta[i];
	}
	return simulate_table;
}

} // END DISMOD_AT_NAMESPACE
