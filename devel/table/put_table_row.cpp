// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin put_table_row$$
$spell
   sqlite
   const
   std
   CppAD
   vec
$$

$section C++: Insert New Row at End of a Table$$

$head Syntax$$
$icode%table_name_id% = put_table_row(
   %db%, %table_name%, %col_name_vec%, %row_val_vec%
)%$$
$codei%put_table_row(
   %db%, %table_name%, %col_name_vec%, %row_val_vec%. %primary_key%
)%$$

$head db$$
This argument has prototype
$codei%
   sqlite3* %db%
%$$
and is the database we are putting information into.

$head table_name$$
This argument has prototype
$codei%
   const std::string& %table_name%
%$$
and is the name of the table we are putting information into.

$head col_name_vec$$
This argument has prototype
$codei%
   const CppAD::vector<std::string>& %col_name_vec%
%$$
and is a vector of the names for the column names in this table.
The size of this vector must be the number of columns in the table
minus one and the primary key column is not included.

$head row_val_vec$$
This argument has prototype
$codei%
   const CppAD::vector<std::string>& %row_val_vec%
%$$
and is a vector of the values we are inserting into the table.
It must have the same size, and be in the same order,
as $icode col_name_vec$$.
There cannot be any single quote characters $code '$$
in any of the values.

$subhead null$$
The special value $code null$$ can be used for $code integer$$ and
$code real$$ columns. If it is used for a $code text$$ column,
it will be interpreted as the text $code 'null'$$ and not a missing value.

$head primary_key$$
The primary key for this table name must have column name
$icode%table_name%_id%$$.
This argument has prototype
$codei%
   const size_t& %primary_key%
%$$.
If this argument is present, this value is used for the primary key.
Otherwise, if the table is empty,
the value zero is used for the primary key.
Otherwise, the value is for the primary key is one greater than
the current maximum value for the primary key column
$icode%table_name%_id%$$.

$head table_name_id$$
If the return value is present, it has prototype
$codei%
   size_t %table_name_id%
%$$
It is the value of the primary key for this row;
i.e, the value in the $icode%table_name%_id%$$ column for this row.

$children%example/devel/table/put_table_row_xam.cpp
%$$
$head Example$$
The file $cref put_table_row_xam.cpp$$ contains an example that uses
this function.

$end
------------------------------------------------------------------------------
*/
# include <dismod_at/put_table_row.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_column_max.hpp>
# include <cppad/utility/to_string.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void put_table_row(
   sqlite3*                          db           ,
   const std::string&                table_name   ,
   const CppAD::vector<std::string>& col_name_vec ,
   const CppAD::vector<std::string>& row_val_vec  ,
   const size_t&                     primary_key  )
{  using CppAD::to_string;

   size_t n_col = col_name_vec.size();
   std::string table_name_id = to_string(primary_key);
   //
   std::string name_list  = "(";
   std::string value_list = "(";
   name_list            += table_name + "_id";
   value_list           += table_name_id;
   for(size_t i = 0; i < n_col; i++)
   {  // check that there are not single quotes in the value
      assert( row_val_vec[i].find('\'') == std::string::npos );

      // add this column to name_list
      name_list += ",";
      name_list += col_name_vec[i];

      // add this column to value_list
      std::string column_type = get_table_column_type(
         db, table_name, col_name_vec[i]
      );
      if( column_type == "text" )
      {  value_list += ",'";
         value_list += row_val_vec[i];
         value_list += "'";
      }
      else
      {  value_list += ",";
         value_list += row_val_vec[i];
      }
   }
   name_list  += ")";
   value_list += ")";
   //
   std::string cmd = "insert into ";
   cmd            += table_name + " ";
   cmd            += name_list + " values ";
   cmd            +=  value_list + ";";
   //
   exec_sql_cmd(db, cmd);
   //
   return;
}
size_t put_table_row(
   sqlite3*                          db           ,
   const std::string&                table_name   ,
   const CppAD::vector<std::string>& col_name_vec ,
   const CppAD::vector<std::string>& row_val_vec  )
{  using std::string;

   string select_cmd       = "select * from " + table_name;
   std::string column_name = table_name + "_id";
   std::string max_str     = get_column_max(db, select_cmd, column_name);
   //
   size_t primary_key      = 0;
   if( max_str != "" )
      primary_key = std::atoi( max_str.c_str() ) + 1;;
   //
   put_table_row(db, table_name, col_name_vec, row_val_vec, primary_key);
   return primary_key;
}

} // END_DISMOD_AT_NAMESPACE
