// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_smooth_grid dev}

C++: Get the Smoothing Grid Information
#######################################

Syntax
******
*smooth_grid* = ``get_smooth_grid`` ( *db* )

Purpose
*******
To read the :ref:`smooth_grid_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

smooth_grid
***********
The return value *smooth_grid* has prototype

   ``CppAD::vector<smooth_grid_struct>`` *smooth_grid*

For each :ref:`smooth_grid_table@smooth_grid_id` ,

   *smooth_grid* [ *smooth_grid_id* ]

is the information for a specific smoothing and specific age and time.

smooth_grid_struct
******************
This is a structure with the following fields

.. list-table::

   * - Type
     - Field
     - Description
   * - ``int``
     - ``smooth_id``
     - The :ref:`smooth_grid_table@smooth_id` for this smoothing.
   * - ``int``
     - ``age_id``
     - The :ref:`smooth_grid_table@age_id` for this grid point
   * - ``int``
     - ``time_id``
     - The :ref:`smooth_grid_table@time_id` for this grid point
   * - ``int``
     - ``value_prior_id``
     - The :ref:`smooth_grid_table@value_prior_id`
       for this smoothing, age, and time.
   * - ``int``
     - ``dage_prior_id``
     - The :ref:`smooth_grid_table@dage_prior_id`
       for this smoothing, age, and time.
   * - ``int``
     - ``dtime_prior_id``
     - The :ref:`smooth_grid_table@dtime_prior_id`
       for this smoothing, age, and time.
   * - ``int``
     - ``value_prior_id``
     - The :ref:`smooth_grid_table@const_value`
       for this smoothing, age, and time.

Check
*****
The values in the ``smooth_grid_table`` are checked to make sure that
one of the following two cases holds:

#. :ref:`smooth_grid_table@const_value` is not ``null`` and
   :ref:`smooth_grid_table@value_prior_id` is ``null`` .
#. *value_prior* is not ``null`` and *const_value* is null.

{xrst_toc_hidden
   example/devel/table/get_smooth_grid_xam.cpp
}
Example
*******
The file :ref:`get_smooth_grid_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_smooth_grid}
-----------------------------------------------------------------------------
*/

# include <cmath>


# include <dismod_at/get_smooth_grid.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<smooth_grid_struct> get_smooth_grid(
   sqlite3*                           db            ,
   const CppAD::vector<density_enum>& density_table ,
   const CppAD::vector<prior_struct>& prior_table
)
{  using std::string;

   string table_name  = "smooth_grid";
   size_t n_smooth    = check_table_id(db, table_name);

   string column_name = "smooth_id";
   CppAD::vector<int>    smooth_id;
   get_table_column(db, table_name, column_name, smooth_id);
   assert( smooth_id.size() == n_smooth );

   column_name        =  "age_id";
   CppAD::vector<int>     age_id;
   get_table_column(db, table_name, column_name, age_id);
   assert( age_id.size() == n_smooth );

   column_name        =  "time_id";
   CppAD::vector<int>     time_id;
   get_table_column(db, table_name, column_name, time_id);
   assert( time_id.size() == n_smooth );

   column_name        =  "value_prior_id";
   CppAD::vector<int>     value_prior_id;
   get_table_column(db, table_name, column_name, value_prior_id);
   assert( value_prior_id.size() == n_smooth );

   column_name        =  "dage_prior_id";
   CppAD::vector<int>     dage_prior_id;
   get_table_column(db, table_name, column_name, dage_prior_id);
   assert( dage_prior_id.size() == n_smooth );

   column_name        =  "dtime_prior_id";
   CppAD::vector<int>     dtime_prior_id;
   get_table_column(db, table_name, column_name, dtime_prior_id);
   assert( dtime_prior_id.size() == n_smooth );

   column_name        =  "const_value";
   CppAD::vector<double> const_value;
   get_table_column(db, table_name, column_name, const_value);
   assert( const_value.size() == n_smooth );

   size_t n_prior = prior_table.size();
   CppAD::vector<smooth_grid_struct> smooth_grid(n_smooth);
   for(size_t i = 0; i < n_smooth; i++)
   {  smooth_grid[i].smooth_id      = smooth_id[i];
      smooth_grid[i].age_id         = age_id[i];
      smooth_grid[i].time_id        = time_id[i];
      smooth_grid[i].value_prior_id = value_prior_id[i];
      smooth_grid[i].dage_prior_id  = dage_prior_id[i];
      smooth_grid[i].dtime_prior_id = dtime_prior_id[i];
      smooth_grid[i].const_value    = const_value[i];
      if( std::isnan(const_value[i]) )
      if( value_prior_id[i] == DISMOD_AT_NULL_INT)
      {  string msg = "both const_value and value_prior_id are null.\n";
         error_exit(msg, table_name, i);
      }
      if( ! std::isnan(const_value[i]) )
      if( value_prior_id[i] != DISMOD_AT_NULL_INT)
      {  string msg = "both const_value and value_prior_id are not null.\n";
         error_exit(msg, table_name, i);
      }
      // if prior_id >= n_prior and prior_id != DISMOD_AT_NULL_INT
      // an error is printed elsewhere
      int prior_id = dage_prior_id[i];
      if( size_t(prior_id) < n_prior )
      {  int density_id = prior_table[prior_id].density_id;
         if( censored_density( density_table[density_id] ) )
         {  string msg = "dage_prior_id corresponds to a "
            "censored distribution";
            error_exit(msg, table_name, i);
         }
      }
      prior_id = dtime_prior_id[i];
      if( size_t(prior_id) < n_prior )
      {  int density_id = prior_table[prior_id].density_id;
         if( censored_density( density_table[density_id] ) )
         {  string msg = "dtime_prior_id corresponds to a "
            "censored distribution";
            error_exit(msg, table_name, i);
         }
      }
   }
   return smooth_grid;
}

} // END DISMOD_AT_NAMESPACE
