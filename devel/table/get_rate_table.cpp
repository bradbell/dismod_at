// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_rate_table dev}
{xrst_spell
   enum
}

C++: Get the Rate Table Information
###################################

Syntax
******

| *rate_table* = ``get_rate_table`` ( *db* )
| *rate_name*  = ``get_rate_name`` ( *rate_id* )

get_rate_table
**************
Reads the :ref:`rate_table-name` and return it as a C++ data structure.

get_rate_name
*************
Sets
:ref:`rate_table@rate_name`
to a ``std::string`` that corresponds to
:ref:`rate_table@rate_id` .

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

rate_struct
***********
This is a structure with the following fields

.. list-table::

   * - Type
     - Field
     - Description
   * - ``rate_enum``
     - ``rate``
     - enum corresponding to :ref:`rate_table@rate_name`
   * - ``int``
     - ``parent_smooth_id``
     - The :ref:`rate_table@parent_smooth_id`
   * - ``int``
     - ``child_smooth_id``
     - The :ref:`rate_table@child_smooth_id`
   * - ``int``
     - ``child_nslist_id``
     - The :ref:`rate_table@child_nslist_id`

rate_table
**********
The return value *rate_table* has prototype

   ``CppAD::vector<rate_struct>`` *rate_table*

For each :ref:`rate_table@rate_id` ,

   *rate_table* [ *rate_id* ]

is the information value for the corresponding
:ref:`rate_table@rate_id` .

rate_enum
*********
The *rate_table* can also be addressed
using the ``rate_enum`` values as follows:

.. csv-table::
   :widths: auto

   *rate_id*,*rate_name*
   ``pini_enum``,``pini``
   ``iota_enum``,``iota``
   ``rho_enum``,``rho``
   ``chi_enum``,``chi``
   ``omega_enum``,``omega``

The number of these enum values is ``number_rate_enum`` .
{xrst_toc_hidden
   example/devel/table/get_rate_table_xam.cpp
}
Example
*******
The file :ref:`get_rate_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_rate_table}
-----------------------------------------------------------------------------
*/


# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

std::string get_rate_name(size_t rate_id)
{  using std::string;

   assert( rate_id < size_t( number_rate_enum ) );
   //
   // rate names in same order as enum type in get_rate_table.hpp
   // and in the documentation for rate_table.omh
   const char* rate_enum2name[] = {
      "pini",
      "iota",
      "rho",
      "chi",
      "omega"
   };
   assert( string("pini")  == rate_enum2name[pini_enum] );
   assert( string("iota")  == rate_enum2name[iota_enum] );
   assert( string("rho")   == rate_enum2name[rho_enum] );
   assert( string("chi")   == rate_enum2name[chi_enum] );
   assert( string("omega") == rate_enum2name[omega_enum] );
   //
   return  std::string( rate_enum2name[rate_id] );
}

CppAD::vector<rate_struct> get_rate_table(sqlite3* db)
{  using std::string;
   using CppAD::to_string;
   //
   // for error messaging
   string msg;
   size_t null_id  = DISMOD_AT_NULL_SIZE_T;


   string table_name   = "rate";
   size_t n_rate       = check_table_id(db, table_name);
   if( n_rate != size_t( number_rate_enum ) )
   {  msg  = "rate table does not have ";
      msg += to_string( size_t( number_rate_enum) )  + "rows.";
      error_exit(msg, table_name, null_id);
   }

   string column_name  = "rate_name";
   CppAD::vector<string>  rate_name;
   get_table_column(db, table_name, column_name, rate_name);
   assert( n_rate == rate_name.size() );

   column_name         = "parent_smooth_id";
   CppAD::vector<int>     parent_smooth_id;
   get_table_column(db, table_name, column_name, parent_smooth_id);
   assert( n_rate == parent_smooth_id.size() );

   column_name         = "child_smooth_id";
   CppAD::vector<int>     child_smooth_id;
   get_table_column(db, table_name, column_name, child_smooth_id);
   assert( n_rate == child_smooth_id.size() );

   column_name         = "child_nslist_id";
   CppAD::vector<int>     child_nslist_id;
   get_table_column(db, table_name, column_name, child_nslist_id);
   assert( n_rate == child_nslist_id.size() );

   CppAD::vector<rate_struct> rate_table(number_rate_enum);
   for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
   {  if( rate_name[rate_id] != get_rate_name(rate_id) )
      {  msg  = "expected rate_name to be ";
         msg += get_rate_name(rate_id);
         msg += " but found " + rate_name[rate_id];
         error_exit(msg, table_name, rate_id);
      }
      if( child_smooth_id[rate_id] != DISMOD_AT_NULL_INT
      &&  child_nslist_id[rate_id] != DISMOD_AT_NULL_INT )
      {  msg = "both child_smooth_id and child_nslist_id are not null";
         error_exit(msg, table_name, rate_id);
      }
      rate_table[rate_id].rate             = rate_enum(rate_id);
      rate_table[rate_id].parent_smooth_id = parent_smooth_id[rate_id];
      rate_table[rate_id].child_smooth_id  = child_smooth_id[rate_id];
      rate_table[rate_id].child_nslist_id  = DISMOD_AT_NULL_INT;
      rate_table[rate_id].child_nslist_id  = child_nslist_id[rate_id];
   }

   return rate_table;
}

} // END DISMOD_AT_NAMESPACE
