// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin check_pini_n_age}

Check Initial Prevalence Grid Has One Age
#########################################

syntax
******
``check_pini_n_age`` ( *db* , *rate_table* , *smooth_table* )

db
**
This argument has prototype

   ``sqlite3`` * *db*

and is the database connection for :ref:`logging<log_message-name>` errors.

rate_table
**********
This argument has prototype

   ``const CppAD::vector<rate_struct>&`` *rate_table*

and it is the
:ref:`get_rate_table@rate_table` .
For this table,
only the fields ``parent_smooth_id`` and ``child_smooth_id``
are used.

smooth_table
************
This argument has prototype

   ``const CppAD::vector<smooth_struct>&`` *smooth_table*

and it is the
:ref:`get_smooth_table@smooth_table` .
For this table, only the ``n_age`` field is used.
{xrst_toc_hidden
   example/devel/table/check_pini_n_age_xam.cpp
}
Example
*******
:ref:`check_pini_n_age_xam.cpp-name`

{xrst_end check_pini_n_age}
*/
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_smooth_table.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void check_pini_n_age(
   sqlite3*                            db            ,
   const CppAD::vector<rate_struct>&   rate_table    ,
   const CppAD::vector<smooth_struct>& smooth_table  )
{  assert( rate_table.size()   == number_rate_enum );
   std::string message;
   std::string table_name = "rate";
   //
   size_t rate_id = size_t( pini_enum );
   size_t parent_smooth_id = rate_table[rate_id].parent_smooth_id;
   if( parent_smooth_id != DISMOD_AT_NULL_SIZE_T )
   {  size_t n_age = smooth_table[parent_smooth_id].n_age;
      if( n_age != 1 )
      {  message = "parent_smooth_id, for pini, corresponds to a smoothing"
            " with n_age not equal to one";
      }
   }
   size_t child_smooth_id  = rate_table[rate_id].child_smooth_id;
   if( child_smooth_id != DISMOD_AT_NULL_SIZE_T )
   {  size_t n_age = smooth_table[child_smooth_id].n_age;
      if( n_age != 1 )
      {  message = "child_smooth_id, for pini, corresponds to a smoothing"
            " with n_age not equal to one";
      }
   }
   if( message != "" )
      error_exit(message, table_name, rate_id);
}

} // END DISMOD_AT_NAMESPACE
