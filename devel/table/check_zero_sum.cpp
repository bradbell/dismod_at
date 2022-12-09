// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin check_zero_sum}

Check Only One Smoothing for Each Rate in zero_sum_child_rate
#############################################################

syntax
******

| ``check_zero_sum`` (
| |tab| *db* , *rate_table* , *option_table*
| )

Purpose
*******
For each *rate_name* in
:ref:`option_table@Zero Sum Constraints@zero_sum_child_rate` ,
the corresponding
:ref:`rate_table@child_nslist_id` must be null.

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
only the *child_nslist_id* column is used.

option_table
************
This argument has prototype

   ``const CppAD::vector<option_struct>&`` *option_table*

and it is the
:ref:`get_option_table@option_table` .
For this table, only the row  with

   *option_name* = ``"zero_sum_child_rate"``

is used.

{xrst_end check_zero_sum}
*/
# include <dismod_at/check_zero_sum.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void check_zero_sum(
   sqlite3*                                  db            ,
   const CppAD::vector<rate_struct>&         rate_table    ,
   const CppAD::vector<option_struct>&       option_table  )
{  assert( rate_table.size()   == number_rate_enum );
   using std::string;
   string msg;
   //
   for(size_t option_id = 0; option_id < option_table.size(); option_id++)
   if( option_table[option_id].option_name == "zero_sum_child_rate" )
   {  string option_value = option_table[option_id].option_value;
      size_t option_size  = option_value.size();
      for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
      {  string rate_name  = get_rate_name(rate_id);
         bool found        = option_value.find( rate_name ) < option_size;
         int child_nslist_id = rate_table[rate_id].child_nslist_id;
         if( found && child_nslist_id != DISMOD_AT_NULL_INT )
         {  msg = "found " + rate_name + " in value for zero_sum_child_rate";
            msg += " option and corresponding child_nslist_id not null";
            string table_name = "rate";
            error_exit(msg, table_name, rate_id);
         }
      }
   }
}

} // END DISMOD_AT_NAMESPACE
