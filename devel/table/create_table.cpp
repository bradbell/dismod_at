// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin cpp_create_table$$
$spell
	sqlite
	const
	std
	CppAD
	bool
	dismod_at
$$

$section Use C++ to Create a Database Table$$

$head Syntax$$
$codei%dismod_at.create_table(
	%db%, %table_name%, %col_name%, %col_type%, %col_unique%, %row_value%
)%$$

$head Purpose$$
Create a table and place data in the table.

$head db$$
This argument has prototype
$codei%
	sqlite3* %db%
%$$
and is the database we are placing the table into.

$head table_name$$
This argument has prototype
$codei%
	const std::string& %table_name%
%$$
and is the name of the table we are putting information into.

$head col_name$$
This argument has prototype
$codei%
	const CppAD::vector<std::string>& %col_name%
%$$
and is a vector of the names for the column names in this table.
The size of this vector must greater than zero
and is number of columns in the table
minus one (the primary key column is not included).
The primary key first is the first column,
and then the order of the other columns in the same order as $icode col_name$$.

$head col_type$$
This argument has prototype
$codei%
	const CppAD::vector<std::string>& %col_type%
%$$
It has the same size and order as $icode col_name$$ and specifies
the type for the corresponding columns.
The valid types are $code text$$, $code integer$$, and $code real$$.

$head col_unique$$
This argument has prototype
$codei%
	const CppAD::vector<bool>& %col_unique%
%$$
It has the same size and order as $icode col_name$$ and specifies
if the corresponding columns have a
$code unique$$ constraint (no repeated values).

$head row_value$$
This argument has prototype
$codei%
	const CppAD::vector<std::string>& %row_value%
%$$
We use $icode n_row$$ for the number of rows in the table.
The size of $icode row_value$$ is $icode%n_row% * %col_name.size()%$$.
For $icode%i% = 0 , %...%, %n_row%-1%$$,
$icode%j% = 0 , %...%, %col_name.size()%-1%$$,
$codei%
	%row_value%[ i * %col_name%.size() + %j% ]
%$$
is the value placed in the $th i$$ row and column with name
$icode%col_name%[%j%%$$.

$subhead single quote$$
The single quote character cannot appear in any of the values.
If the column has type $code text$$, its values get surrounded by
the single quote character $code '$$.

$subhead null$$
Note that the special case where the value is the empty string,
and the type is $code integer$$ or $code real$$, the $code null$$
value is placed at the corresponding location in the table.

$head table_name_id$$
A column with name $icode%table_name%_id%$$ and type
$code integer primary key$$ is included as the first column in the table.
Its values start with zero (for the first row) and
increment by one for each row.

$children%example/devel/table/create_table_xam.cpp
%$$
$head Example$$
The file $cref create_table_xam.cpp$$ is an example use of
$code cpp_create_table$$.

$end
---------------------------------------------------------------------------
*/
# include <dismod_at/create_table.hpp>
# include <dismod_at/to_string.hpp>
# include <dismod_at/exec_sql_cmd.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void create_table(
	sqlite3*                            db             ,
	const std::string&                  table_name     ,
	const CppAD::vector<std::string>&   col_name       ,
	const CppAD::vector<std::string>&   col_type       ,
	const CppAD::vector<bool>&          col_unique     ,
	const CppAD::vector<std::string>&   row_value      )
{	std::string cmd;
	size_t n_col = col_name.size();
	size_t n_row = row_value.size() / n_col;
	//
	assert( col_type.size() == n_col );
	assert( col_unique.size() == n_col );
	assert( row_value.size() == n_row * n_col );

	// the create the table command
	cmd  = "create table " + table_name;
	cmd += " (" + table_name + "_id integer primary key";
	for(size_t j = 0; j < n_col; j++)
	{	cmd += ", " + col_name[j] + " " + col_type[j];
		if( col_unique[j] )
			cmd += " unique";
	}
	cmd += ");";
	dismod_at::exec_sql_cmd(db, cmd);
	//
	// start the multiple insert command
	cmd = "insert into " + table_name;
	cmd += " (" + table_name + "_id";
	for(size_t j = 0; j < n_col; j++)
		cmd += ", " + col_name[j];
	cmd += " ) values\n";
	//
	if( n_row == 0 )
		return;
	//
	// data for the multiple insert
	for(size_t i = 0; i < n_row; i++)
	{	cmd += "( "  + to_string(i);
		for(size_t j = 0; j < n_col; j++)
		{	cmd += ", ";
			if( col_type[j] == "text" )
				cmd += "'" + row_value[i * n_col + j] + "'";
			else if( row_value[i * n_col + j] == "" )
				cmd += "null";
			else
				cmd += row_value[i * n_col + j];
		}
		if( i + 1 < n_row )
			cmd += " ),\n";
		else
			cmd += " )\n";
	}
	dismod_at::exec_sql_cmd(db, cmd);
}

} // END_DISMOD_AT_NAMESPACE
