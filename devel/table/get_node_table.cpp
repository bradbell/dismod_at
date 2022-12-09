// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_node_table}

C++: Get the Node Table Information
###################################

Syntax
******
*node_table* = ``get_node_table`` ( *db* )

Purpose
*******
To read the :ref:`node_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

node_struct
***********
This is a structure with the following fields

.. list-table::

   * - Type
     - Field
     - Description
   * - ``std::string``
     - ``node_name``
     - The :ref:`node_table@node_name` for this node
   * - ``int``
     - ``parent``
     - The :ref:`node_table@parent` *node_id*

node_table
**********
The return value *node_table* has prototype

   ``CppAD::vector<node_struct>`` *node_table*

For each :ref:`node_table@node_id` ,

   *node_table* [ *node_id* ]

is the information for the corresponding node.
{xrst_toc_hidden
   example/devel/table/get_node_table_xam.cpp
}
Example
*******
The file :ref:`get_node_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_node_table}
-----------------------------------------------------------------------------
*/



# include <dismod_at/get_node_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<node_struct> get_node_table(sqlite3* db)
{  using std::string;

   string table_name  = "node";
   size_t n_node      = check_table_id(db, table_name);

   string column_name = "node_name";
   CppAD::vector<string>  node_name;
   get_table_column(db, table_name, column_name, node_name);
   assert( n_node == node_name.size() );

   column_name        = "parent";
   CppAD::vector<int>   parent;
   get_table_column(db, table_name, column_name, parent);
   assert( n_node == parent.size() );

   CppAD::vector<node_struct> node_table(n_node);
   for(size_t i = 0; i < n_node; i++)
   {  node_table[i].node_name  = node_name[i];
      node_table[i].parent     = parent[i];
   }
   return node_table;
}

} // END DISMOD_AT_NAMESPACE
