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
$begin get_data_sim_table$$
$spell
	sim
	sqlite
	CppAD
	struct
	std
	cv
	stdcv
$$

$section C++: Get the Data Simulation Table$$

$head Syntax$$
$icode%data_sim_table% = get_data_sim_table(%db%)%$$

$head Purpose$$
To read the $cref data_sim_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head data_sim_table$$
The return value $icode data_sim_table$$ has prototype
$codei%
	CppAD::vector<data_sim_struct>  %data_sim_table%
%$$
For each $cref/data_sim_id/data_sim_table/data_sim_id/$$,
$codei%
	%data_sim_table%[%data_sim_id%]
%$$
is the information for the corresponding
$cref/data_sim_id/data_sim_table/data_sim_id/$$.

$head data_sim_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code int$$ $cnext $code simulate_index$$ $cnext
	The $cref/simulate_index/data_sim_table/simulate_index/$$
	for this simulated measurement.
$rnext
$code int$$ $cnext $code data_subset_id$$ $cnext
	The $cref/data_subset_id/data_subset_table/data_subset_id/$$
	for this simulated measurement.
$rnext
$code double$$ $cnext $code data_sim_value$$ $cnext
	The $cref/meas_value/data_table/meas_value/$$
	for this simulated measurement.
$rnext
$tend

$children%example/devel/table/get_data_sim_table_xam.cpp
%$$
$head Example$$
The file $cref get_data_sim_table_xam.cpp$$ contains an example
and test of this function.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_data_sim_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<data_sim_struct> get_data_sim_table(sqlite3* db)
{	using std::string;

	string table_name  = "data_sim";
	size_t n_data_sim  = check_table_id(db, table_name);

	std::string column_name =  "simulate_index";
	CppAD::vector<int>          simulate_index;
	get_table_column(db, table_name, column_name, simulate_index);
	assert( simulate_index.size() == n_data_sim );

	column_name             =  "data_subset_id";
	CppAD::vector<int>          data_subset_id;
	get_table_column(db, table_name, column_name, data_subset_id);
	assert( data_subset_id.size() == n_data_sim );

	column_name             =  "data_sim_value";
	CppAD::vector<double>       data_sim_value;
	get_table_column(db, table_name, column_name, data_sim_value);
	assert( data_sim_value.size() == n_data_sim );

	CppAD::vector<data_sim_struct> data_sim_table(n_data_sim);
	for(size_t i = 0; i < n_data_sim; i++)
	{	data_sim_table[i].simulate_index   = simulate_index[i];
		data_sim_table[i].data_subset_id   = data_subset_id[i];
		data_sim_table[i].data_sim_value   = data_sim_value[i];
	}
	return data_sim_table;
}

} // END DISMOD_AT_NAMESPACE
