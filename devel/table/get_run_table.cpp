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
$begin get_run_table$$
$spell
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Run Table Information$$
$index get, run table$$
$index run, get table$$
$index table, get run$$

$head Syntax$$
$icode%run_table% = get_run_table(%db%)%$$

$head Purpose$$
To read the $cref run_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head run_table$$
The return value $icode run_table$$ has prototype
$codei%
	CppAD::vector<run_struct>  %run_table%
%$$
For each $cref/run_id/run_table/run_id/$$,
$codei%
	%run_table%[%run_id%]
%$$
is the information for the corresponding run.

$head run_struct$$
This is a structure with the following fields
$table
Type  $cnext Field $cnext Description
$rnext
$code int$$ $cnext $code parent_node_id$$ $cnext
	The $cref/parent_node_id/run_table/parent_node_id/$$
	for this run
$rnext
$code double$$ $cnext $code ode_step_size$$ $cnext
	The $cref/ode_step_size/run_table/ode_step_size/$$
	for this run
$rnext
$code int$$ $cnext $code n_sample$$ $cnext
	The $cref/n_sample/run_table/n_sample/$$
	for this run
$tend

$children%example/devel/table/get_run_table_xam.cpp
%$$
$head Example$$
The file $cref get_run_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_run_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<run_struct> get_run_table(sqlite3* db)
{	using std::string;
	string table_name         = "run";
	size_t n_run = check_table_id(db, table_name);

	string column_name =  "parent_node_id";
	CppAD::vector<int>    parent_node_id;
	get_table_column(db, table_name, column_name, parent_node_id);
	assert( n_run == parent_node_id.size() );

	column_name         =  "ode_step_size";
	CppAD::vector<double>   ode_step_size;
	get_table_column(db, table_name, column_name, ode_step_size);
	assert( n_run == ode_step_size.size() );

	column_name =        "n_sample";
	CppAD::vector<int>    n_sample;
	get_table_column(db, table_name, column_name, n_sample);
	assert( n_run == n_sample.size() );

	CppAD::vector<run_struct> run_table(n_run);
	for(size_t i = 0; i < n_run; i++)
	{	run_table[i].parent_node_id      = parent_node_id[i];
		run_table[i].ode_step_size       = ode_step_size[i];
		run_table[i].n_sample            = n_sample[i];
	}
	return run_table;
}

} // END DISMOD_AT_NAMESPACE
