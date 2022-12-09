// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin check_child_nslist}

Check Assumptions About Lists of Child Node Smoothing Pairs
###########################################################

syntax
******

| ``check_child_nslist`` (
| |tab| *db* ,
| |tab| *option_table* ,
| |tab| *rate_table* ,
| |tab| *node_table* ,
| |tab| *smooth_table* ,
| |tab| *nslist_table* ,
| |tab| *nslist_pair*
| )

Purpose
*******
Checks the assumptions related to the list specified by each
:ref:`rate_table@child_nslist_id` .

db
**
This argument has prototype

   ``sqlite3`` * *db*

and is the database connection for :ref:`logging<log_message-name>` errors.

option_table
************
This argument has prototype

   ``const CppAD::vector<option_struct>&`` *option_table*

and it is the
:ref:`get_option_table@option_table` .

rate_table
**********
This argument has prototype

   ``const CppAD::vector<rate_struct>&`` *rate_table*

and it is the
:ref:`get_rate_table@rate_table` .

node_table
**********
This argument has prototype

   ``const CppAD::vector<node_struct>&`` *node_table*

and it is the
:ref:`get_node_table@node_table` .

smooth_table
************
This argument has prototype

   ``const CppAD::vector<smooth_struct>&`` *smooth_table*

and it is the
:ref:`get_smooth_table@smooth_table` .

nslist_table
************
This argument has prototype

   ``const CppAD::vector<std::string>&`` *nslist_table*

and it is the
:ref:`get_nslist_table@nslist_table` .

nslist_pair
***********
This argument has prototype

   ``const CppAD::vector<nslist_pair_struct>&`` *nslist_pair*

and it is the
:ref:`get_nslist_pair@nslist_pair` .

{xrst_end check_child_nslist}
*/
# include <dismod_at/check_child_nslist.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void check_child_nslist(
   sqlite3*                                 db             ,
   const CppAD::vector<option_struct>&      option_table   ,
   const CppAD::vector<rate_struct>&        rate_table     ,
   const CppAD::vector<node_struct>&        node_table     ,
   const CppAD::vector<std::string>&        nslist_table   ,
   const CppAD::vector<nslist_pair_struct>& nslist_pair    )
{  assert( rate_table.size()   == number_rate_enum );
   using std::string;
   string msg;
   //
   // parent_node_id
   int    parent_node_id   = DISMOD_AT_NULL_INT;
   string parent_node_name = "";
   for(size_t i = 0; i < option_table.size(); i++)
   {  if( option_table[i].option_name == "parent_node_id" )
      {  if( option_table[i].option_value != "" )
             parent_node_id =
               std::atoi( option_table[i].option_value.c_str() );
      }
      if( option_table[i].option_name == "parent_node_name" )
      {  if( option_table[i].option_value != "" )
         {  parent_node_name = option_table[i].option_value;
            size_t n_node    = node_table.size();
            for(size_t node_id = 0; node_id < n_node; ++node_id)
            {  if( node_table[node_id].node_name == parent_node_name )
                  parent_node_id = int( node_id );
            }
         }
      }
   }
   if( parent_node_id == DISMOD_AT_NULL_INT )
   {  if( parent_node_name != "" )
      {  msg  = "Cannot find parent_node_name in node table\n";
         msg += "parent_node_name in option table is ";
         msg += parent_node_name;
         error_exit(msg);
      }
      msg  = "Cannot find parent_node_name or parent_node_id ";
      string table_name = "option";
      error_exit(msg, table_name);
   }
   else if( size_t( parent_node_id ) >= node_table.size() )
   {  msg  = "parent_node_id in option table greater than or equal ";
      msg += "number of nodes in node table";
      error_exit(msg);
   }
   //
   // child_node_id
   CppAD::vector<size_t> child_node_id(0);
   for(size_t node_id = 0; node_id < node_table.size(); node_id++)
   {  if( node_table[node_id].parent == parent_node_id )
         child_node_id.push_back( node_id );
   }
   size_t n_child = child_node_id.size();
   //
   // child_found
   //
   for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
   if(  rate_table[rate_id].child_nslist_id != DISMOD_AT_NULL_INT )
   {  int nslist_id  = rate_table[rate_id].child_nslist_id;
      //
      // initialize child_found for this rate
      CppAD::vector<bool> child_found(n_child);
      for(size_t i = 0; i < n_child; i++)
         child_found[i] = false;
      //
      // run thourgh the entries in this list
      for(size_t pair_id = 0; pair_id < nslist_pair.size(); pair_id++)
      if( nslist_pair[pair_id].nslist_id == nslist_id )
      {  size_t node_id        = nslist_pair[pair_id].node_id;
         string table_name     = "nslist_pair";
         if( node_table[node_id].parent != parent_node_id )
         {  msg  = node_table[node_id].node_name;
            msg += " is not a child of the parent node ";
            msg += node_table[parent_node_id].node_name;
            msg += " specified in the option table";
            error_exit(msg, table_name, pair_id);
         }
         for(size_t i = 0; i < n_child; i++)
         {  if( child_node_id[i] == node_id )
            {  if( child_found[i] )
               {  msg += "node " + node_table[node_id].node_name;
                  msg += " found multiple times in the list ";
                  msg += nslist_table[nslist_id];
                  error_exit(msg, table_name, pair_id);
               }
               child_found[i] = true;
            }
         }
      }
      for(size_t i = 0; i < n_child; i++) if( ! child_found[i] )
      {  size_t node_id = child_node_id[i];
         string table_name = "nslist";
         msg += "child node " + node_table[node_id].node_name;
         msg += " not found in the list ";
         msg += nslist_table[nslist_id];
         error_exit(msg, table_name, nslist_id);
      }
   }
}

} // END DISMOD_AT_NAMESPACE
