// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin check_table_id$$
$spell
	sqlite
	dismod
	hpp
	const
	std
$$

$section C++: Check That table_id Column Has Proper Values$$

$head Syntax$$
$icode%n_row% = check_table_id(%db%, %table_name%)%$$

$head Purpose$$
Check that the column $icode%table_name%_id%$$
starts at zero and increments by one between rows.
It not, an error message is printed and the program exists.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

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

$head Speed$$
Note that if one only wants to determine $icode n_row$$,
it should be faster to use $cref get_column_max$$ to find the maximum
value of $icode%table_name%_id%$$ (the primary key) and then add one;
see $cref/primary key/database/Primary Key/$$.

$end
------------------------------------------------------------------------------
*/
# include <iostream>

# include <cppad/cppad.hpp>
# include <dismod_at/error_exit.hpp>
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
				error_exit(msg, table_name, i);
			}
		}
		return n_row;
	}
}
