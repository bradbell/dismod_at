// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin child_info_xam.cpp$$
$spell
   var
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section C++ child_info: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/null_int.hpp>

bool child_info_xam(void)
{  bool ok = true;
   using CppAD::vector;

   // parent node_id
   size_t parent_node_id = 0;

   // node_table
   size_t n_node = 4;
   CppAD::vector<dismod_at::node_struct> node_table(n_node);
   node_table[0].parent = DISMOD_AT_NULL_INT; // parent node
   node_table[1].parent =  0; // child_id = 0
   node_table[2].parent =  0; // child_id = 1
   node_table[3].parent =  2; // grand child node below child_id = 2

   // data_table
   size_t n_data = 4;
   CppAD::vector<dismod_at::data_struct> data_table(n_data);
   data_table[0].node_id = 3; // in child_id = 1  set
   data_table[1].node_id = 2; // in child_id = 1  set
   data_table[2].node_id = 1; // in child_id = 0  set
   data_table[3].node_id = 0; // in parent node   set

   // constructor
   dismod_at::child_info child_info4data(
      parent_node_id, node_table, data_table
   );

   // check child_size
   size_t n_child = child_info4data.child_size();
   ok &= n_child == 2;

   // check child_id2node_id
   ok &= child_info4data.child_id2node_id(0) == 1; // first  child node_id = 1
   ok &= child_info4data.child_id2node_id(1) == 2; // second child node_id = 2

   // check table_id2child_id
   ok &= child_info4data.table_id2child(0) == 1;      // first  data child_id = 1
   ok &= child_info4data.table_id2child(1) == 1;      // second data child_id = 1
   ok &= child_info4data.table_id2child(2) == 0;      // third  data child_id = 0
   ok &= child_info4data.table_id2child(3) == n_child;// fourth in parent set

   return ok;
}
// END C++
