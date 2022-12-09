// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_density_table}
{xrst_spell
   enum
   nonsmooth
}

C++: Get the Density Table Information
######################################

Syntax
******

| *density_table* = ``get_density_table`` ( *db* )
| *is_log* = ``log_density`` ( *density* )
| *is_nonsmooth* = ``nonsmooth_density`` ( *density* )
| *is_censored* = ``censored_density`` ( *density* )

Purpose
*******
To read the :ref:`density_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

density_table
*************
The return value *density_table* has prototype

   ``CppAD::vector<density_enum>`` *density_table*

For each :ref:`density_table@density_id` ,

   *density_table* [ *density_id* ]

is the enum value for the corresponding
:ref:`density_table@density_name` .

density_enum
************
This enum type has the following values:

.. csv-table::
   :widths: auto
   :align: right

   *value*,*density_name*
   ``uniform_enum``,``uniform``
   ``gaussian_enum``,``gaussian``
   ``cen_gaussian_enum``,``c_gaussian``
   ``log_gaussian_enum``,``log_gaussian``
   ``cen_log_gaussian_enum``,``cen_log_gaussian``
   ``laplace_enum``,``laplace``
   ``cen_laplace_enum``,``laplace``
   ``log_laplace_enum``,``log_laplace``
   ``cen_log_laplace_enum``,``cen_log_laplace``
   ``students_enum``,``students``
   ``log_students_enum``,``log_students``

The number of these enum values is ``number_density_enum`` .

log_density
***********
The return value *is_log* is true if the corresponding density is
:ref:`density_table@Notation@Log Scaled` .
It is false otherwise.

nonsmooth_density
*****************
The return value *is_nonsmooth* is true if the corresponding density is
:ref:`density_table@Notation@Nonsmooth` .
It is false otherwise.

censored_density
****************
The return value *is_censored* is true if the corresponding density is
:ref:`density_table@Notation@Censored` .
It is false otherwise.

density_enum2name
*****************
This is a global variable.
If *density* , is an ``density_enum`` value,
``density_enum2name`` [ *density* ] is a ``const char*[]``
representation of the *density_name* corresponding to the enum value.
{xrst_toc_hidden
   example/devel/table/get_density_table_xam.cpp
}
Example
*******
The file :ref:`get_density_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_density_table}
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_density_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// density names in scame order as enum type in get_density.hpp
const char* density_enum2name[] = {
   "uniform",
   "gaussian",
   "cen_gaussian",
   "log_gaussian",
   "cen_log_gaussian",
   "laplace",
   "cen_laplace",
   "log_laplace",
   "cen_log_laplace",
   "students",
   "log_students"
};


CppAD::vector<density_enum> get_density_table(sqlite3* db)
{  using std::string;
   using CppAD::to_string;
   using CppAD::vector;
   //
   // for error messaging
   string message;
   //
   // check density names in same order as enum type in get_density_table.hpp
# ifndef NDEBUG
   const char** enum2name = density_enum2name;
   assert( string("uniform")          == enum2name[uniform_enum] );
   assert( string("gaussian")         == enum2name[gaussian_enum] );
   assert( string("cen_gaussian")     == enum2name[cen_gaussian_enum] );
   assert( string("log_gaussian")     == enum2name[log_gaussian_enum] );
   assert( string("cen_log_gaussian") == enum2name[cen_log_gaussian_enum] );
   assert( string("laplace")          == enum2name[laplace_enum] );
   assert( string("cen_laplace")      == enum2name[cen_laplace_enum] );
   assert( string("log_laplace")      == enum2name[log_laplace_enum] );
   assert( string("cen_log_laplace")  == enum2name[cen_log_laplace_enum] );
   assert( string("students")         == enum2name[students_enum] );
   assert( string("log_students")     == enum2name[log_students_enum] );
# endif
   //
   vector<bool> found(number_density_enum);
   for(size_t i = 0; i < size_t(number_density_enum); i++)
      found[i] = false;
   //
   string table_name  = "density";
   size_t n_density   = check_table_id(db, table_name);
   //
   string column_name =  "density_name";
   CppAD::vector<string>  density_name;
   get_table_column(db, table_name, column_name, density_name);
   assert( n_density == density_name.size() );
   //
   CppAD::vector<density_enum> density_table(n_density);
   for(size_t density_id = 0; density_id < n_density; density_id++)
   {  string name          = density_name[density_id];
      density_enum density = number_density_enum;
      for(size_t j = 0; j < number_density_enum; ++j)
      {  if( name == density_enum2name[j] )
            density = density_enum(j);
      }
      if( density == number_density_enum )
      {  string msg  = name;
         msg        += " is not a valid choice for a density_name";
         error_exit(msg, table_name, density_id);
      }
      if( found[density] )
      {  string msg = "The density_name " + name;
         msg += " appears more than once";
         error_exit(msg, table_name, density_id);
      }
      found[density] = true;
      density_table[density_id] = density;
   }
   return density_table;
}
bool log_density(density_enum density)
{  bool result = density == log_gaussian_enum;
   result     |= density == cen_log_gaussian_enum;
   result     |= density == log_laplace_enum;
   result     |= density == cen_log_laplace_enum;
   result     |= density == log_students_enum;
   return result;
}
bool nonsmooth_density(density_enum density)
{  bool result = density == laplace_enum;
   result     |= density == cen_laplace_enum;
   result     |= density == log_laplace_enum;
   result     |= density == cen_log_laplace_enum;
   return result;
}
bool censored_density(density_enum density)
{  bool result = density == cen_gaussian_enum;;
   result     |= density == cen_log_gaussian_enum;
   result     |= density == cen_laplace_enum;
   result     |= density == cen_log_laplace_enum;
   return result;
}

} // END DISMOD_AT_NAMESPACE
