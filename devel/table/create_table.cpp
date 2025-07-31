// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-24 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin cpp_create_table dev}
{xrst_spell
  bool
}

Use C++ to Create a Database Table
##################################

Syntax
******

| ``dismod_at.create_table`` (
| |tab| *db* , *table_name* , *col_name* , *col_type* , *col_unique* , *row_value*
| )

Purpose
*******
Create a table and place data in the table.

db
**
This argument has prototype

   ``sqlite3`` * *db*

and is the database we are placing the table into.

table_name
**********
This argument has prototype

   ``const std::string&`` *table_name*

and is the name of the table we are putting information into.

col_name
********
This argument has prototype

   ``const CppAD::vector<std::string>&`` *col_name*

and is a vector of the names for the column names in this table.
The size of this vector must greater than zero
and is number of columns in the table
minus one (the primary key column is not included).
The primary key first is the first column,
and then the order of the other columns in the same order as *col_name* .

col_type
********
This argument has prototype

   ``const CppAD::vector<std::string>&`` *col_type*

It has the same size and order as *col_name* and specifies
the type for the corresponding columns.
The valid types are ``text`` , ``integer`` , and ``real`` .

col_unique
**********
This argument has prototype

   ``const CppAD::vector<bool>&`` *col_unique*

It has the same size and order as *col_name* and specifies
if the corresponding columns have a
``unique`` constraint (no repeated values).

row_value
*********
This argument has prototype

   ``const CppAD::vector<std::string>&`` *row_value*

We use *n_row* for the number of rows in the table.
The size of *row_value* is *n_row* * *col_name.size* () .
For *i* = 0 , ..., *n_row* ``-1`` ,
*j* = 0 , ..., *col_name.size* () ``-1`` ,

   *row_value* [ ``i`` * *col_name* . ``size`` () + *j*  ]

is the value placed in the *i*-th row and column with name
*col_name* [ *j* .

single quote
============
The single quote character cannot appear in any of the values.
If the column has type ``text`` , its values get surrounded by
the single quote character ``'`` .

null
====
Note that the special case where the value is the empty string,
and the type is ``integer`` or ``real`` , the ``null``
value is placed at the corresponding location in the table.

table_name_id
*************
A column with name *table_name* _ ``id`` and type
``integer primary key`` is included as the first column in the table.
Its values start with zero (for the first row) and
increment by one for each row.
{xrst_toc_hidden
   example/devel/table/create_table_xam.cpp
}
Example
*******
The file :ref:`create_table_xam.cpp-name` is an example use of
``cpp_create_table`` .

{xrst_end cpp_create_table}
---------------------------------------------------------------------------
*/
# include <dismod_at/create_table.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/exec_sql_cmd.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void create_table(
   sqlite3*                            db             ,
   const std::string&                  table_name     ,
   const CppAD::vector<std::string>&   col_name       ,
   const CppAD::vector<std::string>&   col_type       ,
   const CppAD::vector<bool>&          col_unique     ,
   const CppAD::vector<std::string>&   row_value      )
{  using CppAD::to_string;

   std::string cmd;
   std::string cmd_n;
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
   {  cmd += ", " + col_name[j] + " " + col_type[j];
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
   size_t cut_size = 5000000;
   for(size_t n = cut_size; n <= n_row; n += cut_size)
   {  if (n > n_row)
         n = n_row;
      for(size_t i = n - cut_size; i < n; i++)
      {  cmd_n = cmd + "( "  + to_string(i);
         for(size_t j = 0; j < n_col; j++)
         {  cmd_n += ", ";
            if( col_type[j] == "text" )
               cmd_n += "'" + row_value[i * n_col + j] + "'";
            else if( row_value[i * n_col + j] == "" )
               cmd_n += "null";
            else
               cmd_n += row_value[i * n_col + j];
         }
         if( i + 1 < n )
            cmd_n += " ),\n";
         else
            cmd_n += " )\n";
      }
      dismod_at::exec_sql_cmd(db, cmd_n);
   }
}

} // END_DISMOD_AT_NAMESPACE
