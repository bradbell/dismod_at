// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_avgint_table}

C++: Get the Average Integrand Case Table
#########################################

Syntax
******

| ``get_avgint_table`` (
| |tab| *db* , *n_covariate* , *age_min* , *age_max* , *time_min* , *time_max* ,
| |tab| *avgint_table* , *avgint_cov_value*
| )

Purpose
*******
To read the :ref:`avgint_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

n_covariate
***********
This argument has prototype

   ``size_t n_covariate``

and is the size of the :ref:`covariate_table<get_covariate_table-name>` .

age_min
*******
This argument has prototype

   ``double`` *age_min*

and is the minimum value in the :ref:`age_table-name` .
If an :ref:`data_table@age_lower` value is less than
*age_min* , an error is reported.

age_max
*******
This argument has prototype

   ``double`` *age_max*

and is the maximum value in the :ref:`age_table-name` .
If an :ref:`data_table@age_upper` value is greater than
*age_max* , an error is reported.
The condition

   ``age_lower`` <= ``age_upper``

is also checked.

time_min
********
This argument has prototype

   ``double`` *time_min*

and is the minimum value in the :ref:`time_table-name` .
If an :ref:`data_table@time_lower` value is less than
*time_min* , an error is reported.

time_max
********
This argument has prototype

   ``double`` *time_max*

and is the maximum value in the :ref:`time_table-name` .
If an :ref:`data_table@time_upper` value is greater than
*time_max* , an error is reported.
The condition

   ``time_lower`` <= ``time_upper``

is also checked.

avgint_table
************
This argument has prototype

   ``CppAD::vector<avgint_struct>&`` *avgint_table*

On input its size is zero and upon return it has one element for
each row in the avgint table.
For each :ref:`avgint_table@avgint_id` ,

   *avgint_table* [ *avgint_id* ]

is the information for the corresponding row.

avgint_struct
*************
This is a structure with the following fields

.. list-table::

   * - Type
     - Field
     - Description
   * - ``int``
     - ``integrand_id``
     - The :ref:`avgint_table@integrand_id` for this measurement
   * - ``int``
     - ``node_id``
     - The :ref:`avgint_table@node_id` for this measurement
   * - ``int``
     - ``subgroup_id``
     - The :ref:`avgint_table@subgroup_id` for this measurement
   * - ``int``
     - ``weight_id``
     - The :ref:`avgint_table@weight_id` for this measurement
   * - ``double``
     - ``age_lower``
     - The :ref:`avgint_table@age_lower` for this measurement
   * - ``double``
     - ``age_upper``
     - The :ref:`avgint_table@age_upper` for this measurement
   * - ``double``
     - ``time_lower``
     - The :ref:`avgint_table@time_lower` for this measurement
   * - ``double``
     - ``time_upper``
     - The :ref:`avgint_table@time_upper` for this measurement

avgint_cov_value
****************
This argument has prototype

   ``CppAD::vector<double>&`` *avgint_cov_value*

On input its size is zero.
Upon return, its size is the number of rows in the avgint table times
the number of covariates.
For each
:ref:`covariate_table@covariate_id` and
*avgint_id* pair

   ``avgint_cov_value`` [ *avgint_id* * *n_covariate* + *covariate_id* ]

is the corresponding covariate value.
{xrst_toc_hidden
   example/devel/table/get_avgint_table_xam.cpp
}
Example
*******
The file :ref:`get_avgint_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_avgint_table}
-----------------------------------------------------------------------------
*/
# include <dismod_at/get_avgint_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void get_avgint_table(
   sqlite3* db        ,
   size_t                          n_covariate         ,
   double                          age_min             ,
   double                          age_max             ,
   double                          time_min            ,
   double                          time_max            ,
   CppAD::vector<avgint_struct>&   avgint_table        ,
   CppAD::vector<double>&          avgint_cov_value    )
{  using std::string;
   //
   string table_name  = "avgint";
   string column_name;
   size_t n_avgint      = check_table_id(db, table_name);
   //
   // avgint_table
   {  // read avgint_table

      column_name        = "integrand_id";
      CppAD::vector<int>    integrand_id;
      get_table_column(db, table_name, column_name, integrand_id);
      assert( n_avgint == integrand_id.size() );

      column_name        = "node_id";
      CppAD::vector<int>    node_id;
      get_table_column(db, table_name, column_name, node_id);
      assert( n_avgint == node_id.size() );

      column_name        = "weight_id";
      CppAD::vector<int>    weight_id;
      get_table_column(db, table_name, column_name, weight_id);
      assert( n_avgint == weight_id.size() );

      column_name           =  "age_lower";
      CppAD::vector<double>     age_lower;
      get_table_column(db, table_name, column_name, age_lower);
      assert( n_avgint == age_lower.size() );

      column_name           =  "age_upper";
      CppAD::vector<double>     age_upper;
      get_table_column(db, table_name, column_name, age_upper);
      assert( n_avgint == age_upper.size() );

      column_name           =  "time_lower";
      CppAD::vector<double>     time_lower;
      get_table_column(db, table_name, column_name, time_lower);
      assert( n_avgint == time_lower.size() );

      column_name           =  "time_upper";
      CppAD::vector<double>     time_upper;
      get_table_column(db, table_name, column_name, time_upper);
      assert( n_avgint == time_upper.size() );

      column_name           = "subgroup_id";
      CppAD::vector<int>       subgroup_id;
      get_table_column(db, table_name, column_name, subgroup_id);
      assert( n_avgint == subgroup_id.size() );

      // set avgint_table
      assert( avgint_table.size() == 0 );
      avgint_table.resize(n_avgint);
      for(size_t i = 0; i < n_avgint; i++)
      {  avgint_table[i].integrand_id  = integrand_id[i];
         avgint_table[i].node_id       = node_id[i];
         avgint_table[i].subgroup_id   = subgroup_id[i];
         avgint_table[i].weight_id     = weight_id[i];
         avgint_table[i].age_lower     = age_lower[i];
         avgint_table[i].age_upper     = age_upper[i];
         avgint_table[i].time_lower    = time_lower[i];
         avgint_table[i].time_upper    = time_upper[i];
      }
   }
   //
   // set avgint_cov_value
   assert( avgint_cov_value.size() == 0 );
   avgint_cov_value.resize(n_avgint * n_covariate);
   for(size_t j = 0; j < n_covariate; j++)
   {  std::stringstream ss;
      ss << "x_" << j;
      column_name = ss.str();
      CppAD::vector<double> x_j;
      get_table_column(db, table_name, column_name, x_j);
      for(size_t i = 0; i < n_avgint; i++)
         avgint_cov_value[ i * n_covariate + j ] = x_j[i];
   }

   // check for erorr conditions
   string msg;
   for(size_t avgint_id = 0; avgint_id < n_avgint; avgint_id++)
   {  // ------------------------------------------------------------
      int subgroup_id = avgint_table[avgint_id].subgroup_id;
      if( subgroup_id == DISMOD_AT_NULL_INT )
      {  msg = "subgroup_id is null";
         error_exit(msg, table_name, avgint_id);
      }
      // -------------------------------------------------------------
      double age_lower  = avgint_table[avgint_id].age_lower;
      double age_upper  = avgint_table[avgint_id].age_upper;
      if( age_lower < age_min )
      {  msg = "age_lower is less than minimum age in age table";
         error_exit(msg, table_name, avgint_id);
      }
      if( age_max < age_upper )
      {  msg = "age_upper is greater than maximum age in age table";
         error_exit(msg, table_name, avgint_id);
      }
      if( age_upper < age_lower )
      {  msg = "age_lower is greater than age_upper";
         error_exit(msg, table_name, avgint_id);
      }
      // ------------------------------------------------------------
      double time_lower = avgint_table[avgint_id].time_lower;
      double time_upper = avgint_table[avgint_id].time_upper;
      if( time_lower < time_min )
      {  msg = "time_lower is less than minimum time in time table";
         error_exit(msg, table_name, avgint_id);
      }
      if( time_max < time_upper )
      {  msg = "time_upper is greater than maximum time in time table";
         error_exit(msg, table_name, avgint_id);
      }
      if( time_upper < time_lower )
      {  msg = "time_lower is greater than time_upper";
         error_exit(msg, table_name, avgint_id);
      }
   }
   return;
}

} // END DISMOD_AT_NAMESPACE
