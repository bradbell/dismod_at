// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_age_table}

C++: Get the Age Table Information
##################################

Syntax
******
*age_table* = ``get_age_table`` ( *db* )

Purpose
*******
To read the :ref:`age_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

age_table
*********
The return value *age_table* has prototype

   ``CppAD::vector<double>`` *age_table*

For each :ref:`age_table@age_id` ,

   *age_table* [ *age_id* ]

is the age for the corresponding
:ref:`age_table@age` .
{xrst_toc_hidden
   example/devel/table/get_age_table_xam.cpp
}
Example
*******
The file :ref:`get_age_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_age_table}
-----------------------------------------------------------------------------
*/



# include <dismod_at/get_age_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<double> get_age_table(sqlite3* db)
{  using std::string;

   string table_name  = "age";
# ifndef NDEBUG
   size_t n_age       = check_table_id(db, table_name);
# else
   check_table_id(db, table_name);
# endif

   string column_name =  "age";
   CppAD::vector<double>  age;
   get_table_column(db, table_name, column_name, age);
   assert( age.size() == n_age );

   return age;
}

} // END DISMOD_AT_NAMESPACE
