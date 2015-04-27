// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin check_table_id$$
$spell
	dismod
	hpp
	const
	std
$$

$section C++: Check That table_id Column Has Proper Values$$
$index table, check id$$
$index check, table id$$
$index id, check table$$

$head Syntax$$
$icode%n_row% = check_table_id(%table_name%)%$$

$head Purpose$$
Check that the column $icode%table_name%_id%$$
starts at zero and increments by one between rows.
It not, an error message is printed and the program exists.

$head table_name$$
This argument has prototype
$codei%
	const std::string& %table_name%
%$$
It is the name of the table that we are checking.

$head n_row$$
The return value has prototype
$codei%
	size_t %n_row%
%$$
It is the number of rows in the table.

$end
------------------------------------------------------------------------------
*/
# include <iostream>

# include <cppad/cppad.hpp>
# include <dismod_at/table_error_exit.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at {
	size_t  check_table_id(sqlite3* db, const std::string& table_name )
	{	using std::string;
		string column_name = table_name + "_id";
		CppAD::vector<int>   table_id;
		get_table_column(db, table_name, column_name, table_id);

		size_t n_row = table_id.size();
		for(size_t i = 0; i < n_row; i++)
		{	if( table_id[i] != int(i) )
			{	string msg = table_name + "_id must start at zero";
				msg       += " and increment by one.";
				table_error_exit(table_name, i, msg);
			}
		}
		return n_row;
	}
}
