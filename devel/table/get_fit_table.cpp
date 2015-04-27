// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_fit_table$$
$spell
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Fit Table Information$$

$head Syntax$$
$icode%fit_table% = get_fit_table(%db%)%$$

$head Purpose$$
To read the $cref fit_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head fit_table$$
The return value $icode fit_table$$ has prototype
$codei%
	CppAD::vector<fit_struct>  %fit_table%
%$$
For each $cref/fit_id/fit_table/fit_id/$$,
$codei%
	%fit_table%[%fit_id%]
%$$
is the information for the corresponding fit.

$head fit_struct$$
This is a structure with the following fields
$table
Type  $cnext Field $cnext Description
$rnext
$code int$$ $cnext $code parent_node_id$$ $cnext
	The $cref/parent_node_id/fit_table/parent_node_id/$$
	for this fit
$rnext
$code double$$ $cnext $code ode_step_size$$ $cnext
	The $cref/ode_step_size/fit_table/ode_step_size/$$
	for this fit
$rnext
$code double$$ $cnext $code tolerance$$ $cnext
	The $cref/tolerance/fit_table/tolerance/$$
	for this fit
$rnext
$code int$$ $cnext $code max_num_iter$$ $cnext
	The $cref/max_num_iter/fit_table/max_num_iter/$$
	for this fit
$tend

$children%example/devel/table/get_fit_table_xam.cpp
%$$
$head Example$$
The file $cref get_fit_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_fit_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<fit_struct> get_fit_table(sqlite3* db)
{	using std::string;
	string table_name         = "fit";
	size_t n_fit = check_table_id(db, table_name);

	string column_name =  "parent_node_id";
	CppAD::vector<int>    parent_node_id;
	get_table_column(db, table_name, column_name, parent_node_id);
	assert( n_fit == parent_node_id.size() );

	column_name         =  "ode_step_size";
	CppAD::vector<double>   ode_step_size;
	get_table_column(db, table_name, column_name, ode_step_size);
	assert( n_fit == ode_step_size.size() );

	column_name =         "tolerance";
	CppAD::vector<double>  tolerance;
	get_table_column(db, table_name, column_name, tolerance);
	assert( n_fit == tolerance.size() );

	column_name =         "max_num_iter";
	CppAD::vector<int>     max_num_iter;
	get_table_column(db, table_name, column_name, max_num_iter);
	assert( n_fit == max_num_iter.size() );

	CppAD::vector<fit_struct> fit_table(n_fit);
	for(size_t i = 0; i < n_fit; i++)
	{	fit_table[i].parent_node_id      = parent_node_id[i];
		fit_table[i].ode_step_size       = ode_step_size[i];
		fit_table[i].tolerance           = tolerance[i];
		fit_table[i].max_num_iter        = max_num_iter[i];
	}
	return fit_table;
}

} // END DISMOD_AT_NAMESPACE
