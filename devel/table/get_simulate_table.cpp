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
$begin get_simulate_table$$
$spell
	sqlite
	CppAD
	struct
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
$code int$$ $cnext $code sample_index$$ $cnext
	The $cref/sample_index/simulate_table/sample_index/$$
	for this simulated measurement.
$rnext
$code int$$ $cnext $code data_subset_id$$ $cnext
	The $cref/data_subset_id/data_subset_table/data_subset_id/$$
	for this simulated measurement.
$rnext
$code double$$ $cnext $code meas_value$$ $cnext
	The $cref/meas_value/data_table/meas_value/$$
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

	std::string column_name =  "sample_index";
	CppAD::vector<int>          sample_index;
	get_table_column(db, table_name, column_name, sample_index);
	assert( sample_index.size() == n_simulate );

	column_name             =  "data_subset_id";
	CppAD::vector<int>          data_subset_id;
	get_table_column(db, table_name, column_name, data_subset_id);
	assert( data_subset_id.size() == n_simulate );

	column_name             =  "meas_value";
	CppAD::vector<double>       meas_value;
	get_table_column(db, table_name, column_name, meas_value);
	assert( meas_value.size() == n_simulate );

	CppAD::vector<simulate_struct> simulate_table(n_simulate);
	for(size_t i = 0; i < n_simulate; i++)
	{	simulate_table[i].sample_index   = sample_index[i];
		simulate_table[i].data_subset_id = data_subset_id[i];
		simulate_table[i].meas_value    = meas_value[i];
	}
	return simulate_table;
}

} // END DISMOD_AT_NAMESPACE
