// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin check_zero_sum$$
$spell
   const
   CppAD
   struct
   pini
   sqlite
   nslist
$$

$section Check Only One Smoothing for Each Rate in zero_sum_child_rate$$

$head syntax$$
$codei%check_zero_sum(
   %db%, %rate_table%, %option_table%
)%$$

$head Purpose$$
For each $icode rate_name$$ in
$cref/
   zero_sum_child_rate/
   option_table/
   Zero Sum Constraints/
   zero_sum_child_rate
/$$,
the corresponding
$cref/child_nslist_id/rate_table/child_nslist_id/$$ must be null.

$head db$$
This argument has prototype
$codei%
   sqlite3* %db%
%$$
and is the database connection for $cref/logging/log_message/$$ errors.

$head rate_table$$
This argument has prototype
$codei%
   const CppAD::vector<rate_struct>& %rate_table%
%$$
and it is the
$cref/rate_table/get_rate_table/rate_table/$$.
For this table,
only the $icode child_nslist_id$$ column is used.

$head option_table$$
This argument has prototype
$codei%
   const CppAD::vector<option_struct>& %option_table%
%$$
and it is the
$cref/option_table/get_option_table/option_table/$$.
For this table, only the row  with
$codei%
   %option_name% = "zero_sum_child_rate"
%$$
is used.

$end
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
