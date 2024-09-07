// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-24 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_subgroup_table dev}

C++: Get the Subgroup Table Information
#######################################

Syntax
******
*subgroup_table* = ``get_subgroup_table`` ( *db* )

Prototype
*********
{xrst_literal
   // BEGIN_GET_SUBGROUP_TABLE
   // END_GET_SUBGROUP_TABLE
}

Purpose
*******
To read the :ref:`subgroup_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

subgroup_table
**************
For each :ref:`subgroup_table@subgroup_id` ,

   *subgroup_table* [ *subgroup_id* ]

is the information for the corresponding subgroup.

subgroup_struct
***************
This is a structure with the following fields

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - ``std::string``
     - ``subgroup_name``
     - The :ref:`subgroup_table@subgroup_name` for this subgroup
   * - ``int``
     - ``group_id``
     - The :ref:`subgroup_table@group_id` for this subgroup
   * - ``std::string``
     - ``group_name``
     - The :ref:`subgroup_table@group_name` for this subgroup

{xrst_toc_hidden
   example/devel/table/get_subgroup_table_xam.cpp
}
Example
*******
The file :ref:`get_subgroup_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_subgroup_table}
-----------------------------------------------------------------------------
*/



# include <cstring>
# include <dismod_at/get_subgroup_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <cppad/utility/to_string.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_GET_SUBGROUP_TABLE
CppAD::vector<subgroup_struct> get_subgroup_table(sqlite3* db )
// END_GET_SUBGROUP_TABLE
{  using std::string;
   string msg, sql_cmd;

   string table_name  = "subgroup";
   size_t n_subgroup = check_table_id(db, table_name);

   string column_name =  "subgroup_name";
   CppAD::vector<string>  subgroup_name;
   get_table_column(db, table_name, column_name, subgroup_name);
   assert( n_subgroup == subgroup_name.size() );

   column_name = "group_id";
   CppAD::vector<int> group_id;
   get_table_column(db, table_name, column_name, group_id);
   assert( n_subgroup == group_id.size() );

   column_name =  "group_name";
   CppAD::vector<string>  group_name;
   get_table_column(db, table_name, column_name, group_name);
   assert( n_subgroup == group_name.size() );

   // set subgroup_table
   CppAD::vector<subgroup_struct> subgroup_table(n_subgroup);
   for(size_t subgroup_id = 0; subgroup_id < n_subgroup; ++subgroup_id)
   {
      subgroup_table[subgroup_id].subgroup_name = subgroup_name[subgroup_id];
      subgroup_table[subgroup_id].group_id      = group_id[subgroup_id];
      subgroup_table[subgroup_id].group_name    = group_name[subgroup_id];
   }

   // ------------------------------------------------------------------------
   // check for errors
   int previous_group_id      = subgroup_table[0].group_id;
   string previous_group_name = subgroup_table[0].group_name;
   if( previous_group_name.find(' ') != string::npos )
   {  msg  = "There is a space in following group name: ";
      msg += "'" + previous_group_name + "'";
      size_t subgroup_id = 0;
      error_exit(msg, table_name, subgroup_id);
   }
   if( previous_group_id != 0 )
   {  msg = "first group_id is not zero";
      size_t subgroup_id = 0;
      error_exit(msg, table_name, subgroup_id);
   }
   for(size_t subgroup_id = 1; subgroup_id < n_subgroup; ++subgroup_id)
   {  int    this_group_id      = subgroup_table[subgroup_id].group_id;
      string this_group_name    = subgroup_table[subgroup_id].group_name;
      string this_subgroup_name = subgroup_table[subgroup_id].subgroup_name;
      // cannot have a space in a group name
      if(this_group_name.find(' ') != string::npos )
      {  msg  = "There is a space in following group name: ";
         msg += "'" + previous_group_name + "'";
         error_exit(msg, table_name, subgroup_id);
      }
      // special case where group can also be a subgroup
      if( this_group_name == this_subgroup_name )
      {  bool ok = true;
         if( 0 < subgroup_id )
            ok &= this_group_id != previous_group_id;
         if( subgroup_id + 1 < n_subgroup )
            ok &= this_group_id != subgroup_table[subgroup_id+1].group_id;
         if( ! ok )
         {  msg  = "This group and subgroup name are the same ";
            msg += "and this group has multipler subgroups";
            error_exit(msg, table_name, subgroup_id);
         }
      }
      // Otherwise a group name cannot be a subgroup name
      for(size_t i = 0; i < n_subgroup; ++i) if( i != subgroup_id )
      {  if( subgroup_table[i].subgroup_name == this_group_name )
         {  msg  = "This group name is also a subgroup name ";
            msg += "in a different row of the subgroup table";
            error_exit(msg, table_name, subgroup_id);
         }
      }
      // -------------------------------------------------------------------
      // group_id is monotone non-decreasing
      if( this_group_id < previous_group_id )
      {  msg = "group_id < previous group_id";
         error_exit(msg, table_name, subgroup_id);
      }
      // group name must be the same for entire group
      else if( this_group_id == previous_group_id )
      {  if( this_group_name != previous_group_name )
         {  msg = "group_id equals previous group_id but the group names ";
            msg += "are different";
            error_exit(msg, table_name, subgroup_id);
         }
      }
      // two groups cannot have the same name
      else if( this_group_id == previous_group_id + 1 )
      {  for(size_t i = 0; i < subgroup_id; ++i)
         {  if( this_group_name == subgroup_table[i].group_name )
            {  msg = "This group_name was used for a different group";
               error_exit(msg, table_name, subgroup_id);
            }
         }
      }
      // cannot skip a group_id
      else
      {  msg = "group_id != (previous group_id) or (previous group_id+1)";
         error_exit(msg, table_name, subgroup_id);
      }
      // -------------------------------------------------------------------
      previous_group_id   = this_group_id;
      previous_group_name = this_group_name;
   }

   return subgroup_table;
}

} // END DISMOD_AT_NAMESPACE
