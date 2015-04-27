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
$begin get_time_table$$
$spell
	sqlite
	enum
	cpp
	std
$$

$section C++: Get the Time Table Information$$
$index get, time table$$
$index time, get table$$
$index table, get time$$

$head Syntax$$
$icode%time_table% = get_time_table(%db%)%$$

$head Purpose$$
To read the $cref time_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head time_table$$
The return value $icode time_table$$ has prototype
$codei%
	CppAD::vector<double>  %time_table%
%$$
For each $cref/time_id/time_table/time_id/$$,
$codei%
	%time_table%[%time_id%]
%$$
is the time for the corresponding
$cref/time/time_table/time/$$.

$head Assumptions$$
For $icode%time_id% = 1 , %...%, %time_table%.size()-2%$$,
the following assumption is checked:
$codei%
	%time_table%[%time_id%] < %time_table%[%time_id%+1]
%$$

$children%example/devel/table/get_time_table_xam.cpp
%$$
$head Example$$
The file $cref get_time_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/get_time_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/table_error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<double> get_time_table(sqlite3* db)
{	using std::string;

	string table_name  = "time";
	size_t n_time      = check_table_id(db, table_name);

	string column_name =  "time";
	CppAD::vector<double>  time;
	get_table_column(db, table_name, column_name, time);
	assert( time.size() == n_time );

	for(size_t time_id = 1; time_id < n_time; time_id++) if(
		time[time_id-1] >= time[time_id]
	)
	{	string messtime = "time is not monotone increasing";
		table_error_exit("time", time_id, messtime);
	}

	return time;
}

} // END DISMOD_AT_NAMESPACE
