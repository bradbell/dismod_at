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
$begin get_sample_table$$
$spell
	var
	sqlite
	CppAD
	struct
$$

$section C++: Get the Simulate Table$$

$head Syntax$$
$icode%sample_table% = get_sample_table(%db%)%$$

$head Purpose$$
To read the $cref sample_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head sample_table$$
The return value $icode sample_table$$ has prototype
$codei%
	CppAD::vector<sample_struct>  %sample_table%
%$$
For each $cref/sample_id/sample_table/sample_id/$$,
$codei%
	%sample_table%[%sample_id%]
%$$
is the information for the corresponding
$cref/sample_id/sample_table/sample_id/$$.

$head sample_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$code int$$ $cnext $code sample_index$$ $cnext
	The $cref/sample_index/sample_table/sample_index/$$
	for this sampled measurement.
$rnext
$code int$$ $cnext $code var_id$$ $cnext
	The $cref/var_id/sample_table/var_id/$$
	for this sample
$rnext
$code double$$ $cnext $code var_value$$ $cnext
	The $cref/var_value/sample_table/var_value/$$
	for this sample.
$tend

$children%example/devel/table/get_sample_table_xam.cpp
%$$
$head Example$$
The file $cref get_sample_table_xam.cpp$$ contains an example
and test of this function.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_sample_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<sample_struct> get_sample_table(sqlite3* db)
{	using std::string;

	string table_name  = "sample";
	size_t n_sample = check_table_id(db, table_name);

	std::string column_name =  "sample_index";
	CppAD::vector<int>          sample_index;
	get_table_column(db, table_name, column_name, sample_index);
	assert( sample_index.size() == n_sample );

	column_name             =  "var_id";
	CppAD::vector<int>          var_id;
	get_table_column(db, table_name, column_name, var_id);
	assert( var_id.size() == n_sample );

	column_name             =  "var_value";
	CppAD::vector<double>       var_value;
	get_table_column(db, table_name, column_name, var_value);
	assert( var_value.size() == n_sample );

	CppAD::vector<sample_struct> sample_table(n_sample);
	for(size_t i = 0; i < n_sample; i++)
	{	sample_table[i].sample_index   = sample_index[i];
		sample_table[i].var_id         = var_id[i];
		sample_table[i].var_value      = var_value[i];
	}
	return sample_table;
}

} // END DISMOD_AT_NAMESPACE
