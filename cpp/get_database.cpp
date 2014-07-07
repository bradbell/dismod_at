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
$begin get_database$$
$spell
	sqlite
	enum
	cpp
	std
$$

$section C++: Get the Data Base Input Information$$
$index get, input tables$$
$index table, get input$$
$index input, get tables$$

$head Under Construction$$
This routine is under construction and not yet used.

$head Syntax$$
$codei%# include <get_database>
%$$
$icode%get_database(%db%)%$$

$head Purpose$$
To read the $cref database$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_database.hpp>



namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<double> get_database(sqlite3* db)
{	using std::string;

	string table_name  = "age";
	size_t n_age       = check_table_id(db, table_name);
	
	string column_name =  "age";
	CppAD::vector<double>  age;
	get_table_column(db, table_name, column_name, age);
	assert( age.size() == n_age );

	return age;
}

} // END DISMOD_AT_NAMESPACE
