// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin child_info dev}

Child Indices and Tables Indices
################################

Syntax
******

| ``child_info`` *child_object* ( *parent_node_id* , *node_table* , *table* )
| *n_child* = *child_object* . ``child_size`` ()
| *node_id* = *child_object* . ``child_id2node_id`` ( *child_id* )

*child* = *child_object* . ``table_id2child`` ( *table_id* )

child_object
************
Except for it's constructor, this object has prototype

   ``const child_info`` *child_object*

Constructor
***********

parent_node_id
==============
This argument has prototype

   ``size_t`` *parent_node_id*

and is the
:ref:`parent node id<option_table@Parent Node>` .
for the fit command.

node_table
==========
This argument has prototype

   ``const CppAD::vector<node_struct>&`` *node_table*

and is the :ref:`get_node_table@node_table` .
Only the following fields of this table are used: ``parent`` .

table
=====
This argument has one of the following prototypes

| |tab| ``const CppAD::vector<`` *data_struct* >& *table*
| |tab| ``const CppAD::vector<`` *avgint_struct* >& *table*

child_size
**********

n_child
=======
This return value has prototype

   ``size_t`` *n_child*

and is the size of the set of
:ref:`node_table@parent@Children` corresponding to the
for the specified parent node.

child_id2node_id
****************

child_id
========
This argument has prototype

   ``size_t`` *child_id*

and is less than *n_child* .
Note that the corresponding node order is the order that the
children appear in *node_table* .
Also note that if *n_child*  == 0 ,
there is no valid value for *child_id* .

node_id
=======
This return value has prototype

   ``size_t`` *node_id*

and is the :ref:`node_table@node_id` for the
corresponding *child_id* .

table_id2child
**************

table_id
========
This argument has prototype

   ``size_t`` *table_id*

and is the primary key in the *table* , i.e.,
the index into the vector *table* .

child
=====
This return value has prototype

   ``size_t`` *child*

If *child* < *n_child* ,

   *table* [ *table_id* ]. ``node_id``

is the child or a descendant of the child

   *child_object* . ``child_id2node_id`` ( *child* )

in the :ref:`node_table-name` .

If *child* == *n_child* ,

   *table* [ *table_id* ]. ``node_id``

is the parent node.

If *child* == *n_child* +1 ,

   *table* [ *table_id* ]. ``node_id``

is not the parent node and not a descendant of the parent node.
{xrst_toc_hidden
   example/devel/utility/child_info_xam.cpp
}
Example
*******
:ref:`child_info_xam.cpp-name`

{xrst_end child_info}
*/
# include <dismod_at/child_info.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/get_avgint_table.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template<class Row>
child_info::child_info(
   size_t                            parent_node_id         ,
   const CppAD::vector<node_struct>& node_table             ,
   const CppAD::vector<Row>&         table                  )
{  assert( parent_node_id != size_t(-1) );

   // child_id2node_id
   assert( child_id2node_id_.size() == 0 );
   for(size_t node_id = 0; node_id < node_table.size(); node_id++)
   {  size_t parent = size_t( node_table[node_id].parent );
      if( parent == parent_node_id )
         child_id2node_id_.push_back(node_id);
   }

   // table_id2child_id
   size_t n_table = table.size();
   table_id2child_.resize(n_table);
   for(size_t table_id = 0; table_id < n_table; table_id++)
   {  size_t node_id = size_t( table[table_id].node_id );
      // check if this is the parent node
      bool   found   = parent_node_id == node_id;
      // special child index for the parent node
      size_t child   = child_id2node_id_.size();
      // loop to check child nodes
      bool   more    = ! found;
      while(more)
      {  for(size_t i = 0; i < child_id2node_id_.size(); i++)
         {  if( child_id2node_id_[i] == node_id )
            {  child = i;
               found = true;
            }
         }
         more = (! found) && (node_id != DISMOD_AT_NULL_SIZE_T);
         if( more )
         {  size_t parent = size_t ( node_table[node_id].parent );
            if( parent == node_id )
            {  // infinite loop checking if parent_node_id is an ancestor
               std::string msg = "This node is a descendant of itself";
               error_exit(msg, "node", node_id);
            }
            node_id = parent;
         }
      }
      if( ! found )
         child = child_id2node_id_.size() + 1;
      table_id2child_[table_id] = child;
   }
}

size_t child_info::child_size(void) const
{  return child_id2node_id_.size(); }

size_t child_info::child_id2node_id(size_t child_id) const
{  return child_id2node_id_[child_id]; }

size_t child_info::table_id2child(size_t table_id) const
{  return table_id2child_[table_id]; }


// instantiate child_info constructor for two possible cases
template child_info::child_info(
   size_t                            parent_node_id         ,
   const CppAD::vector<node_struct>& node_table             ,
   const CppAD::vector<data_struct>& table
);
template child_info::child_info(
   size_t                                parent_node_id             ,
   const CppAD::vector<node_struct>&     node_table             ,
   const CppAD::vector<avgint_struct>& table
);

} // END DISMOD_AT_NAMESPACE
