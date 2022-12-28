// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin cov2weight_map dev}

Mapping From (covariate_id, node_id, split_value) to weight_id
##############################################################

Constructor
***********

Inputs
======
{xrst_literal
   // BEGIN_CTOR_INPUTS
   // END_CTOR_INPUTS
}

n_node
------
is the size of the :ref:`get_node_table@node_table` .

n_weight
--------
is the size of the :ref:`get_weight_table@weight_table` .

splitting_covariate
-------------------
is the :ref:`option_table@splitting_covariate` .

covariate_table
---------------
is the :ref:`get_covariate_table@covariate_table` .

node_cov_table
--------------
is the :ref:`get_node_cov_table@node_cov_table` .

Outputs
=======
{xrst_literal
   include/dismod_at/cov2weight_map.hpp
   // BEGIN_CTOR_OUTPUTS
   // END_CTOR_OUTPUTS
}

n_covariate\_
-------------
is the size of the covariate table.

n_node\_
--------
is the size of the node table.

n_weight\_
----------
is the size of the weight table.

split_covariate_id\_
--------------------
If the splitting covariate is empty, this is set to *n_covariate* .
Otherwise, it is set to the :ref:`covariate_table@covariate_id`
that corresponds to this covariate name.

split_covariate_reference\_
---------------------------
If the splitting covariate is empty, this is set to zero.
Otherwise, it is set to the :ref:`covariate_table@reference`
that corresponds to this covariate name.

split_value_vec\_
-----------------
This vector contains all the :ref:`split values<node_cov_table@split_value>`
that appear in node_cov table.

weight_id_vec\_
---------------
For *covariate_id* equal zero to *n_covariate* - 1,
*node_id* equal to zero to *n_node* - 1,
and *split_id* equal to zero to split_value_vec\_.size() - 1, let::

   index = (n_node * covariate_id + node_id) * n_split + split_id

#. if *covariate_id* is in the node_cov table,
   weight_id_vec_[index] is the corresponding *weight_id* in node_cov table.
#. if *covariate_id* is **not** in the node_cov table,
   weight_id_vec_[index] is *n_weight*.

weight_id
*********

Syntax
======
{xrst_literal
   // BEGIN_WEIGHT_ID
   // END_WEIGHT_ID
}

covariate_id
============
This specifies the covariate we are retrieving the weighting for.

node_id
=======
This specifies the node we are retrieving the weighting for.

x
=
This vector has length *n_covariate* and contains the covariate
value, minus its reference, for covariates that are not in the node_cov table.
In particular::

   x[split_value_id_] + split_value_reference_

is the value of the splitting covariate.

weight_id
=========
#. If *covariate_id* is not in node_cov table,
   the return value *weight_id* is equal to *n_weight* .
#. Otherwise, *weight_id* identifies the bilinear function
   of age and time for this covariate, node, and splitting covariate value.


{xrst_end cov2weight_map}
*/
# include <set>
# include <string>
# include <dismod_at/cov2weight_map.hpp>
// ---------------------------------------------------------------------------
// BEGIN_CTOR_INPUTS
dismod_at::cov2weight_map::cov2weight_map(
      size_t                                      n_node              ,
      size_t                                      n_weight            ,
      const std::string&                          splitting_covariate ,
      const CppAD::vector<covariate_struct>&      covariate_table     ,
      const CppAD::vector<node_cov_struct>&       node_cov_table      )
// END_CTOR_INPUTS
:
n_covariate_( covariate_table.size() ) ,
n_node_( n_node )                      ,
n_weight_( n_weight )
{  //
   // split_covariate_id_, split_covariate_reference_
   split_covariate_id_        = n_covariate_;
   split_covariate_reference_ = 0.0;
   for(size_t id = 0; id < n_covariate_; ++id)
   {  if( covariate_table[id].covariate_name == splitting_covariate )
      {  split_covariate_id_        = id;
         split_covariate_reference_ = covariate_table[id].reference;
      }
   }
   assert( (splitting_covariate=="") == (split_covariate_id_==n_covariate_) );
   //
   // n_node_cov
   size_t n_node_cov = node_cov_table.size();
   //
   // split_value_set
   std::set<double> split_value_set;
   if( split_covariate_id_ == n_covariate_ )
      split_value_set.insert(0.0); // value is not used
   else
   {  for(size_t node_cov_id = 0; node_cov_id < n_node_cov; ++node_cov_id)
         split_value_set.insert( node_cov_table[node_cov_id].split_value );
   }
   //
   // split_value_vec_
   {  std::set<double>::const_iterator itr;
      for(itr = split_value_set.begin(); itr != split_value_set.end(); ++itr)
         split_value_vec_.push_back( *itr );
   }
   //
   // weight_id_vec_
   size_t n_split = split_value_vec_.size();
   size_t n_weight_id   = n_covariate_ * n_node_ * n_split;
   weight_id_vec_.resize( n_weight_id );
   for(size_t index = 0; index < n_weight_id; ++index )
      weight_id_vec_[index] = n_weight_;
   for(size_t node_cov_id = 0; node_cov_id < n_node_cov; ++node_cov_id)
   {  int covariate_id    = node_cov_table[node_cov_id].covariate_id;
      int node_id         = node_cov_table[node_cov_id].node_id;
      int weight_id       = node_cov_table[node_cov_id].weight_id;
      assert( size_t( covariate_id ) < n_covariate_ );
      assert( size_t( node_id )      < n_node_ );
      assert( size_t( weight_id )    < n_weight_ );
      //
      double split_value  = node_cov_table[node_cov_id].split_value;
      size_t split_id = n_split;
      for(size_t i = 0; i < n_split; ++i)
      {  if( split_value_vec_[i] == split_value )
         {  assert( split_id == n_split );
            split_id = i;
         }
      }
      assert( split_id != n_split );
      size_t index = (n_node * covariate_id + node_id) * n_split + split_id;
      weight_id_vec_[index] = weight_id;
   }
}
// ---------------------------------------------------------------------------
// BEGIN_WEIGHT_ID
size_t dismod_at::cov2weight_map::weight_id(
   size_t                       covariate_id ,
   size_t                       node_id      ,
   const CppAD::vector<double>& x            ) const
// END_WEIGHT_ID
{  //
   // n_split
   size_t n_split = split_value_vec_.size();
   //
   // split_id
   size_t split_id = n_split;
   if( split_covariate_id_ != n_covariate_ )
   {  double split_value = x[split_covariate_id_] + split_covariate_reference_;
      for(size_t i = 0; i < n_split; ++i)
      {  if( split_value_vec_[i] == split_value )
         {  assert( split_id == n_split );
            split_id = i;
         }
      }
      assert( split_id != n_split );
   }
   if( split_id == n_split )
      split_id = 0;
   //
   assert( covariate_id < n_covariate_ );
   assert( node_id < n_node_ );
   assert( split_id < n_split );
   size_t index = (n_node_ * covariate_id + node_id) * n_split + split_id;
   return weight_id_vec_[index];
}
