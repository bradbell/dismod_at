/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
         GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <dismod_at/old2new_command.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/create_table.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
$begin old2new_command$$
$spell
	dismod
	avgint
	mulcov
$$

$section Convert an Old Database to New Format$$

$head Syntax$$
$codei%dismod_at %database% old2new%$$

$head Discussion$$
This conversion if for the change on
$cref/2019-11-22/whats_new_2019/11-22/$$
when $cref subgroup_table$$ information was added to the database.
Adds the subgroup table information to an old database
(that satisfies the requirements before the this information was added).
It must be run before the $cref init_command$$.

$head Subgroup Table$$
A $cref subgroup_table$$ is added with one row and the following values:
$icode%
	%subgroup_id% %subgroup_name% %group_id% %group_name%
	0             world            0          world
%$$

$head data Table$$
The column $cref/subgroup_id/data_table/subgroup_id/$$ is added to the
$cref data_table$$ with the value $icode%subgroup_id% = 0%$$ for every row.

$head avgint Table$$
The column $cref/subgroup_id/avgint_table/subgroup_id/$$ is added to the
$cref avgint_table$$ with the value $icode%subgroup_id% = 0%$$ for every row.

$head mulcov Table$$
The column named $icode smooth_id$$ is changed to be named
$cref/group_smooth_id/mulcov_table/group_smooth_id/$$.
The columns
$cref/group_id/mulcov_table/group_id/$$ and
$cref/subgroup_smooth_id/mulcov_table/subgroup_smooth_id/$$
are added to the $cref mulcov_table$$ with values
$icode%group_id% = 0%$$, and $icode%subgroup_smooth_id% = null%$$
for every row.

$end
*/

// ----------------------------------------------------------------------------
void old2new_command(sqlite3* db)
{	using std::string;
	using CppAD::vector;
	//
	// check if subgroup table exists
	string sql_cmd = "SELECT name FROM sqlite_master WHERE name='subgroup'";
	char   sep = ',';
	string sql_ret = exec_sql_cmd(db, sql_cmd, sep);
	if( sql_ret.size() != 0 )
	{	string msg = "old2new_command: subgroup table is already in database";
		error_exit(msg);
		return;
	}
	// -----------------------------------------------------------------------
	// subgroup table
	string table_name = "subgroup";
	vector<string> col_name(3);
	vector<string> col_type(3);
	vector<bool>   col_unique(3);
	vector<string> row_value(3);
	//
	col_name[0]   = "subgroup_name";
	col_type[0]   = "text";
	col_unique[0] = true;
	row_value[0]  = "world";
	//
	col_name[1]   = "group_id";
	col_type[1]   = "integer";
	col_unique[1] = false;
	row_value[1]  = "0";
	//
	col_name[2]   = "group_name";
	col_type[2]   = "text";
	col_unique[2] = false;
	row_value[2]  = "world";
	//
	create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	// --------------------------------------------------------------------
	// mulcov table
	//
	// rename smooth_id -> group_smooth_id
	sql_cmd = "ALTER TABLE mulcov RENAME COLUMN smooth_id TO group_smooth_id";
	exec_sql_cmd(db, sql_cmd);
	//
	// mulcov_table: Add group_id column
	sql_cmd = "ALTER TABLE mulcov ADD COLUMN group_id integer";
	exec_sql_cmd(db, sql_cmd);
	//
	// mulcov_table: Add subgroup_smooth_id column
	sql_cmd = "ALTER TABLE mulcov ADD COLUMN subgroup_smooth_id integer";
	exec_sql_cmd(db, sql_cmd);
	//
	// mulcov_table: set group_id and subgroup_smooth_id to zero
	sql_cmd = "UPDATE mulcov SET group_id = 0, subgroup_smooth_id = null";
	exec_sql_cmd(db, sql_cmd);
	// --------------------------------------------------------------------
	// data table
	sql_cmd = "ALTER TABLE data ADD COLUMN subgroup_id integer";
	exec_sql_cmd(db, sql_cmd);
	sql_cmd = "UPDATE data SET subgroup_id = 0";
	exec_sql_cmd(db, sql_cmd);
	// --------------------------------------------------------------------
	// avgint table
	sql_cmd = "ALTER TABLE avgint ADD COLUMN subgroup_id integer";
	exec_sql_cmd(db, sql_cmd);
	sql_cmd = "UPDATE avgint SET subgroup_id = 0";
	exec_sql_cmd(db, sql_cmd);
	// --------------------------------------------------------------------

	return;
}

} // END_DISMOD_AT_NAMESPACE
