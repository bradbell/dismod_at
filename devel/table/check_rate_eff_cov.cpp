// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin check_rate_eff_cov dev}

Check Only One Smoothing for Each Rate in zero_sum_child_rate
#############################################################

Syntax
******
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

Purpose
*******
Check for any errors in the :ref:`rate_eff_cov_table-name` .
This includes checking for an invalid :ref:`option_table@splitting_covariate`
option value.

data_cov_value
**************
is the :ref:`get_data_table@data_cov_value` .

avgint_cov_value
****************
is the :ref:`get_avgint_table@avgint_cov_value` .

covariate_table
***************
is the :ref:`get_covariate_table@covariate_table` .

node_table
**********
is the :ref:`get_node_table@node_table` .

rate_eff_cov_table
******************
is the :ref:`get_rate_eff_cov_table@rate_eff_cov_table` .

option_table
************
is the :ref:`get_option_table@option_table` .

{xrst_end check_rate_eff_cov}
*/
# include <set>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/check_rate_eff_cov.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
void check_rate_eff_cov(
   const CppAD::vector<double>&                data_cov_value     ,
   const CppAD::vector<double>&                avgint_cov_value   ,
   const CppAD::vector<covariate_struct>&      covariate_table    ,
   const CppAD::vector<node_struct>&           node_table         ,
   const CppAD::vector<rate_eff_cov_struct>&   rate_eff_cov_table ,
   const CppAD::vector<option_struct>&         option_table       )
// END_PROTOTYPE
{  using std::string;
   using CppAD::to_string;
   string msg;
   //
   // n_option
   size_t n_option = option_table.size();
   //
   // n_node
   size_t n_node = node_table.size();
   //
   // n_rate_eff_cov
   size_t n_rate_eff_cov = rate_eff_cov_table.size();
   //
   // n_covariate
   size_t n_covariate = covariate_table.size();
   //
   // n_data
   size_t n_data = 0;
   if( n_covariate != 0 )
   {  assert( data_cov_value.size() % n_covariate == 0 );
      n_data = data_cov_value.size() / n_covariate;
   }
   //
   // n_avgint
   size_t n_avgint = 0;
   if( n_covariate != 0 )
   {  assert( avgint_cov_value.size() % n_covariate == 0 );
      n_avgint = avgint_cov_value.size() / n_covariate;
   }
   //
   // splitting_covariate, option_id
   string splitting_covariate = "";
   size_t option_id = n_option;
   for(size_t i = 0; i < n_option; ++i)
   {  if( option_table[i].option_name == "splitting_covariate" )
      {  splitting_covariate = option_table[i].option_value;
         option_id           = i;
      }
   }
   if( splitting_covariate != "" && rate_eff_cov_table.size() == 0 )
   {  msg  = "splitting_covariate not null and rate_eff_cov table is empty";
      string table_name = "option";
      error_exit(msg, table_name, option_id);
   }
   //
   // split_covariate_id
   size_t split_covariate_id = n_covariate;
   for(size_t covariate_id = 0; covariate_id < n_covariate; ++covariate_id)
   {  if(
         covariate_table[covariate_id].covariate_name == splitting_covariate
      )
      {  split_covariate_id = covariate_id;
         assert( splitting_covariate != "" );
      }
   }
   if( splitting_covariate != "" && split_covariate_id == n_covariate )
   {  msg  = "splitting_covariate value is not a valid covariate name";
      string table_name = "option";
      error_exit(msg, table_name, option_id);
   }
   //
   // split_value_set
   std::set<double> split_value_set;
   if( splitting_covariate == "" )
      split_value_set.insert(0.0); // value is not used
   else
   {  assert( split_covariate_id < n_covariate);
      //
      for(size_t data_id = 0; data_id < n_data; ++data_id)
      {  size_t index = data_id * n_covariate + split_covariate_id;
         split_value_set.insert( data_cov_value[index] );
      }
      for(size_t avgint_id = 0; avgint_id < n_avgint; ++avgint_id)
      {  size_t index = avgint_id * n_covariate + split_covariate_id;
         split_value_set.insert( avgint_cov_value[index] );
      }
   }
   //
   // covariate_id_set
   std::set<int> covariate_id_set;
   for(size_t rate_eff_cov_id = 0; rate_eff_cov_id < n_rate_eff_cov; ++rate_eff_cov_id)
         covariate_id_set.insert( rate_eff_cov_table[rate_eff_cov_id].covariate_id );
   //
   // covariate_id_itr
   std::set<int>::const_iterator covariate_id_itr;
   //
   covariate_id_itr = covariate_id_set.find( int(split_covariate_id) );
   if( covariate_id_itr != covariate_id_set.end() )
   {  msg  = "splitting covariate_id = " + to_string(split_covariate_id);
      msg += " appears in the rate_eff_cov table";
      string table_name = "rate_eff_cov";
      error_exit(msg, table_name);
   }
   //
   // covariate_id
   for(
      covariate_id_itr =  covariate_id_set.begin();
      covariate_id_itr != covariate_id_set.end();
      ++covariate_id_itr )
   {  int covariate_id = *covariate_id_itr;
      //
      // split_value
      std::set<double>::const_iterator split_value_itr;
      for(
         split_value_itr =  split_value_set.begin();
         split_value_itr != split_value_set.end();
         ++split_value_itr )
      {  double split_value = *split_value_itr;
         //
         // found_node_id
         CppAD::vector<bool> found_node_id(n_node);
         for(size_t node_id = 0; node_id < n_node; ++node_id)
            found_node_id[node_id] = false;
         //
         for(size_t rate_eff_cov_id = 0; rate_eff_cov_id < n_rate_eff_cov; ++rate_eff_cov_id)
         {  // match
            bool match = true;
            match &= rate_eff_cov_table[rate_eff_cov_id].covariate_id == covariate_id;
            if( splitting_covariate != "" ) match &=
               rate_eff_cov_table[rate_eff_cov_id].split_value == split_value;
            //
            // found_node_id
            if( match )
            {  int node_id = rate_eff_cov_table[rate_eff_cov_id].node_id;
               if( node_id < 0 || n_node <= size_t(node_id) )
               {  msg  = "node_id " + to_string( node_id );
                  msg += " is not a valid node table node_id";
                  string table_name = "rate_eff_cov";
                  error_exit(msg, table_name, rate_eff_cov_id);
               }
               if( found_node_id[node_id] )
               {  msg  = "node_id " + to_string( node_id );
                  msg += " appears more than once with covariae_id ";
                  msg += to_string( covariate_id );
                  if( splitting_covariate != "" )
                     msg += " and split_value " + to_string(split_value);
                  else
                     msg += " and there is no splitting covariate";
                  string table_name = "rate_eff_cov";
                  error_exit(msg, table_name, rate_eff_cov_id);
               }
               found_node_id[node_id] = true;
            }
         }
         //
         // check_found_node_id
         for(size_t node_id = 0; node_id < n_node; ++node_id)
         {  if( ! found_node_id[node_id] )
            {  msg  = "covariate_id " + to_string(covariate_id);
               msg += " appread in this table but not with node_id ";
               msg += to_string(node_id);
               if( splitting_covariate != "" )
                  msg += " and split_value " + to_string(split_value);
               else
                  msg += " and there is no splitting covariate";
               string table_name = "rate_eff_cov";
               error_exit(msg, table_name);
            }
         }
      }
   }
}
} // END DISMOD_AT_NAMESPACE
