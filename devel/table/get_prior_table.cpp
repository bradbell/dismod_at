// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-24 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_prior_table dev}

C++: Get the Prior Table Information
####################################

Syntax
******
*prior_table* = ``get_prior_table`` ( *db* , *density_table* )

Prototype
*********
{xrst_literal
   // BEGIN_GET_PRIOR_TABLE
   // END_GET_PRIOR_TABLE
}

Purpose
*******
To read the :ref:`prior_table-name` and return it as a C++ data structure.

db
**
The argument *db*
is an open connection to the database.

prior_table
***********
For each :ref:`prior_table@prior_id` ,

   *prior_table* [ *prior_id* ]

is the information for the corresponding prior.

density_table
*************
The :ref:`density_table-name` is used to check for errors in the prior table;
e.g., the standard deviation for a prior must be positive unless the
density is uniform.

prior_struct
************
This is a structure with the following fields

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - ``std::string``
     - ``prior_name``
     - The :ref:`prior_table@prior_name` for this prior
   * - ``int``
     - ``density_id``
     - The :ref:`prior_table@density_id` for this prior
   * - ``double``
     - ``lower``
     - The :ref:`prior_table@lower` limit for this prior
   * - ``double``
     - ``upper``
     - The :ref:`prior_table@upper` limit for this prior
   * - ``double``
     - ``mean``
     - The :ref:`prior_table@mean` for this prior
   * - ``double``
     - ``std``
     - The :ref:`prior_table@std` for this prior
   * - ``double``
     - ``eta``
     - The :ref:`prior_table@eta` for this prior
   * - ``double``
     - ``nu``
     - The :ref:`prior_table@nu` for this prior

Checks
******
The values in the :ref:`prior_table-name` are checked to make sure that:

#. The limits and mean satisfy *lower* <= *mean* <= *upper* .
#. The standard deviation *std*  > 0
   (except for the uniform density case).
#. In the log density cases *eta* is not null
   and *eta*  > 0 .
#. In the Student density cases *nu* is not null
   and *nu*  > 2 .

{xrst_toc_hidden
   example/devel/table/get_prior_table_xam.cpp
}
Example
*******
The file :ref:`get_prior_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_prior_table}
-----------------------------------------------------------------------------
*/
# include <cmath>
# include <dismod_at/get_prior_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/get_density_table.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_GET_PRIOR_TABLE
CppAD::vector<prior_struct> get_prior_table(
   sqlite3*                           db            ,
   const CppAD::vector<density_enum>& density_table )
// END_GET_PRIOR_TABLE
{  using std::string;

   // user for error messaging
   string msg;

   string table_name  = "prior";
   size_t n_prior      = check_table_id(db, table_name);

   string column_name =  "prior_name";
   CppAD::vector<string>  prior_name;
   get_table_column(db, table_name, column_name, prior_name);
   assert( prior_name.size() == n_prior );

   column_name        =  "density_id";
   CppAD::vector<int>     density_id;
   get_table_column(db, table_name, column_name, density_id);
   assert( density_id.size() == n_prior );

   column_name        =  "lower";
   CppAD::vector<double>  lower;
   get_table_column(db, table_name, column_name, lower);
   assert( lower.size() == n_prior );

   column_name        =  "upper";
   CppAD::vector<double>  upper;
   get_table_column(db, table_name, column_name, upper);
   assert( upper.size() == n_prior );

   column_name        =  "mean";
   CppAD::vector<double>  mean;
   get_table_column(db, table_name, column_name, mean);
   assert( mean.size() == n_prior );

   column_name        =  "std";
   CppAD::vector<double>  std;
   get_table_column(db, table_name, column_name, std);
   assert( std.size() == n_prior );

   column_name        =  "eta";
   CppAD::vector<double>  eta;
   get_table_column(db, table_name, column_name, eta);
   assert( eta.size() == n_prior );

   column_name        =  "nu";
   CppAD::vector<double>  nu;
   get_table_column(db, table_name, column_name, nu);
   assert( nu.size() == n_prior );

   double minus_infinity = std::atof("-inf");
   double plus_infinity  = std::atof("+inf");
   CppAD::vector<prior_struct> prior_table(n_prior);
   bool limit_warning = false;
   for(size_t i = 0; i < n_prior; i++)
   {  prior_table[i].prior_name  = prior_name[i];
      prior_table[i].density_id = density_id[i];
      //
      size_t number_limit = 0;
      //
      prior_table[i].lower      = lower[i];
      if( std::isnan( lower[i] ) )
         prior_table[i].lower      = minus_infinity;
      else
         ++number_limit;
      //
      prior_table[i].upper      = upper[i];
      if( std::isnan( upper[i] ) )
         prior_table[i].upper      = plus_infinity;
      else
         ++number_limit;
      //
      if( (number_limit == 1) & (! limit_warning) )
      {  msg = "A lower or upper limit is null (infinite) but not both.\n"
            "This can lead to problems in Ipopt initialization.\n"
            "It is currently a warning but will be changed to an error.";
         log_message(db, &std::cerr, "warning", msg, table_name, i);
         limit_warning = true;
      }
      //
      prior_table[i].mean       = mean[i];
      prior_table[i].std        = std[i];
      prior_table[i].eta        = eta[i];
      prior_table[i].nu         = nu[i];
      //
      // ok
      bool ok;
      //
      // check density
      ok =  density_table[ density_id[i] ] != binomial_enum;
      if( ! ok )
      {  msg = "cannot use binomial density in prior table";
         error_exit(msg, table_name, i);
      }
      //
      // check mean
      ok = mean[i] <= prior_table[i].upper;
      if(  ! ok )
      {  msg = "mean greater than upper limit";
         error_exit(msg, table_name, i);
      }
      ok = prior_table[i].lower <= mean[i];
      if(  ! ok )
      {  msg = "mean less than lower limit";
         error_exit(msg, table_name, i);
      }
      //
      // check std
      ok = density_table[density_id[i]] == uniform_enum || std[i] > 0.0;
      if( ! ok )
      {  msg = "std <= 0 and density is not uniform";
         error_exit(msg, table_name, i);
      }
      //
      // check eta
      switch( density_table[density_id[i]]  )
      {  case log_gaussian_enum:
         case log_laplace_enum:
         case log_students_enum:
         ok = eta[i] >= 0.0;
         break;

         default:
         ok = true;
      }
      if( ! ok )
      {  msg = "This is a log density and eta not greater than or equal zero.";
         error_exit(msg, table_name, i);
      }
      //
      // check nu
      switch( density_table[density_id[i]] )
      {  case students_enum:
         case log_students_enum:
         ok = nu[i] > 2.0;
         break;

         default:
         ok = true;
      }
      if( ! ok )
      {  msg = "This is a Student's density and nu not greater than two.";
         error_exit(msg, table_name, i);
      }
   }
   return prior_table;
}

} // END DISMOD_AT_NAMESPACE
